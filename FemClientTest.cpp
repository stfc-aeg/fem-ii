#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>
#include "MsgPackEncoder.hpp"
/*
Mock Fem Client - uses ZMQ and MsgPack encoding to send and receive a single
Fem2ControlMsg in a req-reply synchronous loop.

Generates a Fem2ControlMsg, sends the encoded Fem2ControlMsg and receives..
"the identical" encoded Fem2ControlMsg response. 
Asserts that the message is returned in tact and functional.
*/
using namespace Femii;

//  create a zmq context and request socket
zmq::context_t context(1);
zmq::socket_t socket_(context, ZMQ_REQ);

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
std::string receive(){

    zmq::message_t request;
    socket_.recv (&request);
    //  Dump the message as text or binary
    int size = request.size();
    std::string request_string(static_cast<char*>(request.data()), size);

    return request_string;
}

int main(){

    socket_.connect("tcp://localhost:5555");
    printf("Client Booted \n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.

    //Fem2ControlMsg request; //default message
    std::cout << request.get_posix_timestamp() << std::endl;
    std::cout << request.get_string_timestamp() << std::endl;

    //  create a msgpack encoder
    MsgPackEncoder encoder;    

    //  encode the fem2controlmsg as a string (byte string)
    std::string encoded_request = encoder.encode(request);

     //print encoded request 
    print_as_python_bytes(encoded_request);

    //  send the encoded request via zmq
    send(encoded_request);

    //receive reply from server via zmq
    std::string encoded_reply = receive();

    // decode the response using the encoder
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
  
    //assert encoded/decoded round trip msgs are the same thing
    assert(request == reply);
    std::cout << "MATCH" << std::endl;

    //print the fem2controlmsg as a string.
    std::cout << reply;
}


