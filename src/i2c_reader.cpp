#include "../include/i2c_reader.h"

using namespace Femii;

i2c_reader::i2c_reader(uint8_t bus_num, uint8_t slave_address, uint8_t i2c_register, DataWidth width){

    std::string x = "/dev/i2c-" + std::to_string(bus_num);
    char *y = new char[x.length() + 1]; 
    std::strcpy(y, x.c_str());
    //delete[] y;

    this->bus_access = y; //"dev/i2c-" + (char)bus_num;
    this->slave_address = slave_address;
    this->i2c_register = i2c_register;
    this->width = width;

}

int i2c_reader::open_bus()
{
    this->i2c_bus_handle = open(this->bus_access, O_RDWR);
    if (this->i2c_bus_handle < 0)
    {
        std::stringstream error_stream;
        error_stream << "Error oppening i2c bus : " << this->bus_access << std::endl;
        throw Fem2Exception(error_stream.str());
    }
    return this->i2c_bus_handle;
}

void i2c_reader::close_bus()
{
    close(this->i2c_bus_handle);
}

void i2c_reader::set_slave()
{
    //Set the slave address to communicate with
    if(ioctl(this->i2c_bus_handle, I2C_SLAVE, this->slave_address) < 0)
    {
        std::stringstream error_stream;
        error_stream << "Slave unreachable : " << this->slave_address << std::endl;
        throw Fem2Exception(error_stream.str());
    }
}

uint8_t i2c_reader::read_byte(int i2c_reg)
{
    if (i2c_reg== -1){
        i2c_reg = this->i2c_register;
    }
    int byte_data = i2c_smbus_read_byte_data(this->i2c_bus_handle, i2c_reg);
    if(byte_data < 0)
    {
        std::stringstream error_stream;
        error_stream << "Byte error reading register : " << i2c_reg << std::endl;
        throw Fem2Exception(error_stream.str());
    }
    return byte_data;
}

uint8_t i2c_reader::write_byte(uint8_t byte_data, int i2c_reg)
{
    if (i2c_reg== -1){
        i2c_reg = this->i2c_register;
    }
    int write_data = i2c_smbus_write_byte_data(this->i2c_bus_handle, i2c_reg, byte_data);
    if(write_data < 0)
    {
        std::stringstream error_stream;
        error_stream << "Byte error writing register : " << i2c_reg << std::endl;
        throw Fem2Exception(error_stream.str()); 
    }
    return write_data;
}

uint16_t i2c_reader::read_word(int i2c_reg)
{
    if (i2c_reg== -1){
        i2c_reg = this->i2c_register;
    }    
    int word_data = i2c_smbus_read_word_data(this->i2c_bus_handle, i2c_reg);
    if(word_data < 0)
    {
        std::stringstream error_stream;
        error_stream << "Word error reading register : " << i2c_reg << std::endl;
        throw Fem2Exception(error_stream.str());
    }
    return word_data;
}

uint16_t i2c_reader::write_word(uint16_t word_data, int i2c_reg)
{
    if (i2c_reg== -1){
        i2c_reg = this->i2c_register;
    }
    int write_data = i2c_smbus_write_word_data(this->i2c_bus_handle, i2c_reg, word_data);

    if(write_data < 0)
    {
        std::stringstream error_stream;
        error_stream << "Word error writing register : " << i2c_reg << std::endl;
        throw Fem2Exception(error_stream.str()); 
    }
    return write_data;
}
 