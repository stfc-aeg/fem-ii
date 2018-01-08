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

    //  vector of ints - not natively supported by msgpack
    std::vector<int> vect_int;
    vect_int.push_back(0);
    vect_int.push_back(1);
    vect_int.push_back(2);

    //natively supported vector of chars
    std::vector<char> vect_chars;
    vect_chars.push_back('c');
    vect_chars.push_back('h');
    vect_chars.push_back('g');

    // natively supported types
    int an_int = 1;
    char a_char = 'a';
    bool a_bool = true;
    std::string a_string = "string";

    int test_array[200];

    for(int i =0; i < 200; i++){
        test_array[i] = i;
    }
    // flat vector of variant types natively supported
    std::vector<msgpack::type::variant> flat_variant_vect;
    flat_variant_vect.push_back(0x05);
    flat_variant_vect.push_back("test");
    // still don't know what happens when I nest a vector<int>

    //nested vector of variant types. ok with vect_char, not ok with vect_int
    std::vector<msgpack::type::variant> nested_variant_vector;
    nested_variant_vector.push_back(vect_chars);    //OK
    nested_variant_vector.push_back(test_array);    //normal int arrays are fine
    //nested_variant_vector.push_back(vect_int);      //NOT OK
    
    //set the payload with the flat vector
    request.set_payload(flat_variant_vect);
    int append_int = 32;
    printf("payload is a %s \n", request.get_payload_type().c_str());
    //append the integer onto the end of the payload. all ok. 
    request.append_payload(append_int);

    std::vector<msgpack::type::variant> i2cRead;
    i2cRead.push_back(0x01);
    i2cRead.push_back(0x02);
    i2cRead.push_back(0x03);
    i2cRead.push_back(WIDTH_BYTE);

    request.set_payload(i2cRead);

    I2C_READ testread = request.get_payload<I2C_READ>();
    printf("I2C bus : %s \n", std::to_string(testread.i2c_bus).c_str());
    printf("I2C slave address : %s \n", std::to_string(testread.slave_address).c_str());
    printf("I2C register : %s \n", std::to_string(testread.i2c_register).c_str());
    printf("I2C data width : %s \n", std::to_string(testread.data_width).c_str());


    //request.set_payload(vect_chars); // char set
    //request.set_payload<std::vector<int> >(vect_int); // overloaded specialisation
    
    //----------------------------------------//

    //Fem2ControlMsg request; //default message

    //  check the timestamps have been encoded ok
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

    //---------------payload type testing-------------------------//
    printf("payload is a %s \n", reply.get_payload_type().c_str());

    //----------------------------------------//



    //assert encoded/decoded round trip msgs are the same thing
    assert(request == reply);
    std::cout << "MATCH" << std::endl;

    //print the fem2controlmsg as a string.
    std::cout << reply;
}


