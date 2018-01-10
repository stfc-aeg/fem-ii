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

    //-------- set with vector<char> -----//
    std::vector<char> vect_chars;
    vect_chars.push_back('c');
    vect_chars.push_back('h');
    vect_chars.push_back('g');
    request.set_payload(vect_chars); // vector char is ok but vector<u8> not.



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
    
    unsigned char the_char = 20;

    request.set_payload(the_char);

    //------- set with vector<unint64_t> ---//

    std::vector<uint64_t> int64_vect;
    int64_vect.push_back(1234);

    //request.set_payload(int64_vect);

    //-------- set with vector<variant> and append to -----//
    std::vector<msgpack::type::variant> flat_variant_vect;
    flat_variant_vect.push_back(0x05);
    flat_variant_vect.push_back("test");
    request.set_payload(flat_variant_vect);
    int append_int = 32;
    request.append_payload(append_int);  //append the integer onto the end of the payload. all ok. 

    uint8_t test8 = 1;
    request.append_payload(test8);
    //-------- set with nested vector<variant> ------//

    int test_array[200];
    for(int i =0; i < 200; i++){
        test_array[i] = i;
    }
    std::vector<msgpack::type::variant> nested_variant_vector;
    nested_variant_vector.push_back(vect_chars);    //OK
    nested_variant_vector.push_back(test_array);    //normal int arrays are fine
    
    

    //-------- set with vector<variant> as I2C READ -----//
    std::vector<msgpack::type::variant> i2cRead;
    i2cRead.push_back(1);
    i2cRead.push_back(2);
    i2cRead.push_back(3);
    int the_width = WIDTH_BYTE;
    uint8_t the_data[20];

    for(uint8_t i =0; i < 20; i++){
        the_data[i] = i;
    }

    i2cRead.push_back(the_width); // problem if direct as ENUM
    i2cRead.push_back(the_data);
    request.set_payload(i2cRead);

    I2C_READ the_read;
    the_read.i2c_bus = 1;
    the_read.slave_address = 2;
    the_read.i2c_register = 3;
    the_read.data_width = WIDTH_BYTE;

    request.set_payload<I2C_READ>(the_read);

    // ----- check the type of the payload --//
    printf("payload is a %s \n", request.get_payload_type().c_str());

    /*

    //checking the variant payload is correctly generated.
    I2C_READ testread = request.get_payload<I2C_READ>();
    printf("I2C bus : %s \n", std::to_string(testread.i2c_bus).c_str());
    printf("I2C slave address : %s \n", std::to_string(testread.slave_address).c_str());
    printf("I2C register : %s \n", std::to_string(testread.i2c_register).c_str());
    printf("I2C data width : %s \n", std::to_string(testread.data_width).c_str());
    printf("I2C the data  : \n");

    
    for(int i =0; i < testread.the_data.size(); i++){

        std::cout << std::to_string(testread.the_data.at(i));
        std::cout << " ";
    }
    */

    //---------------- sending and receiving zmq ------------------------//

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

    //---------------payload testing-------------------------//

    printf("payload is a %s \n", reply.get_payload_type().c_str());

    //----------------------------------------//

    //assert encoded/decoded round trip msgs are the same thing
    assert(request == reply);
    assert(the_read == the_read_back);
    std::cout << "MATCH" << std::endl;

    //print the fem2controlmsg (header only) as a string.
    std::cout << reply;
}


