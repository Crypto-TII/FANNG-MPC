//
// Created by Abdelrahaman ALY on 09/02/2023.
//
#ifndef _BaseMatrixTriple
#define _BaseMatrixTriple

#include <map>
#include <memory>
#include "Math/bigint.h"
#include "Triple_Type_DTO.h"

/*
 * Our reserved numbers are currently (for non-directly called
 * Matrix Triples):
 *   * 777777 LENET_1_1
 *   * 777778 PRUNED_RESNET_TRIPLE
 */
class Base_Matrix_Triple
{
public:
  
  
  std::map<int, Triple_Type_DTO> Triple_Types;
  std::map<unsigned int, int> last_retrieved_index_per_channel;
  
  Base_Matrix_Triple()= default;
  
  void initialize(const bigint &p);

   /* Prevent use of these */
    Base_Matrix_Triple(const Base_Matrix_Triple &) = delete;

};

#endif
