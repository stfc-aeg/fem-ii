#ifndef I2C_READER_H_
#define I2C_READER_H_


#include <string> //
#include <sstream> //
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "femii_include.h"
#include "datawidth.h"
#include "Fem2Exception.h"

using namespace Femii;

class i2c_reader{

    public:

        const char* bus_access;
        int i2c_bus_handle;
        uint8_t i2c_register;
        uint8_t slave_address;
        DataWidth width = WIDTH_UNSUPPORTED; // width

        i2c_reader(uint8_t bus_num, uint8_t slave_address, uint8_t i2c_register, DataWidth width);
        ~i2c_reader(){};

        int open_bus(); 
        void close_bus();
        void set_slave();
        uint8_t read_byte(int i2c_reg=-1);
        uint8_t write_byte(uint8_t byte_data, int i2c_reg=-1);
        uint16_t read_word(int i2c_reg=-1);
        uint16_t write_word(uint16_t word_data, int i2c_reg=-1);

};

#endif