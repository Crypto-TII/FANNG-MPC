/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Mirea, Ivan and Jose Angel on 13/12/2021.
//
#ifndef _GfpDTO
#define _GfpDTO

#include <stdlib.h>
#include <string>

using namespace std;

class GFP_DTO {
    int id;
    string value;

public:

    GFP_DTO(int id, const string &value) : id(id), value(value) {}

    int get_id() const;

    string get_value() const;
};

#endif
