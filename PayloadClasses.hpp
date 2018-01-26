#ifndef PAYLOADCLASSES_H_
#define PAYLOADCLASSES_H_

#include <stdint.h>
#include <vector>
#include "Fem2ControlMsg.hpp"

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

class Payload{

    public:

        Femii::DataWidth data_width;
        virtual ~Payload(){};
        virtual std::string print() = 0;
};

//--------- Read-Write Payloads -----------//

class PayloadRW : public Payload{

    public: 
        std::vector<uint8_t> the_data;
        virtual ~PayloadRW(){};
};



class I2C_RW : public PayloadRW{

    public:
        int i2c_bus;
        int slave_address;
        int i2c_register;
        I2C_RW(){};

        std::string print(){

            std::string output = "";
            output += "    I2C Bus : " + std::to_string(this->i2c_bus) + ",\n";
            output += "    I2C Slave Address : " + std::to_string(this->slave_address) + ",\n";
            output += "    I2C Register : " + std::to_string(this->i2c_register) + ",\n";
            output += "    Data Width : " + init_data_width_map(this->data_width) + ",\n";
            output += "    The Data : ";

            for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
                output += std::to_string(*i) + ", ";
            }   

            return output;
        };

        friend bool operator == (I2C_RW const& lefthand_payload, I2C_RW const& righthand_payload){
            bool equal = true;
            equal += (lefthand_payload.i2c_bus == righthand_payload.i2c_bus);
            equal += (lefthand_payload.slave_address == righthand_payload.slave_address);
            equal += (lefthand_payload.i2c_register == righthand_payload.i2c_register);
            equal += (lefthand_payload.data_width == righthand_payload.data_width);
            equal += (lefthand_payload.the_data == righthand_payload.the_data);
            //the_data equality.
            return equal;
        };

};

class MEM_RW : public PayloadRW{

    public:
        int mem_address;
        int page;
        int offset;
        MEM_RW(){};

        std::string print(){

            std::string output = "";
            output += "    Memory Address : " + std::to_string(this->mem_address) + ",\n";
            output += "    Memory Page : " + std::to_string(this->page) + ",\n";
            output += "    Memory Address Offset : " + std::to_string(this->offset) + ",\n";            
            output += "    Data Width : " + init_data_width_map(this->data_width) + ",\n";
            output += "    The Data : ";

            for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
                output += std::to_string(*i) + ", ";
            }   
            return output;
        }

        friend bool operator == (MEM_RW const& lefthand_payload, MEM_RW const& righthand_payload){
            bool equal = true;
            equal += (lefthand_payload.mem_address == righthand_payload.mem_address);
            equal += (lefthand_payload.page == righthand_payload.page);
            equal += (lefthand_payload.offset == righthand_payload.offset);
            equal += (lefthand_payload.data_width == righthand_payload.data_width);
            equal += (lefthand_payload.the_data == righthand_payload.the_data);
            //the_data equality.
            return equal;            
        };
};


class QDR_RW : public MEM_RW{

    public:
 
        QDR_RW(){};
       
};

class QSPI_RW : public MEM_RW{

    public:
 
        QSPI_RW(){};
       
};

class DDR_RW : public MEM_RW{

    public:
 
        DDR_RW(){};
       
};


class Basic_RW : public PayloadRW{

    public: 
        std::vector<uint8_t> the_data;
        int mem_address;
        int mem_register;
        Basic_RW(){};

        std::string print(){

            std::string output = "";
            output += "    Memory Address : " + std::to_string(this->mem_address) + ",\n";
            output += "    Memory Register : " + std::to_string(this->mem_register) + ",\n";
            output += "    Data Width : " + init_data_width_map(this->data_width) + ",\n";
            output += "    The Data : ";

            for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
                output += std::to_string(*i) + ", ";
            }   

            return output;
        }

        friend bool operator == (Basic_RW const& lefthand_payload, Basic_RW const& righthand_payload){
            bool equal = true;
            equal += (lefthand_payload.mem_address == righthand_payload.mem_address);
            equal += (lefthand_payload.mem_register == righthand_payload.mem_register);
            equal += (lefthand_payload.data_width == righthand_payload.data_width);
            equal += (lefthand_payload.the_data == righthand_payload.the_data);
            //the_data equality.
            return equal;
        };

};

class GPIO_RW : public Basic_RW{

    public:
 
        GPIO_RW(){};
       
};

class XADC_RW : public Basic_RW{

    public:

        XADC_RW(){};
 
};

class RAWREG_RW : public Basic_RW{

    public:

        RAWREG_RW(){};
};


//--------- Config Payloads -----------//

class PayloadConf : public Payload{

};

class I2C_CONFIG : public PayloadConf{

};

class GPIO_CONFIG : public PayloadConf{

};

class XADC_CONFIG : public PayloadConf{

};

class RAWREG_CONFIG : public PayloadConf{

};

class MEM_CONFIG : public PayloadConf{

};


};

#endif