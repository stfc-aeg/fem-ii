#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>
#include "MsgPackEncoder.hpp"
#include "mem_reader.hpp"

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

int main(){

    socket_.bind("tcp://*:5555");
    printf("server Booted \n");
    //   create a msgpack encoder
    MsgPackEncoder encoder;

    int test_num = 13;
    
    for(int x = 0; x < test_num; x++){ 

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
                GPIO_RW gpio_req;
                gpio_req.mem_address = 0xA0010000;
                gpio_req.mem_register = 0;
                gpio_req.data_width = WIDTH_BYTE;
                // memory read to write the page
                mem_reader gpio_memory_reader(gpio_req.mem_address, gpio_req.mem_register, gpio_req.data_width);


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
                    unsigned long result = ddr_memory_reader.write_mem(ddr_req.the_data.at(0));

                    Fem2ControlMsg write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    DDR_RW ddr_reply;
                    ddr_reply.mem_address = ddr_req.mem_address;
                    ddr_reply.page = ddr_req.page;
                    ddr_reply.offset = ddr_req.offset; 
                    ddr_reply.data_width = ddr_req.data_width;
                    ddr_reply.the_data.push_back(result);
                    write_reply.set_payload<DDR_RW>(ddr_reply);
                    std::cout<< ddr_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(write_reply);

                }
                else if(decoded_request.get_cmd_type() == Femii::Fem2ControlMsg::CMD_READ){
                
                        unsigned long result = ddr_memory_reader.read_mem();
                        std::cout << "mem result: " << std::to_string(result) << std::endl;
                        ddr_memory_reader.unmap();

                        // form a new Fem2ControlMsg to send back. 

                        Fem2ControlMsg read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                        DDR_RW ddr_reply;
                        ddr_reply.mem_address = ddr_req.mem_address;
                        ddr_reply.page = ddr_req.page;
                        ddr_reply.offset = ddr_req.offset; 
                        ddr_reply.data_width = ddr_req.data_width;
                        ddr_reply.the_data.push_back(result);
                        read_reply.set_payload<DDR_RW>(ddr_reply);
                        std::cout<< ddr_reply.print() << std::endl;
                        std::cout<< "data length: " << std::to_string(read_reply.data_length_) << std::endl;
                            //encode the fem2controlmsg reply 
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
                    unsigned long gpio_write_result = gpio_mem_reader.write_mem(gpio_req.the_data.at(0));
                    std::cout << "GPIO Write : " << std::to_string(gpio_write_result) << std::endl;
                    gpio_mem_reader.unmap();

                    Fem2ControlMsg gpio_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    GPIO_RW gpio_w_reply;
                    gpio_w_reply.mem_address = gpio_req.mem_address;
                    gpio_w_reply.mem_register = gpio_req.mem_register;
                    gpio_w_reply.data_width = gpio_req.data_width;
                    gpio_w_reply.the_data.push_back(gpio_write_result);
                    gpio_write_reply.set_payload<GPIO_RW>(gpio_w_reply);
                    std::cout<< gpio_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(gpio_write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(gpio_write_reply);

                } //gpio write
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

                    unsigned long gpio_r_result = gpio_mem_reader.read_mem();
                    std::cout << "mem result: " << std::to_string(gpio_r_result) << std::endl;
                    gpio_mem_reader.unmap();


                    Fem2ControlMsg gpio_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    GPIO_RW gpio_reply;
                    gpio_reply.mem_address = gpio_req.mem_address;
                    gpio_reply.mem_register = gpio_req.mem_register;
                    gpio_reply.data_width = gpio_req.data_width;
                    gpio_reply.the_data.push_back(gpio_r_result);
                    gpio_read_reply.set_payload<GPIO_RW>(gpio_reply);
                    std::cout<< gpio_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(gpio_read_reply.data_length_) << std::endl;
                            //encode the fem2controlmsg reply 
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
                    unsigned long rreg_write_result = rreg_mem_reader.write_mem(rreg_req.the_data.at(0));
                    std::cout << "RAWREG Write : " << std::to_string(rreg_write_result) << std::endl;
                    rreg_mem_reader.unmap();

                    Fem2ControlMsg rreg_write_reply(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    RAWREG_RW rreg_w_reply;
                    rreg_w_reply.mem_address = rreg_req.mem_address;
                    rreg_w_reply.mem_register = rreg_req.mem_register;
                    rreg_w_reply.data_width = rreg_req.data_width;
                    rreg_w_reply.the_data.push_back(rreg_write_result);
                    rreg_write_reply.set_payload<RAWREG_RW>(rreg_w_reply);
                    std::cout<< rreg_w_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(rreg_write_reply.data_length_) << std::endl;
                    //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(rreg_write_reply);

                }
                else if(decoded_request.get_cmd_type() == Fem2ControlMsg::CMD_READ){

                    unsigned long rreg_r_result = rreg_mem_reader.read_mem();
                    std::cout << "mem result: " << std::to_string(rreg_r_result) << std::endl;
                    rreg_mem_reader.unmap();

                    Fem2ControlMsg rreg_read_reply(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK, 0x1234, 10, 0);
                    RAWREG_RW rreg_reply;
                    rreg_reply.mem_address = rreg_req.mem_address;
                    rreg_reply.mem_register = rreg_req.mem_register;
                    rreg_reply.data_width = rreg_req.data_width;
                    rreg_reply.the_data.push_back(rreg_r_result);
                    rreg_read_reply.set_payload<RAWREG_RW>(rreg_reply);
                    std::cout<< rreg_reply.print() << std::endl;
                    std::cout<< "data length: " << std::to_string(rreg_read_reply.data_length_) << std::endl;
                            //encode the fem2controlmsg reply 
                    encoded_reply = encoder.encode(rreg_read_reply);

                }
            }  // raw-reg

        } // read-write-config







    /*
        else{    
            //encode the fem2controlmsg reply 
            encoded_reply = encoder.encode(decoded_request);
        }
    */
        // send the encoded reply via zmq for comparison
        send(encoded_reply);
    } //for loop
}