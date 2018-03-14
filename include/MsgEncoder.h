#ifndef MSGENCODER_H_
#define MSGENCODER_H_

//#include <exception>
//#include <vector>
//#include <iostream>
//#include <string>
//#include <sstream>
//#include <msgpack.hpp>

#include "femii_include.h"
#include "Fem2ControlMsg.h"

//TODO - include JSON

using namespace Femii;

/*
Abstract base class for a message encoder
Encodes and decodes Fem2ControlMsg's
*/
class MsgEncoder{

public:

    virtual ~MsgEncoder(){};
    virtual void encode(Fem2ControlMsg& the_msg, std::string& string_msg) = 0;
    virtual void decode(std::string& the_msg, Fem2ControlMsg& decoded_msg) = 0;
};


#endif