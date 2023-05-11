/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
/* This file holds the main SCALE runtime in a
 * single function call, to allow programmatic
 * setup of SCALE outside of using Player.x
 */

#include "System/RunTime.h"
#include "GC/Base_Circuits.h"
#include "Mod2Engine/Mod2_Thread.h"
#include "Mod2Engine/aBitVector2.h"
#include "OT/OT_Thread_Data.h"
#include "OT/aBitVector.h"
#include "Offline/FHE_Factory.h"
#include "Offline/offline_phases.h"
#include "Online/Online.h"
#include "System/Networking.h"
#include "Tools/Timer.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
//#include <pthread.h>
#include <sstream>
#include <string>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>
#ifdef BENCH_MEMORY
#include <sys/resource.h>
#endif
using namespace std;

struct timespec time_s= {0, sleep_val};

extern Base_Circuits Global_Circuit_Store;

template<class SRegint, class SBit>
class thread_info
{
public:
  int thread_num;
  const SystemData *SD;
  bigint pr;
  offline_control_data *OCD;
  SSL_CTX *ctx;
  int me;
  unsigned int no_online_threads;
  vector<vector<int>> csockets;
  vector<gfp> MacK;

  // FHE Data
  const FHE_PK *pk;
  const FHE_SK *sk;
  const FFT_Data *PTD;

  int verbose;

  // Pointer to the machine
  Machine<SRegint, SBit> *machine;
  FHE_Industry *industry; // Pointer to the FHE set of factories
};

// We have 5 threads per online phase
//   - Online
//   - Sacrifice (and input tuple production)
//   - Mult Triple Production
//   - Square Pair Production
//   - Bit Production
vector<thread> threads;

Timer global_time;

// Forward declarations to make code easier to read
//template<class SRegint, class SBit>
template<class SRegint, class SBit>
void Main_Func(thread_info<SRegint, SBit> &tinfo);

vector<sacrificed_data> SacrificeD;

/* Global data structure to hold the OT stuff */
OT_Thread_Data OTD;

/* Global data structure to hold the Mod2Triple stuff */
Mod2_Thread_Data MTD;

/* Before calling this we assume various things have
 * been set up. In particular the following functions have
 * been called
 *
 *    Init_SSL_CTX
 *    gfp::init_field
 *    Share::init_share_data
 *    machine.SetUp_Memory
 *    machine.IO.init
 *    FHE data has been initialized if needed
 *
 * We also assume the machine.schedule has been initialised
 * with some stringstream tapes and a stringstream to a schedule file
 *
 * This function assumes that afterwards we sort out 
 * closing down SSL and Dump'ing memory if need be
 * for a future application
 *
 */
template<class SRegint, class SBit>
void Run_Scale(unsigned int my_number, unsigned int no_online_threads,
               const FFT_Data &PTD, const FHE_PK &pk, const FHE_SK &sk, const vector<gfp> &MacK,
               SSL_CTX *ctx, const vector<unsigned int> &portnum,
               const SystemData &SD,
               Machine<SRegint, SBit> &machine,
               offline_control_data &OCD,
               unsigned int number_FHE_threads,
               bool OT_disable,
               int verbose)
{
  machine.Load_Schedule_Into_Memory();
  machine.SetUp_Threads(no_online_threads);

  Global_Circuit_Store.initialize(gfp::pr());

  OCD.resize(no_online_threads, SD.n, my_number);
  OTD.init(no_online_threads, OT_disable);
  MTD.init(no_online_threads, OT_disable);

  SacrificeD.resize(no_online_threads);
  for (unsigned int i= 0; i < no_online_threads; i++)
    {
      SacrificeD[i].initialize(SD.n);
    }

  unsigned int nthreads= 5 * no_online_threads;
  // Add in the FHE threads
  unsigned int tnthreads= nthreads + number_FHE_threads;
  // Add in the Mod2 Engine threads
  if (!OTD.disabled)
    {
      if (Share::SD.Etype == HSS)
        {
          tnthreads+= 2;
        }
      else
        {
          tnthreads+= 1;
        }
    }
  machine.daBitMachine.Initialize(SD.n, OCD);

  /* Initialize the networking TCP sockets */
  int ssocket;
  vector<vector<vector<int>>> csockets(tnthreads, vector<vector<int>>(SD.n, vector<int>(maximum_number_of_ssl_connections)));
  Get_Connections(ssocket, csockets, portnum, my_number, SD, verbose - 2);
  printf("All connections now done\n");

  FHE_Industry industry(number_FHE_threads);

  global_time.start();

  printf("Setting up threads\n");
  fflush(stdout);
  vector<thread_info<SRegint, SBit>> tinfo(tnthreads);
  for (unsigned int i= 0; i < tnthreads; i++)
    {
      if (i < nthreads)
        {
          tinfo[i].thread_num= i;
        }
      else if (i < nthreads + number_FHE_threads)
        {
          tinfo[i].thread_num= 10000 + i - nthreads;
        }
      else
        {
          tinfo[i].thread_num= 20000 + i - nthreads - number_FHE_threads;
        }
      tinfo[i].SD= &SD;
      tinfo[i].pr= gfp::pr();
      tinfo[i].OCD= &OCD;
      tinfo[i].ctx= ctx;
      tinfo[i].me= my_number;
      tinfo[i].no_online_threads= no_online_threads;
      tinfo[i].csockets= csockets[i];
      tinfo[i].MacK= MacK;
      tinfo[i].machine= &machine;
      tinfo[i].industry= &industry;
      tinfo[i].pk= &pk;
      tinfo[i].sk= &sk;
      tinfo[i].PTD= &PTD;
      tinfo[i].verbose= verbose;
      thread th1(Main_Func<SRegint, SBit>, ref(tinfo[i]));
      threads.push_back(move(th1));
    }

  // Get all online threads in sync
  machine.Synchronize();

  // Now run the programs
  machine.run();

  printf("Waiting for all clients to finish\n");
  fflush(stdout);
  for (unsigned int i= 0; i < tnthreads; i++)
    {
      threads[i].join();
      cout << "Joined Thread " << i << endl;
    }

  Close_Connections(ssocket, csockets, my_number);

  global_time.stop();
  cout << "Total Time (with thread locking) = " << global_time.elapsed() << " seconds" << endl;

  long long total_triples= 0, total_squares= 0, total_bits= 0;
  for (size_t i= 0; i < no_online_threads; i++)
    {
      total_triples+= OCD.totm[i];
      total_squares+= OCD.tots[i];
      total_bits+= OCD.totb[i];
    }
  cout << "Produced a total of " << total_triples << " triples" << endl;
  cout << "Produced a total of " << total_squares << " squares" << endl;
  cout << "Produced a total of " << total_bits << " bits" << endl;
}

#ifdef BENCH_MEMORY
void Print_Memory_Info(int player_num, int thread_num)
{
  int who= RUSAGE_THREAD;
  //int who = RUSAGE_SELF; // for the calling process
  struct rusage r_usage;

  int ret= getrusage(who, &r_usage);
  if (ret != 0)
    {
      printf(BENCH_TEXT_BOLD BENCH_COLOR_RED BENCH_MAGIC_START
             "MEMORY:\n"
             "  PLAYER#%d->THREAD#%u (PROCESS#%d)\n"
             "  ERROR: return value -> %d\n" BENCH_MAGIC_END BENCH_ATTR_RESET,
             player_num, thread_num, who, ret);
    }
  else
    {
      printf(BENCH_TEXT_BOLD BENCH_COLOR_RED BENCH_MAGIC_START
             "{\"player\":%u,\n"
             "  \"thread\":%d,\n"
             "  \"process\":%d,\n"
             "  \"memory\":{\n"
             "    \"max_rss\":{\"KB\":%ld,\"MB\":%.2f}\n"
             "  }\n"
             "}\n" BENCH_MAGIC_END BENCH_ATTR_RESET,
             player_num, thread_num, who, r_usage.ru_maxrss, ((double) r_usage.ru_maxrss / 1000));
    }
}
#endif

template<class SRegint, class SBit>
void Main_Func(thread_info<SRegint, SBit> &tinfo)
{
  //Init thread_local gfp value
  gfp::init_field(tinfo.pr);

  unsigned int num= tinfo.thread_num;
  int me= tinfo.me;
  int verbose= tinfo.verbose;
  printf("I am player %d in thread %d\n", me, num);
  fflush(stdout);

  Player P(me, *(tinfo.SD), num, (tinfo.ctx), (tinfo.csockets), (tinfo.MacK), verbose - 1);

  printf("Set up player %d in thread %d \n", me, num);
  fflush(stdout);

  int num5= num % 5;
  if (num < 10000)
    {
      int num_online= (num - num5) / 5;
      switch (num5)
        {
          case 0:
            mult_phase(num_online, P, (tinfo.SD)->fake_sacrifice, *(tinfo.OCD),
                       *(tinfo.pk), *(tinfo.sk), *(tinfo.PTD), *(tinfo.industry), verbose);
            break;
          case 1:
            square_phase(num_online, P, (tinfo.SD)->fake_sacrifice, *(tinfo.OCD),
                         *(tinfo.pk), *(tinfo.sk), *(tinfo.PTD), *(tinfo.industry), verbose);
            break;
          case 2:
            bit_phase(num_online, P, (tinfo.SD)->fake_sacrifice, *(tinfo.OCD),
                      *(tinfo.pk), *(tinfo.sk), *(tinfo.PTD), *(tinfo.industry), verbose);
            break;
          case 3:
            inputs_phase(num_online, P, (tinfo.SD)->fake_sacrifice, *(tinfo.OCD),
                         *(tinfo.pk), *(tinfo.sk), *(tinfo.PTD), *(tinfo.industry), verbose);
            break;
          case 4:
            online_phase(num_online, P, *(tinfo.OCD), *(tinfo.machine));
            break;
          default:
            throw bad_value();
            break;
        }
    }
  else if (num < 20000)
    {
      (*(tinfo.industry)).FHE_Factory(P, *(tinfo.OCD), *(tinfo.pk), *(tinfo.PTD), num - 10000, verbose);
    }
  else if (num == 20000)
    {
      if (Share::SD.Etype == HSS)
        {
          aBit_Thread(P, (tinfo.no_online_threads), *(tinfo.OCD), verbose);
        }
      else
        {
          Mod2_Triple_Thread(P, (tinfo.no_online_threads), *(tinfo.OCD), verbose);
        }
    }
  else if (num == 20001)
    {
      aAND_Thread(P, (tinfo.no_online_threads), *(tinfo.OCD), verbose);
    }
  else
    {
      throw bad_value();
    }

#ifdef BENCH_NETDATA
  P.print_network_data();
#endif

#ifdef BENCH_MEMORY
  Print_Memory_Info(me, num);
#endif

#ifdef BENCH_OFFLINE
  if (num < 10000 && num5 == 4)
    {
      P.print_offline();
    }
#endif
}

template void Main_Func(thread_info<aBitVector, aBit> &tinfo);
template void Main_Func(thread_info<aBitVector2, Share2> &tinfo);

template void Run_Scale(unsigned int my_number, unsigned int no_online_threads,
                        const FFT_Data &PTD, const FHE_PK &pk, const FHE_SK &sk, const vector<gfp> &MacK,
                        SSL_CTX *ctx, const vector<unsigned int> &portnum,
                        const SystemData &SD,
                        Machine<aBitVector, aBit> &machine,
                        offline_control_data &OCD,
                        unsigned int number_FHE_threads,
                        bool OT_disable,
                        int verbose);

template void Run_Scale(unsigned int my_number, unsigned int no_online_threads,
                        const FFT_Data &PTD, const FHE_PK &pk, const FHE_SK &sk, const vector<gfp> &MacK,
                        SSL_CTX *ctx, const vector<unsigned int> &portnum,
                        const SystemData &SD,
                        Machine<aBitVector2, Share2> &machine,
                        offline_control_data &OCD,
                        unsigned int number_FHE_threads,
                        bool OT_disable,
                        int verbose);
