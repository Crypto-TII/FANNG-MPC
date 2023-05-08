/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _Garbled
#define _Garbled

/* This defines a GC using the method of 2017/214,
 * we use this rather than 2017/189 as we want active
 * functionality. And the protocol of 2017/214 is easier
 * to adapt to this case.
 *
 * As the circular 2-correlation robust PRF F_K(m) for 
 * K=(k1,k2) we use the * following function, MMO based function.
 * The tweak with the keys is to enable free-XOR to be
 * secure, with "*x" meaning multiplying by x in GF(2^128)
 *
 *   M = m xor (k1*x) xor (k2*x^2)
 *   c = AES_k(sigma(M)) xor sigma(M)
 *
 * where sigma(s) = (_mm_shuffle_epi32 ( a , 78)) xor (_mm_and_si128 ( a , mask))
 * with mask = 1^64 || 0^64.
 *
 * For the MMO key we use a per circuit agreed random key
 *   - This is secure only because we garble SMALL circuits in
 *     SCALE, and build bigger circuits out of sequential
 *     execution of small circuits.
 */

#include "Circuit.h"
#include "OT/aAND_Thread.h"
#include "Tools/MMO.h"

/* The Base_Garbled_Circuit operates on inputs which are secret
 * shared. The derived class operates on inputs which are given
 * by each player
 */

class Base_Garbled_Circuit
{

  MMO mmo;

protected:
  void apply_PRF(vector<gf2n> &ans,
                 const gf2n &k1, const gf2n &k2, unsigned int g);

  // Shared signal bits
  vector<aBit> lambda;

  // Wire labels
  vector<vector<gf2n>> k;

  // Shares of garbled AND gates
  // When opened these become the sums across all players
  vector<vector<vector<vector<gf2n>>>> gab;

  // The wire labels for the "one" wire
  //   - An invert is (a XOR 1) so we use a fixed
  //     wire label for the one wire
  // One label per party. When opened we add in Delta here
  vector<gf2n> one_label;

  void Open_Garbling(Player &P);

public:

  bool operator==(const Base_Garbled_Circuit &other) const
  {
    return lambda == other.lambda && k == other.k && gab == other.gab && one_label == other.one_label;
  }

  void Garble(const Circuit &C,
              Player &P,
              unsigned int online_thread_no);

  // This is the core-evaluate routine
  //   Gamma holds the external values for all wires
  // On entry these are defined for all input values of the circuit
  // On exit these are defined for all wires in the circuit
  //    - And thus the external values of the output wires
  void Evaluate_Core(vector<int> &Gamma,
                     const Circuit &C, Player &P);

  // This gives the reactive GC functionality
  // The inputs are vectors of shared bits, the outputs are
  // also vectors of shared bits
  void Evaluate(vector<vector<aBit>> &output,
                const vector<vector<aBit>> &input,
                const Circuit &C, Player &P);

  MMO get_mmo() const;

  vector<aBit> get_lambda() const;

  vector<vector<gf2n>> get_k() const;

  vector<vector<vector<vector<gf2n>>>> get_gab() const;

  vector<gf2n> get_one_label() const;

  void set_mmo(const MMO &mmo);

  void set_lambda(const vector<aBit> &lambda);

  void set_k(const vector<vector<gf2n>> &k);

  void set_gab(vector<vector<vector<vector<gf2n>>>> gab);

  void set_one_label(vector<gf2n> one_label);
};

/* This is a traditional garbled circuit interface */
class Garbled_Circuit : public Base_Garbled_Circuit
{

  /* Assignment of input/output variables to players
   * Vector of length C.num_inputs()/C.num_ouputs()
   * saying who owns this input/output variable
   */
  vector<unsigned int> i_assign;
  vector<unsigned int> o_assign;

  /* The next two vectors have first dimension size 
   *        C.num_inputs()/C.num_outputs()
   * Then the second dimension is only non-zero if
   * this player holds this input/output variable
   */

  // My output wire openings, one for each variable assigned to me
  vector<vector<word>> outputs;

  // My input wire openings, one for each variable assigned to me
  vector<vector<word>> inputs;

public:
  void Garble(const Circuit &C,
              const vector<unsigned int> &i_a, const vector<unsigned int> &o_a,
              Player &P,
              unsigned int online_thread_no);

  /* input/output is this players input/output. Again these 
   * are of length 
   *         C.num_inputs()/C.num_outputs()
   * Then the second dimension is only non-zero if
   * this player holds this input/output variable
   *
   * output is resized by this function, so can be blank on entry
   */
  void Evaluate(vector<vector<int>> &output,
                const vector<vector<int>> &input,
                const Circuit &C, Player &P);
};

#endif
