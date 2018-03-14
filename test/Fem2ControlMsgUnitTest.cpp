#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fem2ControlMsgTests

#include <boost/test/unit_test.hpp>

#include "../include/Fem2ControlMsg.h"

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

    std::cout << test_msg;
    
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

BOOST_AUTO_TEST_CASE(Valid_Roundtrip_Set_Get_For_Write_Payloads){

    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);
    
    //-------- I2C --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    Femii::I2C_RW i2c_set;
    i2c_set.i2c_bus = 0x01010101;
    i2c_set.slave_address = 0x200;
    i2c_set.i2c_register = 0x200;
    i2c_set.data_width = Femii::WIDTH_BYTE;

    //Setting a write payload with no data i.e. data length of 0 = exception.
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set), Femii::Fem2ControlMsgException);

    i2c_set.the_data.push_back(0x05);
    //Setting a write payload with data i.e. data length of > 0  = no exception.
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set));
   
    Femii::I2C_RW i2c_get = payload_test_msg.get_payload<Femii::I2C_RW>();
    BOOST_CHECK_EQUAL(i2c_set, i2c_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), i2c_set.the_data.size());  
    

    //-------- GPIO --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    Femii::GPIO_RW gpio_set;
    gpio_set.mem_address = 0x01010101;
    gpio_set.mem_register = 0x200;
    gpio_set.data_width = Femii::WIDTH_WORD;
    
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_set), Femii::Fem2ControlMsgException);
    gpio_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_set));

    Femii::GPIO_RW gpio_get = payload_test_msg.get_payload<Femii::GPIO_RW>();
    BOOST_CHECK_EQUAL(gpio_set, gpio_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), gpio_set.the_data.size());    


    //-------- XADC --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_XADC);
    Femii::XADC_RW xadc_set;
    xadc_set.mem_address = 0x01010101;
    xadc_set.mem_register = 0x200;
    xadc_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::XADC_RW>(xadc_set), Femii::Fem2ControlMsgException);
    xadc_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::XADC_RW>(xadc_set));
    Femii::XADC_RW xadc_get = payload_test_msg.get_payload<Femii::XADC_RW>();
    BOOST_CHECK_EQUAL(xadc_set, xadc_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), xadc_set.the_data.size());


    //-------- RAWREG --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_RAWREG);
    Femii::RAWREG_RW rawreg_set;
    rawreg_set.mem_address = 0x01010101;
    rawreg_set.mem_register = 0x200;
    rawreg_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::RAWREG_RW>(rawreg_set), Femii::Fem2ControlMsgException);
    
    rawreg_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::RAWREG_RW>(rawreg_set));
    
    Femii::RAWREG_RW rawreg_get = payload_test_msg.get_payload<Femii::RAWREG_RW >();
    BOOST_CHECK_EQUAL(rawreg_set, rawreg_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), rawreg_set.the_data.size());
    
    
    //-------- DDR --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_DDR);
    Femii::DDR_RW ddr_set;
    ddr_set.mem_address = 0x01010101;
    ddr_set.offset = 0x64;
    ddr_set.page = 0x01;
    ddr_set.data_width = Femii::WIDTH_BYTE;
    
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::DDR_RW>(ddr_set), Femii::Fem2ControlMsgException);
    ddr_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::DDR_RW>(ddr_set));

    Femii::DDR_RW ddr_get = payload_test_msg.get_payload<Femii::DDR_RW>();
    BOOST_CHECK_EQUAL(ddr_set, ddr_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), ddr_set.the_data.size());
    

    //-------- QDR --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QDR);
    Femii::QDR_RW qdr_set;
    qdr_set.mem_address = 0x01010101;
    qdr_set.offset = 0x64;
    qdr_set.page = 0x01;
    qdr_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QDR_RW>(qdr_set), Femii::Fem2ControlMsgException);
    qdr_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::QDR_RW>(qdr_set));

    Femii::QDR_RW qdr_get = payload_test_msg.get_payload<Femii::QDR_RW>();
    BOOST_CHECK_EQUAL(qdr_set, qdr_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), qdr_set.the_data.size());    


    //-------- QSPI --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QSPI);
    Femii::QSPI_RW qspi_set;
    qspi_set.mem_address = 0x01010101;
    qspi_set.offset = 0x64;
    qspi_set.page = 0x01;
    qspi_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_set), Femii::Fem2ControlMsgException);
    qspi_set.the_data.push_back(0x05);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_set));

    Femii::QSPI_RW qspi_get = payload_test_msg.get_payload<Femii::QSPI_RW>();
    BOOST_CHECK_EQUAL(qspi_set, qspi_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), qspi_set.the_data.size());

}


BOOST_AUTO_TEST_CASE(Valid_Roundtrip_Set_Get_For_Read_Payloads){

    int read_length = 1;
    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_READ);
    
    //-------- I2C --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    Femii::I2C_RW i2c_set;
    i2c_set.i2c_bus = 0x01010101;
    i2c_set.slave_address = 0x200;
    i2c_set.i2c_register = 0x200;
    i2c_set.data_width = Femii::WIDTH_BYTE;

    //Setting the payload without a read length provided throws exception
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set), Femii::Fem2ControlMsgException);

    //Setting the payload with a read length provided does not throw an exception
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set, read_length));

    Femii::I2C_RW i2c_get = payload_test_msg.get_payload<Femii::I2C_RW>();
    //Assert that the payload is the same at set as at get.
    BOOST_CHECK_EQUAL(i2c_set, i2c_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), i2c_set.the_data.size());  
    // make sure that the read length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_read_length(), read_length);  
    

    //-------- GPIO --------//
    ++read_length;
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    Femii::GPIO_RW gpio_set;
    gpio_set.mem_address = 0x01010101;
    gpio_set.mem_register = 0x200;
    gpio_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_set, read_length));
    
    Femii::GPIO_RW gpio_get = payload_test_msg.get_payload<Femii::GPIO_RW>();
    BOOST_CHECK_EQUAL(gpio_set, gpio_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), gpio_set.the_data.size());
    BOOST_CHECK_EQUAL(payload_test_msg.get_read_length(), read_length);    


    //-------- XADC --------//
    ++read_length;
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_XADC);
    Femii::XADC_RW xadc_set;
    xadc_set.mem_address = 0x01010101;
    xadc_set.mem_register = 0x200;
    xadc_set.data_width = Femii::WIDTH_WORD;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::XADC_RW>(xadc_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::XADC_RW>(xadc_set, read_length));

    Femii::XADC_RW xadc_get = payload_test_msg.get_payload<Femii::XADC_RW>();
    BOOST_CHECK_EQUAL(xadc_set, xadc_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), xadc_set.the_data.size());
    BOOST_CHECK_EQUAL(payload_test_msg.get_read_length(), read_length);    


    //-------- RAWREG --------//
    ++read_length; 
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_RAWREG);
    Femii::RAWREG_RW rawreg_set;
    rawreg_set.mem_address = 0x01010101;
    rawreg_set.mem_register = 0x200;
    rawreg_set.data_width = Femii::WIDTH_WORD;

    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::RAWREG_RW >(rawreg_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::RAWREG_RW >(rawreg_set, read_length));

    Femii::RAWREG_RW rawreg_get = payload_test_msg.get_payload<Femii::RAWREG_RW >();
    BOOST_CHECK_EQUAL(rawreg_set, rawreg_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), rawreg_set.the_data.size());
    BOOST_CHECK_EQUAL(payload_test_msg.get_read_length(), read_length);    

    
    //-------- DDR --------//
    ++read_length;
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_DDR);
    Femii::DDR_RW ddr_set;
    ddr_set.mem_address = 0x01010101;
    ddr_set.offset = 0x64;
    ddr_set.page = 0x01;
    ddr_set.data_width = Femii::WIDTH_BYTE;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::DDR_RW>(ddr_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::DDR_RW>(ddr_set, read_length));

    Femii::DDR_RW ddr_get = payload_test_msg.get_payload<Femii::DDR_RW>();
    BOOST_CHECK_EQUAL(ddr_set, ddr_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), ddr_set.the_data.size());
    BOOST_CHECK_EQUAL(payload_test_msg.get_read_length(), read_length);    

    
    //-------- QDR --------//
    ++read_length;
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QDR);
    Femii::QDR_RW qdr_set;
    qdr_set.mem_address = 0x01010101;
    qdr_set.offset = 0x64;
    qdr_set.page = 0x01;
    qdr_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QDR_RW>(qdr_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::QDR_RW>(qdr_set, read_length));

    Femii::QDR_RW qdr_get = payload_test_msg.get_payload<Femii::QDR_RW>();
    BOOST_CHECK_EQUAL(qdr_set, qdr_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), qdr_set.the_data.size());
    BOOST_CHECK_EQUAL(payload_test_msg.get_read_length(), read_length);    
    

    //-------- QSPI --------//
    ++read_length;
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QSPI);
    Femii::QSPI_RW qspi_set;
    qspi_set.mem_address = 0x01010101;
    qspi_set.offset = 0x64;
    qspi_set.page = 0x01;
    qspi_set.data_width = Femii::WIDTH_BYTE;

    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_set), Femii::Fem2ControlMsgException);
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_set, read_length));
    Femii::QSPI_RW qspi_get = payload_test_msg.get_payload<Femii::QSPI_RW>();
    BOOST_CHECK_EQUAL(qspi_set, qspi_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.get_data_length(), qspi_set.the_data.size());

}

BOOST_AUTO_TEST_SUITE_END();


//test payloads

// test timestamps

// test msgpack encoding round trip.

// go through all exceptions and check..