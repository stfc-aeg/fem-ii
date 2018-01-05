#include "Fem2ControlMsg.hpp"

typedef unsigned char u8;
namespace Femii
{

//  Default Fem2ControlMsg constructor initialising all fields to their default values.
//  TODO - Payload initialisation
Fem2ControlMsg::Fem2ControlMsg() : header(){
   
    this->header.cmd_type_ = CommandType::CMD_UNSUPPORTED;
    this->header.access_type_ = AccessType::ACCESS_UNSUPPORTED;
    this->header.ack_state_ = AckState::ACK_UNDEFINED;
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
    //   to do - add data width to payload
    if(data_width_map_.size() == 0){
        this->data_width_map_.insert(DataWidthMapEntry("byte", WIDTH_BYTE));
        this->data_width_map_.insert(DataWidthMapEntry("word", WIDTH_WORD));
        this->data_width_map_.insert(DataWidthMapEntry("long", WIDTH_LONG));
        this->data_width_map_.insert(DataWidthMapEntry("unsupported width", WIDTH_UNSUPPORTED));
    }
}

//  helper to check the type of the variant payload.
void Fem2ControlMsg::get_payload_type(){

    if(this->payload.is_vector()){
        printf("vector!");
    }
    else if(this->payload.is_vector_char()){
        printf("vector char!");
    }
    else if(this->payload.is_map()){
        printf("map!");
    }
    else if(this->payload.is_multimap()){
        printf("multi map!");
    }
    else if(this->payload.is_uint64_t()){
        printf("unsigned int!");
    }
    else if(this->payload.is_bool()){
        printf("bool!");
    }
    else if(this->payload.is_string()){
        printf("string!");
    }
    else if(this->payload.is_int64_t()){
        printf("int!");
    }
    else{
        printf("not a type I know");
    }

}

//  TODO Validation/ Exceptions?
std::string Fem2ControlMsg::print(){

    //guard init if already initialised.
    init_maps();    // move initialisation?

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
    /* TODO
    if (data_width_map_.right.count(get_data_width()))
    {
        data_width = data_width_map_.right.at(get_data_width());
    }
    */
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
    output += "PAYLOAD : {\n}\n";

    return output;
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

/*
//figure out returning vectors?
template <> std::vector<int> Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_vector();
    
    std::vector<int> return_vector;
    
    for (unsigned i=0; i < value.size(); i++){
        return_vector.push_back(static_cast<int>(value.at(i)));
    }
   
    return return_vector;
    
}
*/

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

//  TODO - Boost::variant PAYLOAD features

// Definition of static member variables used for type and value mapping
Fem2ControlMsg::CommandTypeMap Fem2ControlMsg::cmd_type_map_;
Fem2ControlMsg::AccessTypeMap Fem2ControlMsg::access_type_map_;
Fem2ControlMsg::DataWidthMap Fem2ControlMsg::data_width_map_;
Fem2ControlMsg::AckStateMap Fem2ControlMsg::ack_state_map_;
}; //   Fem2ControlMsg
