//
// Created by Ivan on 13/12/2021.
//
#ifndef _RegIntDTO
#define _RegIntDTO

#include <stdlib.h>
#include <string>

using namespace std;

class Reg_int_DTO {
    int id;
    long value;

public:
    Reg_int_DTO(int id, long value) : id(id), value(value) {}

    int get_id() const;

    long get_value() const;
};

#endif
