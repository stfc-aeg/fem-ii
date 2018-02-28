#include "../include/PayloadClasses.h"

namespace Femii{

std::string I2C_RW::print() const{

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

std::string I2C_RW::name() const{
    return this->name_;
}

bool operator == (I2C_RW const& lefthand_payload, I2C_RW const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.i2c_bus == righthand_payload.i2c_bus);
    equal &= (lefthand_payload.slave_address == righthand_payload.slave_address);
    equal &= (lefthand_payload.i2c_register == righthand_payload.i2c_register);
    equal &= (lefthand_payload.data_width == righthand_payload.data_width);
    equal &= (lefthand_payload.the_data == righthand_payload.the_data);
    //the_data equality.
    return equal;
};


std::string MEM_RW::print() const{

    std::stringstream sstream;
    sstream << "    Memory Address : 0x" << std::hex << this->mem_address << ",\n";
    sstream << "    Memory Page : 0x" << std::hex << this->page << ",\n";
    sstream << "    Memory Address Offset : 0x" << std::hex << this->offset << ",\n";            
    sstream << "    Data Width : " << init_data_width_map(this->data_width) << ",\n";
    if (! this->the_data.empty()) //if there's no data don't print it
    {
        sstream << "    The Data : ";
        for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
            sstream << "0x" << std::hex << uint32_t(*i) << ", ";
        }
    }   
    std::string output = sstream.str();
    return output;
}

std::string MEM_RW::name() const{
    return this->name_;
}

bool operator == (MEM_RW const& lefthand_payload, MEM_RW const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.mem_address == righthand_payload.mem_address);
    equal &= (lefthand_payload.page == righthand_payload.page);
    equal &= (lefthand_payload.offset == righthand_payload.offset);
    equal &= (lefthand_payload.data_width == righthand_payload.data_width);
    equal &= (lefthand_payload.the_data == righthand_payload.the_data);
    //the_data equality.
    return equal;            
};

std::string QDR_RW::name() const{
    return this->name_;
}        

std::string QSPI_RW::name() const{
    return this->name_;
}       

std::string DDR_RW::name() const{
    return this->name_;
}       

std::string Basic_RW::print() const{

    std::stringstream sstream;
    sstream << "    Memory Address : 0x" << std::hex << this->mem_address << ",\n";
    sstream << "    Memory Register : 0x" << std::hex << this->mem_register << ",\n";
    sstream << "    Data Width : " << init_data_width_map(this->data_width) << ",\n";
    
    if (! this->the_data.empty()) //if there's no data don't print it
    { 
        sstream <<  "    The Data : ";
        for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
            sstream << "0x" << std::hex << uint32_t(*i) << ", ";
        }   
    }
    std::string output = sstream.str();
    return output;
}

std::string Basic_RW::name() const{
    return this->name_;
}

bool operator == (Basic_RW const& lefthand_payload, Basic_RW const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.mem_address == righthand_payload.mem_address);
    equal &= (lefthand_payload.mem_register == righthand_payload.mem_register);
    equal &= (lefthand_payload.data_width == righthand_payload.data_width);
    equal &= (lefthand_payload.the_data == righthand_payload.the_data);
    //the_data equality.
    return equal;
};

std::string GPIO_RW::name() const{
    return this->name_;
}

std::string XADC_RW::name() const{
    return this->name_;
}

std::string RAWREG_RW::name() const{
    return this->name_;
}

std::string FEMII_CONFIG::name() const{
    return this->name_;
};

std::string FEMII_CONFIG::print() const{
    
    std::stringstream sstream;
    sstream << "    Name : " << this->name_ << ",\n";
    /*
    sstream << "    Memory Address : 0x" << std::hex << this->mem_address << ",\n";
    sstream << "    Memory Register : 0x" << std::hex << this->mem_register << ",\n";
    sstream << "    Data Width : " << init_data_width_map(this->data_width) << ",\n";
    
    if (! this->the_data.empty()) //if there's no data don't print it
    { 
        sstream <<  "    The Data : ";
        for (auto i = this->the_data.begin(); i != this->the_data.end(); i++ ) {
            sstream << "0x" << std::hex << uint32_t(*i) << ", ";
        }   
    }
    */
    std::string output = sstream.str();
    return output;
}

// sets parameter with value unless it already exists
int FEMII_CONFIG::set_param(std::string name, msgpack::type::variant value, bool recurssive, msgpack::type::variant const& map){

    int count = 0;
    if(!recurssive){
        std::map<msgpack::type::variant, msgpack::type::variant>::iterator it;
        for(it = this->params.begin(); it != this->params.end(); ++it){
            //handle all nested maps first, adding to count if the parameter name is found
            if(it->second.is_map()){
                count += set_param(name, value, true, it->second.as_map());
            }
        }    
        if (count == 0){ // if count is still 0, check the flat map, setting the value and return 0 if not found 
            if (this->params.count(name) == 0){
                this->params[name] = value;
                return  0; 
            }
            else{// return -1 if it was found
                return -1;
            }
        }
        else{ // if count !=0 return -1 it was found in the nested maps
            return -1;
        }   
    }
    else{// if it is a recurssive call, count the nested map, return 0 if not found, 1 if found.
        if(map.as_map().count(name) == 0){
            return 0; 
        }
        else{
            return 1;
        }  
    }

};

template <> int FEMII_CONFIG::get_param_value(msgpack::type::variant const& value)
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
};


template <> std::string FEMII_CONFIG::get_param_value(msgpack::type::variant const& value)
{
    return value.as_string(); 
};

template <> double FEMII_CONFIG::get_param_value(msgpack::type::variant const& value)
{
    return value.as_double();
};     


std::string I2C_CONFIG::print() const{

    std::stringstream sstream;
    sstream << "    I2C Bus : 0x" << std::hex << this->i2c_bus << ",\n";
    sstream << "    I2C Slave Address : 0x" << std::hex << this->slave_address << ",\n";
    sstream << "    I2C Register : 0x" << std::hex << this->i2c_register << ",\n";
    sstream << "    Data Width : " << init_data_width_map(this->data_width) <<  ",\n";

    if(this->unsigned_int_param){
        sstream << "    Unsigned Int Param : " << std::to_string(*(this->unsigned_int_param)) <<  ",\n";
    }
    if(this->signed_int_param){
        sstream << "    Signed Int Param : " << std::to_string(*(this->signed_int_param)) <<  ",\n";
    }
    if(this->float_param){
        sstream << "    Float Param : " << *(this->float_param) <<  ",\n";
    }
    if(this->string_param){
        sstream << "    String Param : " << *(this->string_param) <<  ",\n";
    }
    if(this->char_param){
        sstream << "    Char param : " << *(this->char_param) <<  ",\n";
    }

    std::string output = sstream.str();
    return output;
};

std::string I2C_CONFIG::name() const{
    return this->name_;
}

bool operator == (I2C_CONFIG const& lefthand_payload, I2C_CONFIG const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.i2c_bus == righthand_payload.i2c_bus);
    equal &= (lefthand_payload.slave_address == righthand_payload.slave_address);
    equal &= (lefthand_payload.i2c_register == righthand_payload.i2c_register);
    equal &= (lefthand_payload.data_width == righthand_payload.data_width);
    // TODO optional param equality

    return equal;
};

/*
class GPIO_CONFIG : public Payload{

};

class XADC_CONFIG : public Payload{

};

class RAWREG_CONFIG : public Payload{

};

class MEM_CONFIG : public Payload{

};
*/
};