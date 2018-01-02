#include "MsgPackEncoder.hpp"

using namespace Femii;

MsgPackEncoder::MsgPackEncoder(){};
  
const std::string MsgPackEncoder::encode(Fem2ControlMsg the_msg){

    std::stringstream ss;
    msgpack::pack(&ss, the_msg);
    return ss.str();
}

const Fem2ControlMsg MsgPackEncoder::decode(std::string the_msg){

    msgpack::unpacked unp;
    msgpack::unpack(unp, the_msg.data(), the_msg.size());
    msgpack::object obj = unp.get();
    Fem2ControlMsg decoded = obj.as<Fem2ControlMsg>();
    return decoded;
}






