#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>

using namespace Femii;

zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REP);

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
  socket.send(msg);
}

// checker method to see how a zmq socket would receive the messages.
std::string receive(){

    zmq::message_t request;
    socket.recv (&request);
    //  Dump the message as text or binary
    int size = request.size();
    std::string request_string(static_cast<char*>(request.data()), size);

    return request_string;

}

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



int main(){


    socket.bind("tcp://*:5555");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.

    //request.set_access_type(Fem2ControlMsg::ACCESS_I2C);
    //request.set_cmd_type(Fem2ControlMsg::CMD_READ);
    boost::posix_time::ptime posix_time = request.get_posix_timestamp();
    std::cout << posix_time << std::endl ;
    std::string string_time = request.get_string_timestamp();
    std::cout << string_time << std::endl ;

    send(encode_msg(request));
    // or
    std::string encoded_req = encode_msg(request);
    send(encoded_req);  //dummy method.

    
    // not a real method.
    //receive(decode_msg(encoded_req));
    //Fem2ControlMsg decoded_req = decode_msg(receive());

    Fem2ControlMsg decoded_req = decode_msg(encoded_req);

    assert(request.get_access_type() == request.get_access_type());
    assert(request.get_ack_state() == request.get_ack_state());
    assert(request.get_cmd_type() == request.get_cmd_type());
    assert(request.get_req_id() == request.get_req_id());
    assert(request.get_posix_timestamp() == request.get_posix_timestamp());
    assert(request.get_retries() == request.get_retries());
    assert(request.get_timeout() == request.get_timeout());


    

    /* old broken code ---- 


    Fem2ControlMsg encoded; // default control message.
    std::cout << " --------------------------- " << std::endl;
    std::cout << encoded.get_access_type() << std::endl;
    std::cout << encoded.get_ack_state() << std::endl;
    std::cout << encoded.get_cmd_type() << std::endl;
    std::cout << encoded.get_req_id() << std::endl;
    std::cout << encoded.get_timestamp() << std::endl;
    std::cout << " --------------------------- " << std::endl;

    // this encodes the internal header structure.
    std::string header = encoded.encode();
    std::cout << "--ENCODED--HEADER--" << std::endl;
    print_as_python_bytes(header);
    std::cout << "--ENCODED--HEADER--" << std::endl;
    
    // this is packing the entire class.
    std::stringstream ss;
    msgpack::pack(&ss, encoded);

    //print(ss.str());
    print_as_python_bytes(ss.str());

    msgpack::unpacked unp;
    msgpack::unpack(unp, ss.str().data(), ss.str().size());
    msgpack::object obj = unp.get();
    std::cout << obj << std::endl;

    Fem2ControlMsg decoded = obj.as<Fem2ControlMsg>();

    std::cout << " --------------------------- " << std::endl;
    std::cout << decoded.get_access_type() << std::endl;
    std::cout << decoded.get_ack_state() << std::endl;
    std::cout << decoded.get_cmd_type() << std::endl;
    std::cout << decoded.get_req_id() << std::endl;
    std::cout << decoded.get_timestamp() << std::endl;
    std::cout << " --------------------------- " << std::endl;

    //encode as a string
    std::string encoded = test_msg.encode();
    send(encoded);
    
    //std::cout << ss << std::endl;
    //print_as_python_bytes(encoded);
    
    msgpack::unpacked unp;
    msgpack::unpack(unp, encoded.data(), encoded.size());
    msgpack::object obj = unp.get();

    std::cout << "------" << std::endl;
    std::cout << obj << std::endl;
    
    try{

        Fem2ControlMsg decoded = obj.as<Fem2ControlMsg>();
    }
    catch(...){

        std::cout << "Could not convert to Fem2ControlMsg" << std::endl;

    };

    Fem2ControlMsg decoded = obj.as<Fem2ControlMsg>();

    std::cout << "------" << std::endl;//test
    //hello
    //std::cout << decoded.get_timestamp();
    
    */


    
}


