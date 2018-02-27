#include <boost/test/unit_test.hpp>

#include "Fem2ControlMsg.h"

BOOST_AUTO_TEST_SUITE(Fem2ControlMsgUnitTest);

BOOST_AUTO_TEST_CASE(ValidConstructionWithValues){

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

    // timestamp?
}

BOOST_AUTO_TEST_CASE(ValidConstructionDefaults){
   
    Femii::Fem2ControlMsg test_msg;
    BOOST_CHECK_EQUAL(Femii::Fem2ControlMsg::ACCESS_UNSUPPORTED, test_msg.get_access_type());
    BOOST_CHECK_EQUAL(Femii::Fem2ControlMsg::CMD_UNSUPPORTED, test_msg.get_cmd_type());
    BOOST_CHECK_EQUAL(Femii::Fem2ControlMsg::ACK_UNDEFINED, test_msg.get_ack_state());
    BOOST_CHECK_EQUAL(0x0000, test_msg.get_req_id());
    BOOST_CHECK_EQUAL(-1, test_msg.get_timeout());
    BOOST_CHECK_EQUAL(-1, test_msg.get_retries());

}

BOOST_AUTO_TEST_CASE(InvalidPayloadPrinting){
   
    //No payload has been initiated so printing this message throws an exception?
    Femii::Fem2ControlMsg test_msg;
    BOOST_CHECK_THROW(test_msg.print(), Femii::Fem2ControlMsgException);   

    //now initalise a payload which sets the string representation of the message..?
    Femii::I2C_RW i2c_set;
    test_msg.set_payload<Femii::I2C_RW>(i2c_set);
    BOOST_CHECK_NO_THROW(test_msg.print()); 

}

BOOST_AUTO_TEST_CASE(Payload_Mismatch_Unsupported_Access_Get_Payload){
   
    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_UNSUPPORTED);
    Femii::I2C_RW i2c_set;
    payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set);
    BOOST_CHECK_THROW(Femii::I2C_RW i2c_get = payload_test_msg.get_payload<Femii::I2C_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::GPIO_RW gpio_get = payload_test_msg.get_payload<Femii::GPIO_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::XADC_RW xadc_get = payload_test_msg.get_payload<Femii::XADC_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::RAWREG_RW rawreg_get = payload_test_msg.get_payload<Femii::RAWREG_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::DDR_RW ddr_get = payload_test_msg.get_payload<Femii::DDR_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::QDR_RW qdr_get = payload_test_msg.get_payload<Femii::QDR_RW>(), Femii::Fem2ControlMsgException);
    BOOST_CHECK_THROW(Femii::QSPI_RW qspi_get = payload_test_msg.get_payload<Femii::QSPI_RW>(), Femii::Fem2ControlMsgException);

}

BOOST_AUTO_TEST_CASE(Payload_Mismatch_Incorrect_Access_Type_Set_Payload){
   
    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    
    Femii::I2C_RW i2c_set;
    BOOST_CHECK_NO_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set));

    Femii::GPIO_RW gpio_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_get), Femii::Fem2ControlMsgException);

    Femii::XADC_RW xadc_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::XADC_RW>(xadc_get), Femii::Fem2ControlMsgException);

    Femii::RAWREG_RW rawreg_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::RAWREG_RW>(rawreg_get), Femii::Fem2ControlMsgException);

    Femii::DDR_RW ddr_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::DDR_RW>(ddr_get), Femii::Fem2ControlMsgException);

    Femii::QDR_RW qdr_get;
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QDR_RW>(qdr_get), Femii::Fem2ControlMsgException);

    Femii::QSPI_RW qspi_get; 
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_get), Femii::Fem2ControlMsgException);

    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    BOOST_CHECK_THROW(payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set), Femii::Fem2ControlMsgException);
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

BOOST_AUTO_TEST_CASE(Valid_Roundtrip_WritePayload_Set_Get){

    Femii::Fem2ControlMsg payload_test_msg;
    payload_test_msg.set_cmd_type(Femii::Fem2ControlMsg::CMD_WRITE);
    
    //-------- I2C --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_I2C);
    Femii::I2C_RW i2c_set;
    i2c_set.i2c_bus = 0x01010101;
    i2c_set.slave_address = 0x200;
    i2c_set.i2c_register = 0x200;
    i2c_set.data_width = Femii::WIDTH_BYTE;
    i2c_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::I2C_RW>(i2c_set);
    Femii::I2C_RW i2c_get = payload_test_msg.get_payload<Femii::I2C_RW>();
    BOOST_CHECK_EQUAL(i2c_set, i2c_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, i2c_set.the_data.size());  
    

    //-------- GPIO --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_GPIO);
    Femii::GPIO_RW gpio_set;
    gpio_set.mem_address = 0x01010101;
    gpio_set.mem_register = 0x200;
    gpio_set.data_width = Femii::WIDTH_WORD;
    gpio_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::GPIO_RW>(gpio_set);
    Femii::GPIO_RW gpio_get = payload_test_msg.get_payload<Femii::GPIO_RW>();
    BOOST_CHECK_EQUAL(gpio_set, gpio_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, gpio_set.the_data.size());    


    //-------- XADC --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_XADC);
    Femii::XADC_RW xadc_set;
    xadc_set.mem_address = 0x01010101;
    xadc_set.mem_register = 0x200;
    xadc_set.data_width = Femii::WIDTH_WORD;
    xadc_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::XADC_RW>(xadc_set);
    Femii::XADC_RW xadc_get = payload_test_msg.get_payload<Femii::XADC_RW>();
    BOOST_CHECK_EQUAL(xadc_set, xadc_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, xadc_set.the_data.size());


    //-------- RAWREG --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_RAWREG);
    Femii::RAWREG_RW rawreg_set;
    rawreg_set.mem_address = 0x01010101;
    rawreg_set.mem_register = 0x200;
    rawreg_set.data_width = Femii::WIDTH_WORD;
    rawreg_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::RAWREG_RW >(rawreg_set);
    Femii::RAWREG_RW rawreg_get = payload_test_msg.get_payload<Femii::RAWREG_RW >();
    BOOST_CHECK_EQUAL(rawreg_set, rawreg_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, rawreg_set.the_data.size());
    
    
    //-------- DDR --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_DDR);
    Femii::DDR_RW ddr_set;
    ddr_set.mem_address = 0x01010101;
    ddr_set.offset = 0x64;
    ddr_set.page = 0x01;
    ddr_set.data_width = Femii::WIDTH_BYTE;//??
    ddr_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::QSPI_RW>(ddr_set);//, data_length);
    Femii::DDR_RW ddr_get = payload_test_msg.get_payload<Femii::DDR_RW>();
    BOOST_CHECK_EQUAL(ddr_set, ddr_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, ddr_set.the_data.size());
    

    //-------- QDR --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QDR);
    Femii::QDR_RW qdr_set;
    qdr_set.mem_address = 0x01010101;
    qdr_set.offset = 0x64;
    qdr_set.page = 0x01;
    qdr_set.data_width = Femii::WIDTH_BYTE;//??
    qdr_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::QSPI_RW>(qdr_set);//, data_length);
    Femii::QDR_RW qdr_get = payload_test_msg.get_payload<Femii::QDR_RW>();
    BOOST_CHECK_EQUAL(qdr_set, qdr_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, qdr_set.the_data.size());    


    //-------- QSPI --------//
    payload_test_msg.set_access_type(Femii::Fem2ControlMsg::ACCESS_QSPI);
    Femii::QSPI_RW qspi_set;
    qspi_set.mem_address = 0x01010101;
    qspi_set.offset = 0x64;
    qspi_set.page = 0x01;
    qspi_set.data_width = Femii::WIDTH_BYTE;//??
    qspi_set.the_data.push_back(0x05);
    payload_test_msg.set_payload<Femii::QSPI_RW>(qspi_set);//, data_length);
    Femii::QSPI_RW qspi_get = payload_test_msg.get_payload<Femii::QSPI_RW>();
    BOOST_CHECK_EQUAL(qspi_set, qspi_get);
    // make sure that the data length variable has been set correctly automatically.
    BOOST_CHECK_EQUAL(payload_test_msg.data_length_, qspi_set.the_data.size());


}


//test sets param/header methods

//test payloads

// test timestamps

// test msgpack encoding round trip.