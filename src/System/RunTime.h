/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.


*/
#ifndef _RunTime
#define _RunTime

/* This file holds the main SCALE runtime in a
 * single function call, to allow programmatic
 * setup of SCALE outside of using Player.x
 *
 */

#include "FHE/FHE_Keys.h"
#include "Offline/offline_data.h"
#include "Online/Machine.h"
#include "System/SystemData.h"
#include "config.h"

template<class SRegint, class SBit>
void Run_Scale(unsigned int my_number, unsigned int no_online_threads,
               const FFT_Data &PTD, const FHE_PK &pk, const FHE_SK &sk, const vector<gfp> &MacK,
               SSL_CTX *ctx, const vector<unsigned int> &portnum,
               const SystemData &SD,
               Machine<SRegint, SBit> &machine,
               offline_control_data &OCD,
               unsigned int number_FHE_threads,
               bool OT_disable,
               int verbose);

#endif
