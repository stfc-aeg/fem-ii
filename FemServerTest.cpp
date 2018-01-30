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
    


    for(int x = 0; x < 7; x++){

        // receive request
        std::string encoded_request = receive();
        //print request
        print_as_python_bytes(encoded_request);

        //  decode the message into a fem2controlmsg using encoder
        Fem2ControlMsg decoded_request = encoder.decode(encoded_request);


        if(decoded_request.get_access_type() == Femii::Fem2ControlMsg::ACCESS_DDR){
            
            DDR_RW ddr_req = decoded_request.get_payload<DDR_RW>();
            mem_reader memory_reader(ddr_req.mem_address, ddr_req.offset, ddr_req.data_width);

            if(decoded_request.get_cmd_type() == Femii::Fem2ControlMsg::CMD_READ){
            
                memory_reader.init_mmap();
                unsigned long result = memory_reader.read_mem();
                std::cout << "mem result: " << std::to_string(result) << std::endl;
                memory_reader.unmap();

            }
        }

        std::cout << decoded_request.get_posix_timestamp() << std::endl;
        std::cout << decoded_request.get_string_timestamp() << std::endl;

        //encode the fem2controlmsg reply 
        std::string encoded_reply = encoder.encode(decoded_request);

        // send the encoded reply via zmq for comparison
        send(encoded_reply);
    }
}