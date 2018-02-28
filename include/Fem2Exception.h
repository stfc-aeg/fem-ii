#ifndef FEM2EXCEPTION_H_
#define FEM2EXCEPTION_H_

#include "femii_include.h"

namespace Femii
{
//! Fem2Exception
class Fem2Exception : public std::exception
{
public:

    // Blank Fem2ControlMsgException constructor
    Fem2Exception(void) throw() :
        error_msg("")
    {};

    // Fem2ControlMsgException with error message
    Fem2Exception(const std::string the_error_msg) throw() :
        error_msg(the_error_msg)
    {};

    // @override returns the error message as a c string
    virtual const char* what(void) const throw()
    {
        return error_msg.c_str();
    };

    // Destructor
    ~Fem2Exception(void) throw() {};

private:

    const std::string error_msg;  // The error message

};// fem2exception
};// femii namespace

#endif