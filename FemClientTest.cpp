#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>
#include "MsgPackEncoder.hpp"

using namespace Femii;

zmq::context_t context(1);
zmq::socket_t socket_(context, ZMQ_REQ);

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
const std::string encode_msg(Fem2ControlMsg the_msg){

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


    socket_.connect("tcp://localhost:5555");
    printf("Client Booted \n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.

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
  
    //assert they are the same thing
    assert(request.get_access_type() == reply.get_access_type());
    assert(request.get_ack_state() == reply.get_ack_state());
    assert(request.get_cmd_type() == reply.get_cmd_type());
    assert(request.get_req_id() == reply.get_req_id());
    assert(request.get_posix_timestamp() == reply.get_posix_timestamp());
    assert(request.get_retries() == reply.get_retries());
    assert(request.get_timeout() == reply.get_timeout());
    
    std::cout << "MATCH" << std::endl;









    /*OLD CODE

    //send the message
    send(encode_msg(request));

    //receive request
    std::string encoded_reply = receive();

    // decode the response
    Fem2ControlMsg reply = decode_msg(encoded_reply);
  
    //print
    print_as_python_bytes(encoded_reply);

    //assert they are the same thing
    assert(request.get_access_type() == reply.get_access_type());
    assert(request.get_ack_state() == reply.get_ack_state());
    assert(request.get_cmd_type() == reply.get_cmd_type());
    assert(request.get_req_id() == reply.get_req_id());
    assert(request.get_posix_timestamp() == reply.get_posix_timestamp());
    assert(request.get_retries() == reply.get_retries());
    assert(request.get_timeout() == reply.get_timeout());
    
    std::cout << "Assertions worked" << std::endl;
    */

}


