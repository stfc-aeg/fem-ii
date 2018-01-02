#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>
#include "MsgPackEncoder.hpp"

using namespace Femii;

zmq::context_t context(1);
zmq::socket_t socket_(context, ZMQ_REP);

/*  
//
//  Helper methods for testing purposes.
//
*/

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

//  checker method to check sending of encoded messages is OK for a socket to send.
void send(const std::string& message_str)
{
  size_t msg_size = message_str.size();
  zmq::message_t msg(msg_size);
  memcpy(msg.data(), message_str.data(), msg_size);
  socket_.send(msg);
}

// checker method to see how a zmq socket would receive the messages.
std::string receive(){

    zmq::message_t request;
    socket_.recv (&request);
    //  Dump the message as text or binary
    int size = request.size();
    std::string request_string(static_cast<char*>(request.data()), size);

    return request_string;

}

/* OLD CODE
std::string encode_msg(Fem2ControlMsg the_msg){

    std::stringstream ss;
    msgpack::pack(&ss, the_msg);
    return ss.str();
}

Fem2ControlMsg decode_msg(std::string the_msg){

    msgpack::unpacked unp;
    msgpack::unpack(unp, the_msg.data(), the_msg.size());
    msgpack::object obj = unp.get();

    std::cout << "---decoded---" << std::endl;
    std::cout << obj << std::endl;
    std::cout << "---decoded---" << std::endl;

    Fem2ControlMsg decoded = obj.as<Fem2ControlMsg>();

    return decoded;
}

*/

int main(){


    socket_.bind("tcp://*:5555");
    printf("server Booted \n");

    MsgPackEncoder encoder;

    // receive request
    std::string encoded_request = receive();

    //print request
    print_as_python_bytes(encoded_request);

    //  decode the message into a fem2controlmsg using encoder
    Fem2ControlMsg decoded_request = encoder.decode(encoded_request);

    //encode the fem2controlmsg reply 
    std::string encoded_reply = encoder.encode(decoded_request);

    // send the encoded reply via zmq.
    send(encoded_reply);
   
   /* old code

    //decode 
    Fem2ControlMsg decoded_request = decode_msg(encoded_request);

    // send same decoded reply back for comparison
    send(encode_msg(decoded_request));
    */


}


