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
    return boost::posix_time::to_iso_extended_string(this->header.internal_timestamp_);
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

//! Overloaded equality relational operator
bool operator ==(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg){

};

//! Overloaded inequality relational operator
bool operator !=(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg){

};

//  TODO - Boost::variant PAYLOAD features


}; //   Fem2ControlMsg
