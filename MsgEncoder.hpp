#ifndef MSGENCODER_H_
#define MSGENCODER_H_

#include "Fem2ControlMsg.hpp"
#include <exception>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <msgpack.hpp>
//JSON

using namespace Femii;

class MsgEncoder{

public:

    virtual ~MsgEncoder(){};

    virtual const std::string encode(Fem2ControlMsg the_msg) = 0;

    virtual const Fem2ControlMsg decode(std::string the_msg) = 0;

};

/*
print as python bytes??
print encoded message??
compare encoded messages??

*/ 

#endif