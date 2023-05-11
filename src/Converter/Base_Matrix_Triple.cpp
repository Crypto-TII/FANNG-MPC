//
// Created by Abdelrahaman ALY on 09/02/2023.
//
#include "Base_Matrix_Triple.h"

void Base_Matrix_Triple::initialize(const bigint &p)
{
  // p is being passed so that in the future we can implement the matrix extraction from files.
  (void) p;
  // LENET_1_1
  auto TT= Triple_Type_DTO(777777, 25, 2, 9, 4, 9, 2);
  Triple_Types.insert(make_pair(777777, TT));
  last_retrieved_index_per_channel.insert(make_pair(777777, 0));

  // PRUNED_RESNET_TRIPLE
  TT= NN_Triple_Type_DTO(777778, 5, 5, 2, 3, 3, 2, 1,0);
  Triple_Types.insert(make_pair(777778, TT));
  last_retrieved_index_per_channel.insert(make_pair(777778, 0));

  /* CUSTOM TYPES GO BELOW THIS LINE*/
}
