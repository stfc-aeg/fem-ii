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
        //  Overloaded outstream operator
        friend std::ostream& operator <<(std::ostream& os, I2C_RW const& payload);
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
        friend std::ostream& operator <<(std::ostream& os, MEM_RW const& payload);
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
        friend std::ostream& operator <<(std::ostream& os, Basic_RW const& payload);

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
        std::multimap<msgpack::type::variant, msgpack::type::variant> params; 
        FEMII_CONFIG(){};

        std::string name() const;
        std::string print() const;
        std::string print_map(std::multimap<msgpack::type::variant, msgpack::type::variant> const& map, bool recursive=false, int depth=0) const;
        void set_param(const std::string& name, msgpack::type::variant const& value);//, bool recurssive=false, msgpack::type::variant const& map=-1);
        friend bool operator == (FEMII_CONFIG const& lefthand_payload, FEMII_CONFIG const& righthand_payload);
        friend bool operator != (FEMII_CONFIG const& lefthand_payload, FEMII_CONFIG const& righthand_payload);
        friend std::ostream& operator <<(std::ostream& os, FEMII_CONFIG const& payload);

        /*
        *   Function template used to get parameters within configuration messages. 
        *   Calls search_map, a recursive search function iterating through nested maps
        *   Throws Fem2Exception if parameter is not found.
        *   Returns the value for the key if was found.
        */
        template <typename T> T get_param(const std::string& name){//, bool reccursive=false, msgpack::type::variant const& map=-1, bool is_nested=false, bool found_in_nested=false){

            try{
                return search_map<T>(this->params, name);
            }
            catch(Fem2Exception e){
                std::stringstream sstream; 
                sstream << "Parameter " << name << " Not Found";
                throw Fem2Exception(sstream.str());
            }
        }
        
        /*
        *   Function template used to recursivly search through nested maps for keys.
        *   Throws Fem2Exception if parameter is not found.
        *   Returns the value for the key if was found.
        */
        template <typename T> T search_map(std::multimap<msgpack::type::variant, msgpack::type::variant> const& map, std::string const& name){

            bool found; 
            std::multimap<msgpack::type::variant, msgpack::type::variant>::const_iterator it;
            it = map.find(name);
            if (it != map.end()){
                found = true;
                return get_param_value<T>(it->second);
            }
            else{
                for(it = map.begin(); it != map.end(); ++it){
                    std::string key = it->first.as_string();
                    if(it->second.is_multimap()){
                        try{
                            return search_map<T>(it->second.as_multimap(), name);
                        }
                        catch(Fem2Exception e){
                            continue;  
                        }               
                    }
                    else{
                        found = false;
                    }
                }
                if(!found){
                    std::stringstream sstream; 
                    sstream << "Parameter " << name << " Not Found";
                    throw Fem2Exception(sstream.str());
                }  
            }
        } 
        
        template <typename T> T get_param_value(msgpack::type::variant const& value);
        int count_name(const std::string& name, std::multimap<msgpack::type::variant, msgpack::type::variant> const& map); 
};



class I2C_CONFIG : public Payload{

    public:
        const std::string name_ = "i2c_config";
        DataWidth data_width;
        uint32_t i2c_bus;
        uint32_t slave_address;
        uint32_t i2c_register;

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
        friend std::ostream& operator <<(std::ostream& os, I2C_CONFIG const& payload);

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