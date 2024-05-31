/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Abdelrahaman ALY on 09/02/2023.
//
#include "Dabit.h"

#include <iostream>
template <class SBit> Share Dabit<SBit>::get_bp() const
{
    return bp_;
}
template <class SBit> void Dabit<SBit>::set_bp(const Share &bp)
{
    this->bp_ = bp;
}
template <class SBit> SBit Dabit<SBit>::get_b2() const
{
    return b2_;
}
template <class SBit> void Dabit<SBit>::set_b2(const SBit &b2)
{
    this->b2_ = b2;
}

template class Dabit<aBit>;
template class Dabit<Share2>;