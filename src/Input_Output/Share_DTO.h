/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Ivan on 01/12/2021.
//
#ifndef _ShareDTO
#define _ShareDTO

#include <stdlib.h>
#include <string>

using namespace std;

class Share_DTO {
    int id;
    int player;
    string share;
    string mac_share;

public:
    Share_DTO(int id,
              int player,
              const string &share,
              const string &mac_share) : id(id), player(player), share(share), mac_share(mac_share) {}

    int get_id() const;

    int get_player() const;

    string get_share() const;

    string get_mac_share() const;
};

#endif
