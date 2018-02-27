#include "mem_reader.h"

using namespace Femii;

mem_reader::mem_reader(uint32_t base, uint32_t offset, DataWidth width){

    this->target = base + offset;
    this->width = width;
}


void mem_reader::init_mmap(){

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        FATAL; // failed to open the dev/mem
    }

    printf("/dev/mem opened.\n");
    fflush(stdout);
 
    /* Map one page */
    this->map_base = mmap(0, 
                    MAP_SIZE, //size of the address space
                    PROT_READ | PROT_WRITE, // read and write
                    MAP_SHARED, //share the mapping
                    this->fd, //dev/mem
                    this->target & ~MAP_MASK // target address entered AND NOT MAP_MASK, always clips to the page boundary (page size) (i.e removes 1 from 0x80000001) for mmap to work
    );

    if(this->map_base == (void *) -1){
        FATAL; // failed to map
    } 
    printf("Memory mapped at address %p.\n", map_base);
    fflush(stdout);
    this->virt_addr = this->map_base + (this->target & MAP_MASK); // base address clipped to page boundary i.e 0x80000000 plus .. (0x80000001 AND MAP_MASK) == 0x1. Getting the target address again.

}

unsigned long mem_reader::read_mem(){    

    // just reading!
    switch(this->width) 
    {
        case WIDTH_BYTE:
            this->read_result = *((unsigned char *) this->virt_addr); // cast virt_address as an unsigned char pointer, then dereference i.e get the value.
            break;
        case WIDTH_WORD:
            this->read_result = *((unsigned short *) this->virt_addr);
            break;
        case WIDTH_LONG:
            this->read_result = *((unsigned long *) this->virt_addr);
            break;
        default:
            throw Fem2ControlMsgException("Illegal Data Type");
    }

    printf("Value at address 0x%X (%p): 0x%X\n", this->target, this->virt_addr, this->read_result);
    fflush(stdout);
    return this->read_result;
}

unsigned long mem_reader::write_mem(unsigned long the_data){
    // if arguments is greater than 3 we have a write call.
 
        this->writeval = the_data;
        switch(this->width) 
        {
            case WIDTH_BYTE:
                *((unsigned char *) this->virt_addr) = this->writeval;
                this->read_result = *((unsigned char *) this->virt_addr);
                break;
            case WIDTH_WORD:
                *((unsigned short *) this->virt_addr) = this->writeval;
                this->read_result = *((unsigned short *) this->virt_addr);
                break;
            case WIDTH_LONG:
                *((unsigned long *) this->virt_addr) = this->writeval;
                this->read_result = *((unsigned long *) this->virt_addr);
                break;
            default:                          
                //fprintf(stderr, "Illegal data type .\n");
                throw Fem2ControlMsgException("Illegal Data Type");
        }
        printf("Written 0x%X; readback 0x%X\n", this->writeval, this->read_result);
        fflush(stdout);
    
    return this->read_result;
}

void mem_reader::unmap(){

    if(munmap((void *)this->map_base, MAP_SIZE) == -1) 
    {
        FATAL;
    }
    close(this->fd);
}

