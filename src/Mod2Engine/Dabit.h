/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Abdelrahaman ALY on 09/02/2023.
//

#ifndef _Dabit
#define _Dabit

#include "LSSS/Share.h"
#include "LSSS/Share2.h"
#include "OT/aBit.h"

template <class SBit> class Dabit
{
  public:
    Share bp_;
    SBit b2_;

    static constexpr unsigned int ID = 2;
    static unsigned int get_ID()
    {
        return ID;
    }
    Dabit() = default;
    Dabit(const Share &bp, const SBit &b2) : bp_(bp), b2_(b2){};

    Share get_bp() const;
    void set_bp(const Share &bp);

    SBit get_b2() const;
    void set_b2(const SBit &b2);
};
#endif