#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>
#include "MsgPackEncoder.hpp"
#include "mem_reader.hpp"
#include "i2c_reader.hpp"

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

template <typename T> void handle_read_request(Fem2ControlMsg& Fem2Request, i2c_reader& i2c_reader, I2C_RW& i2c_req, Fem2ControlMsg& Fem2Reply){

    I2C_RW i2c_reply;
    i2c_reply.i2c_bus = i2c_req.i2c_bus;
    i2c_reply.slave_address = i2c_req.slave_address;
    i2c_reply.i2c_register = i2c_req.i2c_register;                    
    i2c_reply.data_width = i2c_req.data_width;
    unsigned long i2c_r_result;

    uint8_t base = 0x10;// start of mac address
    uint8_t increment = 0x01; // end of mac address = base + 0x05

    for (int i=0; i < Fem2Request.data_length_ ; i++){
        
        i2c_r_result = i2c_reader.read_byte(base);
        std::cout << "i2c result: " << std::to_string(i2c_r_result) << std::endl;
        get_bytes<I2C_RW>(i2c_r_result, i2c_reply);
        base += increment; //this is for a mac address read.

    }

    i2c_reader.close_bus();

    Fem2Reply.set_payload<I2C_RW>(i2c_reply); // at this point don't set a size - use the vector.size()
    std::cout<< i2c_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.data_length_) << std::endl;
    //encode the fem2controlmsg reply 

}

template <typename T> void handle_read_request(Fem2ControlMsg& Fem2Request, mem_reader& mem_reader, MEM_RW& mem_req, Fem2ControlMsg& Fem2Reply){

    T mem_reply;
    mem_reply.mem_address = mem_req.mem_address;
    mem_reply.page = mem_req.page;
    mem_reply.offset = mem_req.offset;
    mem_reply.data_width = mem_req.data_width;

    unsigned long mem_r_result;
    for (int i=0; i < Fem2Request.data_length_ ; i++){
        
        mem_r_result = mem_reader.read_mem();
        std::cout << "mem result: " << std::to_string(mem_r_result) << std::endl;
        get_bytes<T>(mem_r_result, mem_reply);
        
    }

    mem_reader.unmap();
    Fem2Reply.set_payload<T>(mem_reply);
    std::cout<< mem_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.data_length_) << std::endl;
    //encode the fem2controlmsg reply 
 
}

template <typename T> void handle_read_request(Fem2ControlMsg& Fem2Request, mem_reader& mem_reader, Basic_RW& basic_req, Fem2ControlMsg& Fem2Reply){
                    
                    
    T basic_reply;
    basic_reply.mem_address = basic_req.mem_address;
    basic_reply.mem_register = basic_req.mem_register;
    basic_reply.data_width = basic_req.data_width;

    /*
    unsigned long basic_r_result
    for (int i=0; i < Fem2Request.data_length_ ; i++){
        
        basic_r_result = mem_reader.read_mem();
        std::cout << "mem result: " << std::to_string(basic_r_result) << std::endl;
        get_bytes<T>(basic_r_result, basic_reply);
        
    }
    */

    unsigned long basic_r_result = mem_reader.read_mem();
    std::cout << "mem result: " << std::to_string(basic_r_result) << std::endl;
    mem_reader.unmap();
                   
    get_bytes<T>(basic_r_result, basic_reply);
    //rreg_reply.the_data.push_back(rreg_r_result);
    Fem2Reply.set_payload<T>(basic_reply);
    std::cout<< basic_reply.print() << std::endl;
    std::cout<< "data length: " << std::to_string(Fem2Reply.data_length_) << std::endl;
    //encode the fem2controlmsg reply 

}



                   

int main(){

    


    socket_.bind("tcp://*:5555");
    printf("server Booted \n");
    //   create a msgpack encoder
    MsgPackEncoder encoder;

    int test_num = 18;
    
    while(true) /*for(int x = 0; x < test_num; x++)*/{ 

        // receive request
        std::string encoded_request = receive();
        //print request
        print_as_python_bytes(encoded_request);

        //  decode the message into a fem2controlmsg using encoder
        Fem2ControlMsg decoded_request = encoder.decode(encoded_request);
        std::string encoded_reply;

        // CMD_NOTIFY == just testing round trip encoding
        if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_NOTIFY){

            //encode the fem2controlmsg reply 
            encoded_reply = encoder.encode(decoded_request);

        }

        else{ //testing read-write-config
            
            if(decoded_request.get_access_type() == Femii::Fem2ControlMsg::ACCESS_DDR){
                
                DDR_RW ddr_req = decoded_request.get_payload<DDR_RW>();

                // GPIO write to change the page of the DDR memory.
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



                // now we can read ddr memory.
                mem_reader ddr_memory_reader(ddr_req.mem_address, ddr_req.offset, ddr_req.data_width);
                ddr_memory_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Femii::Fem2ControlMsg::CMD_WRITE){

                    // iterate over the data vector, writing memory untill data_length
                    // in this case we KNOW it's 1 byte, at the 1st index.
                    
                    //unsigned long write_data = form_words_longs(ddr_req);
                    unsigned long write_data = form_words_longs<DDR_RW>(ddr_req);
               

                    std::cout << "size = " << ddr_req.the_data.size() << std::endl;
                    std::cout << "length = " << decoded_request.data_length_ << std::endl;
                    unsigned long write_result = ddr_memory_reader.write_mem(write_data);

                    Fem2ControlMsg write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    DDR_RW ddr_reply;
                    ddr_reply.mem_address = ddr_req.mem_address;
                    ddr_reply.page = ddr_req.page;
                    ddr_reply.offset = ddr_req.offset; 
                    ddr_reply.data_width = ddr_req.data_width;
     

                    get_bytes<DDR_RW>(write_result, ddr_reply);

                    write_reply.set_payload<DDR_RW>(ddr_reply);
                    std::cout<< ddr_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(write_reply);
                    ddr_memory_reader.unmap();

                }
                else if(decoded_request.get_cmd_type() == Femii::Fem2ControlMsg::CMD_READ){
                
                        // form a new Fem2ControlMsg to send back. 

                        Fem2ControlMsg read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK, 0x1234, 10, 0);

                        handle_read_request<DDR_RW>(decoded_request, ddr_memory_reader, ddr_req, read_reply);

                        encoded_reply = encoder.encode(read_reply);
                        
                } // ddr-read
            } // ddr
            
            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_GPIO){
                
                GPIO_RW gpio_req = decoded_request.get_payload<GPIO_RW>();
                mem_reader gpio_mem_reader(gpio_req.mem_address, gpio_req.mem_register, gpio_req.data_width);
                gpio_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE){

                     // iterate over the data vector, writing memory untill data_length
                    // in this case we KNOW it's 1 byte, at the 1st index.
                    
                    std::cout << gpio_req.print() << std::endl;
                    std::cout << "size = " << gpio_req.the_data.size() << std::endl;
                    std::cout << "length = " << decoded_request.data_length_ << std::endl;
                    
                    unsigned long gpio_write_data = form_words_longs<GPIO_RW>(gpio_req);
          

                    unsigned long gpio_write_result = gpio_mem_reader.write_mem(gpio_write_data);
                    std::cout << "GPIO Write : " << std::to_string(gpio_write_result) << std::endl;
                    
                    Fem2ControlMsg gpio_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    GPIO_RW gpio_w_reply;
                    gpio_w_reply.mem_address = gpio_req.mem_address;
                    gpio_w_reply.mem_register = gpio_req.mem_register;
                    gpio_w_reply.data_width = gpio_req.data_width;

          
                    get_bytes<GPIO_RW>(gpio_write_result, gpio_w_reply);
                    gpio_write_reply.set_payload<GPIO_RW>(gpio_w_reply);
                    std::cout<< gpio_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(gpio_write_reply.data_length_) << std::endl;

                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(gpio_write_reply);
                    gpio_mem_reader.unmap();

                } //gpio write
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

                    Fem2ControlMsg gpio_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK, 0x1234, 10, 0);

                    handle_read_request<GPIO_RW>(decoded_request, gpio_mem_reader, gpio_req, gpio_read_reply);

                    encoded_reply = encoder.encode(gpio_read_reply);

                } // gpio-read
            } // gpio

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_RAWREG){
                               
                RAWREG_RW rreg_req = decoded_request.get_payload<RAWREG_RW>();
                mem_reader rreg_mem_reader(rreg_req.mem_address, rreg_req.mem_register, rreg_req.data_width);
                rreg_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE){

                     // iterate over the data vector, writing memory untill data_length
                    // in this case we KNOW it's 1 byte, at the 1st index.

                    unsigned long rreg_write_data = form_words_longs<RAWREG_RW>(rreg_req);
               

                    unsigned long rreg_write_result = rreg_mem_reader.write_mem(rreg_write_data);
                    std::cout << "RAWREG Write : " << std::to_string(rreg_write_result) << std::endl;
                    rreg_mem_reader.unmap();

                    Fem2ControlMsg rreg_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    RAWREG_RW rreg_w_reply;
                    rreg_w_reply.mem_address = rreg_req.mem_address;
                    rreg_w_reply.mem_register = rreg_req.mem_register;
                    rreg_w_reply.data_width = rreg_req.data_width;
               

                    get_bytes<RAWREG_RW>(rreg_write_result, rreg_w_reply);
                    rreg_write_reply.set_payload<RAWREG_RW>(rreg_w_reply);
                    std::cout<< rreg_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(rreg_write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(rreg_write_reply);

                }
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

                    Fem2ControlMsg rreg_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    
                    handle_read_request<RAWREG_RW>(decoded_request, rreg_mem_reader, rreg_req, rreg_read_reply);

                    encoded_reply = encoder.encode(rreg_read_reply);

                }
            }  // raw-reg

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_XADC){
                             
                XADC_RW xadc_req = decoded_request.get_payload<XADC_RW>();
                mem_reader xadc_mem_reader(xadc_req.mem_address, xadc_req.mem_register, xadc_req.data_width);
                xadc_mem_reader.init_mmap();
                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE){
                    
                    // iterate over the data vector, writing memory untill data_length
                    // in this case we KNOW it's 1 byte, at the 1st index.

                    unsigned long xadc_write_data = form_words_longs<XADC_RW>(xadc_req);
                 

                    unsigned long xadc_write_result = xadc_mem_reader.write_mem(xadc_write_data);
                    std::cout << "XADC Write : " << std::to_string(xadc_write_result) << std::endl;
                    xadc_mem_reader.unmap();

                    Fem2ControlMsg xadc_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    XADC_RW xadc_w_reply;
                    xadc_w_reply.mem_address = xadc_req.mem_address;
                    xadc_w_reply.mem_register = xadc_req.mem_register;
                    xadc_w_reply.data_width = xadc_req.data_width;
        

                    get_bytes<XADC_RW>(xadc_write_result, xadc_w_reply);
                    xadc_write_reply.set_payload<XADC_RW>(xadc_w_reply);
                    std::cout<< xadc_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(xadc_write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(xadc_write_reply);

                }
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

            
                    Fem2ControlMsg xadc_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK, 0x1234, 10, 0);

                    handle_read_request<XADC_RW>(decoded_request, xadc_mem_reader, xadc_req, xadc_read_reply);
     
                    encoded_reply = encoder.encode(xadc_read_reply);

                }

            }//xadc

            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_QSPI){
                             
                QSPI_RW qspi_req = decoded_request.get_payload<QSPI_RW>();
                mem_reader qspi_mem_reader(qspi_req.mem_address, qspi_req.offset, qspi_req.data_width);
                qspi_mem_reader.init_mmap();

                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE){
                    
                    // iterate over the data vector, writing memory untill data_length
                    // in this case we KNOW it's 1 byte, at the 1st index.

                    unsigned long qspi_write_data = form_words_longs<QSPI_RW>(qspi_req);
                  

                    unsigned long qspi_write_result = qspi_mem_reader.write_mem(qspi_write_data);
                    std::cout << "QSPI Write : " << std::to_string(qspi_write_result) << std::endl;
                    qspi_mem_reader.unmap();

                    Fem2ControlMsg qspi_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    QSPI_RW qspi_w_reply;
                    qspi_w_reply.mem_address = qspi_req.mem_address;
                    qspi_w_reply.offset = qspi_req.offset;
                    qspi_w_reply.data_width = qspi_req.data_width;
                  

                    get_bytes<QSPI_RW>(qspi_write_result, qspi_w_reply);
                    qspi_write_reply.set_payload<QSPI_RW>(qspi_w_reply);
                    std::cout<< qspi_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(qspi_write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(qspi_write_reply);

                }
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

                    Fem2ControlMsg qspi_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    
                    handle_read_request<QSPI_RW>(decoded_request, qspi_mem_reader, qspi_req, qspi_read_reply);
      
                    encoded_reply = encoder.encode(qspi_read_reply);

                }

            }//qspi            
            else if(decoded_request.get_access_type() == Fem2ControlMsg::ACCESS_I2C){
                             
                I2C_RW i2c_req = decoded_request.get_payload<I2C_RW>();
                i2c_reader i2c_reader(i2c_req.i2c_bus, i2c_req.slave_address, i2c_req.i2c_register, i2c_req.data_width);
                i2c_reader.open_bus();
                i2c_reader.set_slave();
                if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_WRITE){
                    
                    // iterate over the data vector, writing memory untill data_length
                    // in this case we KNOW it's 1 byte, at the 1st index.

                    unsigned long i2c_write_data = form_words_longs<I2C_RW>(i2c_req);
    
                    unsigned long i2c_write_result = i2c_reader.write_byte(i2c_write_data);

                    std::cout << "I2C Write : " << std::to_string(i2c_write_result) << std::endl;
                    i2c_reader.close_bus();

                    Fem2ControlMsg i2c_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    I2C_RW i2c_w_reply;
                    i2c_w_reply.i2c_bus = i2c_req.i2c_bus;
                    i2c_w_reply.slave_address = i2c_req.slave_address;
                    i2c_w_reply.i2c_register = i2c_req.i2c_register;                    
                    i2c_w_reply.data_width = i2c_req.data_width;
     
                    get_bytes<I2C_RW>(i2c_write_result, i2c_w_reply);


                    i2c_write_reply.set_payload<I2C_RW>(i2c_w_reply);
                    std::cout<< i2c_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(i2c_write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(i2c_write_reply);

                }
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

                    Fem2ControlMsg i2c_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    
                    handle_read_request<I2C_RW>(decoded_request, i2c_reader, i2c_req, i2c_read_reply);

                    encoded_reply = encoder.encode(i2c_read_reply);

                }

            }//i2c   
        } // read-write-config

        // send the encoded reply via zmq for comparison
        send(encoded_reply);
    } //for or while loop
}