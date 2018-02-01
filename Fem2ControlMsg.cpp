#include "Fem2ControlMsg.hpp"
#include <typeinfo>

typedef unsigned char u8;
namespace Femii
{

//  Default Fem2ControlMsg constructor initialising all fields to their default values.
//  TODO - Payload initialisation
Fem2ControlMsg::Fem2ControlMsg() : header(){
   
    this->header.cmd_type_ = CMD_UNSUPPORTED;
    this->header.access_type_ = ACCESS_UNSUPPORTED;
    this->header.ack_state_ = ACK_UNDEFINED;
    this->header.req_id_ = 0x0000;
    this->header.timeout_ = -1;
    this->header.retries_ = -1;


};

//  Default Fem2ControlMsg constructor with defined field values.
//  TODO - Payload initialisation
Fem2ControlMsg::Fem2ControlMsg(CommandType command_type, AccessType access_type, AckState ack_state, uint16_t req_id, int16_t timeout, int16_t retries) :
    header()      
{
    this->header.cmd_type_ = command_type;
    this->header.access_type_ = access_type;
    this->header.ack_state_ = ack_state;
    this->header.req_id_ = req_id;
    this->header.timeout_ = timeout;
    this->header.retries_ = retries;
    this->header.internal_timestamp_= boost::posix_time::microsec_clock::local_time();  //holds the "real" boost timestamp for time calculcations
    this->header.string_timestamp_ = to_string_timestamp(this->header.internal_timestamp_); //initialise the string timestamp using the posix timestamp for encoding purposes.
};

//  Returns the (enum) command type for the Fem2ControlMsg
const Fem2ControlMsg::CommandType Fem2ControlMsg::get_cmd_type(void) const
{
    return this->header.cmd_type_;
};

//  Returns the (enum) access type for the Fem2ControlMsg
const Fem2ControlMsg::AccessType Fem2ControlMsg::get_access_type(void) const
{
    return this->header.access_type_;
};

//  Returns the (enum) ack state for the Fem2ControlMsg
const Fem2ControlMsg::AckState Fem2ControlMsg::get_ack_state(void) const
{
    return this->header.ack_state_;
};

//  Returns the request id for the Fem2ControlMsg
const uint16_t Fem2ControlMsg::get_req_id(void) const
{
    return this->header.req_id_;
};

//  Returns the number of timeouts specified for the Fem2ControlMsg
const int16_t Fem2ControlMsg::get_timeout(void) const
{
    return this->header.timeout_;
};

//  Returns the retry count for the Fem2ControlMsg
const int16_t Fem2ControlMsg::get_retries(void) const
{
    return this->header.retries_;
};

//  Returns the a iso string representation of the posix timestamp for the Fem2ControlMsg
const std::string Fem2ControlMsg::get_string_timestamp(void) const
{
    return this->header.string_timestamp_;  
};

//  Returns the posix timestamp representation of the string timestamp for the Fem2ControlMsg
const boost::posix_time::ptime Fem2ControlMsg::get_posix_timestamp(void)const
{
    boost::posix_time::ptime pt(boost::posix_time::not_a_date_time);
    try {
        pt = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(this->header.string_timestamp_, 'T');
    }
    catch (...)
    {
        // Do nothing if parse exception occurred - return value of not_a_date_time indicates error
    }
    return pt;
} 

/*
 TODO VALIDATION OF ALL HEADER FIELDS
*/

//  Sets the command type for the Fem2ControlMsg
void Fem2ControlMsg::set_cmd_type(Fem2ControlMsg::CommandType cmd_type){
    this->header.cmd_type_ = cmd_type;
};

//  Sets the access type for the Fem2ControlMsg
void Fem2ControlMsg::set_access_type(Fem2ControlMsg::AccessType access_type){
    this->header.access_type_ = access_type;
};

//  Sets the ack state for the Fem2ControlMsg
void Fem2ControlMsg::set_ack_state(Fem2ControlMsg::AckState ack_state){
    this->header.ack_state_ = ack_state;
};

//  Sets the request id for the Fem2ControlMsg
void Fem2ControlMsg::set_req_id(uint16_t req_id){
    this->header.req_id_ = req_id;
};

//  Sets the retry count for the Fem2ControlMsg
void Fem2ControlMsg::set_retries(int16_t retries){
    this->header.retries_ = retries;
};

//  Sets the timeout field for the Fem2ControlMsg
void Fem2ControlMsg::set_timeout(int16_t timeout){
    this->header.timeout_ = timeout;
};

// initialisation of the bi-maps between enumerations and string representations
void Fem2ControlMsg::init_maps(){

    if (cmd_type_map_.size() == 0){
        this->cmd_type_map_.insert(CommandTypeMapEntry("read", CMD_READ));
        this->cmd_type_map_.insert(CommandTypeMapEntry("write", CMD_WRITE));
        this->cmd_type_map_.insert(CommandTypeMapEntry("config", CMD_CONFIGURE));
        this->cmd_type_map_.insert(CommandTypeMapEntry("notify", CMD_NOTIFY));
        this->cmd_type_map_.insert(CommandTypeMapEntry("alert", CMD_ALERT));
        this->cmd_type_map_.insert(CommandTypeMapEntry("plugin", CMD_PLUGIN));
        this->cmd_type_map_.insert(CommandTypeMapEntry("unsupported command", CMD_UNSUPPORTED));
    }
    if (access_type_map_.size() == 0){
        this->access_type_map_.insert(AccessTypeMapEntry("i2c", ACCESS_I2C));
        this->access_type_map_.insert(AccessTypeMapEntry("xadc", ACCESS_XADC));
        this->access_type_map_.insert(AccessTypeMapEntry("gpio", ACCESS_GPIO));
        this->access_type_map_.insert(AccessTypeMapEntry("rawreg", ACCESS_RAWREG));
        this->access_type_map_.insert(AccessTypeMapEntry("ddr", ACCESS_DDR));
        this->access_type_map_.insert(AccessTypeMapEntry("qdr", ACCESS_QDR));
        this->access_type_map_.insert(AccessTypeMapEntry("qspi", ACCESS_QSPI));
        this->access_type_map_.insert(AccessTypeMapEntry("unsupported acccess", ACCESS_UNSUPPORTED));
    }
    if (ack_state_map_.size() == 0){
        this->ack_state_map_.insert(AckStateMapEntry("ack", ACK));
        this->ack_state_map_.insert(AckStateMapEntry("nack", NACK));
        this->ack_state_map_.insert(AckStateMapEntry("ack undefined", ACK_UNDEFINED));
    }
    //else do nothing
}


// returns the type of the payload as a string representation, if not initialised, return_type=undefined.
std::string Fem2ControlMsg::get_payload_type(){

    std::string return_type = "undefined";
    if(this->payload.is_vector()){
        return_type = "vector";
    }
    else if(this->payload.is_vector_char()){
        return_type = "vector char";
    }
    else if(this->payload.is_map()){
        return_type = "map";
    }
    else if(this->payload.is_multimap()){
        return_type = "multi map";
    }
    else if(this->payload.is_uint64_t()){
        return_type = "unsigned int";
    }
    else if(this->payload.is_bool()){
        return_type = "bool";
    }
    else if(this->payload.is_string()){
        return_type = "string";
    }
    else if(this->payload.is_int64_t()){
        return_type = "signed int";
    }
    return return_type;

}

template <> I2C_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_I2C){

        I2C_RW i2c_payload;
        i2c_payload.i2c_bus = this->get_payload_at<int>(0);
        i2c_payload.slave_address = this->get_payload_at<int>(1);
        i2c_payload.i2c_register = this->get_payload_at<int>(2);
        i2c_payload.data_width = this->get_payload_at<DataWidth>(3);
            
        offset = 4;
        if(!(this->data_length_ == 0))
        {
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                i2c_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }
        return i2c_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not I2C Type");
    }
}

template <> DDR_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_DDR){

        DDR_RW ddr_payload;
        ddr_payload.mem_address = this->get_payload_at<int>(0);
        ddr_payload.page = this->get_payload_at<int>(1);
        ddr_payload.offset = this->get_payload_at<int>(2);
        ddr_payload.data_width = this->get_payload_at<DataWidth>(3);
            
        offset = 4;
        if(!(this->data_length_ == 0))
        {
            /// guard for read sends 
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                ddr_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }
        return ddr_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not DDR Type");
    }
}

template <> QDR_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_QDR){

        QDR_RW qdr_payload;
        qdr_payload.mem_address = this->get_payload_at<int>(0);
        qdr_payload.page = this->get_payload_at<int>(1);
        qdr_payload.offset = this->get_payload_at<int>(2);
        qdr_payload.data_width = this->get_payload_at<DataWidth>(3);
            
        offset = 4;
        if(!(this->data_length_ == 0))
        {
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                qdr_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }
        return qdr_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not QDR Type");
    }
}

template <> QSPI_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_QSPI){

        QSPI_RW qspi_payload;
        qspi_payload.mem_address = this->get_payload_at<int>(0);
        qspi_payload.page = this->get_payload_at<int>(1);
        qspi_payload.offset = this->get_payload_at<int>(2);
        qspi_payload.data_width = this->get_payload_at<DataWidth>(3);
            
        offset = 4;
        if(!(this->data_length_ == 0))
        {
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                qspi_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }
        return qspi_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not QSPI Type");
    }
}


template <> GPIO_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_GPIO){
        GPIO_RW gpio_payload;
        gpio_payload.mem_address = this->get_payload_at<int>(0);
        gpio_payload.mem_register = this->get_payload_at<int>(1);
        gpio_payload.data_width = this->get_payload_at<DataWidth>(2);
            
        offset = 3;
        if(!(this->data_length_ == 0))
        {
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                gpio_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }   
        return gpio_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not GPIO Type");
    }
}

template <> XADC_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_XADC){
        XADC_RW xadc_payload;
        xadc_payload.mem_address = this->get_payload_at<int>(0);
        xadc_payload.mem_register = this->get_payload_at<int>(1);
        xadc_payload.data_width = this->get_payload_at<DataWidth>(2);
            
        offset = 3;
        if(!(this->data_length_ == 0))
        {
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                xadc_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }   
        return xadc_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not XADC Type");
    }
}


template <> RAWREG_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_RAWREG){
        RAWREG_RW rreg_payload;
        rreg_payload.mem_address = this->get_payload_at<int>(0);
        rreg_payload.mem_register = this->get_payload_at<int>(1);
        rreg_payload.data_width = this->get_payload_at<DataWidth>(2);
            
        offset = 3;
        if(!(this->data_length_ == 0))
        {
            for(int i=offset; i < (this->data_length_ + offset); i++){
                //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                rreg_payload.the_data.push_back(this->get_payload_at<int>(i));
            }
        }   
        return rreg_payload; 
    }
    else{
        throw Fem2ControlMsgException("Payload Is Not RAWREG Type");
    }
}

/*
 //these do not work.
template <> Basic_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
    
        std::cout << "getting GPIO payload" << std::endl;
        Basic_RW this_payload;
        this_payload.mem_address = this->get_payload_at<int>(0);
        this_payload.mem_register = this->get_payload_at<int>(1);
        this_payload.data_width = this->get_payload_at<DataWidth>(2);
            
        offset = 3;
        // guard for read sends 
        for(int i=offset; i < (this->data_length_ + offset); i++){
            //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
            this_payload.the_data.push_back(this->get_payload_at<int>(i));
        }   
        return this_payload; 
}

template <> MEM_RW Fem2ControlMsg::get_payload(){

    int offset; // offset for start of the data
  
    MEM_RW mem_payload;
    mem_payload.mem_address = this->get_payload_at<int>(0);
    mem_payload.page = this->get_payload_at<int>(1);
    mem_payload.offset = this->get_payload_at<int>(2);
    mem_payload.data_width = this->get_payload_at<DataWidth>(3);
            
    offset = 4;
    //std::cout << "get length: " << std::to_string(this->data_length_) << std::endl;
    /// guard for read sends 
    for(int i=offset; i < (this->data_length_ + offset); i++){
        //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
        mem_payload.the_data.push_back(this->get_payload_at<int>(i));
    }
    return mem_payload; 
}
*/

//  TODO Validation/ Exceptions?
std::string Fem2ControlMsg::print(){

    init_maps();

    //  replace this with illegals?
    std::string command_string = "undefined";
    std::string access_string = "undefined";
    std::string ack_string = "undefined";

    //  returns the string representations of Fem2ENUMS
    if (cmd_type_map_.right.count(get_cmd_type()))
    {
        command_string = cmd_type_map_.right.at(get_cmd_type());
    }
    if (access_type_map_.right.count(get_access_type()))
    {
        access_string = access_type_map_.right.at(get_access_type());
    }
    if (ack_state_map_.right.count(get_ack_state()))
    {
        ack_string = ack_state_map_.right.at(get_ack_state());
    }

    std::string output = "";
    output += "HEADER : {\n";
    output += "    Command : " + command_string + ",\n";
    output += "    Access : " + access_string + ",\n";
    output += "    Ack State: " + ack_string + ",\n";
    output += "    Timeout : " + std::to_string(get_timeout()) + ",\n"; 
    output += "    Retries : " + std::to_string(get_retries()) + ",\n";
    output += "    Request ID: " + std::to_string(get_req_id()) + ",\n";
    output += "    Timestamp : " + get_string_timestamp() + ",\n}\n";
    output += "PAYLOAD : {\n";
    
    // test by type of message -> 
    if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_I2C){
        I2C_RW printout = this->get_payload<I2C_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    
    else if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_GPIO){
        GPIO_RW printout = this->get_payload<GPIO_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    else if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_XADC){
        XADC_RW printout = this->get_payload<XADC_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    else if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_RAWREG){
        RAWREG_RW printout = this->get_payload<RAWREG_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    else if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_QDR){
        QDR_RW printout = this->get_payload<QDR_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    else if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_DDR){
        DDR_RW printout = this->get_payload<DDR_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    else if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE) && this->get_access_type() == ACCESS_QSPI){
        QSPI_RW printout = this->get_payload<QSPI_RW>(); // object creation - deletion fix
        output += printout.print();
    }
    else{
        throw Fem2ControlMsgException("Unrecognised Payload Type");
    }
    
    output += "\n}\n";
    return output;
}

template <> uint8_t Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return static_cast<uint8_t>(value.as_uint64_t());
}

template <> int Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    if (value.is_uint64_t())
    {
        return static_cast<int>(value.as_uint64_t());
    }
    else if (value.is_int64_t())
    {
        return static_cast<int>(value.as_int64_t());
    }
    else
    {
        std::cout << "Not an int64 or an uint64" << std::endl;
        return 0;
        //TODO exception here
    }
}

template <> std::string Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_string();
}

template <> double Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_double();
}

//  template specialisation for returning data values within the payload as a DataWidth ENUM.
template <> DataWidth Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    int as_number;
    DataWidth return_width;

    if (value.is_uint64_t())
    {
        as_number = static_cast<int>(value.as_uint64_t());
    }
    else if (value.is_int64_t())
    {
        as_number =  static_cast<int>(value.as_int64_t());
    }
    else
    {
        return_width = WIDTH_UNSUPPORTED;
        return return_width;
        //TODO exception here
    }

    switch(as_number){
        case -1:
            return_width = WIDTH_UNSUPPORTED;
            break;
        case 0:
            return_width = WIDTH_BYTE;
            break;
        case 1:
            return_width = WIDTH_WORD;
            break;
        case 2:
            return_width = WIDTH_LONG;
            break;
        default: 
            return_width = WIDTH_UNSUPPORTED;
            break;
    }
    return return_width; 
}


//figure out returning vectors?
template <> std::vector<char> Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_vector_char();
}

//converts a u8 vector into a vector - variant.
std::vector<msgpack::type::variant> Fem2ControlMsg::u8_to_variant_vect(std::vector<uint8_t> const& the_vector){

    // iterate over integer vector and create a variant
    std::vector<msgpack::type::variant> temp;

    for (auto i = the_vector.begin(); i != the_vector.end(); i++ ) {
        uint8_t test = static_cast<uint8_t>(*i);
        temp.push_back(test);
    }   
    return temp;
}


//! Overloaded equality relational operator
bool operator ==(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg){

    bool equal = true;

    equal += (lefthand_msg.get_access_type() == righthand_msg.get_access_type());
    equal += (lefthand_msg.get_ack_state() == righthand_msg.get_ack_state());
    equal += (lefthand_msg.get_cmd_type() == righthand_msg.get_cmd_type());
    equal += (lefthand_msg.get_req_id() == righthand_msg.get_req_id());
    equal += (lefthand_msg.get_posix_timestamp() == righthand_msg.get_posix_timestamp());
    equal += (lefthand_msg.get_string_timestamp() == righthand_msg.get_string_timestamp());
    equal += (lefthand_msg.get_retries() == righthand_msg.get_retries());
    equal += (lefthand_msg.get_timeout() == righthand_msg.get_timeout());

    //TODO - Payload equality 
    return equal;

};

//! Overloaded inequality relational operator
bool operator !=(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg){
    return !(lefthand_msg == righthand_msg);
};

std::ostream& operator <<(std::ostream& os, Fem2ControlMsg& control_message){
    os << control_message.print();
    return os;
}

// Definition of static member variables used for type and value mapping
Fem2ControlMsg::CommandTypeMap Fem2ControlMsg::cmd_type_map_;
Fem2ControlMsg::AccessTypeMap Fem2ControlMsg::access_type_map_;
Fem2ControlMsg::AckStateMap Fem2ControlMsg::ack_state_map_;
}; //   Fem2ControlMsg
