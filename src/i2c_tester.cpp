#include <iostream>
#include "../include/i2c_reader.h"



int main(){

    i2c_reader i2c_reader(0, 0x5c, 0xA, WIDTH_BYTE);
    i2c_reader.open_bus();
    i2c_reader.set_slave();
    uint8_t the_data = i2c_reader.read_byte();
    printf("%s : device 0x%02x at address 0x%02x: 0x%02x\n", i2c_reader.bus_access, i2c_reader.slave_address, i2c_reader.i2c_register, the_data);
    i2c_reader.close_bus();
    return 0;
}