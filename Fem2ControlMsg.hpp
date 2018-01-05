#ifndef FEM2CONTROLMSG_H_
#define FEM2CONTROLMSG_H_

#include <exception>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <msgpack.hpp>
#include "boost/bimap.hpp"


#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

typedef unsigned char u8;

/* 
Inline function converting a posix timestamp to an iso string representation
Used to manage encoding of posix_timestamps.
Function is called in the constructor of a fem2controlmsg object, converting the posix timestamp to a string.
*/
inline static std::string to_string_timestamp(boost::posix_time::ptime the_time) {
    return boost::posix_time::to_iso_extended_string(the_time);
}

namespace Femii
{
//! Fem2ControlMsgException
class Fem2ControlMsgException : public std::exception
{
public:

    // Blank Fem2ControlMsgException constructor
    Fem2ControlMsgException(void) throw() :
        error_msg("")
    {};

    // Fem2ControlMsgException with error message
    Fem2ControlMsgException(const std::string the_error_msg) throw() :
        error_msg(the_error_msg)
    {};

    // @override returns the error message as a c string
    virtual const char* what(void) const throw()
    {
        return error_msg.c_str();
    };

    // Destructor
    ~Fem2ControlMsgException(void) throw() {};

private:

    const std::string error_msg;  // The error message

};// fem2controlmsgexception
};// femii namespace

namespace Femii
{

/*

Fem2ControlMsg class 
Message protocol for control and configuration of the FEM-II

*/
class Fem2ControlMsg
{

public: 
    
    //  Supported command types, default = CMD_UNSUPPORTED (-1)
    enum CommandType{

        CMD_UNSUPPORTED = -1,
        CMD_READ = 0,
        CMD_WRITE = 1,
        CMD_CONFIGURE = 2,
        CMD_NOTIFY = 3,
        CMD_ALERT = 4,
        CMD_PLUGIN = 5

    };
    
    //  Supported hardware access types, default = ACCESS_UNSUPPORTED (-1)
    enum AccessType{

        ACCESS_UNSUPPORTED = -1,
        ACCESS_I2C = 0,
        ACCESS_XADC = 1,
        ACCESS_GPIO = 2,
        ACCESS_RAWREG = 3,
        ACCESS_DDR = 4,
        ACCESS_QDR = 5,
        ACCESS_QSPI = 6

    };

    /*  
    Supported message acknowledgement types, default = ACK_UNDEFINED (-1)
    ACK_UNDEFINED used during initial sending of messages or unsolicited messages.
    */
    enum AckState{

        ACK_UNDEFINED = -1,
        NACK = 0,
        ACK = 1,

    };

    //  Supported data widths, default = WIDTH_UNSUPPORTED (-1) -- is this payload?
    enum DataWidth{

        WIDTH_UNSUPPORTED = -1, 
        WIDTH_BYTE = 0,         //8 bits
        WIDTH_WORD = 1,         //16 bits
        WIDTH_LONG = 2,         //32 bits
       
    };

    //  Default constructor, required for msgpack encoding/decoding
    Fem2ControlMsg();

    //  Constructor with defined header fields. TODO : payload
    Fem2ControlMsg(
        CommandType command_type, 
        AccessType access_type,
        AckState ack_state,
        uint16_t req_id,
        int16_t timeout,
        int16_t retries
        //variant type payload
    );

    //  Default Deconstructor
    virtual ~Fem2ControlMsg(){};

    //  Getter methods for the header fields.
    const CommandType get_cmd_type(void) const;
    const AccessType get_access_type(void) const;
    const AckState get_ack_state(void) const;
    const uint16_t get_req_id(void) const;
    const int16_t get_timeout(void) const;
    const int16_t get_retries(void) const;
    const std::string get_string_timestamp(void) const;
    const boost::posix_time::ptime get_posix_timestamp(void) const;
    
    //  TODO Getter methods for the payload


    //  Setter methods for the header fields.
    void set_cmd_type(CommandType cmd_type);
    void set_access_type(AccessType access_type);
    void set_ack_state(AckState ack_state);
    void set_req_id(uint16_t req_id);
    void set_retries(int16_t retries);
    void set_timeout(int16_t timeout);

    //  TODO Setter methods for the payload

    //! Internal bi-directional mapping of command type from string to enumerated CommandType
    typedef boost::bimap<std::string, CommandType> CommandTypeMap;
    typedef CommandTypeMap::value_type CommandTypeMapEntry;

    //! Internal bi-directional mapping of Access type from string to enumerated AccessType
    typedef boost::bimap<std::string, AccessType> AccessTypeMap;
    typedef AccessTypeMap::value_type AccessTypeMapEntry;

    //! Internal bi-directional mapping of Ack state from string to enumerated AckState
    typedef boost::bimap<std::string, AckState> AckStateMap;
    typedef AckStateMap::value_type AckStateMapEntry;

    //! Internal bi-directional mapping of Data width from string to enumerated DataWidth
    typedef boost::bimap<std::string, DataWidth> DataWidthMap;
    typedef DataWidthMap::value_type DataWidthMapEntry;

    //old payload
    void append_payload(std::vector<msgpack::type::variant>);

    //void append_payload(msgpack::type::variant);

    void init_maps();

    std::string print();
    //  Overloaded outstream operator
    friend std::ostream& operator <<(std::ostream& os, Fem2ControlMsg& control_message);

     //! Overloaded equality relational operator
    friend bool operator ==(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg);

    //! Overloaded inequality relational operator
    friend bool operator !=(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg);
    

    //msgpack::type::variant the_payload;


    // old flat payload.
    std::vector<msgpack::type::variant> payload;

    template <typename T> T get_payload_at(int const& index)
    {
        try{
            return get_value<T>(this->payload.at(index));
        }
        catch(...){
            return 0;
            //to do exception
        }
    }

    template <typename T> T get_value(msgpack::type::variant const& value);

    //  Definition of the header struct for msgpack encoding 
    //  TODO Define the payload for msgpack encoding
    MSGPACK_DEFINE_MAP(header, payload);

private:

    //  Struct to hold the defined header types/fields, helper for defining parameters for msgpack encoding
    struct Header{

        boost::posix_time::ptime internal_timestamp_ = boost::posix_time::not_a_date_time;  //!< Message timestamp (internal representation)
        std::string string_timestamp_ = to_string_timestamp(internal_timestamp_);   //string timestamp holds the encodable format for the timestamp
        CommandType cmd_type_ = CMD_UNSUPPORTED;
        AccessType access_type_ = ACCESS_UNSUPPORTED;
        AckState ack_state_ = ACK_UNDEFINED;
        uint16_t req_id_ = 0x0000;
        int16_t timeout_ = -1;
        int16_t retries_ = -1;

        //  Definition of the header as a map type for msgpack encoding
        MSGPACK_DEFINE_MAP(cmd_type_, access_type_, ack_state_, req_id_, timeout_, retries_, string_timestamp_)
    };

    Header header;

    static CommandTypeMap cmd_type_map_;          //!< Bi-directional command type map
    static AccessTypeMap access_type_map_;        //!< Bi-directional access type map
    static DataWidthMap data_width_map_;          //!< Bi-directional data width map
    static AckStateMap ack_state_map_;            //!< Bi-directional ack state map            
    
};  

};

//  Definition of the fem2controlmsg enumerations for msgpack encoding.
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::CommandType);
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::AccessType);
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::AckState);
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::DataWidth);

//  msgpack.hpp must be re-included after defining enums.
#include <msgpack.hpp>
#endif