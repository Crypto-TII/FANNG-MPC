/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/


//
// Created by Abdelrahaman Aly on 15/02/2023.
//

#ifndef _Randomness_Utils
#define _Randomness_Utils

#include "LSSS/Share.h"
#include <vector>
#include <list>

class Randomness_Utils
{
public:
  Randomness_Utils();
  static list<Share> get_sublist(const list<Share> &original_list, const unsigned int &start_index, const unsigned int &end_index);
  static Share combine_share(const list<Share> &bits);
  static void combine_shares(vector<Share> &target, const list<Share> &bits, const unsigned int &bound, const unsigned int &times);
  static vector<Share> combine_shares(const list<Share> &bits, const unsigned int &bound, const unsigned int &times);
  static vector<Share> combine_triple_shares(const list<Share> &la, const list<Share> &lb, const list<Share> &lc);
  static void combine_triple_shares(vector<Share> &target, const list<Share> &la, const list<Share> &lb, const list<Share> &lc);
  static vector<Share> obtain_zero_shares(const unsigned int number_of_shares, const unsigned int player_id);
};

#endif
