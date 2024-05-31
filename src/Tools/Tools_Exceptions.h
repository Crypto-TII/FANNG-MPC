/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef _Tools_Exceptions
#define _Tools_Exceptions

#include <exception>
#include <string>

class Unknown_config_parameter : public exception {
public:
    string parameter;

    explicit Unknown_config_parameter(const string &parameter) : parameter(parameter) {}

private:
    const char *what() const throw() override {
        return parameter.c_str();
    }
};

#endif