#ifndef FEM2CONTROLMSG_H_
#define FEM2CONTROLMSG_H_

//#include <exception>
//#include <vector>
//#include <iostream>
//#include <string>
//#include <sstream>
//#include "boost/date_time/posix_time/posix_time.hpp"
//#include <msgpack.hpp>
//#include "boost/bimap.hpp"

#include "femii_include.h"
#include "PayloadClasses.h"
#include "datawidth.h"


#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

typedef unsigned char u8;

#define VOID_PAYLOAD "VOID_PAYLOAD"

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

    /*
    *  Struct to hold two bytes in order MSB - > LSB
    */
    struct word_bytes{
        
        uint8_t byte_lsb;
        uint8_t byte_msb;
    };

    /*
    *  Struct to hold four bytes in order MSB - > LSB
    */
    struct long_bytes{
        
        uint8_t byte_lsb;
        uint8_t byte2;
        uint8_t byte3;
        uint8_t byte_msb;
    };


    /*
    *   Converts two bytes into a word (uint16_t),  (r)MSB -> LSB(l)
    *   @Param MSB : Most significant byte.
    *   @Param LSB : Least significant byte.
    *   Returns a word.
    */
    inline uint16_t from_bytes_to_word(uint8_t MSB, uint8_t LSB){
        
        uint16_t result = 0x0000;
        result = MSB;
        result = result << 8;
        result |= LSB;
        return result;
    };


    /*
    *   Converts four bytes into a long (uint32_t),  (r)MSB -> LSB(l)
    *   @Param MSB : Most significant byte.
    *   @Param byte3 : The next byte.
    *   @Param byte2 : The next byte.
    *   @Param LSB : Least significant byte.
    *   Returns a long.
    */
    inline uint32_t from_bytes_to_long(uint8_t MSB, uint8_t byte3, uint8_t byte2, uint8_t LSB){
        
        uint32_t result = 0x00000000;
        result = ((MSB << 24) | (byte3 << 16) | (byte2 << 8) | (LSB << 0));
        return result;
    };

    /*
    *   Converts a long (uint32_t) into four bytes (uint8_t)
    *   @Param data : the long to convert to two bytes.
    *   Returns a long_bytes struct holding the four bytes.
    */
    inline long_bytes from_long_to_bytes(uint32_t data){
        
        printf("long data: %d", data);
        long_bytes return_;
        return_.byte_msb = (data >> 24) & 0x00FF;
        return_.byte3 = (data >> 16) & 0x00FF;
        return_.byte2 = (data >> 8) & 0x00FF;
        return_.byte_lsb = (data >> 0) & 0x00FF;
        return return_;
    };

    /*
    *   Converts a word (uint16_t) into two bytes (uint8_t)
    *   @Param word : the word to convert to two bytes.
    *   Returns a word_bytes struct holding the two bytes.
    */
    inline word_bytes from_word_to_bytes(uint16_t word){
        printf("word data: %d", word);
        word_bytes return_;
        return_.byte_msb = (word >> 8) & 0x00FF;
        return_.byte_lsb = word & 0x00FF;
        return return_;
        
    };

    /*
    *   Converts a long into a single byte.
    *   @Param data : the long to convert to a byte.
    *   Returns a bytes struct holding the byte.
    */
    inline bytes from_long_to_byte(uint32_t data){
        
        bytes the_byte;
        the_byte.byte = (data >> 0) & 0x00FF;
        return the_byte;
        
    }

    /*
    *   Template to convert WORD/LONG data into bytes and to populate the given payload's data vector with the bytes.
    *   @Param result : the WORD/LONG to convert into bytes.
    *   @Param the_payload : const reference a payload object.
    *   Throws Fem2ControlMsgException if the data width is illegal. 
    *   Calls from_word_to_bytes() to convert a word into two bytes.
    *   Calls from_long_to_bytes() to convert a long into four bytes.
    */
    template <typename T> void get_bytes(unsigned long result, T &the_payload){

        switch(the_payload.data_width){

            case WIDTH_BYTE:
                {
                    the_payload.the_data.push_back((uint8_t)result);
                    break;
                }

            case WIDTH_WORD:
                {
                    word_bytes the_bytes = from_word_to_bytes((uint16_t)result);
                    the_payload.the_data.push_back(the_bytes.byte_lsb);
                    the_payload.the_data.push_back(the_bytes.byte_msb);
                    break;
                }
            case WIDTH_LONG:
                {
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

    /*
    *   Template to convert byte data stored in a given payload to words or longs depending on the payload WIDTH.
    *   @Param the_payload : const reference a payload object.
    *   @Param index : index to get the byte data from within the payload's data vector.
    *   Throws Fem2ControlMsgException if the data width is illegal. 
    *   Returns the Word or Long.
    *   Calls from_bytes_to_word() to convert two bytes to a word.
    *   Calls from_bytes_to_long() to convert four bytes to a long.
    */
    template <typename T> uint32_t form_words_longs(T &the_payload, int index){

        uint32_t result;

        switch(the_payload.data_width){

            case WIDTH_BYTE:
                {
                    printf("I'm bytes\n");
                    std::cout << the_payload.print() << std::endl;
                    std::cout << "size = " << the_payload.the_data.size() << std::endl;
                    result = the_payload.the_data.at(index); // for bytes its just every index.
                    break;
                }
            case WIDTH_WORD:
                {    
                    printf("I'm Words\n");
                    std::cout << the_payload.print() << std::endl;
                    std::cout << "size = " << the_payload.the_data.size() << std::endl;
                    uint16_t word_data = from_bytes_to_word(the_payload.the_data.at(index + 1), the_payload.the_data.at(index));
                    result = word_data;
                    break;
                }
            case WIDTH_LONG:
                {
                    std::cout << "size = " << the_payload.the_data.size() << std::endl;
                    uint32_t long_data = from_bytes_to_long(the_payload.the_data.at(index + 3), the_payload.the_data.at(index + 2), the_payload.the_data.at(index + 1), the_payload.the_data.at(index));
                    result = long_data;
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

    /*  
    *   ENUM of Fem2ControlMsg supported message command types, used to identify the type of request/reply.
    *   Default = CMD_UNSUPPORTED (-1)
    */
    enum CommandType{

        CMD_UNSUPPORTED = -1,
        CMD_READ = 0,
        CMD_WRITE = 1,
        CMD_CONFIGURE = 2,
        CMD_NOTIFY = 3,
        CMD_ALERT = 4,
        CMD_PLUGIN = 5

    };
    
   /*  
    *   ENUM of Fem2ControlMsg supported message access types, used to identify the resource type.
    *   Default = ACCESS_UNSUPPORTED (-1)
    *   ACCESS_UNSUPPORTED is used during global FEM-II Configuration Messages.
    */
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
    *   ENUM of Fem2ControlMsg supported message acknowledgement states.
    *   Default = ACK_UNDEFINED (-1)
    *   ACK_UNDEFINED is used during initial sending of messages or unsolicited messages.
    */
    enum AckState{

        ACK_UNDEFINED = -1,
        NACK = 0,
        ACK = 1,

    };

    //  Default constructor, required for msgpack encoding/decoding
    Fem2ControlMsg();

    //  Constructor with defined header fields
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

    std::string string_payload;
    msgpack::type::variant payload = VOID_PAYLOAD;

    //  Getter methods for the header fields.
    const CommandType get_cmd_type(void) const;
    const AccessType get_access_type(void) const;
    const AckState get_ack_state(void) const;
    const uint16_t get_req_id(void) const;
    const int16_t get_timeout(void) const;
    const int16_t get_retries(void) const;
    const std::string get_string_timestamp(void) const;
    const boost::posix_time::ptime get_posix_timestamp(void) const;
    const int get_read_length(void) const;
    const int get_data_length(void) const;
    
    //  Setter methods for the header fields.
    void set_cmd_type(CommandType cmd_type);
    void set_access_type(AccessType access_type);
    void set_ack_state(AckState ack_state);
    void set_req_id(uint16_t req_id);
    void set_retries(int16_t retries);
    void set_timeout(int16_t timeout);

    //! Internal bi-directional mapping of command type from string to enumerated CommandType
    typedef boost::bimap<std::string, CommandType> CommandTypeMap;
    typedef CommandTypeMap::value_type CommandTypeMapEntry;

    //! Internal bi-directional mapping of Access type from string to enumerated AccessType
    typedef boost::bimap<std::string, AccessType> AccessTypeMap;
    typedef AccessTypeMap::value_type AccessTypeMapEntry;

    //! Internal bi-directional mapping of Ack state from string to enumerated AckState
    typedef boost::bimap<std::string, AckState> AckStateMap;
    typedef AckStateMap::value_type AckStateMapEntry;

    void init_maps();
    std::string print();
    std::vector<msgpack::type::variant> u8_to_variant_vect(std::vector<uint8_t> const& the_vector);

    /*
    *   Template to set the variant payload with a I2C_RW object.
    *   @Param the_payload : const reference a I2C_RW object.
    *   @Param read_length : integer value for the number of B/W/L to read default to -1 for write commands.
    *   Throws Fem2ControlMsgException if the Fem2ControlMsg instance access type does not match the I2C_RW payload type provided i.e (access = I2C, payload = QSPI)
    *   Throws Fem2ControlMsgException if CMD_READ and no read_length was provided.
    *   Calls init_maps() to get string representations of the access_type Enum.
    *   Initialises the string_payload payload_print().
    *   Initialises the read_length_ variable using set_read_length() if CMD_READ.
    *   Initialises the data_length_ variable using set_data_length() from the payload.the_data.size()
    *   Clears and shrinks to fit temp vectors for memory deallocation.
    */
    template<typename E> void set_payload(I2C_RW const& the_payload, int read_length=-1){
        
        this->init_maps();
        std::string access_string;
        if(the_payload.name() == "i2c_rw" && this->get_access_type() == ACCESS_I2C)
        {

        // iterate over integer vector and create a variant
            std::vector<msgpack::type::variant> i2c_rw_vect;
            i2c_rw_vect.push_back(the_payload.i2c_bus);
            i2c_rw_vect.push_back(the_payload.slave_address);
            i2c_rw_vect.push_back(the_payload.i2c_register);
            i2c_rw_vect.push_back(static_cast<int>(the_payload.data_width));
            
            if (! the_payload.the_data.empty()) //if the data hasn't been populated we don't send the vector.
            {
                i2c_rw_vect.insert(i2c_rw_vect.end(), the_payload.the_data.begin(), the_payload.the_data.end());
            } 

            this->payload = i2c_rw_vect;
            this->set_data_length(the_payload.the_data.size());
            this->string_payload = the_payload.print();

            if(this->get_cmd_type() == CMD_READ)
            {
                if (read_length != -1){
                    this->set_read_length(read_length);
                }
                else{
                    throw Fem2ControlMsgException("No Read Length Was Provided For A Read Request");
                }
            }    
            //clean up the vector.
            i2c_rw_vect.clear();
            i2c_rw_vect.shrink_to_fit();
            }
        else
        {
            if (access_type_map_.right.count(this->get_access_type()))
            {
                access_string = access_type_map_.right.at(this->get_access_type());
            }
            std::string error_msg = "Cannot Set An Access " + access_string +  " Message with Type I2C Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        } 
    }
  
    /*
    *   Template to set the variant payload with a MEM_RW (DDR, QDR, QSPI) object.
    *   @Param the_payload : const reference a MEM_RW object.
    *   @Param read_length : integer value for the number of B/W/L to read default to -1 for write commands.
    *   Throws Fem2ControlMsgException if the Fem2ControlMsg instance access type does not match the MEM_RW payload type provided i.e (access = DDR, payload = QSPI)
    *   Throws Fem2ControlMsgException if CMD_READ and no read_length was provided.
    *   Calls init_maps() to get string representations of the access_type Enum.
    *   Initialises the string_payload payload_print().
    *   Initialises the read_length_ variable using set_read_length() if CMD_READ.
    *   Initialises the data_length_ variable using set_data_length() from the payload.the_data.size()
    *   Clears and shrinks to fit temp vectors for memory deallocation.
    */
    template<typename E> void set_payload(MEM_RW const& the_payload, int read_length=-1){

        this->init_maps();
        std::string access_string;
        std::string error_msg;

        if (access_type_map_.right.count(this->get_access_type()))
        {
            access_string = access_type_map_.right.at(this->get_access_type());
        }

        if(the_payload.name() == "ddr_rw" && this->get_access_type() != ACCESS_DDR)
        {
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type DDR Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
        else if(the_payload.name() == "qdr_rw" && this->get_access_type() != ACCESS_QDR)
        {
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type QDR Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
        else if(the_payload.name() == "qspi_rw" && this->get_access_type() != ACCESS_QSPI)
        {
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type QSPI Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
        else
        {
        // iterate over integer vector and create a variant
            std::vector<msgpack::type::variant> mem_rw_vect;
            mem_rw_vect.push_back(the_payload.mem_address);
            mem_rw_vect.push_back(the_payload.page);
            mem_rw_vect.push_back(the_payload.offset);
            mem_rw_vect.push_back(static_cast<int>(the_payload.data_width));
            
            if (! the_payload.the_data.empty()) //if the data hasn't been populated we don't send the vector.
            {
                //guard for no data
                mem_rw_vect.insert(mem_rw_vect.end(), the_payload.the_data.begin(), the_payload.the_data.end());
            }
            this->payload = mem_rw_vect;
            this->set_data_length(the_payload.the_data.size());
            // set the string representation of the payload for printing.
            this->string_payload = the_payload.print();
 
            if(this->get_cmd_type() == CMD_READ)
            {
                if (read_length != -1)
                {
                    this->set_read_length(read_length);
                }
                else
                {
                    throw Fem2ControlMsgException("No Read Length Was Provided For A Read Request");
                }
            }    

            //clean up the vector.
            mem_rw_vect.clear();
            mem_rw_vect.shrink_to_fit();
        }
    }

    /*
    *   Template to set the variant payload with a BASIC_RW (XADC, GPIO, RAWREG) object.
    *   @Param the_payload : const reference a BASIC_RW object.
    *   @Param read_length : integer value for the number of B/W/L to read default to -1 for write commands.
    *   Throws Fem2ControlMsgException if the Fem2ControlMsg instance access type does not match the BASIC_RW payload type provided i.e (access = GPIO, payload = XADC)
    *   Throws Fem2ControlMsgException if CMD_READ and no read_length was provided.
    *   Calls init_maps() to get string representations of the access_type Enum.
    *   Initialises the string_payload payload_print().
    *   Initialises the read_length_ variable using set_read_length() if CMD_READ.
    *   Initialises the data_length_ variable using set_data_length() from the payload.the_data.size()
    *   Clears and shrinks to fit temp vectors for memory deallocation.
    */
    template<typename E> void set_payload(Basic_RW const& the_payload, int read_length=-1){
        
        this->init_maps();
        std::string access_string;
        std::string error_msg;
        if (access_type_map_.right.count(this->get_access_type()))
        {
            access_string = access_type_map_.right.at(this->get_access_type());
        }
        if(the_payload.name() == "gpio_rw" && this->get_access_type() != ACCESS_GPIO)
        {
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type GPIO Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
        else if(the_payload.name() == "xadc_rw" && this->get_access_type() != ACCESS_XADC)
        {
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type XADC Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
        else if(the_payload.name() == "rawreg_rw" && this->get_access_type() != ACCESS_RAWREG)
        {
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type Raw Register Read/Write Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
        else
        {
        // iterate over integer vector and create a variant
            std::vector<msgpack::type::variant> payload_rw_vect;
            payload_rw_vect.push_back(the_payload.mem_address);
            payload_rw_vect.push_back(the_payload.mem_register);
            payload_rw_vect.push_back(static_cast<int>(the_payload.data_width));
            
            if (! the_payload.the_data.empty()) //if the data hasn't been populated we don't send the vector.
            {
                payload_rw_vect.insert(payload_rw_vect.end(), the_payload.the_data.begin(), the_payload.the_data.end());
            }
            this->payload = payload_rw_vect;
            this->set_data_length(the_payload.the_data.size());
            this->string_payload = the_payload.print();

            if(this->get_cmd_type() == CMD_READ)
            {
                if (read_length != -1)
                {
                    this->set_read_length(read_length);
                }
                else
                {
                    throw Fem2ControlMsgException("No Read Length Was Provided For A Read Request");
                }
            }
            payload_rw_vect.clear();
            payload_rw_vect.shrink_to_fit();
        }
    }

    //Specialisation of the set_payload template for I2C_CONFIG payloads not natively supported by msgpack
    template<typename E> void set_payload(I2C_CONFIG const& the_payload){

        this->init_maps();
        if(this->get_access_type() == ACCESS_I2C && the_payload.name() == "i2c_config")
        {

        // iterate over integer vector and create a variant
            std::map<msgpack::type::variant, msgpack::type::variant> payload_config_map;
            payload_config_map["i2c_bus"] = the_payload.i2c_bus;
            payload_config_map["i2c_register"] = the_payload.i2c_register;
            payload_config_map["slave_address"] = the_payload.slave_address;
            payload_config_map["data_width"] = static_cast<int>(the_payload.data_width);

            // boost optional parameters hold true if they contain a value, then you deference the value it points to.
            if(the_payload.unsigned_int_param)
            {
                //payload_config_map.insert(std::pair<std::string, uint64_t>("unsigned_int_param", *(the_payload.unsigned_int_param)));
                payload_config_map["unsigned_int_param"] = *(the_payload.unsigned_int_param);
            }
            if(the_payload.signed_int_param)
            {
                payload_config_map["signed_int_param"] = *(the_payload.signed_int_param);
            }
            if(the_payload.float_param)
            {
                payload_config_map["float_param"] = *(the_payload.float_param);
            }
            if(the_payload.string_param)
            {
                payload_config_map["string_param"] = *(the_payload.string_param);
            }
            if(the_payload.char_param)
            {
                payload_config_map["char_param"] = *(the_payload.char_param);
            }      

            this->payload = payload_config_map;
            this->string_payload = the_payload.print();
            payload_config_map.clear();
        }
        else
        {
            std::string access_string;
            std::string error_msg;
            if (access_type_map_.right.count(this->get_access_type()))
            {
                access_string = access_type_map_.right.at(this->get_access_type());
            }
            error_msg = "Cannot Set An Access " + access_string +  " Message with Type I2C Config Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
    }

    /*
    *   Template to set the variant payload with a FEMII_CONFIG object.
    *   @Param the_payload : const reference a FEMII_CONFIG object.
    *   Throws Fem2ControlMsgException if the Fem2ControlMsg instance is not ACCESS_UNSUPPORTED, CMD_CONFIGURE and the payload is not a femii_config object.
    *   Throws Fem2ControlMsgException if the param block of the FEMII Config object is empty.
    *   Calls init_maps() to get string representations of the access_type Enum.
    *   Initialises the string_payload payload_print().
    */
    template<typename E> void set_payload(FEMII_CONFIG const& the_payload){

        this->init_maps();
        if(this->get_access_type() == ACCESS_UNSUPPORTED && this->get_cmd_type() == CMD_CONFIGURE && the_payload.name() == "femii_config") // i just changed this!
        {
            if(the_payload.params.size()!=0)
            {
                this->payload = the_payload.params;
            }
            else
            {
                throw Fem2ControlMsgException("Empty Parameter Block.");
            }
            this->string_payload = the_payload.print();
        }
        else
        {
            std::string access_string;
            std::string error_msg;
            if (access_type_map_.right.count(this->get_access_type()))
            {
                access_string = access_type_map_.right.at(this->get_access_type());
            }
            error_msg = "Cannot Set An Access " + access_string +  " Message with A FEMII Config Payload; Message-Payload Mismatch";
            throw Fem2ControlMsgException(error_msg);
        }
    }

    //  Overloaded outstream operator
    friend std::ostream& operator <<(std::ostream& os, Fem2ControlMsg& control_message);

     //! Overloaded equality relational operator
    friend bool operator ==(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg);

    //! Overloaded inequality relational operator
    friend bool operator !=(Fem2ControlMsg const& lefthand_msg, Fem2ControlMsg const& righthand_msg);
 
    std::string get_payload_type();

    /*
    *   Template for getting variant payloads
    */
    template <typename T> T get_payload();

    /*
    *   Template for getting payload values inside a vector payload.
    *   @Param index : const reference to the index to get the value from.
    *   Throws Fem2ControlMsgException if the value could not be recovered.
    *   Returns the value at the given index.
    */
    template <typename T> T get_payload_at(int const& index)
    {
        //should test for vector.
        try
        {
            return get_value<T>(this->payload.as_vector()[index]);
        }
        catch(...)
        {
            throw Fem2ControlMsgException("Payload value could not be recovered");
        }
    }

    /*
    *   Template for getting parameters inside a map payload.
    *   @Param name : const reference to a string key.
    *   Throws Fem2ControlMsgException if the key was not found.
    *   Returns the value for the given key (name)
    */
    template <typename T> T get_param_at(std::string const& name)
    {
        std::map<msgpack::type::variant, msgpack::type::variant>::iterator it; 
        it = this->payload.as_map().find(name);
        if (it != this->payload.as_map().end()) 
        {
            return get_value<T>(it->second);
        }
        else
        {
            throw Fem2ControlMsgException("Parameter Name Not Found");
        }
    }

    /*
    *   Template for getting values from variant payloads.
    *   @Param value : variant value to be converted.
    */
    template <typename T> T get_value(msgpack::type::variant const& value);

    //  Definition of the header struct for msgpack encoding 
    MSGPACK_DEFINE_MAP(header, payload, data_length_, read_length_);

private:

    int data_length_; // length of the data being sent
    int read_length_; // length of the data to be read.

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
    
    void set_read_length(int read_length);
    void set_data_length(int data_length);   

    /*
    ------- UN-USED FUNCTIONS --------
    
    //Appends a data point (the_data) to the payload.
    //Currently throws a Fem2ControlMsg exception if the payload is undefined(not initialised)
    //And throws an excepton if the payload is not a vector and cannot be appended to
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

    /-------/

    //Specialisation of the set_payload template for vector<int> not natively supported by msgpack
    template<typename E> void set_payload(std::vector<int> const& the_payload){
         
       // iterate over integer vector and create a variant
        std::vector<msgpack::type::variant> temp;

        for (auto i = the_payload.begin(); i != the_payload.end(); i++ ){
            temp.push_back(*i);
        }   
        this->payload = temp;
    }

    /-------/

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

    /------- END OF UN-USED FUNCTIONS --------/

    */

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