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
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    //--------------------payload setting testing--------------------//

    //-------- set with vector<u8> -----//
    std::vector<u8> us_vect_chars;
    us_vect_chars.push_back('c');
    us_vect_chars.push_back('h');
    us_vect_chars.push_back('g');
    request.set_payload<std::vector<u8> >(us_vect_chars); // vector char is ok but vector<u8> not.
    
    //-------- set with vector<int> -----//
    std::vector<int> vect_int;
    vect_int.push_back(0);
    vect_int.push_back(1);
    vect_int.push_back(2);
    request.set_payload<std::vector<int> >(vect_int); // overloaded specialisation
    
    //------- set with I2C READ -------//
    std::vector<uint8_t> this_data;
    
    for(uint8_t i =0; i < 20; i++){
        this_data.push_back(i);
    }
    
    I2C_READ the_read;
    the_read.i2c_bus = 1;
    the_read.slave_address = 2;
    the_read.i2c_register = 3;
    the_read.data_width = WIDTH_BYTE;
    the_read.the_data = this_data;
    request.set_payload<I2C_READ>(the_read);

    // ----- check the type of the payload --//
    printf("payload is a %s \n", request.get_payload_type().c_str());

    // print the request as a string
    std::cout << request;

    //---------------- sending and receiving zmq ------------------------//

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

    //---------------payload testing-------------------------//

    printf("payload is a %s \n", reply.get_payload_type().c_str());

    I2C_READ the_read_back = reply.get_payload<I2C_READ>();
    
    //assert encoded/decoded round trip msgs are the same thing
    assert(request == reply);
    assert(the_read == the_read_back);
    assert(the_read_back.the_data.size() == reply.data_length_);
    std::cout << "MATCH" << std::endl;

    //print the fem2controlmsg as a string.
    std::cout << reply;
}
