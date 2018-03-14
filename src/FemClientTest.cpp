#include <iostream> 
#include <zmq.hpp>
#include <typeinfo>
#include "../include/Fem2Exception.h"
#include "../include/Fem2ControlMsg.h"
#include <cassert>
#include "../include/MsgPackEncoder.h"
//#include "data_helpers.hpp"


#define DDR3_BASE 0x80000000;
#define GPIO_STATUS 0x41210000;
#define GPIO_DDR3_PAGE 0xA0010000;
#define BRAM_CONFIG 0xBFFFF000;
#define XADC_STATUS 0x43C00000;
#define QSPI_BASE 0xA0030000;
#define LED_REGISTER 0xA0020008;
#define I2C_EEPROM 0x5C;
#define I2C_RTC 0x57;


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
MsgPackEncoder encoder; 

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

/*
/   Tests Round trip GPIO encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_GPIO, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_gpio_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting GPIO Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    GPIO_RW the_gpio; 
    the_gpio.mem_address = 1234;
    the_gpio.mem_register = 12;
    the_gpio.data_width = WIDTH_BYTE;
    the_gpio.the_data = the_data;
    request.set_payload<GPIO_RW>(the_gpio);

    printf("GPIO Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    GPIO_RW the_gpio_back = reply.get_payload<GPIO_RW>();
    
    printf("GPIO Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_gpio == the_gpio_back);
    // double check the vector size + data length fields are the same
    assert(the_gpio_back.the_data.size() == request.get_data_length());
    std::cout << "GPIO MATCH" << std::endl;
}

/*
/   Tests Round trip XADC encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_XADC, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_xadc_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting XADC Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    XADC_RW the_xadc; 
    the_xadc.mem_address = 1234;
    the_xadc.mem_register = 12;
    the_xadc.data_width = WIDTH_BYTE;
    the_xadc.the_data = the_data;
    request.set_payload<XADC_RW>(the_xadc);

    printf("XADC Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    XADC_RW the_xadc_back = reply.get_payload<XADC_RW>();

    printf("XADC Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_xadc == the_xadc_back);
    // double check the vector size + data length fields are the same
    assert(the_xadc_back.the_data.size() == request.get_data_length());
    std::cout << "XADC MATCH" << std::endl;
    
}

/*
/   Tests Round trip RARWREG encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_RAWREG, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_rawreg_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting RAWREG Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    RAWREG_RW the_rreg; 
    the_rreg.mem_address = 1234;
    the_rreg.mem_register = 12;
    the_rreg.data_width = WIDTH_BYTE;
    the_rreg.the_data = the_data;
    request.set_payload<RAWREG_RW>(the_rreg);

    printf("RAWREG Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    RAWREG_RW the_rreg_back = reply.get_payload<RAWREG_RW>();

    printf("RAWREG Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_rreg == the_rreg_back);
    // double check the vector size + data length fields are the same
    assert(the_rreg_back.the_data.size() == request.get_data_length());
    std::cout << "RAWREG MATCH" << std::endl;
    
    
}

/*
/   Tests Round trip I2C encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_I2C, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_i2c_encoding(std::vector<uint8_t> const& the_data){


    printf("---------------------------\nTesting I2C Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    I2C_RW the_i2c;
    the_i2c.i2c_bus = 1;
    the_i2c.slave_address = 2;
    the_i2c.i2c_register = 3;
    the_i2c.data_width = WIDTH_BYTE;
    the_i2c.the_data = the_data;
    request.set_payload<I2C_RW>(the_i2c);

    printf("I2C Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);

    I2C_RW the_i2c_back = reply.get_payload<I2C_RW>();
    printf("I2C Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_i2c == the_i2c_back);
    // double check the vector size + data length fields are the same
    assert(the_i2c_back.the_data.size() == request.get_data_length());
    std::cout << "I2C MATCH" << std::endl;
    
}

/*
/   Tests Round trip QDR encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_QDR, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_qdr_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting QDR Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_QDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    QDR_RW the_qdr;
    the_qdr.mem_address = 1;
    the_qdr.page = 2;
    the_qdr.offset = 3;
    the_qdr.data_width = WIDTH_BYTE;
    the_qdr.the_data = the_data;
    request.set_payload<QDR_RW>(the_qdr);

    printf("QDR Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);



    QDR_RW the_qdr_back = reply.get_payload<QDR_RW>();

    printf("QDR Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_qdr == the_qdr_back);
    // double check the vector size + data length fields are the same
    assert(the_qdr_back.the_data.size() == request.get_data_length());
    std::cout << "QDR MATCH" << std::endl;
    
}

/*
/   Tests Round trip DDR encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_DDR, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_ddr_encoding(std::vector<uint8_t> const& the_data){


    printf("---------------------------\nTesting DDR Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    DDR_RW the_ddr;
    the_ddr.mem_address = 1; //DDR base address
    the_ddr.page = 2;
    the_ddr.offset = 3;        //DDR offset address
    the_ddr.data_width = WIDTH_BYTE;
    the_ddr.the_data = the_data;
    request.set_payload<DDR_RW>(the_ddr);

    printf("DDR Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    DDR_RW the_ddr_back = reply.get_payload<DDR_RW>();

    printf("DDR Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_ddr == the_ddr_back);
    // double check the vector size + data length fields are the same
    assert(the_ddr_back.the_data.size() == request.get_data_length());
    std::cout << "DDR MATCH" << std::endl;
    
}

/*
/   Tests Round trip QSPI encoding of a Fem2ControlMsg
/   Sends an encoded ACCESS_QSPI, CMD_NOTIFY message
/   Returns the same message after decoding and encoding server side
/   Asserts equality between sent and receive 
*/
void test_qspi_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting QSPI Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_NOTIFY, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    QSPI_RW the_qspi;
    the_qspi.mem_address = 1;
    the_qspi.page = 2;
    the_qspi.offset = 3;
    the_qspi.data_width = WIDTH_BYTE;
    the_qspi.the_data = the_data;
    request.set_payload<QSPI_RW>(the_qspi);

    printf("QSPI Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    QSPI_RW the_qspi_back = reply.get_payload<QSPI_RW>();

    printf("QSPI Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_qspi == the_qspi_back);
    // double check the vector size + data length fields are the same
    assert(the_qspi_back.the_data.size() == request.get_data_length());
    std::cout << "QSPI MATCH" << std::endl;
  
}


/*
/   Tests WRITE READ Access of GPIO on FEM-II
/   Sends an encoded ACCESS_GPIO, CMD_WRITE message to GPIO_DDR3_PAGE address
/   Changing DDR3 Page to 5.
/   Returns the WRITE result.
/   Sends an encoded ACCESS_GPIO, CMD_READ message to GPIO_DDR3_PAGE address
/   Returns the READ result.
/   Asserts equality between sent WRITE and received READ
*/
void test_gpio_write_read(){

    printf("---------------------------\nTesting GPIO Write - Read Trip...\n");
    printf("---------------------------\nWriting...\n");

    //initialise a control msg with values.
    Fem2ControlMsg write_request(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    GPIO_RW the_gpio_write; 
    the_gpio_write.mem_address = GPIO_DDR3_PAGE;
    the_gpio_write.mem_register = 0;
    the_gpio_write.data_width = WIDTH_BYTE;
    the_gpio_write.the_data.push_back(0x05);
    write_request.set_payload<GPIO_RW>(the_gpio_write);

    printf("GPIO Write Request: \n");
    std::cout << write_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_write_request;
    encoder.encode(write_request, encoded_write_request);
    send(encoded_write_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_write_reply = receive();
    Fem2ControlMsg write_reply;
    encoder.decode(encoded_write_reply, write_reply);




    GPIO_RW the_gpio_write_back = write_reply.get_payload<GPIO_RW>();

    printf("GPIO WRITE Reply: \n");
    std::cout << write_reply;

    printf("---------------------------\nReading...\n");

    //initialise a control msg with values.
    Fem2ControlMsg read_request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    GPIO_RW the_gpio_read; 
    the_gpio_read.mem_address = GPIO_DDR3_PAGE;
    the_gpio_read.mem_register = 0;
    the_gpio_read.data_width = WIDTH_BYTE;
    read_request.set_payload<GPIO_RW>(the_gpio_read, 1); // read 1 byte.

    printf("GPIO Request: \n");
    std::cout << read_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(read_request, encoded_request);

    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg read_reply;
    encoder.decode(encoded_reply, read_reply);


    GPIO_RW the_gpio_read_back = read_reply.get_payload<GPIO_RW>();
    printf("GPIO READ Reply: \n");
    std::cout << read_reply;


    assert(the_gpio_write == the_gpio_read_back);    
 
    // double check the vector size + data length fields are the same
    assert(the_gpio_write.the_data.size() == read_reply.get_data_length());
    std::cout << "GPIO WRITE READ MATCH" << std::endl;

}

/*
/   Tests WRITE READ Access of DDR on FEM-II
/   Sends an encoded ACCESS_DDR, CMD_WRITE message to DDR3_BASE, OFFSET 1..
/   Page 3.
/   Server side changes DDR page through GPIO
/   Returns the WRITE result.
/   Sends an encoded ACCESS_DDR, CMD_READ message to DDR3_BASE, OFFSET 1..
/   Page 3.
/   Server side changes DDR page through GPIO
/   Returns the READ result.
/   Asserts equality between sent WRITE and received READ
*/
void test_ddr_write_read(){


    printf("---------------------------\nTesting DDR Write - Read Trip...\n");
    printf("---------------------------\nWriting...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg write_request(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    DDR_RW the_ddr_write;
    the_ddr_write.mem_address = DDR3_BASE; //DDR base address
    the_ddr_write.page = 3;
    the_ddr_write.offset = 0x00000000;
    
    the_ddr_write.the_data.push_back(0xAA);
    the_ddr_write.the_data.push_back(0xBB);
    the_ddr_write.the_data.push_back(0xCC); 
    the_ddr_write.the_data.push_back(0xDD);
   

    the_ddr_write.data_width = WIDTH_LONG;
    write_request.set_payload<DDR_RW>(the_ddr_write); // at write - we initiate size with size of vector. 

    printf("DDR Write Request: \n");
    std::cout << write_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_write_request;
    encoder.encode(write_request, encoded_write_request);
    send(encoded_write_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_write_reply = receive();
    Fem2ControlMsg write_reply;
    encoder.decode(encoded_write_reply, write_reply);


    DDR_RW the_ddr_write_back = write_reply.get_payload<DDR_RW>();

    printf("DDR Reply: \n");
    std::cout << write_reply;
    printf("---------------------------\nReading...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg read_request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    DDR_RW the_ddr_read;
    the_ddr_read.mem_address = DDR3_BASE; //DDR base address
    the_ddr_read.page = 3;
    the_ddr_read.offset = 0x00000000;        //DDR offset address
    the_ddr_read.data_width = WIDTH_LONG;
    int read_length = 1; //read 1 long
    read_request.set_payload<DDR_RW>(the_ddr_read, read_length);

    printf("DDR Read Request: \n");
    std::cout << read_request << "datalength :" << read_request.get_data_length() << "readlength :" << read_request.get_read_length() << std::endl;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_read_request;
    encoder.encode(read_request, encoded_read_request);
    send(encoded_read_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_read_reply = receive();
    Fem2ControlMsg read_reply;
    encoder.decode(encoded_read_reply, read_reply);


    DDR_RW the_ddr_read_back = read_reply.get_payload<DDR_RW>();
    printf("DDR Read Reply: \n");
    std::cout << read_reply;

    assert(the_ddr_write == the_ddr_read_back);    
 
    // double check the vector size + data length fields are the same
    assert(the_ddr_write.the_data.size() == read_reply.get_data_length());
    std::cout << "DDR WRITE READ MATCH" << std::endl;
}

/*
/   Tests WRITE READ Access of GPIO on FEM-II
/   Sends an encoded ACCESS_GPIO, CMD_WRITE message to GPIO_DDR3_PAGE address
/   Changing DDR3 Page to 5.
/   Returns the WRITE result.
/   Sends an encoded ACCESS_GPIO, CMD_READ message to GPIO_DDR3_PAGE address
/   Returns the READ result.
/   Asserts equality between sent WRITE and received READ
*/
void test_rawreg_write_read(){

    printf("---------------------------\nTesting RAW_REG Write - Read Trip...\n");
    printf("---------------------------\nWriting...\n");

    //initialise a control msg with values.
    Fem2ControlMsg write_request(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    RAWREG_RW the_rreg_write; 
    the_rreg_write.mem_address = BRAM_CONFIG;
    the_rreg_write.mem_register = 0;
    the_rreg_write.data_width = WIDTH_WORD;
    the_rreg_write.the_data.push_back(0x01);
    the_rreg_write.the_data.push_back(0x02);
    write_request.set_payload<RAWREG_RW>(the_rreg_write);

    printf("RAWREG Write Request: \n");
    std::cout << write_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_write_request;
    encoder.encode(write_request, encoded_write_request);
    send(encoded_write_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_write_reply = receive();
    Fem2ControlMsg write_reply;
    encoder.decode(encoded_write_reply, write_reply);


    RAWREG_RW the_rreg_write_back = write_reply.get_payload<RAWREG_RW>();

    printf("RAWREG WRITE Reply: \n");
    std::cout << write_reply;
    printf("---------------------------\nReading...\n");

    //initialise a control msg with values.
    Fem2ControlMsg read_request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    RAWREG_RW the_rreg_read; 
    the_rreg_read.mem_address = BRAM_CONFIG;
    the_rreg_read.mem_register = 0;
    the_rreg_read.data_width = WIDTH_WORD;
    read_request.set_payload<RAWREG_RW>(the_rreg_read, 1);

    printf("RAWREG Request: \n");
    std::cout << read_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(read_request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg read_reply;
    encoder.decode(encoded_reply, read_reply);

    RAWREG_RW the_rreg_read_back = read_reply.get_payload<RAWREG_RW>();

    printf("RAWREG READ Reply: \n");
    std::cout << read_reply;


    assert(the_rreg_write == the_rreg_read_back);    
 
    // double check the vector size + data length fields are the same
    assert(the_rreg_write.the_data.size() == read_reply.get_data_length());
    std::cout << "RAWREG WRITE READ MATCH" << std::endl;

}

void test_xadc_read(){

    printf("---------------------------\nTesting XADC Read Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    XADC_RW the_xadc; 
    the_xadc.mem_address = XADC_STATUS;
    the_xadc.mem_register = 0x200;
    the_xadc.data_width = WIDTH_WORD;//??
    request.set_payload<XADC_RW>(the_xadc, 1);

    printf("XADC Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);

    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    XADC_RW the_xadc_back = reply.get_payload<XADC_RW>();

    printf("XADC Reply: \n");
    std::cout << reply;

    uint32_t temp = form_words_longs<XADC_RW>(the_xadc_back, 0);
    printf("Temp pre shift : 0x%.2X \n", temp);
    temp = temp >> 4;
    temp = ((temp * 503.975)/ 4096) - 273.15;

    printf("FEM-II Temperature: %d Degrees C.\n", temp);

    //assert encoded/decoded round trip msgs and payloads are the same thing
    //assert(request == reply);
    //assert(the_xadc == the_xadc_back);
    // double check the vector size + data length fields are the same
    //assert(the_xadc_back.the_data.size() == request.data_length_);
    std::cout << "XADC MATCH" << std::endl;
}

void test_qspi_read(){

    printf("---------------------------\nTesting QSPI Read Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    QSPI_RW the_qspi; 
    the_qspi.mem_address = QSPI_BASE;
    the_qspi.offset = 0x64;
    the_qspi.data_width = WIDTH_BYTE;//??
    int read_length = 1; // the number of bytes to read.
    request.set_payload<QSPI_RW>(the_qspi, read_length);

    printf("QSPI Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    QSPI_RW the_qspi_back = reply.get_payload<QSPI_RW>();

    printf("QSPI Reply: \n");
    std::cout << reply;
    uint32_t result = form_words_longs<QSPI_RW>(the_qspi_back, 0); // this is just taking the first byte.
   
    assert(result == 0x000000A5);
    assert(request.get_read_length() == reply.get_data_length());
    std::cout << "QSPI READ MATCH" << std::endl;
}

void test_i2c_read(){

    printf("---------------------------\nTesting I2C Read Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    I2C_RW the_i2c; 
    the_i2c.i2c_bus = 0;
    the_i2c.slave_address = I2C_EEPROM;
    the_i2c.i2c_register = 0x10;
    the_i2c.data_width = WIDTH_BYTE;//??
    int read_length = 6; // read 6 bytes from the EEPROM i.e the MAC ADDRESS
    request.set_payload<I2C_RW>(the_i2c, read_length);

    printf("I2C Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);


    I2C_RW the_i2c_back = reply.get_payload<I2C_RW>();
    printf("I2C Reply: \n");
    std::cout << reply;

    assert(request.get_read_length() == reply.get_data_length());

}

void led_control(bool on_off){

    std::string on_off_string;
    if(on_off){
        on_off_string = "on";
    }
    else{
        on_off_string = "off";
    }
    printf("---------------------------\nTurning Both LED's %s...\n", on_off_string.c_str());

    //initialise a control msg with values.
    Fem2ControlMsg write_request(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    GPIO_RW the_gpio_write; 
    the_gpio_write.mem_address = LED_REGISTER;
    the_gpio_write.mem_register = 0;
    the_gpio_write.data_width = WIDTH_BYTE;

    if(on_off){
        the_gpio_write.the_data.push_back(0x03);
    }
    else{
        the_gpio_write.the_data.push_back(0x00);
    }
 
    write_request.set_payload<GPIO_RW>(the_gpio_write);

    printf("GPIO Write Request: \n");
    std::cout << write_request;

    //  encode the fem2controlmsg as a string (byte string) and send    
    std::string encoded_write_request;
    encoder.encode(write_request, encoded_write_request);
    send(encoded_write_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_write_reply = receive();

    //Fem2ControlMsg write_reply = encoder.decode(encoded_write_reply);
    Fem2ControlMsg write_reply;
    encoder.decode(encoded_write_reply, write_reply);


    printf("GPIO WRITE Reply: \n");
    std::cout << write_reply;

    GPIO_RW the_gpio_write_back = write_reply.get_payload<GPIO_RW>();

}

void test_init_config(){

    Fem2ControlMsg request(Fem2ControlMsg::CMD_CONFIGURE, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    I2C_CONFIG i2c_config; 
    i2c_config.i2c_bus = 0x0;
    i2c_config.i2c_register = 0xA;
    i2c_config.slave_address = 0x5C;
    i2c_config.data_width = WIDTH_BYTE;
    i2c_config.unsigned_int_param = 123456;
    i2c_config.signed_int_param = -1;
    i2c_config.float_param = 0.05;
    i2c_config.string_param = "hello";
    i2c_config.char_param = 'c';
    request.set_payload<I2C_CONFIG>(i2c_config);
    std::cout << "Configuration Pre-Encoding:\n" << request << std::endl;
    I2C_CONFIG config_back = request.get_payload<I2C_CONFIG>();
    Fem2ControlMsg reply(Fem2ControlMsg::CMD_CONFIGURE, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    reply.set_payload<I2C_CONFIG>(config_back);

    std::cout << "Configuration Decoded:\n" << reply;
    //assert(request == reply);
    assert(i2c_config == config_back);
}


void test_empty_payload(){

    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    try{
        I2C_RW config_back = request.get_payload<I2C_RW>();
    }
    catch(Fem2ControlMsgException e){
        std::cout << e.what() << std::endl;
    }

}


void test_wrong_payload(){

    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    I2C_RW the_i2c;
    the_i2c.i2c_bus = 1;
    the_i2c.slave_address = 2;
    the_i2c.i2c_register = 3;
    the_i2c.data_width = WIDTH_BYTE;
    try{
        request.set_payload<I2C_RW>(the_i2c);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set I2C" + error <<std::endl;
    }   
 
    GPIO_RW gread;
    try{
        request.set_payload<GPIO_RW>(gread);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set GPIO" + error <<std::endl;
    }  
    XADC_RW xread;
    try{
        request.set_payload<XADC_RW>(xread);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set XADC" + error <<std::endl;
    }  

    RAWREG_RW rread;
     try{
        request.set_payload<RAWREG_RW>(rread);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set XADC" + error <<std::endl;
    } 

    DDR_RW ddrread;
    try{
        request.set_payload<DDR_RW>(ddrread);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set DDR" + error <<std::endl;
    }

    QDR_RW qdrread;
    try{
        request.set_payload<QDR_RW>(qdrread);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set QDR" + error <<std::endl;
    }

    QSPI_RW qspiread;
    try{
        request.set_payload<QSPI_RW>(qspiread);
    }
    catch(Fem2ControlMsgException e){
        std::string error = e.what();
        std::cout << "Threw @ set QSPI" + error <<std::endl;
    }
}

void test_femii_config(){

    FEMII_CONFIG fem_config;
    fem_config.set_param("param1", 1234);

    try {
        fem_config.set_param("param1", 1234);
    }
    catch(Fem2ControlMsgException e){
        std::cout << e.what() << std::endl;
    }
    fem_config.set_param("float_param", 1.234);
    fem_config.set_param("string_param", "1234");
    fem_config.set_param("negative_param", -1234);

    int param1 = fem_config.get_param<int>("param1");
    double d_param = fem_config.get_param<double>("float_param");
    std::string s_param = fem_config.get_param<std::string>("string_param");
    int n_param = fem_config.get_param<int>("negative_param");

    std::cout << param1 << std::endl << d_param  << std::endl << s_param << std::endl << n_param << std::endl;

 
    try {
        int test = fem_config.get_param<int>("wibblewobble");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    std::map<msgpack::type::variant, msgpack::type::variant> nest;
    nest["nest1"] = 1;
    nest["nest2"] = "nest";

    std::map<msgpack::type::variant, msgpack::type::variant> nest4;
    nest4["triple1"] = "triple";
    nest4["triple2"] = 3;

    std::map<msgpack::type::variant, msgpack::type::variant> nest3;
    nest3["double1"] = "abcxyz";
    nest3["double2"] = 9;
    nest3["triple_nest"] = nest4;

    std::map<msgpack::type::variant, msgpack::type::variant> nest2;
    nest2["new_nest1"] = 80;
    nest2["new_nest2"] = "new_nest";
    nest2["double_nest"] = nest3;

  
    fem_config.set_param("nested_map", nest);
    fem_config.set_param("nested_map2", nest2);
    std::cout << fem_config.print();
    
    try {
        int nested_int = fem_config.get_param<int>("nest1");
        std::cout << "nested int.. " << nested_int << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }
    try{
        int flat_int = fem_config.get_param<int>("param1");
        std::cout << "flat_int.. " << flat_int << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }
    try{
        std::string test_string = fem_config.get_param<std::string>("agkskjgBAJEGB");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
       fem_config.set_param("negative_param", "whatever");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    Fem2ControlMsg request;
    request.set_cmd_type(Fem2ControlMsg::CMD_CONFIGURE);
    request.set_payload<FEMII_CONFIG>(fem_config);

    std::cout << "request : \n" << request;

    std::cout << "request payload is : " <<  request.get_payload_type() << std::endl;

    
    FEMII_CONFIG fem_config_back = request.get_payload<FEMII_CONFIG>();
    assert(fem_config == fem_config_back);
    fem_config_back.set_param("new_param", "I'm gunna break you!");
    assert(fem_config != fem_config_back);

    FEMII_CONFIG test1;
    test1.set_param("hello", 1);
    test1.set_param("nested_map", nest);

    FEMII_CONFIG test2;
    test2.set_param("goodbye", 1);
    test2.set_param("nested_map", nest);

    assert(test1 != test2);

}

void test_femii_config_encoding(){


    printf("---------------------------\nTesting FEMII CONFIG Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_CONFIGURE, Fem2ControlMsg::ACCESS_UNSUPPORTED, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    FEMII_CONFIG the_config;

    the_config.set_param("param1", 1234);
    the_config.set_param("float_param", 1.234);
    the_config.set_param("string_param", "1234");
    the_config.set_param("negative_param", -1234);

    std::multimap<msgpack::type::variant, msgpack::type::variant> nest;
    nest.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("nest1", 1));
    nest.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("nest2", "nest"));
    //nest["nest1"] = 1;
    //nest["nest2"] = "nest";

    int test_int = 3;
    std::multimap<msgpack::type::variant, msgpack::type::variant> nest4;
    nest4.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("triple1", "triple"));
    nest4.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("triple2", test_int));
    //nest4["triple1"] = "triple";
    //nest4["triple2"] = 3;

    std::multimap<msgpack::type::variant, msgpack::type::variant> nest3;
    std::string test_string = "abcxyz";
    nest3.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("double1", test_string));
    nest3.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("double2", 9));
    nest3.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("triple_nest", nest4));

    //nest3["double1"] = "abcxyz";
    //nest3["double2"] = 9;
    //nest3["triple_nest"] = nest4;

    std::multimap<msgpack::type::variant, msgpack::type::variant> nest2;
    nest2.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("new_nest1", 80));
    nest2.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("new_nest2", "new_nest"));
    nest2.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("double_nest", nest3));
    //nest2["new_nest1"] = 
    //nest2["new_nest2"] = "new_nest";
    //nest2["double_nest"] = nest3;

  
    the_config.set_param("nested_map", nest);
    the_config.set_param("nested_map2", nest2);

    request.set_payload<FEMII_CONFIG>(the_config);

    printf("FEMII CONFIG Request: \n");
    std::cout << request;
    std::cout << "payload request is : " << request.get_payload_type() << std::endl;

    try{
        std::cout << "param1 :" << the_config.get_param<int>("param1") << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
         std::cout << "nest1 :" << the_config.get_param<int>("nest1") << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        std::cout << "new_nest1 :" << the_config.get_param<int>("new_nest1") << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }
 
    try{
         std::cout << "double1 :" << the_config.get_param<std::string>("double1") << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        std::cout << "triple2 :" << the_config.get_param<int>("triple2") << std::endl;
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    } 

    try{
        std::string test_string = the_config.get_param<std::string>("agkskjgBAJEGB");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        the_config.set_param("param1", "whatever");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        the_config.set_param("triple1", "whatever");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        the_config.set_param("double1", "whatever");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        the_config.set_param("new_nest1", "whatever");
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    try{
        the_config.set_param("TEST", "HELLO"); // this will work.
    }
    catch(Fem2Exception e){
        std::cout << e.what() << std::endl;
    }

    request.set_payload<FEMII_CONFIG>(the_config);

    printf("FEMII CONFIG Request AGAIN : \n");
    std::cout << request;
    
    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request;
    encoder.encode(request, encoded_request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply;
    encoder.decode(encoded_reply, reply);

    std::cout << "payload reply is : " << reply.get_payload_type() << std::endl;

    FEMII_CONFIG the_config_back = reply.get_payload<FEMII_CONFIG>();
    std::string test_string_back = the_config_back.get_param<std::string>("double1");
    int test_int_back = the_config_back.get_param<int>("triple2");
   
    printf("FEMII CONFIG Reply: \n");
    std::cout << reply;

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_config == the_config_back);
    assert(test_string == test_string_back);
    assert(test_int == test_int_back);
    std::cout << "FEMII CONFIG MATCH" << std::endl;
    

}


int main(){

    socket_.connect("tcp://192.168.0.122:5555");
    //socket_.connect("tcp://localhost:5555");
    printf("Client Booted \n");

    std::vector<uint8_t> this_data;
    
    for(uint8_t i =0; i < 20; i++){
        this_data.push_back(i);
    }

    test_empty_payload();
    test_wrong_payload();
    //test_femii_config();
    test_femii_config_encoding();
    //test_init_config();
    
    
    test_gpio_encoding(this_data);
    test_i2c_encoding(this_data);
    
    test_xadc_encoding(this_data);
    test_rawreg_encoding(this_data);
    test_qdr_encoding(this_data);
    test_qspi_encoding(this_data);
    test_ddr_encoding(this_data);

    test_ddr_write_read();  // writes 0xFF to 0x8000 0001 reads 0xFF to 0x80000001?? x2
    test_gpio_write_read();
    test_rawreg_write_read();
    test_xadc_read();
    test_qspi_read();
    test_i2c_read();
    
    
    /*
    while(true){
        led_control(true);
        sleep(1);
        led_control(false);
        sleep(1);
    }
    */
    //led_control(false);
 

    return 0;
    
}
