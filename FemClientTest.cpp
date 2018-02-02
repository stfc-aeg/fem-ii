#include <iostream> 
#include <zmq.hpp>
#include "Fem2ControlMsg.hpp"
#include <cassert>
#include "MsgPackEncoder.hpp"


#define DDR3_BASE 0x80000000;
#define GPIO_STATUS 0x41210000;

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

void test_gpio_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting GPIO Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    GPIO_RW the_gpio; 
    the_gpio.mem_address = 1234;
    the_gpio.mem_register = 12;
    the_gpio.data_width = WIDTH_BYTE;
    the_gpio.the_data = the_data;
    request.set_payload<GPIO_RW>(the_gpio);

    printf("GPIO Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("GPIO Reply: \n");
    std::cout << reply;

    GPIO_RW the_gpio_back = reply.get_payload<GPIO_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_gpio == the_gpio_back);
    // double check the vector size + data length fields are the same
    assert(the_gpio_back.the_data.size() == request.data_length_);
    std::cout << "GPIO MATCH" << std::endl;
}

void test_xadc_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting XADC Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_XADC, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    XADC_RW the_xadc; 
    the_xadc.mem_address = 1234;
    the_xadc.mem_register = 12;
    the_xadc.data_width = WIDTH_BYTE;
    the_xadc.the_data = the_data;
    request.set_payload<XADC_RW>(the_xadc);

    printf("XADC Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("XADC Reply: \n");
    std::cout << reply;

    XADC_RW the_xadc_back = reply.get_payload<XADC_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_xadc == the_xadc_back);
    // double check the vector size + data length fields are the same
    assert(the_xadc_back.the_data.size() == request.data_length_);
    std::cout << "XADC MATCH" << std::endl;
    
}

void test_rawreg_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting RAWREG Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_RAWREG, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    RAWREG_RW the_rreg; 
    the_rreg.mem_address = 1234;
    the_rreg.mem_register = 12;
    the_rreg.data_width = WIDTH_BYTE;
    the_rreg.the_data = the_data;
    request.set_payload<RAWREG_RW>(the_rreg);

    printf("RAWREG Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("RAWREG Reply: \n");
    std::cout << reply;

    RAWREG_RW the_rreg_back = reply.get_payload<RAWREG_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_rreg == the_rreg_back);
    // double check the vector size + data length fields are the same
    assert(the_rreg_back.the_data.size() == request.data_length_);
    std::cout << "RAWREG MATCH" << std::endl;
    
    
}

void test_i2c_encoding(std::vector<uint8_t> const& the_data){


    printf("---------------------------\nTesting I2C Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_I2C, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
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
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("I2C Reply: \n");
    std::cout << reply;

    I2C_RW the_i2c_back = reply.get_payload<I2C_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_i2c == the_i2c_back);
    // double check the vector size + data length fields are the same
    assert(the_i2c_back.the_data.size() == request.data_length_);
    std::cout << "I2C MATCH" << std::endl;
    
}

void test_qdr_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting QDR Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_QDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
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
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("QDR Reply: \n");
    std::cout << reply;

    QDR_RW the_qdr_back = reply.get_payload<QDR_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_qdr == the_qdr_back);
    // double check the vector size + data length fields are the same
    assert(the_qdr_back.the_data.size() == request.data_length_);
    std::cout << "QDR MATCH" << std::endl;
    
}
void test_ddr_encoding(std::vector<uint8_t> const& the_data){


    printf("---------------------------\nTesting DDR Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
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
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("DDR Reply: \n");
    std::cout << reply;

    DDR_RW the_ddr_back = reply.get_payload<DDR_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_ddr == the_ddr_back);
    // double check the vector size + data length fields are the same
    assert(the_ddr_back.the_data.size() == request.data_length_);
    std::cout << "DDR MATCH" << std::endl;
    
}

void test_qspi_encoding(std::vector<uint8_t> const& the_data){

    printf("---------------------------\nTesting QSPI Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_QSPI, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
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
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("QSPI Reply: \n");
    std::cout << reply;

    QSPI_RW the_qspi_back = reply.get_payload<QSPI_RW>();

    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_qspi == the_qspi_back);
    // double check the vector size + data length fields are the same
    assert(the_qspi_back.the_data.size() == request.data_length_);
    std::cout << "QSPI MATCH" << std::endl;
  
}


void test_gpio_read(){

    printf("---------------------------\nTesting GPIO Round Trip...\n");

    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_GPIO, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    GPIO_RW the_gpio; 
    the_gpio.mem_address = GPIO_STATUS;
    the_gpio.mem_register = 0;
    the_gpio.data_width = WIDTH_BYTE;
    request.set_payload<GPIO_RW>(the_gpio);

    printf("GPIO Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("GPIO Reply: \n");
    std::cout << reply;

    GPIO_RW the_gpio_back = reply.get_payload<GPIO_RW>();

    /*
    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_gpio == the_gpio_back);
    // double check the vector size + data length fields are the same
    assert(the_gpio_back.the_data.size() == request.data_length_);
    std::cout << "GPIO MATCH" << std::endl;
    */
}

void test_ddr_read_femii(){


    printf("---------------------------\nTesting DDR Round Trip...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    DDR_RW the_ddr;
    the_ddr.mem_address = DDR3_BASE; //DDR base address
    the_ddr.page = 0;
    the_ddr.offset = 0x00000001;        //DDR offset address
    the_ddr.data_width = WIDTH_BYTE;
    request.set_payload<DDR_RW>(the_ddr);

    printf("DDR Request: \n");
    std::cout << request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_request = encoder.encode(request);
    send(encoded_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_reply = receive();
    Fem2ControlMsg reply = encoder.decode(encoded_reply);
    printf("DDR Reply: \n");
    std::cout << reply;

    DDR_RW the_ddr_back = reply.get_payload<DDR_RW>();

    /*
    //assert encoded/decoded round trip msgs and payloads are the same thing
    assert(request == reply);
    assert(the_ddr == the_ddr_back);
    // double check the vector size + data length fields are the same
    assert(the_ddr_back.the_data.size() == request.data_length_);
    std::cout << "DDR MATCH" << std::endl;
    */
}

void test_ddr_readwrite_femii(){


    printf("---------------------------\nTesting DDR Write - Read Trip...\n");
    printf("---------------------------\nWriting...\n");
    //initialise a control msg with values.
    Fem2ControlMsg write_request(Fem2ControlMsg::CMD_WRITE, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    DDR_RW the_ddr_write;
    the_ddr_write.mem_address = DDR3_BASE; //DDR base address
    the_ddr_write.page = 3;
    the_ddr_write.offset = 0x00000001;
    the_ddr_write.the_data.push_back(0xCC);        //DDR offset address
    the_ddr_write.data_width = WIDTH_BYTE;
    write_request.set_payload<DDR_RW>(the_ddr_write);

    printf("DDR Write Request: \n");
    std::cout << write_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_write_request = encoder.encode(write_request);
    send(encoded_write_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_write_reply = receive();
    Fem2ControlMsg write_reply = encoder.decode(encoded_write_reply);
    printf("DDR Reply: \n");
    std::cout << write_reply;

    DDR_RW the_ddr_write_back = write_reply.get_payload<DDR_RW>();

    printf("---------------------------\nReading...\n");
    
    //initialise a control msg with values.
    Fem2ControlMsg read_request(Fem2ControlMsg::CMD_READ, Fem2ControlMsg::ACCESS_DDR, Fem2ControlMsg::ACK_UNDEFINED, 0x1234, 10, 0); // default control message.
    
    DDR_RW the_ddr_read;
    the_ddr_read.mem_address = DDR3_BASE; //DDR base address
    the_ddr_read.page = 3;
    the_ddr_read.offset = 0x00000001;        //DDR offset address
    the_ddr_read.data_width = WIDTH_BYTE;
    read_request.set_payload<DDR_RW>(the_ddr_read);

    printf("DDR Read Request: \n");
    std::cout << read_request;

    //  encode the fem2controlmsg as a string (byte string) and send
    std::string encoded_read_request = encoder.encode(read_request);
    send(encoded_read_request);

    //receive reply from server via zmq and decode into Fem2ControlMsg
    std::string encoded_read_reply = receive();
    Fem2ControlMsg read_reply = encoder.decode(encoded_read_reply);
    printf("DDR Read Reply: \n");
    std::cout << read_reply;

    DDR_RW the_ddr_read_back = read_reply.get_payload<DDR_RW>();

    assert(the_ddr_write == the_ddr_read_back);    
 
    // double check the vector size + data length fields are the same
    assert(the_ddr_write.the_data.size() == read_reply.data_length_);
    std::cout << "DDR WRITE READ MATCH" << std::endl;
}


int main(){

    socket_.connect("tcp://192.168.0.122:5555");
    //socket_.connect("tcp://localhost:5555");
    printf("Client Booted \n");

    std::vector<uint8_t> this_data;
    
    for(uint8_t i =0; i < 20; i++){
        this_data.push_back(i);
    }

    //test_gpio_encoding(this_data);
    test_i2c_encoding(this_data);
    test_xadc_encoding(this_data);
    test_rawreg_encoding(this_data);
    test_qdr_encoding(this_data);
    test_qspi_encoding(this_data);
    //test_ddr_encoding(this_data);

    //test_ddr_write_femii(); // writes 0xFF to 0x80000001
    test_ddr_readwrite_femii();  // writes 0xFF to 0x8000 0001 reads 0xFF to 0x80000001??
    test_gpio_read();
    return 0;
    
}
