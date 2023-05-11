#ifndef _IO_Exceptions
#define _IO_Exceptions

#include <exception>
using namespace std;

class No_share_found_exception : public exception
{
    virtual const char *what() const throw()
    {
        return "Unable to read share: Requested input out of bound";
    }
};

class No_reg_int_found_exception: public exception
{
    virtual const char* what() const throw()
    {
        return "Unable to read reg int: Requested input out of bound";
    }
};

class No_gfp_found_exception: public exception
{
    virtual const char* what() const throw()
    {
        return "Unable to read gfp: Requested input out of bound";
    }
};

class Invalid_format_exception: public exception
{
    virtual const char* what() const throw()
    {
        return "Invalid number format: Only positive, negative and float numbers using dot notation are allowed.";
    }
};


#endif