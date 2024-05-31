/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Ajith Suresh on 13/03/2024.
//
#include "Base_Triple.h"

void Base_Triple::initialize(const bigint &p) 
{
    // p is being passed so that in the future we can implement the matrix extraction from files.
    (void) p;

    /* 
     * USE INTEGERS IN THE RANGE AS ID [70000,80000) 
     * (Next multiple of 10^4 after the INTEGER RANGE 65536)
     */

    // CLASSIC_BEAVER
    Beaver_Type = Triple_Type_DTO(70000);
    last_retrieved_index_per_channel.insert(make_pair(70000, 0));

    /* MATRIX TRIPLE TYPES */

    // LENET_1_1
    auto TT= Triple_Type_DTO(777777, 25, 2, 9, 4, 9, 2);
    Matrix_Triple_Types.insert(make_pair(777777, TT));
    last_retrieved_index_per_channel.insert(make_pair(777777, 0));

    // PRUNED_RESNET_TRIPLE
    TT= NN_Triple_Type_DTO(777778, 5, 5, 2, 3, 3, 2, 1,0);
    Matrix_Triple_Types.insert(make_pair(777778, TT));
    last_retrieved_index_per_channel.insert(make_pair(777778, 0));

    /* CUSTOM TYPES GO BELOW THIS LINE*/

}
