/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Ivan on 01/12/2021.
//

#include "Share_DTO.h"

int Share_DTO::get_id() const {
    return this->id;
}

int Share_DTO::get_player() const {
    return this->player;
}

string Share_DTO::get_share() const {
    return this->share;
}

string Share_DTO::get_mac_share() const {
    return this->mac_share;
}


