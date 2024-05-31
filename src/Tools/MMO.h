/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.


*/

#ifndef TOOLS_MMO_H_
#define TOOLS_MMO_H_

#include "Tools/aes.h"

// Matyas-Meyer-Oseas hashing
//
//  We use the MMO function from eprint 2019/074 defined by
//     MMO(x) = AES_k(sigma(x)) xor sigma(x)
//  where sigma(a) = (_mm_shuffle_epi32 ( a , 78)) xor (_mm_and_si128 ( a , mask))
//  with mask = 1^64 || 0^64.
//
//  This gives a Circular Correlation Robust hash function
//
//  The k is the IV below. So in default mode this uses the zero IV

class MMO
{
  uint8_t IV[176] __attribute__((aligned(16)));

  template<int N>
  static void encrypt_and_xor(void *output, const void *input,
                              const uint8_t *key);
  template<int N>
  static void encrypt_and_xor(void *output, const void *input,
                              const uint8_t *key, const int *indices);

public:
  MMO()
  {
    zeroIV();
  }
  MMO(uint8_t key[AES_BLK_SIZE])
  {
    setIV(key);
  }
  void zeroIV();

  uint8_t* getIV(){
    return IV;
  }

  void setIV(uint8_t key[AES_BLK_SIZE]);

  template<class T>
  void hashOneBlock(uint8_t *output, const uint8_t *input);

  template<class T, int N>
  void hashBlockWise(uint8_t *output, const uint8_t *input);

  template<class T>
  void outputOneBlock(uint8_t *output);
};

#endif /* TOOLS_MMO_H_ */
