#include "../include/i2c_reader.h"

using namespace Femii;

/*
*   I2C Reader constructor.
*   @Param busnum : unsigned 8 bit integer for the i2c bus number.
*   @Param slave_address : unsigned 8 bit integer for the i2c slave address.
*   @Param i2c_register : unsigned 8 bit integer for the i2c register.
*   @Param width : Femii::DataWidth for the width of the reader.
*/
i2c_reader::i2c_reader(uint8_t bus_num, uint8_t slave_address, uint8_t i2c_register, DataWidth width){

    std::string x = "/dev/i2c-" + std::to_string(bus_num);
    char *y = new char[x.length() + 1]; 
    std::strcpy(y, x.c_str());
    this->bus_access = y; //"dev/i2c-" + (char)bus_num;
    this->slave_address = slave_address;
    this->i2c_register = i2c_register;
    this->width = width;
}

/*
*   Opens the I2C bus for the i2c_reader instance.
*   Returns the bus handle if successfull. 
*   Throws fem2exception on error opening the bus.
*/
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

/*
*   Closes the i2c bus for the i2c_reader instance.
*/
void i2c_reader::close_bus()
{
    close(this->i2c_bus_handle);
}

/*
*   Sets the slave using ioctl for the i2c_reader instance. 
*   Throws fem2exception if the slave was unreachable.
*/
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

/*
*   Reads a single byte from the i2c_reader instance (using the set bus and slave)
*   @param i2c_reg : Defualt parameter set to -1 to read from the member variable i2c_register, if provided - the param i2c_register will be used.
*   Returns the byte data read
*   Throws Fem2Exception on error reading the register.
*/
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

/*
*   Writes a single byte to the i2c_reader instance (using the set bus and slave)
*   @param byte_data : The byte data to write.
*   @param i2c_reg : Defualt parameter set to -1 to write to the member variable i2c_register, if provided - the param i2c_register will be used.
*   Returns the byte data written
*   Throws Fem2Exception on error writing to the register.
*/
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

/*
*   Reads a single word from the i2c_reader instance (using the set bus and slave)
*   @param i2c_reg : Defualt parameter set to -1 to read from the member variable i2c_register, if provided - the param i2c_register will be used.
*   Returns the word data read
*   Throws Fem2Exception on error reading the register.
*/
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

/*
*   Writes a single word to the i2c_reader instance (using the set bus and slave)
*   @param word_data : The word data to write.
*   @param i2c_reg : Defualt parameter set to -1 to write to the member variable i2c_register, if provided - the param i2c_register will be used.
*   Returns the word data written
*   Throws Fem2Exception on error writing to the register.
*/
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
 