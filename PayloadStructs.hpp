#ifndef PAYLOADSTRUCTS_H_
#define PAYLOADSTRUCTS_H_

#include "Fem2ControlMsg.hpp"
#include <stdint.h>

namespace Femii{

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

struct I2C_READ{

    int i2c_bus;
    int slave_address;
    int i2c_register;
    Femii::DataWidth data_width;
    //std::vector<msgpack::type::variant> the_data;

    std::string print(){
        std::string output = "";
        output += "    I2C Bus : " + std::to_string(this->i2c_bus) + ",\n";
        output += "    I2C Slave Address : " + std::to_string(this->slave_address) + ",\n";
        output += "    I2C Register : " + std::to_string(this->i2c_register) + ",\n";
        output += "    Data Width : " + init_data_width_map(this->data_width) + ",\n";
        return output;
    }

    friend bool operator ==(I2C_READ const& lefthand_read, I2C_READ const& righthand_read){
        bool equal = true;
        equal += (lefthand_read.i2c_bus == righthand_read.i2c_bus);
        equal += (lefthand_read.slave_address == righthand_read.slave_address);
        equal += (lefthand_read.i2c_register == righthand_read.i2c_register);
        equal += (lefthand_read.data_width == righthand_read.data_width);
        //the_data equality.
        return equal;
    }
};



struct I2C_WRITE{
    
    uint16_t i2c_bus;
    uint16_t slave_address;
    uint16_t i2c_register;
    Femii::DataWidth data_width;
    std::vector<u8> the_data;
    void print(){}
};

struct I2C_CONFIG{

};

struct XADC_READ{

};

struct XADC_WRITE{

};

struct XADC_CONFIG{

};

struct GPIO_READ{

};

struct GPIO_WRITE{

};

struct RAWREG_READ{

};

struct RAWREG_WRITE{

};

struct RAWREG_CONFIG{

};

//  should these all be MEMORY_READ / MEMORY_WRITE
struct DDR_READ{

};

struct DDR_WRITE{

};

struct DDR_CONFIG{

};

struct QDR_READ{

};

struct QDR_WRITE{

};

struct QDR_CONFIG{

};

struct QSPI_READ{

};

struct QSPI_WRITE{

};

struct QSPI_CONFIGP{

};

};
#endif