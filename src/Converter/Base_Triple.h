/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Ajith Suresh on 13/03/2024.
//
#ifndef _BaseTriple
#define _BaseTriple

#include <map>
#include <memory>
#include "Math/bigint.h"
#include "Triple_Type_DTO.h"

/*
 * Our reserved numbers are currently the following
 * for non-directly called Offline Triples:
 *   Beaver Triples:
 *   * 70000 CLASSIC_BEAVER
 *   Matrix Triples:
 *   * 777777 LENET_1_1
 *   * 777778 PRUNED_RESNET_TRIPLE
 */
class Base_Triple {
public:
    Triple_Type_DTO Beaver_Type; 
    std::map<int, Triple_Type_DTO> Matrix_Triple_Types;
    std::map<unsigned int, int> last_retrieved_index_per_channel;

    Base_Triple() = default;

    void initialize(const bigint &p);

    // Prevent use of these
    Base_Triple(const Base_Triple &) = delete;
};

#endif
