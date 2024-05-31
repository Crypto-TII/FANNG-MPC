/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Randomness_Utils.h"

#include "Exceptions/Exceptions.h"

Randomness_Utils::Randomness_Utils()= default;

list<Share> Randomness_Utils::get_sublist(const list<Share> &original_list, const unsigned int &start_index, const unsigned int &end_index)
{
  if (start_index >= end_index || end_index > original_list.size())
    {
      throw randomness_not_available();
    }
  std::list<Share> sub_list;
  auto it= original_list.begin();
  std::advance(it, start_index);
  sub_list.insert(sub_list.end(), it, std::next(it, end_index - start_index));
  return sub_list;
}

Share Randomness_Utils::combine_share(const list<Share> &bits)
{
  Share bounded_rand= *(bits.begin());
  gfp count(2);
  for (list<Share>::const_iterator zz= std::next(bits.begin()); zz != bits.end(); ++zz)
    {
      Share l_zz= *zz;
      bounded_rand= bounded_rand + l_zz * count;
      count= count * 2;
    }
  return bounded_rand;
}

void Randomness_Utils::combine_shares(vector<Share> &target, const list<Share> &bits, const unsigned int &bound, const unsigned int &times)
{

  for (unsigned int i= 0; i < times; i++)
    {
      target.push_back(Randomness_Utils::combine_share(Randomness_Utils::get_sublist(bits, bound * i, bound * i + bound)));
    }
}

vector<Share> Randomness_Utils::combine_shares(const list<Share> &bits, const unsigned int &bound, const unsigned int &times)
{
  vector<Share> bounded_rands;
  Randomness_Utils::combine_shares(bounded_rands, bits, bound, times);
  return bounded_rands;
}

vector<Share> Randomness_Utils::combine_triple_shares(const list<Share> &la, const list<Share> &lb, const list<Share> &lc)
{
  vector<Share> triple_shares;
  
  auto ita = la.begin();
  auto itb = lb.begin();
  auto itc = lc.begin();

  while (ita != la.end() && itb != lb.end() && itc != lc.end()) 
  {
      triple_shares.push_back(*ita);
      triple_shares.push_back(*itb);
      triple_shares.push_back(*itc);

      ++ita; 
      ++itb; 
      ++itc;
  }
  return triple_shares;
}

void Randomness_Utils::combine_triple_shares(vector<Share> &target, const list<Share> &la, const list<Share> &lb, const list<Share> &lc)
{
  vector<Share> triple_shares = Randomness_Utils::combine_triple_shares(la, lb, lc);
  target.insert(target.end(), triple_shares.begin(), triple_shares.end());
}

vector<Share> Randomness_Utils::obtain_zero_shares(const unsigned int number_of_shares, const unsigned int player_id)
{
      vector<Share> matrix_triple;
      for(unsigned int i = 0; i < number_of_shares; i++)
      {
        Share value(player_id);
        value.assign_zero();
        matrix_triple.push_back(value);
      }
      return matrix_triple;
}