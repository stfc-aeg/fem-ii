#include "../include/PayloadClasses.h"

namespace Femii{

/*
*   Prints the contents of an I2C Read/Write Payload to a string stream
*   Returns a string representation of the payload.
*/
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

/*
*   Returns the string member variable "name" of the payload.
*/
std::string I2C_RW::name() const{
    return this->name_;
}

/*
*   Equality operator for I2C Read/Write Payloads.
*   @Param lefthand_payload : const reference to an I2C R/W payload for comparison
*   @Param righthand_payload : const reference to an I2C R/W payload for comparison
*   Returns the true if lefthand_payload and righthand_payload are equal else returns false
*/
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

bool operator != (I2C_RW const& lefthand_payload, I2C_RW const& righthand_payload){
    return !(lefthand_payload == righthand_payload);
};

std::ostream& operator <<(std::ostream& os, I2C_RW const& payload){

    os << payload.print();
    return os;
}

/*
*   Prints the contents of an Memory (DDR, QSPI, QDR) Read/Write Payload to a string stream
*   Returns a string representation of the payload.
*/
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

/*
*   Returns the string member variable "name" of the payload.
*/
std::string MEM_RW::name() const{
    return this->name_;
}

/*
*   Equality operator for Memory (DDR, QDR, QSPI) Read/Write Payloads.
*   @Param lefthand_payload : const reference to an Memory R/W payload for comparison
*   @Param righthand_payload : const reference to an Memory R/W payload for comparison
*   Returns the true if lefthand_payload and righthand_payload are equal else returns false
*/
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

bool operator != (MEM_RW const& lefthand_payload, MEM_RW const& righthand_payload){
    return !(lefthand_payload == righthand_payload);
};

std::ostream& operator <<(std::ostream& os, MEM_RW const& payload){

    os << payload.print();
    return os;
}

/*
*   Returns the string member variable "name" of the payload.
*/
std::string QDR_RW::name() const{
    return this->name_;
}        

/*
*   Returns the string member variable "name" of the payload.
*/
std::string QSPI_RW::name() const{
    return this->name_;
}       

/*
*   Returns the string member variable "name" of the payload.
*/
std::string DDR_RW::name() const{
    return this->name_;
}       

/*
*   Prints the contents of an Basic (XADC, GPIO, RAWREG) Read/Write Payload to a string stream
*   Returns a string representation of the payload.
*/
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

/*
*   Returns the string member variable "name" of the payload.
*/
std::string Basic_RW::name() const{
    return this->name_;
}

/*
*   Equality operator for Basic (XADC, GPIO, RAWREG) Read/Write Payloads.
*   @Param lefthand_payload : const reference to an Basic R/W payload for comparison
*   @Param righthand_payload : const reference to an Basic R/W payload for comparison
*   Returns the true if lefthand_payload and righthand_payload are equal else returns false
*/
bool operator == (Basic_RW const& lefthand_payload, Basic_RW const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.mem_address == righthand_payload.mem_address);
    equal &= (lefthand_payload.mem_register == righthand_payload.mem_register);
    equal &= (lefthand_payload.data_width == righthand_payload.data_width);
    equal &= (lefthand_payload.the_data == righthand_payload.the_data);
    //the_data equality.
    return equal;
};

bool operator != (Basic_RW const& lefthand_payload, Basic_RW const& righthand_payload){
   return !(lefthand_payload == righthand_payload);
};

std::ostream& operator <<(std::ostream& os, Basic_RW const& payload){

    os << payload.print();
    return os;
}

/*
*   Returns the string member variable "name" of the payload.
*/
std::string GPIO_RW::name() const{
    return this->name_;
}

/*
*   Returns the string member variable "name" of the payload.
*/
std::string XADC_RW::name() const{
    return this->name_;
}

/*
*   Returns the string member variable "name" of the payload.
*/
std::string RAWREG_RW::name() const{
    return this->name_;
}

/*
*   Returns the string member variable "name" of the payload.
*/
std::string FEMII_CONFIG::name() const{
    return this->name_;
};

/*
*   Prints the contents of an FEMII CONFIG Payload to a string stream
*   Calls print_map function, a recursive map printing function for nested maps.
*   Returns a string representation of the payload.
*/
std::string FEMII_CONFIG::print() const{
    
    std::stringstream sstream;
    sstream << "\tName : " << this->name_ << ",\n";
    sstream << "\tParams : {\n";
    sstream << this->print_map(this->params);
    sstream << "\t}\n";
    std::string output = sstream.str();
    return output;
}

/*
*   Recursive map printing function for nested maps.
*   @Param map : const reference to the multimap to search and print.
*   @Param recursive :  boolean condition to indicate a recursive call.
*   @Param depth :  integer value to indicate the current depth (in terms of tabs)
*   Returns a string representation of the payload.
*/
std::string FEMII_CONFIG::print_map(std::multimap<msgpack::type::variant, msgpack::type::variant> const& map, bool recursive, int depth) const{
    
    std::stringstream sstream;
    bool was_map = false;
    if(map.size() != 0){
        std::multimap<msgpack::type::variant, msgpack::type::variant>::const_iterator it;
        for(it = map.begin(); it != map.end(); ++it){
            std::string key = it->first.as_string();
            msgpack::type::variant value = it->second;
            
            if(recursive){
                sstream << "\t\t";
                if(was_map && !value.is_multimap()){
                    depth = depth - 2;
                }
                for (int i = 0; i < depth; ++i)
                {
                    sstream << "\t";
                }
            }
            else{
                sstream << "\t\t";
            }

            sstream << key << " : ";
            std::string val = "undefined";
            
            if(value.is_multimap()){
                ++depth;
                was_map = true;
                sstream << "{\n";
                sstream << this->print_map(value.as_multimap(), true, depth);
                sstream << "\t";
                for (int i = 0; i < depth; ++i)
                {
                    sstream << "\t";
                }
                sstream << "},\n";
            }
            else{
                was_map = false;
                if(value.is_int64_t()){
                    val = boost::lexical_cast<std::string>(value.as_int64_t()); 
                    sstream << val << ",\n";
                }
                else if(value.is_uint64_t()){
                    val = boost::lexical_cast<std::string>(value.as_uint64_t()); 
                    sstream << val << ",\n";
                }
                else if(value.is_string()){
                    val = value.as_string();
                    sstream << val << ",\n";
                }
                else if(value.is_bool()){
                    val = boost::lexical_cast<std::string>(value.as_bool());
                    sstream << val << ",\n";
                }
                else if(value.is_double()){
                    val = boost::lexical_cast<std::string>(value.as_double());
                    sstream << val << ",\n";
                }
            }
        }   
    }
    std::string output = sstream.str();
    return output;
}

/*
*   Function used to set parameters within configuration messages. 
*   @Param name : const reference to the string key to use.
*   @Param value : const reference to the variant value to set.
*   Calls count_name, a recursive counting function iterating through nested maps to count occurences of keys
*   Throws Fem2Exception if parameter is not found.
*   Returns nothing.
*/
void FEMII_CONFIG::set_param(const std::string& name, msgpack::type::variant const& value){

    int count = 0;
    count += this->count_name(name, this->params);
    if (count != 0) {
        std::stringstream sstream; 
        sstream << "Parameter " << name << " Already Exists.\n";
        throw Fem2Exception(sstream.str());
        //return -1;
    }
    else{
        this->params.insert(std::pair<msgpack::type::variant, msgpack::type::variant>(name, value));
        //return 0;
    }
};

/*
*   Function used to recursively count occurences of keys within arbitnested maps
*   @Param name : const reference to the string key to use.
*   @Param map : const reference to the variant multimap to search through.
*   Returns count, an integer value representing the number of occurences for the given key.
*/
int FEMII_CONFIG::count_name(const std::string& name, std::multimap<msgpack::type::variant, msgpack::type::variant> const& map){

    int count = 0;
    std::multimap<msgpack::type::variant, msgpack::type::variant>::const_iterator it;
    count = map.count(name);
    if(count != 0){
        return 1;
    }
    else{
        for(it = map.begin(); it != map.end(); ++it){
            std::string key = it->first.as_string();
            if(it->second.is_multimap()){
                count += count_name(name, it->second.as_multimap());            
            }
            else{
                continue;
            }
        }
        return count;
    }
}

/*
*   Template specialisation for integers to get param values
*   @Param value : const reference to a variant value.
*   Returns the integer value
*/
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

/*
*   Template specialisation for strings to get param values
*   @Param value : const reference to a variant value.
*   Returns the string value
*/
template <> std::string FEMII_CONFIG::get_param_value(msgpack::type::variant const& value)
{
    return value.as_string(); 
};

/*
*   Template specialisation for doubles to get param values
*   @Param value : const reference to a variant value.
*   Returns the double value
*/
template <> double FEMII_CONFIG::get_param_value(msgpack::type::variant const& value)
{
    return value.as_double();
};

/*
*   Equality operator for FEMII CONFIG Payloads.
*   @Param lefthand_payload : const reference to an FEMII CONFIG  payload for comparison
*   @Param righthand_payload : const reference to an FEMII CONFIG payload for comparison
*   Returns the true if lefthand_payload and righthand_payload are equal else returns false
*/
bool operator == (FEMII_CONFIG const& lefthand_payload, FEMII_CONFIG const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.name() == righthand_payload.name());
    equal &= (lefthand_payload.params.size() == righthand_payload.params.size());
    equal &= std::equal(lefthand_payload.params.begin(), lefthand_payload.params.end(), righthand_payload.params.begin());
    return equal;            
};   

bool operator != (FEMII_CONFIG const& lefthand_payload, FEMII_CONFIG const& righthand_payload){
   return !(lefthand_payload == righthand_payload);
};

std::ostream& operator <<(std::ostream& os, FEMII_CONFIG const& payload){

    os << payload.print();
    return os;
}    

/*
*   Prints the contents of an I2C CONFIG Payload to a string stream
*   Returns a string representation of the payload.
*/
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

/*
*   Returns the string member variable "name" of the payload.
*/
std::string I2C_CONFIG::name() const{
    return this->name_;
}

/*
*   Equality operator for I2C_CONFIG Payloads.
*   @Param lefthand_payload : const reference to an I2C_CONFIG payload for comparison
*   @Param righthand_payload : const reference to an I2C_CONFIG payload for comparison
*   Returns the true if lefthand_payload and righthand_payload are equal else returns false
*/
bool operator == (I2C_CONFIG const& lefthand_payload, I2C_CONFIG const& righthand_payload){
    bool equal = true;
    equal &= (lefthand_payload.i2c_bus == righthand_payload.i2c_bus);
    equal &= (lefthand_payload.slave_address == righthand_payload.slave_address);
    equal &= (lefthand_payload.i2c_register == righthand_payload.i2c_register);
    equal &= (lefthand_payload.data_width == righthand_payload.data_width);
    // TODO optional param equality

    return equal;
};

bool operator != (I2C_CONFIG const& lefthand_payload, I2C_CONFIG const& righthand_payload){
   return !(lefthand_payload == righthand_payload);
};


std::ostream& operator <<(std::ostream& os, I2C_CONFIG const& payload){
    os << payload.print();
    return os;
}   

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