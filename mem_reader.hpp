#ifndef MEM_READER_H_
#define MEM_READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "Fem2ControlMsg.hpp"
//#include "data_helpers.hpp"

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); throw Fem2ControlMsgException("Fatal Error");} while(0)
 
#define MAP_SIZE 4096UL // minimum memory space - size of address space? 
#define MAP_MASK (MAP_SIZE - 1)

using namespace Femii;

class mem_reader{

    public:

        int size = getpagesize();
        //printf("size: %d \n", size);
        int fd; //this is file to open using open(dev/mem)
        void *map_base, *virt_addr; 
        unsigned long read_result, writeval;
        off_t target; //address
        DataWidth width = WIDTH_UNSUPPORTED; // width

        mem_reader(uint32_t base, uint32_t offset, DataWidth width);
        ~mem_reader(){};

        unsigned long read_mem();
        unsigned long write_mem(unsigned long the_data);
        void init_mmap();
        void unmap();
};

#endif
