#ifndef _Processor_Exceptions
#define _Processor_Exceptions

#include <exception>

class Instruction_not_supported_exception : public exception
{
    const char *what() const throw() override
    {
        return "Instruction not supported: Offline garbling and evaluation is not supported in Shamir setups";
    }
};

#endif