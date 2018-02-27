#ifndef PAYLOADCLASSES_H_
#define PAYLOADCLASSES_H_


//#include <stdint.h>
//#include <vector>
#include "include_files.h"
#include "datawidth.h"
//#include "Fem2ControlMsg.h"
//#include <boost/optional.hpp>


namespace Femii{
/*
typedef unsigned char u8;

//  Supported data widths, default = WIDTH_UNSUPPORTED (-1) -- moved to payload as specific to these?
enum DataWidth{

    WIDTH_UNSUPPORTED = -1, 
    WIDTH_BYTE = 0,         //could do 2^8  -- 8bits
    WIDTH_WORD = 1,         //16 bits
    WIDTH_LONG = 2,         //32 bits
       
};
 //! Internal bi-directional mapping of Data width from string to enumerated DataWidth
typedef boost::bimap<std::string, DataWidth> DataWidthMap;
typedef DataWidthMap::value_type DataWidthMapEntry;
static DataWidthMap data_width_map_;          //!< Bi-directional data width map

//  Initialises the data_width_map and returns a string representation of the datawidth for printing
inline static std::string init_data_width_map(DataWidth the_width){
    if(data_width_map_.size() == 0){
        data_width_map_.insert(DataWidthMapEntry("byte", WIDTH_BYTE));
        data_width_map_.insert(DataWidthMapEntry("word", WIDTH_WORD));
        data_width_map_.insert(DataWidthMapEntry("long", WIDTH_LONG));
        data_width_map_.insert(DataWidthMapEntry("unsupported width", WIDTH_UNSUPPORTED));
    }
    if (data_width_map_.right.count(the_width))
    {
        return data_width_map_.right.at(the_width);
    }
    else{
        return "undefined"; // to do exception
    }   
}
*/
class Payload{

    public:

        const std::string name_;
        virtual ~Payload(){};
        virtual std::string print() const = 0;
        virtual std::string name() const = 0;

};

//--------- Read-Write Payloads -----------//

class PayloadRW : public Payload{

    public:
        DataWidth data_width; 
        std::vector<uint8_t> the_data;
        virtual ~PayloadRW(){};
};



class I2C_RW : public PayloadRW{

    public:
        uint32_t i2c_bus;
        uint32_t slave_address;
        uint32_t i2c_register;
        const std::string name_ = "i2c_rw";
        I2C_RW(){};
        /*
        std::string print() const{

            std::stringstream sstream;
            sstream << "    I2C Bus : 0x" << std::hex << this->i2c_bus << ",\n";
            sstream << "    I2C Slave Address : 0x" << std::hex << this->slave_address << ",\n";
            sstream << "    I2C Register : 0x" << std::hex << this->i2c_register << ",\n";
            sstream << "    Data Width : " << init_data_width_map(this->data_width) <<  ",\n";
            if (! this->the_data.empty())
            {
                sstream << "    The Data : ";
                for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
                    sstream << "0x" << std::hex << uint32_t(*i) << ", ";
                }   
            }

            std::string output = sstream.str();
            return output;
        };

        std::string name() const{
            return this->name_;
        }

        friend bool operator == (I2C_RW const& lefthand_payload, I2C_RW const& righthand_payload){
            bool equal = true;
            equal &= (lefthand_payload.i2c_bus == righthand_payload.i2c_bus);
            equal &= (lefthand_payload.slave_address == righthand_payload.slave_address);
            equal &= (lefthand_payload.i2c_register == righthand_payload.i2c_register);
            equal &= (lefthand_payload.data_width == righthand_payload.data_width);
            equal &= (lefthand_payload.the_data == righthand_payload.the_data);
            //the_data equality.
            return equal;
        };
        */
        std::string print() const;
        std::string name() const;
        friend bool operator == (I2C_RW const& lefthand_payload, I2C_RW const& righthand_payload);
};

class MEM_RW : public PayloadRW{

    public:
        uint32_t mem_address;
        uint32_t page = 0;
        uint32_t offset;
        MEM_RW(){};

        std::string print() const;
        std::string name() const;
        friend bool operator == (MEM_RW const& lefthand_payload, MEM_RW const& righthand_payload);
};


class QDR_RW : public MEM_RW{

    public:
        const std::string name_ = "qdr_rw";
        QDR_RW(){};

        std::string name() const;       
       
};

class QSPI_RW : public MEM_RW{

    public:
        const std::string name_ = "qspi_rw";
        QSPI_RW(){};

        std::string name() const;    
};

class DDR_RW : public MEM_RW{

    public:
        const std::string name_ = "ddr_rw";
        DDR_RW(){};

        std::string name() const;    
};


class Basic_RW : public PayloadRW{

    public: 
        std::vector<uint8_t> the_data;
        uint32_t mem_address;
        uint32_t mem_register;
        Basic_RW(){};

        std::string print() const;
        std::string name() const;
        friend bool operator == (Basic_RW const& lefthand_payload, Basic_RW const& righthand_payload);

};

class GPIO_RW : public Basic_RW{

    public:
        const std::string name_ = "gpio_rw";
        GPIO_RW(){};

        std::string name() const;
       
};

class XADC_RW : public Basic_RW{

    public:
        const std::string name_ = "xadc_rw";
        XADC_RW(){};

        std::string name() const;
 
};

class RAWREG_RW : public Basic_RW{

    public:
        const std::string name_ = "rawreg_rw";
        RAWREG_RW(){};

        std::string name() const;
};


//--------- Config Payloads -----------//

/*
class PayloadConf : public Payload{
    
    public: 
        //std::vector<uint8_t> the_data;
        virtual ~Payload(){};
};
*/

class FEMII_CONFIG : public Payload{

    public:
        const std::string name_ = "fem-ii_config";
        std::map<std::string, msgpack::type::variant> params; 
        FEMII_CONFIG(){};

        std::string name() const;


        std::string print() const;


        int set_param(std::string name, msgpack::type::variant value);
        
        template <typename T> T get_param(std::string name){

            std::map<std::string, msgpack::type::variant>::iterator it = this->params.find(name);
            if (it != this->params.end()) {
                return get_param_value<T>(it->second);
            }
            else{
                static_cast<T>(0);
            }
        }
        
        template <typename T> T get_param_value(msgpack::type::variant const& value);    
};



class I2C_CONFIG : public Payload{

    public:
        const std::string name_ = "i2c_config";
        DataWidth data_width;
        uint32_t i2c_bus;
        uint32_t slave_address;
        uint32_t i2c_register;
        //std::map<std::string, msgpack::type::variant> param_block;

        // boost optional allows quick checking if parameters have been initialised.
        boost::optional <uint64_t> unsigned_int_param; 
        boost::optional <int> signed_int_param;
        boost::optional <float> float_param;
        boost::optional <std::string> string_param; 
        boost::optional <char> char_param;

        I2C_CONFIG(){};

        std::string print() const;

        std::string name() const;

        friend bool operator == (I2C_CONFIG const& lefthand_payload, I2C_CONFIG const& righthand_payload);

};

class GPIO_CONFIG : public Payload{

};

class XADC_CONFIG : public Payload{

};

class RAWREG_CONFIG : public Payload{

};

class MEM_CONFIG : public Payload{

};


};

#endif