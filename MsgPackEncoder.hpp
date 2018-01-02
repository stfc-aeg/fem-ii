#ifndef MSGPACKENCODER_H_
#define MSGPACKENCODER_H_

#include "MsgEncoder.hpp"

using namespace Femii;

/*
Sub-class of MsgEncoder, specialising for MessagePack Encoding/Decoding
*/
class MsgPackEncoder : MsgEncoder{

public:

    MsgPackEncoder();

    virtual ~MsgPackEncoder(){};

    const std::string encode(Fem2ControlMsg the_msg);

    const Fem2ControlMsg decode(std::string the_msg);

};

#endif