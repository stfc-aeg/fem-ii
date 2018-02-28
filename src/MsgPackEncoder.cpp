#include "../include/MsgPackEncoder.h"

using namespace Femii;

//  Default constructor
MsgPackEncoder::MsgPackEncoder(){};

/* 0 Copy Encoding of Fem2ControlMsg
/ :param the_msg : refernce to the fem2controlmsg to encode
/ :param string_msg : refernce to a string to hold the encoded msg
*/
void MsgPackEncoder::encode(Fem2ControlMsg& the_msg, std::string& string_msg){

    std::stringstream ss;
    msgpack::pack(&ss, the_msg);
    string_msg = ss.str();

}

/* 0 Copy Decoding of a String Representation of a Fem2ControlMsg
/ :param the_msg : reference to the encoded string
/ :param decoded_msg : refernce to a fem2control msg constructed by the encoded string
*/
void MsgPackEncoder::decode(std::string& the_msg, Fem2ControlMsg& decoded_msg){

    msgpack::unpacked unp;
    msgpack::unpack(unp, the_msg.data(), the_msg.size());
    msgpack::object obj = unp.get();
    decoded_msg = obj.as<Fem2ControlMsg>();

}






