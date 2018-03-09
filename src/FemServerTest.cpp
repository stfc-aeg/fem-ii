#include <iostream> 
#include <zmq.hpp>
#include "../include/Fem2ControlMsg.h"
#include <cassert>
#include "../include/MsgPackEncoder.h"
#include "../include/mem_reader.h"
#include "../include/i2c_reader.h"

/*
Mock Fem Server - uses ZMQ and MsgPack encoding to receive and send a single
Fem2ControlMsg in a req-reply synchronous loop.

Receives an encoded Fem2ControlMsg, decodes and creates a new Fem2ControlMsg
from the content. Sends the same message back. 
*/


using namespace Femii;

//  create a zmq context and request socket
zmq::context_t context(1);
zmq::socket_t socket_(context, ZMQ_REP);


//  prints the encoded content (buf) as python bytes
void print_as_python_bytes(std::string const& buf)
{
    std::cout << "b\'";
    for (std::string::const_iterator it = buf.begin(), end = buf.end(); it!= end; ++it)
    {
        std::cout
            << "\\x"
            << std::setw(2)
            << std::hex
            << std::setfill('0')
            << (static_cast<int>(*it) & 0xff);
    }
    std::cout << std::dec << "\'" << std::endl;
}

// sends an encoded (string) message over zmq
void send(const std::string& message_str)
{
  size_t msg_size = message_str.size();
  zmq::message_t msg(msg_size);
  memcpy(msg.data(), message_str.data(), msg_size);
  socket_.send(msg);
}

// receives a request over zmq, returns the string reply
std::string receive()
{
    zmq::message_t request;
    socket_.recv (&request);
    //  Dump the message as text or binary
    int size = request.size();
    std::string request_string(static_cast<char*>(request.data()), size);
    return request_string;
}

/*
/   Processes a read request for I2C Access Types.
/   THROWS Fem2ControlMsgException
/   :Param Fem2Request: Reference to the decoded Fem2ControlMsg from client.
/   :Param i2c_reader: Reference to the i2c_reader for accessing /dev/i2c-.
/   :Param i2c_req: Reference to the i2c payload from the decoded Fem2ControlMsg.
/   :Param Fem2Reply: Reference to the Fem2ControlMsg constructed to send back to the client.
/   
/   Constructs a I2C_RW payload, reading the data_length_ number of Bytes/Words/Longs
/   From the i2c slave at the i2c register provided.
/   Sets the Fem2Reply payload with the read data.
/
*/
template <typename T> void handle_read_request(Fem2ControlMsg& Fem2Request, i2c_reader& i2c_reader, I2C_RW& i2c_req, Fem2ControlMsg& Fem2Reply){

    I2C_RW i2c_reply;
    i2c_reply.i2c_bus = i2c_req.i2c_bus;
    i2c_reply.slave_address = i2c_req.slave_address;
    i2c_reply.i2c_register = i2c_req.i2c_register;                    
    i2c_reply.data_width = i2c_req.data_width;
    unsigned long i2c_r_result;

    uint8_t base = 0x10;// start of mac address
    uint8_t increment = 0x01; // end of mac address = base + 0x05

    for (int i=0; i < Fem2Request.get_read_length() ; i++){
        
        i2c_r_result = i2c_reader.read_byte(base); // this is for mac address
        std::cout << "i2c result: " << std::to_string(i2c_r_result) << std::endl;
        get_bytes<I2C_RW>(i2c_r_result, i2c_reply);
        base += increment; //this is for a mac address read.

    }

    i2c_reader.close_bus();
    int read_length = Fem2Request.get_read_length();
    Fem2Reply.set_payload<I2C_RW>(i2c_reply, read_length);
    std::cout<< i2c_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.get_data_length()) << std::endl;
    std::cout<< "read length: " << std::to_string(Fem2Reply.get_read_length()) << std::endl;
    //encode the fem2controlmsg reply 

}

/*
/   Processes a read request for MEM_RW (DDR, QDR, QSPI) Access Types.
/   THROWS Fem2ControlMsgException
/   :Param Fem2Request: Reference to the decoded Fem2ControlMsg from client.
/   :Param mem_reader: Reference to the m-mapped mem_reader for accessing /dev/mem
/   :Param mem_req: Reference to the memory type payload from the decoded Fem2ControlMsg.
/   :Param Fem2Reply: Reference to the Fem2ControlMsg constructed to send back to the client.
/   
/   Constructs a T payload, reading the data_length_ number of Bytes/Words/Longs
/   From the mem address provided.
/   Sets the Fem2Reply payload with the read data.
/
*/
template <typename T> void handle_read_request(Fem2ControlMsg& Fem2Request, mem_reader& mem_reader, MEM_RW& mem_req, Fem2ControlMsg& Fem2Reply){

    T mem_reply;
    mem_reply.mem_address = mem_req.mem_address;
    mem_reply.page = mem_req.page;
    mem_reply.offset = mem_req.offset;
    mem_reply.data_width = mem_req.data_width;

    unsigned long mem_r_result;
    for (int i=0; i < Fem2Request.get_read_length() ; i++){
        
        mem_r_result = mem_reader.read_mem();
        std::cout << "mem result: " << std::to_string(mem_r_result) << std::endl;
        get_bytes<T>(mem_r_result, mem_reply);
        
    }
    mem_reader.unmap();
    int read_length = Fem2Request.get_read_length();
    Fem2Reply.set_payload<T>(mem_reply, read_length);
    std::cout<< mem_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.get_data_length()) << std::endl;
    std::cout<< "read length: " << std::to_string(Fem2Reply.get_read_length()) << std::endl;
}

/*
/   Processes a read request for BASIC_RW (XADC, GPIO, RAW_REG) Access Types.
/   THROWS Fem2ControlMsgException
/   :Param Fem2Request: Reference to the decoded Fem2ControlMsg from client.
/   :Param mem_reader: Reference to the m-mapped mem_reader for accessing /dev/mem
/   :Param basic_req: Reference to the basic r/w type payload from the decoded Fem2ControlMsg.
/   :Param Fem2Reply: Reference to the Fem2ControlMsg constructed to send back to the client.
/   
/   Constructs a T payload, reading the data_length_ number of Bytes/Words/Longs
/   From the mem address provided.
/   Sets the Fem2Reply payload with the read data.
/
*/
template <typename T> void handle_read_request(Fem2ControlMsg& Fem2Request, mem_reader& mem_reader, Basic_RW& basic_req, Fem2ControlMsg& Fem2Reply){
                          
    T basic_reply;
    basic_reply.mem_address = basic_req.mem_address;
    basic_reply.mem_register = basic_req.mem_register;
    basic_reply.data_width = basic_req.data_width;

    unsigned long basic_r_result;
    for (int i=0; i < Fem2Request.get_read_length(); i++){
        
        basic_r_result = mem_reader.read_mem(); // throws Fem2ControlMsgException
        std::cout << "mem result: " << std::to_string(basic_r_result) << std::endl;
        get_bytes<T>(basic_r_result, basic_reply);
        
    }
    mem_reader.unmap();
    int read_length = Fem2Request.get_read_length();
    Fem2Reply.set_payload<T>(basic_reply, read_length);
    std::cout<< basic_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.get_data_length()) << std::endl;
    std::cout<< "read length: " << std::to_string(Fem2Reply.get_read_length()) << std::endl;
    //encode the fem2controlmsg reply 
}

/*
/   Processes a write request for MEM_RW (DDR, QDR, QSPI) Access Types.
/   THROWS Fem2ControlMsgException
/   :Param Fem2Request: Reference to the decoded Fem2ControlMsg from client.
/   :Param mem_reader: Reference to the m-mapped mem_reader for accessing /dev/mem
/   :Param mem_req: Reference to the memory type payload from the decoded Fem2ControlMsg.
/   :Param Fem2Reply: Reference to the Fem2ControlMsg constructed to send back to the client.
/   
/   Constructs a T payload
/   Writing the data_length_ number of Bytes/Words/Longs
/   using form_words_longs to write in correct widths
/   From the mem address provided.
/   Re-bit splits the data into bytes and 
/   Sets the Fem2Reply payload with the written data.
/
*/
template <typename T> void handle_write_request(Fem2ControlMsg& Fem2Request, mem_reader& mem_reader, MEM_RW& mem_req, Fem2ControlMsg& Fem2Reply){

    T mem_reply;
    mem_reply.mem_address = mem_req.mem_address;
    mem_reply.page = mem_req.page;
    mem_reply.offset = mem_req.offset; 
    mem_reply.data_width = mem_req.data_width;

    unsigned long write_data;
    unsigned long write_result;

    switch(mem_req.data_width){

        case WIDTH_BYTE:
            for(int i = 0; i < Fem2Request.get_data_length(); i++){
                
                write_data = form_words_longs<T>(static_cast<T&>(mem_req), i);
                write_result = mem_reader.write_mem(write_data);
                get_bytes<T>(write_result, mem_reply);
            }
            break;
        case WIDTH_WORD:
            for(int i = 0; i < Fem2Request.get_data_length(); i=i+2){
                
                write_data = form_words_longs<T>(static_cast<T&>(mem_req), i);
                write_result = mem_reader.write_mem(write_data);
                get_bytes<T>(write_result, mem_reply);
            }
            break;
        case WIDTH_LONG:
            for(int i = 0; i < Fem2Request.get_data_length(); i=i+4){
                
                write_data = form_words_longs<T>(static_cast<T&>(mem_req), i);
                write_result = mem_reader.write_mem(write_data);
                get_bytes<T>(write_result, mem_reply);
            }
            break;
        default:
            throw Fem2ControlMsgException("Illegal Data Width");
            break;

    }

    Fem2Reply.set_payload<T>(mem_reply);
    mem_reader.unmap();

    std::cout<< mem_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.get_data_length()) << std::endl;
    //encode the fem2controlmsg reply 

}

/*
/   Processes a write request for I2C_RW Access Types.
/   THROWS Fem2ControlMsgException
/   :Param Fem2Request: Reference to the decoded Fem2ControlMsg from client.
/   :Param i2c_reader: Reference to the m-mapped mem_reader for accessing /dev/i2c-
/   :Param i2c_req: Reference to the i2c type payload from the decoded Fem2ControlMsg.
/   :Param Fem2Reply: Reference to the Fem2ControlMsg constructed to send back to the client.
/   
/   Constructs a T payload
/   Writing the data_length_ number of Bytes/Words/Longs
/   using form_words_longs to write in correct widths
/   From the mem address provided.
/   Re-bit splits the data into bytes and 
/   Sets the Fem2Reply payload with the written data.
/
*/
template <typename T> void handle_write_request(Fem2ControlMsg& Fem2Request, i2c_reader& i2c_reader, I2C_RW& i2c_req, Fem2ControlMsg& Fem2Reply){

    I2C_RW i2c_w_reply;
    i2c_w_reply.i2c_bus = i2c_req.i2c_bus;
    i2c_w_reply.slave_address = i2c_req.slave_address;
    i2c_w_reply.i2c_register = i2c_req.i2c_register;                    
    i2c_w_reply.data_width = i2c_req.data_width;

    unsigned long write_data;
    unsigned long write_result;

    switch(i2c_req.data_width){

        case WIDTH_BYTE:
            for(int i = 0; i < Fem2Request.get_data_length(); i++){
                
                write_data = form_words_longs<I2C_RW>(i2c_req, i);
                write_result = i2c_reader.write_byte(write_data);
                get_bytes<I2C_RW>(write_result, i2c_w_reply);
            }
            break;
        case WIDTH_WORD:
            for(int i = 0; i < Fem2Request.get_data_length(); i=i+2){
                write_data = form_words_longs<I2C_RW>(i2c_req, i);
                write_result = i2c_reader.write_word(write_data);
                get_bytes<I2C_RW>(write_result, i2c_w_reply);
            }
            break;
        case WIDTH_LONG:
            throw Fem2ControlMsgException("You haven't implemented longs yet...!");
            /*
            for(int i = 0; i < Fem2Request.data_length_; i=i+4){
                write_data = form_words_longs<I2C_RW>(i2c_req, i);
                write_result = i2c_reader.write_long(write_data);
                get_bytes<I2C_RW>(write_result, i2c_w_reply);
            }
            */
            break;
        default:
            throw Fem2ControlMsgException("Illegal Data Width");
            break;
    }

    Fem2Reply.set_payload<I2C_RW>(i2c_w_reply);
    i2c_reader.close_bus();

    std::cout<< i2c_w_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.get_data_length()) << std::endl;
}

/*
/   Processes a write request for BASIC_RW (XADC, GPIO, RAW_REG) Access Types.
/   THROWS Fem2ControlMsgException
/   :Param Fem2Request: Reference to the decoded Fem2ControlMsg from client.
/   :Param mem_reader: Reference to the m-mapped mem_reader for accessing /dev/mem
/   :Param basic_req: Reference to the basic r/w type payload from the decoded Fem2ControlMsg.
/   :Param Fem2Reply: Reference to the Fem2ControlMsg constructed to send back to the client.
/   
/   Constructs a T payload
/   Writing the data_length_ number of Bytes/Words/Longs
/   using form_words_longs to write in correct widths
/   From the mem address provided.
/   Re-bit splits the data into bytes and 
/   Sets the Fem2Reply payload with the written data.
/
*/
template<typename T> void handle_write_request(Fem2ControlMsg& Fem2Request, mem_reader& mem_reader, Basic_RW& basic_req, Fem2ControlMsg& Fem2Reply){
    
    T basic_reply;
    basic_reply.mem_address = basic_req.mem_address;
    basic_reply.mem_register = basic_req.mem_register;
    basic_reply.data_width = basic_req.data_width;

    unsigned long write_data;
    unsigned long write_result;

    switch(basic_req.data_width){

        case WIDTH_BYTE:
            for(int i = 0; i < Fem2Request.get_data_length(); i++){
                
                write_data = form_words_longs<T>(static_cast<T&>(basic_req), i);
                write_result = mem_reader.write_mem(write_data);
                get_bytes<T>(write_result, basic_reply);
            }
            break;
        case WIDTH_WORD:
            for(int i = 0; i < Fem2Request.get_data_length(); i=i+2){
                
                write_data = form_words_longs<T>(static_cast<T&>(basic_req), i);
                write_result = mem_reader.write_mem(write_data);
                get_bytes<T>(write_result, basic_reply);
            }
            break;
        case WIDTH_LONG:
            for(int i = 0; i < Fem2Request.get_data_length(); i=i+4){
                
                write_data = form_words_longs<T>(static_cast<T&>(basic_req), i);
                write_result = mem_reader.write_mem(write_data);
                get_bytes<T>(write_result, basic_reply);
            }
            break;
        default:
            throw Fem2ControlMsgException("Illegal Data Width");
            break;

    }

    Fem2Reply.set_payload<T>(basic_reply);
    mem_reader.unmap();

    std::cout<< basic_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.get_data_length()) << std::endl;
}
 
                
int main(){

    socket_.bind("tcp://*:5555");
    printf("server Booted \n");
    //   create a msgpack encoder
    MsgPackEncoder encoder;

    int test_num = 18;
    
    while(true) { 

        // receive request
        std::string encoded_request = receive();
        //print request
        print_as_python_bytes(encoded_request);

        //  decode the message into a fem2controlmsg using encoder
        Fem2ControlMsg decoded_request;
        
        encoder.decode(encoded_request, decoded_request);

        std::string encoded_reply;

        // CMD_NOTIFY == just testing round trip encoding
        if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_NOTIFY || decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_CONFIGURE){

            //encode the fem2controlmsg reply directly for encoding comparison.
            //encoded_reply = encoder.encode(decoded_request);
            encoder.encode(decoded_request, encoded_reply);
        }

        else{ //testing read-write-config
            
            if(decoded_request.get_access_type() == Femii::Fem2ControlMsg::ACCESS_DDR){
                
                DDR_RW ddr_req = decoded_request.get_payload<DDR_RW>();

                // ------------- GPIO write to change the page of the DDR memory -------------//
                GPIO_RW gpio_page_req;
                gpio_page_req.mem_address = 0xA0010000;
                gpio_page_req.mem_register = 0;
                gpio_page_req.data_width = WIDTH_BYTE;
                // memory read to write the page
                mem_reader gpio_memory_reader(gpio_page_req.mem_address, gpio_page_req.mem_register, gpio_page_req.data_width);
                gpio_memory_reader.init_mmap();
                unsigned long page_result = gpio_memory_reader.write_mem(ddr_req.page);
                std::cout << "DDR page: " << std::to_string(page_result) << std::endl;
                gpio_memory_reader.unmap();
                // -------------------------------------------------------------------------//

                // DDR ACCESS
                mem_reader ddr_memory_reader(ddr_req.mem_address, ddr_req.offset, ddr_req.data_width);
                ddr_memory_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Femii::Fem2ControlMsg::CMD_WRITE)
                {
                    Fem2ControlMsg write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    try
                    {
                        handle_write_request<DDR_RW>(decoded_request, ddr_memory_reader, ddr_req, write_reply);   
                    }
                    catch(Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        write_reply.set_ack_state(Fem2ControlMsg::NACK);  
                    }
                    //encoded_reply = encoder.encode(write_reply);
                    encoder.encode(write_reply, encoded_reply);

                } // ddr-write

                else if(decoded_request.get_cmd_type() == Femii::Fem2ControlMsg::CMD_READ)
                {
                    Fem2ControlMsg read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    
                    try
                    {
                        handle_read_request<DDR_RW>(decoded_request, ddr_memory_reader, ddr_req, read_reply);
                    }
                    catch(Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        read_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }
                    //encoded_reply = encoder.encode(read_reply);
                    encoder.encode(read_reply, encoded_reply);

                        
                } // ddr-read
            } // ddr
            
            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_GPIO)
            {
                
                GPIO_RW gpio_req = decoded_request.get_payload<GPIO_RW>();
                mem_reader gpio_mem_reader(gpio_req.mem_address, gpio_req.mem_register, gpio_req.data_width);
                gpio_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE)
                {

                    Fem2ControlMsg gpio_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    try
                    {
                        handle_write_request<GPIO_RW>(decoded_request, gpio_mem_reader, gpio_req, gpio_write_reply);
                    }
                    catch(Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        gpio_write_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }
                    //encoded_reply = encoder.encode(gpio_write_reply);
                    encoder.encode(gpio_write_reply, encoded_reply);

                } //gpio write

                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ)
                {

                    Fem2ControlMsg gpio_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK, 0x1234, 10, 0);

                    try
                    {
                        handle_read_request<GPIO_RW>(decoded_request, gpio_mem_reader, gpio_req, gpio_read_reply);
                    }
                    catch (Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        gpio_read_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }

                    //encoded_reply = encoder.encode(gpio_read_reply);
                    encoder.encode(gpio_read_reply, encoded_reply);


                } // gpio-read
            } // gpio

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_RAWREG)
            {
                               
                RAWREG_RW rreg_req = decoded_request.get_payload<RAWREG_RW>();
                mem_reader rreg_mem_reader(rreg_req.mem_address, rreg_req.mem_register, rreg_req.data_width);
                rreg_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE)
                {

                    Fem2ControlMsg rreg_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    try
                    {
                        handle_write_request<RAWREG_RW>(decoded_request, rreg_mem_reader, rreg_req, rreg_write_reply);
                    }
                    catch (Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        rreg_write_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }
                    //encoded_reply = encoder.encode(rreg_write_reply);
                    encoder.encode(rreg_write_reply, encoded_reply);

                }

                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ)
                {

                    Fem2ControlMsg rreg_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    
                    try
                    {
                        handle_read_request<RAWREG_RW>(decoded_request, rreg_mem_reader, rreg_req, rreg_read_reply);
                    }
                    catch (Fem2ControlMsgException e) 
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        rreg_read_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }

                    //encoded_reply = encoder.encode(rreg_read_reply);
                    encoder.encode(rreg_read_reply, encoded_reply);

                }
            }  // raw-reg

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_XADC)
            {
                             
                XADC_RW xadc_req = decoded_request.get_payload<XADC_RW>();
                mem_reader xadc_mem_reader(xadc_req.mem_address, xadc_req.mem_register, xadc_req.data_width);
                xadc_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE)
                {
                    
                    Fem2ControlMsg xadc_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK, 0x1234, 10, 0);

                    try
                    {
                        handle_write_request<XADC_RW>(decoded_request, xadc_mem_reader, xadc_req, xadc_write_reply); // untested.
                    }
                    catch (Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        xadc_write_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }

                    //encoded_reply = encoder.encode(xadc_write_reply);
                    encoder.encode(xadc_write_reply, encoded_reply);
                }

                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ)
                {

                    Fem2ControlMsg xadc_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK, 0x1234, 10, 0);

                    try
                    {
                        handle_read_request<XADC_RW>(decoded_request, xadc_mem_reader, xadc_req, xadc_read_reply);
                    }
                    catch(Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        xadc_read_reply.set_ack_state(Fem2ControlMsg::NACK);
                    }
                    
                    //encoded_reply = encoder.encode(xadc_read_reply);
                    encoder.encode(xadc_read_reply, encoded_reply);

                }

            }//xadc

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_QSPI)
            {
                             
                QSPI_RW qspi_req = decoded_request.get_payload<QSPI_RW>();
                mem_reader qspi_mem_reader(qspi_req.mem_address, qspi_req.offset, qspi_req.data_width);
                qspi_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE)
                {

                    Fem2ControlMsg qspi_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    try
                    {
                        handle_write_request<QSPI_RW>(decoded_request, qspi_mem_reader, qspi_req, qspi_write_reply);
                    }
                    catch (Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        qspi_write_reply.set_ack_state(Fem2ControlMsg::NACK);                        
                    }
                    //encoded_reply = encoder.encode(qspi_write_reply);
                    encoder.encode(qspi_write_reply, encoded_reply);

                }

                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ)
                {
                    Fem2ControlMsg qspi_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    try
                    {
                        handle_read_request<QSPI_RW>(decoded_request, qspi_mem_reader, qspi_req, qspi_read_reply);
                    }
                    catch (Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        qspi_read_reply.set_ack_state(Fem2ControlMsg::NACK);                        
                    } 
                    //encoded_reply = encoder.encode(qspi_read_reply);
                    encoder.encode(qspi_read_reply, encoded_reply);
                }
            }//qspi    

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_I2C)
            {
                             
                I2C_RW i2c_req = decoded_request.get_payload<I2C_RW>();
                i2c_reader i2c_reader(i2c_req.i2c_bus, i2c_req.slave_address, i2c_req.i2c_register, i2c_req.data_width);
                i2c_reader.open_bus();
                i2c_reader.set_slave();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE)
                {
                    
                    Fem2ControlMsg i2c_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    
                    try
                    {
                        handle_write_request<I2C_RW>(decoded_request, i2c_reader, i2c_req, i2c_write_reply);
                    }
                    catch(Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        i2c_write_reply.set_ack_state(Fem2ControlMsg::NACK);                        
                    }

                    //encoded_reply = encoder.encode(i2c_write_reply);
                    encoder.encode(i2c_write_reply, encoded_reply);


                }

                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ)
                {

                    Fem2ControlMsg i2c_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    try
                    {
                        handle_read_request<I2C_RW>(decoded_request, i2c_reader, i2c_req, i2c_read_reply);
                    }
                    catch(Fem2ControlMsgException e)
                    {
                        std::cout << "Fem2ControlMsg Error: " << e.what() << std::endl;
                        i2c_read_reply.set_ack_state(Fem2ControlMsg::NACK);                        
                    }
                    //encoded_reply = encoder.encode(i2c_read_reply);
                    encoder.encode(i2c_read_reply, encoded_reply);
                }
            }//i2c   
        } // read-write-config

        // send the encoded reply via zmq for comparison
        send(encoded_reply);
    } //for or while loop
}