#include "../include/Fem2ControlMsg.h"
#include <typeinfo>

typedef unsigned char u8;
namespace Femii
{

/*
*   Default Fem2ControlMsg Constructor with default values, required for MsgPack Encoding/Decoding.
*   Initialises all header variables to their default values.
*   Initialises the internal_timestamp_ using posix_time::local_time().
*   Initialises the string_timestamp by calling to_string_timestamp().
*   On Decode as<Fem2ControlMsg>(), timestamps are initialised from the encoded content.
*/
Fem2ControlMsg::Fem2ControlMsg() : header(){
   
    this->header.cmd_type_ = CMD_UNSUPPORTED;
    this->header.access_type_ = ACCESS_UNSUPPORTED;
    this->header.ack_state_ = ACK_UNDEFINED;
    this->header.req_id_ = 0x0000;
    this->header.timeout_ = -1;
    this->header.retries_ = -1;
    this->header.internal_timestamp_= boost::posix_time::microsec_clock::local_time(); 
    this->header.string_timestamp_ = to_string_timestamp(this->header.internal_timestamp_); 

};

/*
*   Fem2ControlMsg Constructor with values.
*   @ param command_type : Fem2ControlMsg::CommandType the command type i.e. READ/WRITE
*   @ param access_type : Fem2ControlMsg::AccessType the access type i.e. GPIO/I2C etc.
*   @ param ack_state : Fem2ControlMsg::AckState the ack state i.e. ACK/NACK.
*   @ param req_id : uint16_t unique id for the Fem2ControlMsg.
*   @ param timeout : int16_t value for message timeout in ms.
*   @ param retries : int16_t value for messsage retries.
*   Initialises the internal_timestamp_ using posix_time::local_time().
*   Initialises the string_timestamp by calling to_string_timestamp().
*/
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

/*
*   Gets the command type value for the Fem2ControlMsg.
*   Returns Fem2ControlMsg::CommandType enum value, cmd_type_.
*/
const Fem2ControlMsg::CommandType Fem2ControlMsg::get_cmd_type(void) const
{
    return this->header.cmd_type_;
};

/*
*   Gets the access type value for the Fem2ControlMsg.
*   Returns Fem2ControlMsg::AccessType enum value, access_type_.
*/
const Fem2ControlMsg::AccessType Fem2ControlMsg::get_access_type(void) const
{
    return this->header.access_type_;
};

/*
*   Gets the ack state value for the Fem2ControlMsg.
*   Returns Fem2ControlMsg::AckState enum value, ack_state_.
*/
const Fem2ControlMsg::AckState Fem2ControlMsg::get_ack_state(void) const
{
    return this->header.ack_state_;
};

/*
*   Gets the req_id value for the Fem2ControlMsg.
*   Returns the int req_id_.
*/
const uint16_t Fem2ControlMsg::get_req_id(void) const
{
    return this->header.req_id_;
};

/*
*   Gets the timeout value for the Fem2ControlMsg.
*   Returns the int timeout_.
*/
const int16_t Fem2ControlMsg::get_timeout(void) const
{
    return this->header.timeout_;
};

/*
*   Gets the retry value for the Fem2ControlMsg.
*   Returns the int retries_.
*/
const int16_t Fem2ControlMsg::get_retries(void) const
{
    return this->header.retries_;
};

/*
*   Gets the string timestamp ISO string format for the Fem2ControlMsg.
*   Returns the string timestamp_.
*/
const std::string Fem2ControlMsg::get_string_timestamp(void) const
{
    return this->header.string_timestamp_;  
};

/*
*   Gets the timestamp in boost::posix format for the Fem2ControlMsg.
*   Returns the posix timestamp, set to not_a_date_time if timestamp could not be parsed.
*/
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
};

/*
*   Gets the private member variable read_length_.
*   Returns read_length_
*/    
const int Fem2ControlMsg::get_read_length(void) const{
    return this->read_length_;
};

/*
*   Gets the private member variable data_length_.
*   Returns data_length
*/    
const int Fem2ControlMsg::get_data_length(void) const{
    return this->data_length_;
};

/*
*   Sets the command type variable for the Fem2ControlMsg Header.
*   @Param cmd_type : Fem2ControlMsg CommandType enum value.
*/
void Fem2ControlMsg::set_cmd_type(Fem2ControlMsg::CommandType cmd_type){
    this->header.cmd_type_ = cmd_type;
};

/*
*   Sets the access type variable for the Fem2ControlMsg Header.
*   @Param access_type : Fem2ControlMsg AccessType enum value.
*/
void Fem2ControlMsg::set_access_type(Fem2ControlMsg::AccessType access_type){
    this->header.access_type_ = access_type;
};

/*
*   Sets the ack state variable for the Fem2ControlMsg Header.
*   @Param ack_state : Fem2ControlMsg AckState enum value.
*/
void Fem2ControlMsg::set_ack_state(Fem2ControlMsg::AckState ack_state){
    this->header.ack_state_ = ack_state;
};

/*
*   Sets the timeout variable for the Fem2ControlMsg Header.
*   @Param req_id : 16 bit unsigned integer value for number of req_id for the message.
*   Throws Fem2ControlMsgException if req_id is less than 0.
*/
void Fem2ControlMsg::set_req_id(uint16_t req_id){
    if(req_id < 0){
        throw Fem2ControlMsgException("Request ID cannot be negative");
    }
    this->header.req_id_ = req_id;
};

/*
*   Sets the timeout variable for the Fem2ControlMsg Header.
*   @Param retries : 16 bit signed integer value for number of retries for the message.
*   Throws Fem2ControlMsgException if retries is less than 0.
*/
void Fem2ControlMsg::set_retries(int16_t retries){
    if(retries < 0){
        throw Fem2ControlMsgException("Retry count cannot be negative");
    }
    this->header.retries_ = retries;
};

/*
*   Sets the timeout variable for the Fem2ControlMsg Header.
*   @Param read_length : integer value for the length of data to read.
*   Throws Fem2ControlMsgException if the message is type READ and the read_length is less than 1.
*/
void Fem2ControlMsg::set_timeout(int16_t timeout){
    this->header.timeout_ = timeout;
};

/*
*   Sets the private member variable read_length_.
*   @Param read_length : integer value for the length of data to read.
*   Throws Fem2ControlMsgException if the message is type READ and the read_length is less than 1.
*/
void Fem2ControlMsg::set_read_length(int read_length){
    if(this->get_cmd_type() == CMD_READ && read_length < 1){
        throw Fem2ControlMsgException("Read Length Cannot Be Less than 1 For Read Commands."); // check this is ok
    }
    this->read_length_ = read_length;
}

/*
*   Sets the private member variable data_length_.
*   @Param data_length : integer value for the length of data.
*   Throws Fem2ControlMsgException if the message is type WRITE and the data_length is less than 1.
*/
void Fem2ControlMsg::set_data_length(int data_length){
    if(this->get_cmd_type() == CMD_WRITE && data_length < 1){
        throw Fem2ControlMsgException("Data Length Cannot Be Less than 1 For Write Commands.");//check this is right
    }
    this->data_length_ = data_length;
}

/*
*   Initialises the bidirectional maps for the AccessType, CommandType and AckState Enums.
*   Checks that the maps have not already been initialised.
*/
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
        this->access_type_map_.insert(AccessTypeMapEntry("I2C", ACCESS_I2C));
        this->access_type_map_.insert(AccessTypeMapEntry("XADC", ACCESS_XADC));
        this->access_type_map_.insert(AccessTypeMapEntry("GPIO", ACCESS_GPIO));
        this->access_type_map_.insert(AccessTypeMapEntry("RAW REG", ACCESS_RAWREG));
        this->access_type_map_.insert(AccessTypeMapEntry("DDR", ACCESS_DDR));
        this->access_type_map_.insert(AccessTypeMapEntry("QDR", ACCESS_QDR));
        this->access_type_map_.insert(AccessTypeMapEntry("QSPI", ACCESS_QSPI));
        this->access_type_map_.insert(AccessTypeMapEntry("unsupported acccess", ACCESS_UNSUPPORTED));
    }
    if (ack_state_map_.size() == 0){
        this->ack_state_map_.insert(AckStateMapEntry("ACK", ACK));
        this->ack_state_map_.insert(AckStateMapEntry("NACK", NACK));
        this->ack_state_map_.insert(AckStateMapEntry("ACK undefined", ACK_UNDEFINED));
    }
    //else do nothing
}


/*
*   Tests the payload type for the given Fem2ControlMsg Instance. 
*   Returns a string representation of the payload type.
*/
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

/*
*   Template specialisation to get the variant payload as a I2C_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a I2C_RW Payload.
*   Returns a populated I2C_RW object.
*/
template <> I2C_RW Fem2ControlMsg::get_payload(){

    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_I2C){

            I2C_RW i2c_payload;
            i2c_payload.i2c_bus = this->get_payload_at<int>(0);
            i2c_payload.slave_address = this->get_payload_at<int>(1);
            i2c_payload.i2c_register = this->get_payload_at<int>(2);
            i2c_payload.data_width = this->get_payload_at<DataWidth>(3);
                
            offset = 4;
            if(!(this->get_data_length() == 0))
            {
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    i2c_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = i2c_payload.print();
            return i2c_payload; 
        }// if read/write/type i2c
        else{
            throw Fem2ControlMsgException("Payload Is Not I2C Type");
        }
    } // if not unspecified
}

/*
*   Template specialisation to get the variant payload as a DDR_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a DDR_RW Payload.
*   Returns a populated DDR_RW object.
*/
template <> DDR_RW Fem2ControlMsg::get_payload(){

    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_DDR){

            DDR_RW ddr_payload;
            ddr_payload.mem_address = this->get_payload_at<int>(0);
            ddr_payload.page = this->get_payload_at<int>(1);
            ddr_payload.offset = this->get_payload_at<int>(2);
            ddr_payload.data_width = this->get_payload_at<DataWidth>(3);
                
            offset = 4;
            if(!(this->get_data_length() == 0))
            {
                /// guard for read sends 
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    ddr_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = ddr_payload.print();
            return ddr_payload; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not DDR Type");
        }
    }
}

/*
*   Template specialisation to get the variant payload as a QDR_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a QDR_RW Payload.
*   Returns a populated QDR_RW object.
*/
template <> QDR_RW Fem2ControlMsg::get_payload(){

    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_QDR){

            QDR_RW qdr_payload;
            qdr_payload.mem_address = this->get_payload_at<int>(0);
            qdr_payload.page = this->get_payload_at<int>(1);
            qdr_payload.offset = this->get_payload_at<int>(2);
            qdr_payload.data_width = this->get_payload_at<DataWidth>(3);
                
            offset = 4;
            if(!(this->get_data_length() == 0))
            {
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    qdr_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = qdr_payload.print();
            return qdr_payload; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not QDR Type");
        }
    }
}

/*
*   Template specialisation to get the variant payload as a QSPI_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a QSPI_RW Payload.
*   Returns a populated QSPI_RW object.
*/
template <> QSPI_RW Fem2ControlMsg::get_payload(){

    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_QSPI){

            QSPI_RW qspi_payload;
            qspi_payload.mem_address = this->get_payload_at<int>(0);
            qspi_payload.page = this->get_payload_at<int>(1);
            qspi_payload.offset = this->get_payload_at<int>(2);
            qspi_payload.data_width = this->get_payload_at<DataWidth>(3);
                
            offset = 4;
            if(!(this->get_data_length() == 0))
            {
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    qspi_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = qspi_payload.print();
            return qspi_payload; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not QSPI Type");
        }
    }
}


/*
*   Template specialisation to get the variant payload as a GPIO_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a GPIO_RW Payload.
*   Returns a populated GPIO_RW object.
*/
template <> GPIO_RW Fem2ControlMsg::get_payload(){

    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_GPIO){
            GPIO_RW gpio_payload;
            gpio_payload.mem_address = this->get_payload_at<int>(0);
            gpio_payload.mem_register = this->get_payload_at<int>(1);
            gpio_payload.data_width = this->get_payload_at<DataWidth>(2);
                
            offset = 3;
            if(!(this->get_data_length() == 0))
            {
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    gpio_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = gpio_payload.print();   
            return gpio_payload; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not GPIO Type");
        }
    }
}

/*
*   Template specialisation to get the variant payload as a XADC_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a XADC_RW Payload.
*   Returns a populated RAWREG_RW object.
*/
template <> XADC_RW Fem2ControlMsg::get_payload(){
    
    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_XADC){
            XADC_RW xadc_payload;
            xadc_payload.mem_address = this->get_payload_at<int>(0);
            xadc_payload.mem_register = this->get_payload_at<int>(1);
            xadc_payload.data_width = this->get_payload_at<DataWidth>(2);
                
            offset = 3;
            if(!(this->get_data_length() == 0))
            {
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    xadc_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = xadc_payload.print();   
            return xadc_payload; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not XADC Type");
        }
    }
}

/*
*   Template specialisation to get the variant payload as a RAWREG_RW payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a RAWREG_RW Payload.
*   Returns a populated RAWREG_RW object.
*/
template <> RAWREG_RW Fem2ControlMsg::get_payload(){
    
    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        int offset; // offset for start of the data
        if ((this->get_cmd_type() == CMD_READ || this->get_cmd_type() == CMD_WRITE || this->get_cmd_type() == CMD_NOTIFY) && this->get_access_type() == ACCESS_RAWREG){
            RAWREG_RW rreg_payload;
            rreg_payload.mem_address = this->get_payload_at<int>(0);
            rreg_payload.mem_register = this->get_payload_at<int>(1);
            rreg_payload.data_width = this->get_payload_at<DataWidth>(2);
                
            offset = 3;
            if(!(this->get_data_length() == 0))
            {
                for(int i=offset; i < (this->get_data_length() + offset); i++){
                    //std::cout << "value : " << std::to_string(this->get_payload_at<int>(i)) << std::endl;
                    rreg_payload.the_data.push_back(this->get_payload_at<int>(i));
                }
            }
            // set the string representation of the payload for printing.
            this->string_payload = rreg_payload.print();     
            return rreg_payload; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not RAWREG Type");
        }
    }
}

/*
*   Template specialisation to get the variant payload as a I2C_CONFIG payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a I2C_CONFIG Payload.
*   Returns a populated I2C_CONFIG object.
*/
template <> I2C_CONFIG Fem2ControlMsg::get_payload(){
    
    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{

        if (this->get_cmd_type() == CMD_CONFIGURE && this->get_access_type() == ACCESS_I2C){

            I2C_CONFIG i2c_config;
            i2c_config.i2c_bus = this->get_param_at<int>("i2c_bus");
            i2c_config.i2c_register = this->get_param_at<int>("i2c_register");
            i2c_config.slave_address = this->get_param_at<int>("slave_address");
            i2c_config.data_width = this->get_param_at<DataWidth>("data_width");

            // TODO - manage non existant param.
            i2c_config.unsigned_int_param = this->get_param_at<int>("unsigned_int_param");
            i2c_config.signed_int_param = this->get_param_at<int>("signed_int_param");
            i2c_config.float_param = this->get_param_at<double>("float_param");
            i2c_config.string_param = this->get_param_at<std::string>("string_param");
            i2c_config.char_param = static_cast<char>(this->get_param_at<uint8_t>("char_param"));

            this->string_payload = i2c_config.print();     
            return i2c_config; 
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not I2C CONFIG Type");
        }
    }
}

/*
*   Template specialisation to get the variant payload as a FEMII CONFIG payload class. 
*   Throws Fem2ControlMsg Exception if the payload is VOID
*   Throws Fem2ControlMsg Exception if the payload is not a multimap
*   Throws Fem2ControlMsg Exception if the Fem2ControlMsg is incompatible with a FEMII CONFIG Payload.
*   Returns a populated FEMII CONFIG object.
*/
template <> FEMII_CONFIG Fem2ControlMsg::get_payload(){
    
    if(this->payload.is_string() && this->payload.as_string() == VOID_PAYLOAD){
        throw Fem2ControlMsgException("Missing Payload.");
    }
    else{
        if (this->get_cmd_type() == CMD_CONFIGURE && this->get_access_type() == ACCESS_UNSUPPORTED){

            FEMII_CONFIG femii_config;
            if(this->payload.is_multimap()){
                std::cout << "Im a map/multimap" <<std::endl;
                std::cout << " was a multimap" << std::endl;
                std::multimap<msgpack::type::variant, msgpack::type::variant> mmap;
                mmap = this->payload.as_multimap();
                femii_config.params = mmap;
                this->string_payload = femii_config.print();     
                return femii_config; 
            }
            else{
                throw Fem2ControlMsgException("Incompatible payload type, payload is not a key value structure.");
            }
        }
        else{
            throw Fem2ControlMsgException("Payload Is Not FEMII CONFIG Type");
        }
    }
}


/*
*   Prints the Fem2ControlMsg as a string for use in overloaded outstream operator.
*   Calls init_maps() to initialise bi-directional maps to get string representations for enumerations
*   Throws Fem2ControlMsg Exception if the payload has not been initialised i.e. string_payload has not been set.
*   Returns a string of the Fem2ControlMsg header and payload.
*/
std::string Fem2ControlMsg::print(){

    init_maps();
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
    
    if(this->string_payload == ""){
        throw Fem2ControlMsgException("Payload has not been initialised, use get_payload<> before printing.");
    }
    else{
        output += this->string_payload;
    }
    output += "\n}\n";
    return output;
}

/*
*   Template specialisation to get unsigned 8 bit ints from variant payload.
*   @Param value : const reference a variant value.
*   Returns a unsigned 8 bit int.
*/
template <> uint8_t Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return static_cast<uint8_t>(value.as_uint64_t());
}

/*
*   Template specialisation to get integers from variant payload.
*   @Param value : const reference a variant value.
*   Returns a integer.
*/
template <> int Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{

    if (value.is_uint64_t())
    {
        return static_cast<unsigned int>(value.as_uint64_t());
    }
    else if (value.is_int64_t())
    { 
        return static_cast<signed int>(value.as_int64_t());
    }
    else
    {
        std::cout << "Not an int64 or an uint64" << std::endl;
        return 0;
        //TODO exception here
    }
}

/*
*   Template specialisation to get strings from variant payload.
*   @Param value : const reference a variant value.
*   Returns a string.
*/
template <> std::string Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_string();
}

/*
*   Template specialisation to get Femii::DatWidths from variant payload.
*   @Param value : const reference a variant value.
*   Returns a double.
*/
template <> double Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_double();
}

/*
*   Template specialisation to get Femii::DatWidths from variant payload.
*   @Param value : const reference a variant value.
*   Returns a DataWidth.
*/
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

/*
*   Template specialisation to get vector<chars> from variant payload.
*   @Param value : const reference a variant value.
*   Returns a vector of variants.
*/
template <> std::vector<char> Fem2ControlMsg::get_value(msgpack::type::variant const& value)
{
    return value.as_vector_char();
}

/*
*   Converts a vector of u8 to a vector of variants.
*   @Param the_vector : const reference a vector of u8s.
*   Returns a vector of variants.
*/
std::vector<msgpack::type::variant> Fem2ControlMsg::u8_to_variant_vect(std::vector<uint8_t> const& the_vector){

    // iterate over integer vector and create a variant
    std::vector<msgpack::type::variant> temp;
    for (auto i = the_vector.begin(); i != the_vector.end(); i++ ) {
        uint8_t test = static_cast<uint8_t>(*i);
        temp.push_back(test);
    }   
    return temp;
}

/*
*   Equality operator for Fem2ControlMsg 
*   @Param lefthand_msg : const reference to a Fem2ControlMsg for comparison
*   @Param righthand_msg : const reference to a Fem2ControlMsg for comparison
*   Returns true if lefthand_msg and righthand_msg are equal else returns false
*/
bool operator ==(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg){

    bool equal = true;

    equal &= (lefthand_msg.get_access_type() == righthand_msg.get_access_type());
    equal &= (lefthand_msg.get_ack_state() == righthand_msg.get_ack_state());
    equal &= (lefthand_msg.get_cmd_type() == righthand_msg.get_cmd_type());
    equal &= (lefthand_msg.get_req_id() == righthand_msg.get_req_id());
    equal &= (lefthand_msg.get_posix_timestamp() == righthand_msg.get_posix_timestamp());
    equal &= (lefthand_msg.get_string_timestamp() == righthand_msg.get_string_timestamp());
    equal &= (lefthand_msg.get_retries() == righthand_msg.get_retries());
    equal &= (lefthand_msg.get_timeout() == righthand_msg.get_timeout());
    //equal &= (lefthand_msg.get_data_length() == righthand_msg.get_data_length());
    //equal &= (lefthand_msg.get_read_length() == righthand_msg.get_read_length());

    //TODO - Payload equality 
    return equal;

};

/*
*   In-Equality operator for Fem2ControlMsg 
*   @Param lefthand_payload : const reference to a Fem2ControlMsg for comparison
*   @Param righthand_payload : const reference to a Fem2ControlMsg for comparison
*   Calls equality operator.
*   Returns true if lefthand_msg and righthand_msg are not equal else returns false
*/
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
