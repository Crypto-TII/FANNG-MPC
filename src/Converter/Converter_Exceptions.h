/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef _Converter_Exceptions
#define _Converter_Exceptions

#include <exception>
#include <string>

class Not_Enough_Preprocessed_Elements : public exception
{
  string message;

public:
  explicit Not_Enough_Preprocessed_Elements(unsigned long available, unsigned int requested) : message("Requested " + to_string(requested) + " but " + to_string(available) + " available preprocessed elements") {}
  const char *what() const throw() override
  {
    return this->message.c_str();
  }
};

class Not_Enough_Matrix_Triple_Elements : public exception
{
  string message;

public:
  explicit Not_Enough_Matrix_Triple_Elements(unsigned long available, unsigned int requested) : message("Requested " + to_string(requested) + " but " + to_string(available) + " available elements for your Triple") {}
  const char *what() const throw() override
  {
    return this->message.c_str();
  }
};

#endif