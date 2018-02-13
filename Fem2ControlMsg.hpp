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
//#include "PayloadStructs.hpp"
#include "PayloadClasses.hpp"
//#include "data_helpers.hpp"

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


namespace Femii{

    struct bytes{
        uint8_t byte;
    };

    //
    //   Struct to hold two bytes in order MSB - > LSB
    //
    struct word_bytes{
        
        uint8_t byte_lsb;
        uint8_t byte_msb;
    };

    //
    //  Struct to hold four bytes in order MSB - > LSB
    //
    struct long_bytes{
        
        uint8_t byte_lsb;
        uint8_t byte2;
        uint8_t byte3;
        uint8_t byte_msb;
    };

    //
    //   Converts two bytes into a word (uint16_t)
    //   Byte params are in order MSB - > LSB
    //
    inline uint16_t from_bytes_to_word(uint8_t MSB, uint8_t LSB){
        
        uint16_t result = 0x0000;
        result = MSB;
        result = result << 8;
        result |= LSB;
        return result;
    };

    //
    //   Converts four bytes into a long (uint32_t)
    //   Byte params are in order MSB - > LSB
    //
    inline uint32_t from_bytes_to_long(uint8_t MSB, uint8_t byte3, uint8_t byte2, uint8_t LSB){
        
        uint32_t result = 0x00000000;
        result = ((MSB << 24) | (byte3 << 16) | (byte2 << 8) | (LSB << 0));
        return result;
    };

    //
    //   Converts a long (uint32_t) into four bytes (uint8_t)
    //   Returns a long_bytes struct 
    //
    inline long_bytes from_long_to_bytes(uint32_t data){
        
        printf("long data: %d", data);
        long_bytes return_;
        return_.byte_msb = (data >> 24) & 0x00FF;
        return_.byte3 = (data >> 16) & 0x00FF;
        return_.byte2 = (data >> 8) & 0x00FF;
        return_.byte_lsb = (data >> 0) & 0x00FF;
        return return_;
    };

    //
    //   Converts a word (uint16_t) into two bytes (uint8_t)
    //   Returns a word_bytes struct 
    //
    inline word_bytes from_word_to_bytes(uint16_t word){
        printf("word data: %d", word);
        word_bytes return_;
        return_.byte_msb = (word >> 8) & 0x00FF;
        return_.byte_lsb = word & 0x00FF;
        return return_;
        
    };

    inline bytes from_long_to_byte(uint32_t data){
        
        bytes the_byte;
        the_byte.byte = (data >> 0) & 0x00FF;
        return the_byte;
        
    }

    template <typename T> void get_bytes(unsigned long result, T &the_payload){

        switch(the_payload.data_width){

            case WIDTH_BYTE:
                {
                    std::cout << "result: " << (uint8_t)result << std::endl;
                    the_payload.the_data.push_back((uint8_t)result);
                    break;
                }

            case WIDTH_WORD:
                {
                    std::cout << "result: " << (uint16_t)result << std::endl;
                    word_bytes the_bytes = from_word_to_bytes((uint16_t)result);
                    the_payload.the_data.push_back(the_bytes.byte_lsb);
                    the_payload.the_data.push_back(the_bytes.byte_msb);
                    break;
                }
            case WIDTH_LONG:
                {
                    std::cout << "result: " << (uint32_t)result << std::endl;
                    long_bytes the_long_bytes = from_long_to_bytes((uint32_t)result);
                    the_payload.the_data.push_back(the_long_bytes.byte_lsb);
                    the_payload.the_data.push_back(the_long_bytes.byte2);
                    the_payload.the_data.push_back(the_long_bytes.byte3);
                    the_payload.the_data.push_back(the_long_bytes.byte_msb);
                    break;
                }
            default: 
                throw Fem2ControlMsgException("Illegal Data Width");
                break;
        }
    }

    template <typename T> uint32_t form_words_longs(T &the_payload){

        uint32_t result;

        switch(the_payload.data_width){

            case WIDTH_BYTE:
                {
                    printf("I'm bytes\n");
                    std::cout << the_payload.print() << std::endl;
                    std::cout << "size = " << the_payload.the_data.size() << std::endl;
                    result = the_payload.the_data.at(0);
                    break;
                }
            case WIDTH_WORD:
                {    
                    printf("I'm Words\n");
                    std::cout << the_payload.print() << std::endl;
                    std::cout << "size = " << the_payload.the_data.size() << std::endl;
                    uint16_t word_data = from_bytes_to_word(the_payload.the_data.at(1), the_payload.the_data.at(0));
                    result = word_data;
                    //result = ddr_memory_reader.write_mem(word_data);
                    break;
                }
            case WIDTH_LONG:
                {
                    std::cout << "size = " << the_payload.the_data.size() << std::endl;
                    uint32_t long_data = from_bytes_to_long(the_payload.the_data.at(3), the_payload.the_data.at(2), the_payload.the_data.at(1), the_payload.the_data.at(0));
                    result = long_data;
                    //result = ddr_memory_reader.write_mem(long_data);
                    break;
                }
            default:
                result = 0; 
                throw Fem2ControlMsgException("Illegal Data Width");
        }

        return result;
    }
    
};

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
    //typedef boost::bimap<std::string, DataWidth> DataWidthMap;
    //typedef DataWidthMap::value_type DataWidthMapEntry;

    // initialise the bimaps for enums
    void init_maps();

    //returns the header and payload in string format.
    std::string print();
    std::vector<msgpack::type::variant> u8_to_variant_vect(std::vector<uint8_t> const& the_vector);

    /* 
    Appends a data point (the_data) to the payload.
    Currently throws a Fem2ControlMsg exception if the payload is undefined(not initialised)
    And throws an excepton if the payload is not a vector and cannot be appended to
    */ 
    template <typename T> void append_payload(T const& the_data)
    {
        if(this->get_payload_type() != "undefined"){
            if(this->payload.is_vector()){
                this->payload.as_vector().push_back(the_data);
            }
            else{
                throw Fem2ControlMsgException("Unable to append data, payload not a vector.");
            }
        }
        else{
            throw Fem2ControlMsgException("Payload has not been initialised");
        } 
    }
    //template <typename T> void set_payload(T const& the_payload);

   //Specialisation of the set_payload template for I2C_READ payloads not natively supported by msgpack
    template<typename E> void set_payload(I2C_RW const& the_payload){
         
       // iterate over integer vector and create a variant
        std::vector<msgpack::type::variant> i2c_rw_vect;

        i2c_rw_vect.push_back(the_payload.i2c_bus);
        i2c_rw_vect.push_back(the_payload.slave_address);
        i2c_rw_vect.push_back(the_payload.i2c_register);
        i2c_rw_vect.push_back(static_cast<int>(the_payload.data_width));
        
        if (! the_payload.the_data.empty()) //if the data hasn't been populated we don't send the vector.
        {
            for (auto i = the_payload.the_data.begin(); i!= the_payload.the_data.end(); i++)
            {
                i2c_rw_vect.push_back(*i);
            }
        } 
        this->payload = i2c_rw_vect;
        //initialise the data length 
        this->data_length_ = the_payload.the_data.size();
      
    }
  

       //Specialisation of the set_payload template for I2C_READ payloads not natively supported by msgpack
    template<typename E> void set_payload(MEM_RW const& the_payload){
         
       // iterate over integer vector and create a variant
        std::vector<msgpack::type::variant> mem_rw_vect;

        mem_rw_vect.push_back(the_payload.mem_address);
        mem_rw_vect.push_back(the_payload.page);
        mem_rw_vect.push_back(the_payload.offset);
        mem_rw_vect.push_back(static_cast<int>(the_payload.data_width));
        
        if (! the_payload.the_data.empty()) //if the data hasn't been populated we don't send the vector.
        {
            //guard for no data
            for (auto i = the_payload.the_data.begin(); i!= the_payload.the_data.end(); i++)
            {
                mem_rw_vect.push_back(*i);
            }
        }

        this->payload = mem_rw_vect;
        //initialise the data length 
        this->data_length_ = the_payload.the_data.size();
    }

    //Specialisation of the set_payload template for I2C_READ payloads not natively supported by msgpack
    template<typename E> void set_payload(Basic_RW const& the_payload){

       // iterate over integer vector and create a variant
        std::vector<msgpack::type::variant> payload_rw_vect;

        payload_rw_vect.push_back(the_payload.mem_address);
        payload_rw_vect.push_back(the_payload.mem_register);
        payload_rw_vect.push_back(static_cast<int>(the_payload.data_width));
        
        if (! the_payload.the_data.empty()) //if the data hasn't been populated we don't send the vector.
        {
            for (auto i = the_payload.the_data.begin(); i!= the_payload.the_data.end(); i++)
            {
                payload_rw_vect.push_back(*i);
            }
        }
        this->payload = payload_rw_vect;
        //initialise the data length 
        this->data_length_ = the_payload.the_data.size();
    }

    //Specialisation of the set_payload template for vector<int> not natively supported by msgpack
    template<typename E> void set_payload(std::vector<int> const& the_payload){
         
       // iterate over integer vector and create a variant
        std::vector<msgpack::type::variant> temp;

        for (auto i = the_payload.begin(); i != the_payload.end(); i++ ) {
            temp.push_back(*i);
        }   
        this->payload = temp;
    }

    //Specialisation of the set_payload template for vector<u8> not natively supported by msgpack
    template<typename E> void set_payload(std::vector<u8> const& the_payload){
       // iterate over integer vector and create a variant
        std::vector<msgpack::type::variant> temp;

        for (auto i = the_payload.begin(); i != the_payload.end(); i++ ) {
            uint8_t test = *i;
            temp.push_back(test);
        }   
        this->payload = temp;
    }
  

    //  Overloaded outstream operator
    friend std::ostream& operator <<(std::ostream& os, Fem2ControlMsg& control_message);

     //! Overloaded equality relational operator
    friend bool operator ==(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg);

    //! Overloaded inequality relational operator
    friend bool operator !=(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg);
    
    // the payload
    msgpack::type::variant payload;

    // Returns a string representation of the payload type.
    std::string get_payload_type();

    // returns a struct corresponding to the type of msg.
    template <typename T> T get_payload();

    //To be used in getting data values in vector payloads
    template <typename T> T get_payload_at(int const& index)
    {
        //should test for vector.
        try{
            return get_value<T>(this->payload.as_vector().at(index));   // does this need a guard.
        }
        catch(...){
            return static_cast<T>(0); // this is nonsense
            //to do exception
        }

    }

    //to be used in getting data values in vector payloads
    template <typename T> T get_value(msgpack::type::variant const& value);

    int data_length_; // length of the data being sent

    //  Definition of the header struct for msgpack encoding 
    MSGPACK_DEFINE_MAP(header, payload, data_length_);

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
    static AckStateMap ack_state_map_;            //!< Bi-directional ack state map            
    
}; 
 

};

//  Definition of the fem2controlmsg enumerations for msgpack encoding.
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::CommandType);
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::AccessType);
MSGPACK_ADD_ENUM(Femii::Fem2ControlMsg::AckState);
MSGPACK_ADD_ENUM(Femii::DataWidth);

//  msgpack.hpp must be re-included after defining enums.
#include <msgpack.hpp>
#endif