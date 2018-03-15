#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fem2ControlMsgTests

#include <boost/test/unit_test.hpp>

#include "../include/Fem2ControlMsg.h"
#include "../include/MsgPackEncoder.h"

BOOST_AUTO_TEST_SUITE(Fem2ControlMsgUnitTest);

BOOST_AUTO_TEST_CASE(Valid_Construction_With_Values){

    Femii::Fem2ControlMsg::AccessType access_type = Femii::Fem2ControlMsg::ACCESS_DDR;
    Femii::Fem2ControlMsg::CommandType command_type = Femii::Fem2ControlMsg::CMD_READ;
    Femii::Fem2ControlMsg::AckState ack_state = Femii::Fem2ControlMsg::ACK;
    uint16_t req_id = 0x01;
    int16_t timeout = 10;
    int16_t retries = 5;

    Femii::Fem2ControlMsg test_msg(command_type, access_type, ack_state, req_id, timeout,retries);

    BOOST_CHECK_EQUAL(access_type, test_msg.get_access_type());
    BOOST_CHECK_EQUAL(command_type, test_msg.get_cmd_type());
    BOOST_CHECK_EQUAL(ack_state, test_msg.get_ack_state());
    BOOST_CHECK_EQUAL(req_id, test_msg.get_req_id());
    BOOST_CHECK_EQUAL(timeout, test_msg.get_timeout());
    BOOST_CHECK_EQUAL(retries, test_msg.get_retries());
    BOOST_CHECK(test_msg.get_string_timestamp() != "not-a-date-time");
}

BOOST_AUTO_TEST_CASE(Valid_Construction_With_Defaults){
   
    Femii::Fem2ControlMsg test_msg;
    BOOST_CHECK_EQUAL(Femii::Fem2ControlMsg::ACCESS_UNSUPPORTED, test_msg.get_access_type());
    BOOST_CHECK_EQUAL(Femii::Fem2ControlMsg::CMD_UNSUPPORTED, test_msg.get_cmd_type());
    BOOST_CHECK_EQUAL(Femii::Fem2ControlMsg::ACK_UNDEFINED, test_msg.get_ack_state());
    BOOST_CHECK_EQUAL(0x0000, test_msg.get_req_id());
    BOOST_CHECK_EQUAL(-1, test_msg.get_timeout());
    BOOST_CHECK_EQUAL(-1, test_msg.get_retries());
    BOOST_CHECK(test_msg.get_string_timestamp() != "not-a-date-time");

}

BOOST_AUTO_TEST_CASE(Valid_Construction_With_Set_Param_From_Defualt){
   
    Femii::Fem2ControlMsg test_msg; //holds default values. 

    Femii::Fem2ControlMsg::CommandType command = Femii::Fem2ControlMsg::CMD_READ;
    Femii::Fem2ControlMsg::AccessType access =  Femii::Fem2ControlMsg::ACCESS_GPIO;
    Femii::Fem2ControlMsg::AckState ack = Femii::Fem2ControlMsg::ACK;
    uint16_t request_id = 0x16;
    int16_t timeout = 8;
    int16_t retries = 2;

    //set header values to non-default vals.
    test_msg.set_cmd_type(command);
    test_msg.set_access_type(access);
    test_msg.set_ack_state(ack);
    test_msg.set_req_id(request_id);
    test_msg.set_timeout(timeout);
    test_msg.set_retries(retries);

    //std::cout << test_msg;
    
    BOOST_CHECK(test_msg.get_cmd_type() != Femii::Fem2ControlMsg::CMD_UNSUPPORTED);
    BOOST_CHECK(test_msg.get_access_type() != Femii::Fem2ControlMsg::ACCESS_UNSUPPORTED);
    BOOST_CHECK(test_msg.get_ack_state() != Femii::Fem2ControlMsg::ACK_UNDEFINED);
    BOOST_CHECK(test_msg.get_req_id() != 0x0000);
    BOOST_CHECK(test_msg.get_timeout() != -1);
    BOOST_CHECK(test_msg.get_retries() != -1);
    
    BOOST_CHECK_EQUAL(test_msg.get_cmd_type(), command);
    BOOST_CHECK_EQUAL(test_msg.get_access_type(), access);
    BOOST_CHECK_EQUAL(test_msg.get_ack_state(), ack);
    BOOST_CHECK_EQUAL(test_msg.get_req_id(), request_id);
    BOOST_CHECK_EQUAL(test_msg.get_timeout(), timeout);
    BOOST_CHECK_EQUAL(test_msg.get_retries(), retries);
    BOOST_CHECK(test_msg.get_string_timestamp() != "not-a-date-time");

}

BOOST_AUTO_TEST_CASE(Invalid_Payload_Printing){
   
    //No payload has been initiated so printing this message throws an exception?
    Femii::Fem2ControlMsg test_msg;
    test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    BOOST_CHECK_THROW(test_msg.print(), Femii::Fem2ControlMsgException);   

    //now initalise a payload which sets the string representation of the message
    Femii::I2C_RW i2c_set;
    int read_length = 6;
    test_msg.set_payload<Femii::I2C_RW>(i2c_set, read_length);

    BOOST_CHECK_NO_THROW(test_msg.print()); 

}

BOOST_AUTO_TEST_CASE(Payload_Mismatch_Incorrect_Access_Type_Get_Payload){
   
    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    Femii::I2C_RW i2c_set;
    int read_length = 2;
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set, read_length));
    BOOST_CHECK_THROW(Femii::GPIO_RW gpio_get = payload_test_msg.get_payload<Femii::GPIO_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::XADC_RW xadc_get = payload_test_msg.get_payload<Femii::XADC_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::RAWREG_RW rawreg_get = payload_test_msg.get_payload<Femii::RAWREG_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::DDR_RW ddr_get = payload_test_msg.get_payload<Femii::DDR_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::QDR_RW qdr_get = payload_test_msg.get_payload<Femii::QDR_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::QSPI_RW qspi_get = payload_test_msg.get_payload<Femii::QSPI_RW>(), Femii::Fem2ControlMsgException);

    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_UNSUPPORTED);
    BOOST_CHECK_THROW(Femii::I2C_RW i2c_get = payload_test_msg.get_payload<Femii::I2C_RW>(), Femii::Fem2ControlMsgException);


}

BOOST_AUTO_TEST_CASE(Payload_Mismatch_Incorrect_Access_Type_Set_Payload){
   
    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    int read_length = 2;

    Femii::I2C_RW i2c_set;
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set, read_length));

    Femii::GPIO_RW gpio_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_get, read_length), Femii::Fem2ControlMsgException);

    Femii::XADC_RW xadc_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::XADC_RW>(xadc_get, read_length), Femii::Fem2ControlMsgException);

    Femii::RAWREG_RW rawreg_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::RAWREG_RW>(rawreg_get, read_length), Femii::Fem2ControlMsgException);

    Femii::DDR_RW ddr_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::DDR_RW>(ddr_get, read_length), Femii::Fem2ControlMsgException);

    Femii::QDR_RW qdr_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QDR_RW>(qdr_get, read_length), Femii::Fem2ControlMsgException);

    Femii::QSPI_RW qspi_get; 
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_get, read_length), Femii::Fem2ControlMsgException);

    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set, read_length), Femii::Fem2ControlMsgException);
}

BOOST_AUTO_TEST_CASE(Empty_Payload){
   
    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_UNSUPPORTED);
    BOOST_CHECK_THROW(Femii::I2C_RW i2c_get = payload_test_msg.get_payload<Femii::I2C_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::GPIO_RW gpio_get = payload_test_msg.get_payload<Femii::GPIO_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::XADC_RW xadc_get = payload_test_msg.get_payload<Femii::XADC_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::RAWREG_RW rawreg_get = payload_test_msg.get_payload<Femii::RAWREG_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::DDR_RW ddr_get = payload_test_msg.get_payload<Femii::DDR_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::QDR_RW qdr_get = payload_test_msg.get_payload<Femii::QDR_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::QSPI_RW qspi_get = payload_test_msg.get_payload<Femii::QSPI_RW>(), Femii::Fem2ControlMsgException);

}

BOOST_AUTO_TEST_CASE(Valid_I2C_WRITE_Roundtrip_Set_Encode_Decode_Get){
    
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg I2C_write_msg;
    I2C_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);
    
    //-------- I2C --------//
    I2C_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    Femii::I2C_RW i2c_set;
    i2c_set.i2c_bus = 0x01010101;
    i2c_set.slave_address = 0x200;
    i2c_set.i2c_register = 0x200;
    i2c_set.data_width = Femii::WIDTH_BYTE;

    //Setting a write payload with no data i.e. data length of 0 = exception.
    BOOST_CHECK_THROW(I2C_write_msg.set_payload<Femii::I2C_RW>(i2c_set), Femii::Fem2ControlMsgException);

    i2c_set.the_data.push_back(0x05);
    //Setting a write payload with data i.e. data length of > 0  = no exception.
    BOOST_CHECK_NO_THROW(I2C_write_msg.set_payload<Femii::I2C_RW>(i2c_set));
   
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(I2C_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_I2C_write_msg;
    encoder.decode(encoded_reply, decoded_I2C_write_msg);

    //get the payload from the decoded message
    Femii::I2C_RW i2c_get = decoded_I2C_write_msg.get_payload<Femii::I2C_RW>();

    BOOST_CHECK_EQUAL(i2c_set, i2c_get);
    BOOST_CHECK(I2C_write_msg == decoded_I2C_write_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(I2C_write_msg.get_data_length(), i2c_set.the_data.size());  
    
}

BOOST_AUTO_TEST_CASE(Valid_GPIO_WRITE_Roundtrip_Set_Encode_Decode_Get){
   
    //-------- GPIO --------//
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg GPIO_write_msg;
    GPIO_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);

    GPIO_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    Femii::GPIO_RW gpio_set;
    gpio_set.mem_address = 0x01010101;
    gpio_set.mem_register = 0x200;
    gpio_set.data_width = Femii::WIDTH_WORD;
    
    BOOST_CHECK_THROW(GPIO_write_msg.set_payload<Femii::GPIO_RW>(gpio_set), Femii::Fem2ControlMsgException);
    gpio_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(GPIO_write_msg.set_payload<Femii::GPIO_RW>(gpio_set));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(GPIO_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_GPIO_write_msg;
    encoder.decode(encoded_reply, decoded_GPIO_write_msg);

    Femii::GPIO_RW gpio_get = decoded_GPIO_write_msg.get_payload<Femii::GPIO_RW>();

    BOOST_CHECK_EQUAL(gpio_set, gpio_get);
    BOOST_CHECK(GPIO_write_msg == decoded_GPIO_write_msg);

    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(GPIO_write_msg.get_data_length(), gpio_set.the_data.size());    
}

BOOST_AUTO_TEST_CASE(Valid_XADC_WRITE_Roundtrip_Set_Encode_Decode_Get){

    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg XADC_write_msg;
    XADC_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);

    //-------- XADC --------//
    XADC_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_XADC);
    Femii::XADC_RW xadc_set;
    xadc_set.mem_address = 0x01010101;
    xadc_set.mem_register = 0x200;
    xadc_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(XADC_write_msg.set_payload<Femii::XADC_RW>(xadc_set), Femii::Fem2ControlMsgException);

    xadc_set.the_data.push_back(0x05);

    BOOST_CHECK_NO_THROW(XADC_write_msg.set_payload<Femii::XADC_RW>(xadc_set));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(XADC_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_XADC_write_msg;
    encoder.decode(encoded_reply, decoded_XADC_write_msg);

    Femii::XADC_RW xadc_get = decoded_XADC_write_msg.get_payload<Femii::XADC_RW>();
    BOOST_CHECK_EQUAL(xadc_set, xadc_get);
    BOOST_CHECK(XADC_write_msg == decoded_XADC_write_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(XADC_write_msg.get_data_length(), xadc_set.the_data.size());
}

BOOST_AUTO_TEST_CASE(Valid_RAWREG_WRITE_Roundtrip_Set_Encode_Decode_Get){

    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg RAWREG_write_msg;
    RAWREG_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);

    //-------- RAWREG --------//
    RAWREG_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_RAWREG);
    Femii::RAWREG_RW rawreg_set;
    rawreg_set.mem_address = 0x01010101;
    rawreg_set.mem_register = 0x200;
    rawreg_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(RAWREG_write_msg.set_payload<Femii::RAWREG_RW>(rawreg_set), Femii::Fem2ControlMsgException);
    
    rawreg_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(RAWREG_write_msg.set_payload<Femii::RAWREG_RW>(rawreg_set));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(RAWREG_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_RAWREG_write_msg;
    encoder.decode(encoded_reply, decoded_RAWREG_write_msg);

    
    Femii::RAWREG_RW rawreg_get = decoded_RAWREG_write_msg.get_payload<Femii::RAWREG_RW >();
    BOOST_CHECK_EQUAL(rawreg_set, rawreg_get);
    BOOST_CHECK(RAWREG_write_msg == decoded_RAWREG_write_msg);

    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(RAWREG_write_msg.get_data_length(), rawreg_set.the_data.size());
}   

BOOST_AUTO_TEST_CASE(Valid_DDR_WRITE_Roundtrip_Set_Encode_Decode_Get){

    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg DDR_write_msg;
    DDR_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);

    //-------- DDR --------//
    DDR_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_DDR);
    Femii::DDR_RW ddr_set;
    ddr_set.mem_address = 0x01010101;
    ddr_set.offset = 0x64;
    ddr_set.page = 0x01;
    ddr_set.data_width = Femii::WIDTH_BYTE;
    
    BOOST_CHECK_THROW(DDR_write_msg.set_payload<Femii::DDR_RW>(ddr_set), Femii::Fem2ControlMsgException);
    ddr_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(DDR_write_msg.set_payload<Femii::DDR_RW>(ddr_set));
    
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(DDR_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_DDR_write_msg;
    encoder.decode(encoded_reply, decoded_DDR_write_msg);


    Femii::DDR_RW ddr_get = decoded_DDR_write_msg.get_payload<Femii::DDR_RW>();
    BOOST_CHECK_EQUAL(ddr_set, ddr_get);
    BOOST_CHECK(DDR_write_msg == decoded_DDR_write_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(DDR_write_msg.get_data_length(), ddr_set.the_data.size());
}

BOOST_AUTO_TEST_CASE(Valid_QDR_WRITE_Roundtrip_Set_Encode_Decode_Get){


    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg QDR_write_msg;
    QDR_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);


    //-------- QDR --------//
    QDR_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QDR);
    Femii::QDR_RW qdr_set;
    qdr_set.mem_address = 0x01010101;
    qdr_set.offset = 0x64;
    qdr_set.page = 0x01;
    qdr_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(QDR_write_msg.set_payload<Femii::QDR_RW>(qdr_set), Femii::Fem2ControlMsgException);
    qdr_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(QDR_write_msg.set_payload<Femii::QDR_RW>(qdr_set));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(QDR_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_QDR_write_msg;
    encoder.decode(encoded_reply, decoded_QDR_write_msg);


    Femii::QDR_RW qdr_get = decoded_QDR_write_msg.get_payload<Femii::QDR_RW>();
    BOOST_CHECK_EQUAL(qdr_set, qdr_get);

    BOOST_CHECK(QDR_write_msg == decoded_QDR_write_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(QDR_write_msg.get_data_length(), qdr_set.the_data.size());    
}

BOOST_AUTO_TEST_CASE(Valid_QSPI_WRITE_Roundtrip_Set_Encode_Decode_Get){

    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg QSPI_write_msg;
    QSPI_write_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);

    //-------- QSPI --------//
    QSPI_write_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QSPI);
    Femii::QSPI_RW qspi_set;
    qspi_set.mem_address = 0x01010101;
    qspi_set.offset = 0x64;
    qspi_set.page = 0x01;
    qspi_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(QSPI_write_msg.set_payload<Femii::QSPI_RW>(qspi_set), Femii::Fem2ControlMsgException);
    qspi_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(QSPI_write_msg.set_payload<Femii::QSPI_RW>(qspi_set));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(QSPI_write_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_QSPI_write_msg;
    encoder.decode(encoded_reply, decoded_QSPI_write_msg);


    Femii::QSPI_RW qspi_get = decoded_QSPI_write_msg.get_payload<Femii::QSPI_RW>();
    BOOST_CHECK_EQUAL(qspi_set, qspi_get);

    BOOST_CHECK(QSPI_write_msg == decoded_QSPI_write_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(QSPI_write_msg.get_data_length(), qspi_set.the_data.size());

}


BOOST_AUTO_TEST_CASE(Valid_I2C_READ_Roundtrip_Set_Encode_Decode_Get){

    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg I2C_read_msg;
    I2C_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    
    //-------- I2C --------//
    I2C_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    Femii::I2C_RW i2c_set;
    i2c_set.i2c_bus = 0x01010101;
    i2c_set.slave_address = 0x200;
    i2c_set.i2c_register = 0x200;
    i2c_set.data_width = Femii::WIDTH_BYTE;

    //Setting the payload without a read length provided throws exception
    BOOST_CHECK_THROW(I2C_read_msg.set_payload<Femii::I2C_RW>(i2c_set), Femii::Fem2ControlMsgException);

    //Setting the payload with a read length provided does not throw an exception
    BOOST_CHECK_NO_THROW(I2C_read_msg.set_payload<Femii::I2C_RW>(i2c_set, read_length));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(I2C_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_I2C_read_msg;
    encoder.decode(encoded_reply, decoded_I2C_read_msg);

    Femii::I2C_RW i2c_get = decoded_I2C_read_msg.get_payload<Femii::I2C_RW>();
    //Assert that the payload is the same at set as at get.
    BOOST_CHECK_EQUAL(i2c_set, i2c_get);
    BOOST_CHECK(I2C_read_msg == decoded_I2C_read_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(I2C_read_msg.get_data_length(), i2c_set.the_data.size());  
    // make sure that the read length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(I2C_read_msg.get_read_length(), read_length);  
}    

BOOST_AUTO_TEST_CASE(Valid_GPIO_READ_Roundtrip_Set_Encode_Decode_Get){

    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg GPIO_read_msg;
    GPIO_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);


    //-------- GPIO --------//

    GPIO_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    Femii::GPIO_RW gpio_set;
    gpio_set.mem_address = 0x01010101;
    gpio_set.mem_register = 0x200;
    gpio_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(GPIO_read_msg.set_payload<Femii::GPIO_RW>(gpio_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(GPIO_read_msg.set_payload<Femii::GPIO_RW>(gpio_set, read_length));
    
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(GPIO_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_GPIO_read_msg;
    encoder.decode(encoded_reply, decoded_GPIO_read_msg);

    Femii::GPIO_RW gpio_get = decoded_GPIO_read_msg.get_payload<Femii::GPIO_RW>();
    BOOST_CHECK_EQUAL(gpio_set, gpio_get);

    BOOST_CHECK(GPIO_read_msg == decoded_GPIO_read_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(GPIO_read_msg.get_data_length(), gpio_set.the_data.size());
    BOOST_CHECK_EQUAL(GPIO_read_msg.get_read_length(), read_length);    

}

BOOST_AUTO_TEST_CASE(Valid_XADC_READ_Roundtrip_Set_Encode_Decode_Get){

    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg XADC_read_msg;
    XADC_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);

    //-------- XADC --------//
    
    XADC_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_XADC);
    Femii::XADC_RW xadc_set;
    xadc_set.mem_address = 0x01010101;
    xadc_set.mem_register = 0x200;
    xadc_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(XADC_read_msg.set_payload<Femii::XADC_RW>(xadc_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(XADC_read_msg.set_payload<Femii::XADC_RW>(xadc_set, read_length));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(XADC_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_XADC_read_msg;
    encoder.decode(encoded_reply, decoded_XADC_read_msg);

    Femii::XADC_RW xadc_get = decoded_XADC_read_msg.get_payload<Femii::XADC_RW>();
    BOOST_CHECK_EQUAL(xadc_set, xadc_get);

    BOOST_CHECK(XADC_read_msg == decoded_XADC_read_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(XADC_read_msg.get_data_length(), xadc_set.the_data.size());
    BOOST_CHECK_EQUAL(XADC_read_msg.get_read_length(), read_length);    

}
BOOST_AUTO_TEST_CASE(Valid_RAWREG_READ_Roundtrip_Set_Encode_Decode_Get){


    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg RAWREG_read_msg;
    RAWREG_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);

    //-------- RAWREG --------//
   
    RAWREG_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_RAWREG);
    Femii::RAWREG_RW rawreg_set;
    rawreg_set.mem_address = 0x01010101;
    rawreg_set.mem_register = 0x200;
    rawreg_set.data_width = Femii::WIDTH_WORD;

    BOOST_CHECK_THROW(RAWREG_read_msg.set_payload<Femii::RAWREG_RW >(rawreg_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(RAWREG_read_msg.set_payload<Femii::RAWREG_RW >(rawreg_set, read_length));
    
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(RAWREG_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_RAWREG_read_msg;
    encoder.decode(encoded_reply, decoded_RAWREG_read_msg);

    Femii::RAWREG_RW rawreg_get = decoded_RAWREG_read_msg.get_payload<Femii::RAWREG_RW >();
    BOOST_CHECK_EQUAL(rawreg_set, rawreg_get);

    BOOST_CHECK(RAWREG_read_msg == decoded_RAWREG_read_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(RAWREG_read_msg.get_data_length(), rawreg_set.the_data.size());
    BOOST_CHECK_EQUAL(RAWREG_read_msg.get_read_length(), read_length);    

}

BOOST_AUTO_TEST_CASE(Valid_DDR_READ_Roundtrip_Set_Encode_Decode_Get){


    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg DDR_read_msg;
    DDR_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    
    //-------- DDR --------//

    DDR_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_DDR);
    Femii::DDR_RW ddr_set;
    ddr_set.mem_address = 0x01010101;
    ddr_set.offset = 0x64;
    ddr_set.page = 0x01;
    ddr_set.data_width = Femii::WIDTH_BYTE;
    BOOST_CHECK_THROW(DDR_read_msg.set_payload<Femii::DDR_RW>(ddr_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(DDR_read_msg.set_payload<Femii::DDR_RW>(ddr_set, read_length));

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(DDR_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_DDR_read_msg;
    encoder.decode(encoded_reply, decoded_DDR_read_msg);

    Femii::DDR_RW ddr_get = decoded_DDR_read_msg.get_payload<Femii::DDR_RW>();
    BOOST_CHECK_EQUAL(ddr_set, ddr_get);

    BOOST_CHECK(DDR_read_msg == decoded_DDR_read_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(DDR_read_msg.get_data_length(), ddr_set.the_data.size());
    BOOST_CHECK_EQUAL(DDR_read_msg.get_read_length(), read_length);    

}

BOOST_AUTO_TEST_CASE(Valid_QDR_READ_Roundtrip_Set_Encode_Decode_Get){
  
    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg QDR_read_msg;
    QDR_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);

    //-------- QDR --------//

    QDR_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QDR);
    Femii::QDR_RW qdr_set;
    qdr_set.mem_address = 0x01010101;
    qdr_set.offset = 0x64;
    qdr_set.page = 0x01;
    qdr_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(QDR_read_msg.set_payload<Femii::QDR_RW>(qdr_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(QDR_read_msg.set_payload<Femii::QDR_RW>(qdr_set, read_length));
   
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(QDR_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_QDR_read_msg;
    encoder.decode(encoded_reply, decoded_QDR_read_msg);


    Femii::QDR_RW qdr_get = decoded_QDR_read_msg.get_payload<Femii::QDR_RW>();
    BOOST_CHECK_EQUAL(qdr_set, qdr_get);

    BOOST_CHECK(QDR_read_msg == decoded_QDR_read_msg);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(QDR_read_msg.get_data_length(), qdr_set.the_data.size());
    BOOST_CHECK_EQUAL(QDR_read_msg.get_read_length(), read_length);    
    
}

BOOST_AUTO_TEST_CASE(Valid_QSPI_READ_Roundtrip_Set_Encode_Decode_Get){

    int read_length = 1;
    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg QSPI_read_msg;
    QSPI_read_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);

    //-------- QSPI --------//
   
    QSPI_read_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QSPI);
    Femii::QSPI_RW qspi_set;
    qspi_set.mem_address = 0x01010101;
    qspi_set.offset = 0x64;
    qspi_set.page = 0x01;
    qspi_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(QSPI_read_msg.set_payload<Femii::QSPI_RW>(qspi_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(QSPI_read_msg.set_payload<Femii::QSPI_RW>(qspi_set, read_length));
    
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(QSPI_read_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_QSPI_read_msg;
    encoder.decode(encoded_reply, decoded_QSPI_read_msg);

    Femii::QSPI_RW qspi_get = decoded_QSPI_read_msg.get_payload<Femii::QSPI_RW>();
    BOOST_CHECK_EQUAL(qspi_set, qspi_get);
    BOOST_CHECK(QSPI_read_msg == decoded_QSPI_read_msg);

    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(QSPI_read_msg.get_data_length(), qspi_set.the_data.size());
    BOOST_CHECK_EQUAL(QSPI_read_msg.get_read_length(), read_length);  

}

BOOST_AUTO_TEST_CASE(Valid_FEMII_CONFIG_Roundtrip_Set_Encode_Decode_Get){

    MsgPackEncoder encoder;

    std::multimap<msgpack::type::variant, msgpack::type::variant> nest;
    nest.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("nest1", 1));
    nest.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("nest2", "nest"));

    double set_double = 3.9999;
    std::multimap<msgpack::type::variant, msgpack::type::variant> nest4;
    nest4.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("triple1", "triple"));
    nest4.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("triple2", set_double));

    std::multimap<msgpack::type::variant, msgpack::type::variant> nest3;
    std::string set_string = "abcxyz";
    nest3.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("double1", set_string));
    nest3.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("double2", 9));
    nest3.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("triple_nest", nest4));

    std::multimap<msgpack::type::variant, msgpack::type::variant> nest2;
    nest2.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("new_nest1", 80));
    nest2.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("new_nest2", "new_nest"));
    nest2.insert(std::pair<msgpack::type::variant, msgpack::type::variant>("double_nest", nest3));


    Femii::Fem2ControlMsg FEMII_CONFIG_msg;
    FEMII_CONFIG_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    Femii::FEMII_CONFIG the_config;
    
    //throws because it's a cmd read msg.
    BOOST_CHECK_THROW(FEMII_CONFIG_msg.set_payload<Femii::FEMII_CONFIG>(the_config), Femii::Fem2ControlMsgException);
    
    FEMII_CONFIG_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_CONFIGURE);

    //throws because it's got an empty parameter block so nothing to set. 
    BOOST_CHECK_THROW(FEMII_CONFIG_msg.set_payload<Femii::FEMII_CONFIG>(the_config), Femii::Fem2ControlMsgException);

    //these parameters do not already exist so they should be setable.
    BOOST_CHECK_NO_THROW(the_config.set_param("unique_param1", 1234));
    BOOST_CHECK_NO_THROW(the_config.set_param("unique_float_param", 1.234));
    BOOST_CHECK_NO_THROW(the_config.set_param("unique_string_param", "1234"));
    BOOST_CHECK_NO_THROW(the_config.set_param("unique_negative_param", -1234));
    BOOST_CHECK_NO_THROW(the_config.set_param("unique_nested_map", nest));
    BOOST_CHECK_NO_THROW(the_config.set_param("unique_nested_map2", nest2));

    //should set the payload correctly
    BOOST_CHECK_NO_THROW(FEMII_CONFIG_msg.set_payload<Femii::FEMII_CONFIG>(the_config));

    BOOST_CHECK_NO_THROW(int ok = the_config.get_param<int>("unique_param1"));
    BOOST_CHECK_NO_THROW(int ok = the_config.get_param<int>("nest1"));
    BOOST_CHECK_NO_THROW(int ok = the_config.get_param<int>("new_nest1"));
    BOOST_CHECK_NO_THROW(std::string ok = the_config.get_param<std::string>("double1"));
    BOOST_CHECK_NO_THROW(double ok = the_config.get_param<double>("triple2"));

    //these parameters do not exist so will throw.
    BOOST_CHECK_THROW(the_config.get_param<std::string>("agkskjgBAJEGB"), Femii::Fem2Exception);
    BOOST_CHECK_THROW(the_config.get_param<int>("wibblewobble"), Femii::Fem2Exception);
    BOOST_CHECK_THROW(the_config.get_param<double>("jelly"), Femii::Fem2Exception);

    // cannot set these param names they already exist.
    BOOST_CHECK_THROW(the_config.set_param("unique_param1", "whatever"), Femii::Fem2Exception);
    BOOST_CHECK_THROW(the_config.set_param("triple1", "whatever"), Femii::Fem2Exception);
    BOOST_CHECK_THROW(the_config.set_param("double1", "whatever"), Femii::Fem2Exception);
    BOOST_CHECK_THROW(the_config.set_param("new_nest1", "whatever"), Femii::Fem2Exception);

    // perform encoding 
    std::string encoded_msg;
    encoder.encode(FEMII_CONFIG_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_FEMII_CONFIG_msg;
    encoder.decode(encoded_reply, decoded_FEMII_CONFIG_msg);

    Femii::FEMII_CONFIG the_config_back = decoded_FEMII_CONFIG_msg.get_payload<Femii::FEMII_CONFIG>();
    
    //no throw
    std::string get_string = the_config_back.get_param<std::string>("double1");
    double get_double = the_config_back.get_param<double>("triple2");

    BOOST_CHECK_EQUAL(set_double, get_double);
    BOOST_CHECK_EQUAL(set_string, get_string);
    BOOST_CHECK_EQUAL(the_config, the_config_back);
    BOOST_CHECK(FEMII_CONFIG_msg == decoded_FEMII_CONFIG_msg);

}

BOOST_AUTO_TEST_CASE(Valid_Default_Msg_Roundtrip_Set_Encode_Decode_Get){

    MsgPackEncoder encoder;
    Femii::Fem2ControlMsg default_msg;
    
    // perform encoding 
    std::string encoded_msg;
    encoder.encode(default_msg, encoded_msg);

    //perform decoding
    std::string encoded_reply = encoded_msg;
    Femii::Fem2ControlMsg decoded_default_msg;
    encoder.decode(encoded_reply, decoded_default_msg);

    BOOST_CHECK(default_msg == decoded_default_msg);

}


BOOST_AUTO_TEST_SUITE_END();


// go through all exceptions and check..