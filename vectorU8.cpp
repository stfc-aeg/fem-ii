#include <stdint.h>
#include <msgpack.hpp>
#include <vector>

//to be vector wrappers 
class vectorU8{

    public:

    vectorU8(){
    }

    void push_back(uint8_t element){
        this->variant_vector.push_back(element);
    }



    void to_variant(std::vector<u8> u8_vector){


    }

    void to_u8(std::vector<msgpack::type::variant> variant_vect){




    }
    private: 

    std::vector<uint8_t> uint8_vector;
    std::vector<msgpack::type::variant> variant_vector;
}