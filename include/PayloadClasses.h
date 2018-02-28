#ifndef PAYLOADCLASSES_H_
#define PAYLOADCLASSES_H_


//#include <stdint.h>
//#include <vector>
#include "femii_include.h"
#include "datawidth.h"
#include "Fem2Exception.h"
//#include "Fem2ControlMsg.h"
//#include <boost/optional.hpp>


namespace Femii{

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
        const std::string name_ = "femii_config";
        std::map<msgpack::type::variant, msgpack::type::variant> params; 
        FEMII_CONFIG(){};

        std::string name() const;
        std::string print() const;
        int set_param(std::string name, msgpack::type::variant value, bool recurssive=false, msgpack::type::variant const& map=-1);

        
        template <typename T> T get_param(std::string name, bool reccursive=false, msgpack::type::variant const& map=-1){
            
            bool is_nested = false;
            bool found_in_nested = false;

            //For basic first time calls, iterate over the map
            if(!reccursive){
                std::map<msgpack::type::variant, msgpack::type::variant>::iterator it;// = this->params.find(name);
                for(it = this->params.begin(); it != this->params.end(); ++it){
                    //handle all nested maps first - calling recurssive function
                    if(it->second.is_map()){
                        is_nested = true;
                        try{
                            // call recurssive version
                            return get_param<T>(name, true, it->second.as_map());
                        }
                        catch(Fem2Exception e){
                            found_in_nested = false;
                        }
                    }
                }// if there was no maps or the parameter was not found in the maps just search the flat map
                if (!is_nested || !found_in_nested){
                    it = this->params.find(name);
                    if (it != this->params.end()) {
                        return get_param_value<T>(it->second);
                    }
                    else{
                        std::stringstream sstream; 
                        sstream << "Parameter " << name << " Not Found";
                        throw Fem2Exception(sstream.str());
                    }
                }
            }
            else{ // recurssive version, pass the nested map and either return value or throw exception if not found
                std::map<msgpack::type::variant, msgpack::type::variant> nested_map = map.as_map();
                std::map<msgpack::type::variant, msgpack::type::variant>::iterator it; 
                it = nested_map.find(name);

                if (it != nested_map.end()) {
                    return get_param_value<T>(it->second);
                }
                else{
                    std::stringstream sstream;
                    sstream << "Parameter " << name << " Not Found";
                    throw Fem2Exception(sstream.str());
                }
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