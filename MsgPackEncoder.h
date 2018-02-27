#ifndef MSGPACKENCODER_H_
#define MSGPACKENCODER_H_

#include "MsgEncoder.h"

using namespace Femii;

/*
Sub-class of MsgEncoder, specialising for MessagePack Encoding/Decoding
*/
class MsgPackEncoder : MsgEncoder{

public:

    MsgPackEncoder();

    virtual ~MsgPackEncoder(){};

    void encode(Fem2ControlMsg& the_msg, std::string& string_msg);

    void decode(std::string& the_msg, Fem2ControlMsg& decoded_msg);


};

#endif