/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

/*
 * XOR_Machine.cpp
 *
 *  Created on: Mar 10, 2019
 *      Author: dragos
 */

#include "XOR_Machine.h"
#include "LSSS/Open_Protocol.h"
#include "LSSS/Open_Protocol2.h"
#include "OT/aBit.h"
#include "System/Player.h"
#include <cassert>

extern vector<sacrificed_data> SacrificeD;

template<class SBit>
XOR_Machine<SBit>::XOR_Machine(Player &P, offline_control_data &OCD, int thread) : P(P), OCD(OCD), thread(thread) {}

template<class SBit>
void XOR_Machine<SBit>::check_debug(const vector<vector<Share>> &combinedp, const vector<vector<SBit>> &combined2)
{
  cout << "In CHECK DEBUG" << endl;
  unsigned int total= combinedp[0].size();

  allShp.clear();
  allShp.reserve(total);

  allSh2.clear();
  allSh2.reserve(total);

  // apply permutation
  for (size_t player= 0; player < P.nplayers(); player++)
    {
      for (unsigned int i= 0; i < total; i++)
        {
          allShp.push_back(combinedp[player][i]);
          allSh2.push_back(combined2[player][i]);
        }
    }

  // Open bits in both fields
  P.OP->Open_To_All_Begin(valuesp, allShp, P, 2);
  P.OP->Open_To_All_End(valuesp, allShp, P, 2);

  P.OP2->Open_Bits(values2, allSh2, P);

  // Check bit consistency in both fields
  for (unsigned int i= 0; i < total; ++i)
    {
      if (!bit_equality(valuesp[i], values2[i]))
        throw Sacrifice_Check_Error("daBit check_debug!");
    }
  cout << "Finished CHECK_DEBUG" << endl;
}

// Opens MANY bits according the the CNC parameters
// and check that they are consistent (ie the same) in
// both fields
template<class SBit>
void XOR_Machine<SBit>::consistency_check(const vector<vector<Share>> &Shp,
                                          const vector<vector<SBit>> &Sh2, const vector<int> &perm, int left_out)
{

  int num_allBits= perm.size();
  int total_cnc= (num_allBits - left_out) * P.nplayers();

  allShp.clear();
  allShp.reserve(total_cnc);

  allSh2.clear();
  allSh2.reserve(total_cnc);

  // apply permutation
  for (size_t player= 0; player < P.nplayers(); player++)
    {
      for (int i= left_out; i < num_allBits; i++)
        {
          allShp.push_back(Shp[player][perm[i]]);
          allSh2.push_back(Sh2[player][perm[i]]);
        }
    }

  // Open bits in both fields
  P.OP->Open_To_All_Begin(valuesp, allShp, P, 2);
  P.OP->Open_To_All_End(valuesp, allShp, P, 2);

  P.OP2->Open_Bits(values2, allSh2, P);

  // Check bit consistency in both fields
  for (int i= 0; i < total_cnc; ++i)
    {
      if (!bit_equality(valuesp[i], values2[i]))
        {
          throw Sacrifice_Check_Error("daBit consistency error!");
        }
    }
}

/* This is a modified version from Processor/SPDZ.cpp
 * Does XOR(LBits, RBits) in GFp using triples from OCD
 */
template<class SBit>
void XOR_Machine<SBit>::xors(vector<Share> &result, const vector<Share> &LBits,
                             const vector<Share> &RBits)
{
  assert(LBits.size() == RBits.size());
  int n= LBits.size();
  opened.clear();
  shares.clear();

  // Retrieve triples from OCD
  Wait_For_Preproc(DATA_TRIPLE, n, thread, OCD);
  list<Share> ta, tb, tc;

  OCD.mul_mutex[thread].lock();

  auto &sd_ta= SacrificeD[thread].TD.ta;
  auto &sd_tb= SacrificeD[thread].TD.tb;
  auto &sd_tc= SacrificeD[thread].TD.tc;

  ta.splice(ta.begin(), sd_ta, sd_ta.begin(), next(sd_ta.begin(), n));
  tb.splice(tb.begin(), sd_tb, sd_tb.begin(), next(sd_tb.begin(), n));
  tc.splice(tc.begin(), sd_tc, sd_tc.begin(), next(sd_tc.begin(), n));
#ifdef BENCH_OFFLINE
  P.triples+= n;
#endif

  OCD.mul_mutex[thread].unlock();

  vector<vector<Share>> triples(3);
  for (int i= 0; i < 3; i++)
    triples[i].reserve(n);

  std::copy(begin(ta), end(ta), std::back_inserter(triples[0]));
  std::copy(begin(tb), end(tb), std::back_inserter(triples[1]));
  std::copy(begin(tc), end(tc), std::back_inserter(triples[2]));

  // Do Beaver multiplication here
  for (int i= 0; i < n; i++)
    {
      shares.push_back(LBits[i] - triples[0][i]);
      shares.push_back(RBits[i] - triples[1][i]);
    }

  P.OP->Open_To_All_Begin(opened, shares, P, 2);
  P.OP->Open_To_All_End(opened, shares, P, 2);
  auto it= opened.begin();

  result.resize(LBits.size()); // Make sure we have enough space
  gfp two= 2;
  for (int i= 0; i < n; i++)
    {
      gfp masked[2];
      Share &tmp= triples[2][i];
      for (int k= 0; k < 2; k++)
        {
          masked[k]= *it++;
          Share aux;
          aux.mul(triples[1 - k][i], masked[k]);
          tmp.add(aux);
        }
      tmp.add(tmp, masked[0] * masked[1], P.get_mac_keys());
      tmp.mul(tmp, two);
      result[i]= LBits[i] + RBits[i] - tmp;
    }
}

/* Since each party has inputted bits into the MPC engine,
 * we need to XOR them in order to ensure that there is 
 * at least one honest party which contributed with some
 * randomness
 */
template<class SBit>
void XOR_Machine<SBit>::party_log_xor(vector<Share> &accumulator, vector<vector<Share>> &lsts)
{
  // Multiple lists packed into one vector; Need to XOR-merge them
  // and maximize the MAC Check done in parallel: ie obtain:
  // [ XORsum(L[0][0], ..., L[0][m-1]), ... XORsum(L[n-1][0], ..., L[n-1][m-1]) ]

  // TODO: Optimize by having a single vector as input
  // Probably many cache misses since rows are large and columns are small
  int n= lsts.size();
  int m= lsts[0].size();

  accumulator.resize(n);

  vector<Share> left, right;
  vector<Share> tmp;

  for (int step= 1; step < m; step<<= 1)
    {
      left.clear();
      right.clear();
      for (int i= 0; i < n; i++)
        for (int j= 0; j < m; j+= step * 2)
          if (j + step < m)
            {
              left.push_back(lsts[i][j]);
              right.push_back(lsts[i][j + step]);
            }

      this->xors(tmp, left, right);
      // Now put data back in place
      size_t cnt= 0;
      for (int i= 0; i < n; ++i)
        {
          int j= 0;
          while (j + step < m)
            {
              lsts[i][j]= tmp[cnt++];
              j+= step * 2;
            }
        }
    }
  for (int i= 0; i < n; ++i)
    accumulator[i]= lsts[i][0];
}

/* Does the XOR between parties' shares. 
 * To do XOR in gfp(p) we need triples
 * To do XOR for gf(2^n) is local, hence just simple additions.
 */
template<class SBit>
void XOR_Machine<SBit>::combine(vector<Share> &combinedp, vector<SBit> &combined2,
                                const vector<Share> &Shp, const vector<SBit> Sh2)

{
  // Data from Shp/2 comes as [Player_1, ..., Player_n]
  int num_bits= Shp.size() / P.nplayers();

  // Deal with GFp case
  vector<vector<Share>> Lst;
  for (size_t pnum= 0; pnum < P.nplayers(); pnum++)
    {
      for (int i= 0; i < num_bits; i++)
        {
          if (pnum == 0)
            Lst.push_back(vector<Share>());
          Lst[i].push_back(Shp[pnum * num_bits + i]);
        }
    }
  this->party_log_xor(combinedp, Lst);
  P.OP->Open_To_All_Begin(valuesp, combinedp, P, 2);
  P.OP->Open_To_All_End(valuesp, combinedp, P, 2);

  combined2.clear();
  combined2.resize(num_bits);
  for (int i= 0; i < num_bits; ++i)
    {
      combined2[i].assign_zero(P.whoami());
    }
  // Deal with GF2n case
  for (size_t pnum= 0; pnum < P.nplayers(); pnum++)
    {
      for (int i= 0; i < num_bits; ++i)
        {
          combined2[i].add(Sh2[pnum * num_bits + i]);
        }
    }
}

template<class SBit>
bool XOR_Machine<SBit>::bit_equality(const gfp &x, const word &y)
{
  bool equal= 0;
  equal|= (x.is_zero() & (y == 0));
  equal|= (x.is_one() & (y == 1));
  return equal;
}

/* This is used after the party XOR
 * to combine bits inside the buckets
 * after xoring in both fields, more sacrificing is done.
 */
template<class SBit>
void XOR_Machine<SBit>::check_correctness(vector<Share> &combinedp,
                                          vector<SBit> &combined2, int bucket_size, int nFinalBits)
{
  // Here we need to XOR the head of each bucket with the rest of daBits
  // put heads in left, the rest in right
  vector<Share> left, right;
  for (int i= 0; i < nFinalBits; i++)
    {
      for (int j= 1; j < bucket_size; j++)
        {
          left.push_back(combinedp[i * bucket_size]);
          right.push_back(combinedp[i * bucket_size + j]);
        }
    }
  vector<Share> to_checkp;
  this->xors(to_checkp, left, right);

  // XOR is local for GF2n case
  vector<SBit> to_check2;
  for (int i= 0; i < nFinalBits; i++)
    {
      auto &head= combined2[i * bucket_size];
      for (int j= 1; j < bucket_size; j++)
        {
          SBit tmp;
          tmp.add(head, combined2[i * bucket_size + j]);
          to_check2.push_back(tmp);
        }
    }
  P.OP->Open_To_All_Begin(valuesp, to_checkp, P, 2);
  P.OP->Open_To_All_End(valuesp, to_checkp, P, 2);

  P.OP2->Open_Bits(values2, to_check2, P);
  for (int i= 0; i < (int) to_checkp.size(); ++i)
    if (!bit_equality(valuesp[i], values2[i]))
      throw Sacrifice_Check_Error("daBit correctness error");
}

template class XOR_Machine<aBit>;
template class XOR_Machine<Share2>;
