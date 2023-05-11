/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/
#ifndef _Processor
#define _Processor

#include <map>
#include <stack>

#include "LSSS/Open_Protocol.h"
#include "LSSS/Open_Protocol2.h"
#include "Online/Machine.h"

#include "Converter/Base_Matrix_Triple.h"
#include "Converter/Triple_Type_DTO.h"
#include "Local/Float.h"
#include "Mod2Engine/aBitVector2.h"
#include "OT/Choicebits.h"
#include "OT/aAND.h"
#include "OT/aBitVector.h"
#include "Offline/DABitGenerator.h"
#include "Processor_IO.h"
#include "Processor_exceptions.h"
#include "Program.h"
#include "Utils/DTO_Format_Utils.h"
#include <Converter/Converter_Storage.h>
#include <GC/Garbled_Circuit_Storage.h>

long convert_to_long(double x);

using namespace std;

struct TempVars
{
  gfp ansp;
  Share Sansp;
  bigint aa, aa2;
  // INPUT and LDSI
  gfp rrp, tp, tmpp;
  gfp xip;
  aBitVector aBV;
};

template<class SRegint, class SBit>
class Processor
{
  // Various stacks
  //   The integer stack is also used for call/return positions
  //   within this tape.
  vector<gfp> stack_Cp;
  vector<Share> stack_Sp;
  vector<long> stack_int;
  vector<SRegint> stack_srint;
  vector<SBit> stack_sbit;
  vector<Base_Garbled_Circuit> circuits;
  shared_ptr<Garbled_Circuit_Storage> gc_storage;

  Base_Matrix_Triple base_matrix_triple_store;

  multimap<int, vector<Share>> matrix_triples;
  shared_ptr<Converter_Storage> converter_storage;
  // Optional argument to the tape
  int arg;

  // Registers of various types
  vector<gfp> Cp;
  vector<Share> Sp;
  vector<long> Ri;
  vector<SRegint> srint;
  vector<SBit> sbit;

// In DEBUG mode we keep track of valid/invalid read/writes on the registers
#ifdef DEBUG
  vector<int> rwp;
  vector<int> rwi;
  vector<int> rwsr;
  vector<int> rwsb;
#endif

  // Program counter
  unsigned int PC;
  // Current program size being executed by this processor
  unsigned int current_program_size;

  // These are here for DEBUG mode
  int reg_maxp, reg_maxi, reg_maxb;

  // This is the vector of partially opened values and shares we need to store
  // as the Open commands are split in two
  vector<gfp> PO;
  vector<Share> Sh_PO;

  int online_thread_num;

  // Keep track of how much data sent, and in how many rounds
  unsigned int sent, rounds;

  const unsigned int number_of_original_connections= 3;

  PRNG prng;

  /* Stuff for dealing with the Garbled Circuit functionality
   * within instructions
   */
  // In the case when the OT thread is active this holds the daBitGenerator for this thread
  AbstractDABitGenerator<SBit> *daBitGen;
  // This holds the computed daBits
  daBitVector<SBit> daBitV;

  // To make sure we do not need to keep allocating/deallocating memory
  // we maintain some scratch variables for use in routines
  TempVars temp;

  // Data structures for input and output of private data
  Processor_IO<SRegint, SBit> iop;

  // retrieve dabit generator to avoid dealing with pointers
  AbstractDABitGenerator<SBit> &get_generator()
  {
    return *daBitGen;
  }

  // This is a special version for when log_2(p)<sreg_bitl
  void convert_sint_to_sregint_small(int i0, int i1, Player &P);

  // Subroutine used by the two variants of this
  //   Uses daBits
  void convert_sregint_to_sint_sub(int i0, vector<Share> &apr, Player &P);

  void convert_sint_to_sregint_sub(const vector<vector<aBit>> &input, int i1, Player &P);
  void convert_sint_to_sregint_sub(const vector<vector<Share2>> &input, int i1, Player &P);

public:
  friend class Instruction<SRegint, SBit>;

  Processor(int online_thread_num, unsigned int nplayers,
            Machine<SRegint, SBit> &machine, Player &P);

  ~Processor();

  void clear_registers();

  int get_thread_num() const
  {
    return online_thread_num;
  }

  // Stack operations
  void push_int(long x)
  {
    stack_int.push_back(x);
  }
  void pop_int(long &x)
  {
    if (stack_int.size() < 1)
      {
        throw stack_error();
      }
    x= stack_int.back();
    stack_int.pop_back();
  }
  void peek_int(long &x, long y) const
  {
    if (y < 0 || y >= (long) stack_int.size())
      {
        throw stack_error();
      }
    x= stack_int[y];
  }
  void poke_int(long x, long y)
  {
    if (x < 0 || x >= (long) stack_int.size())
      {
        throw stack_error();
      }
    stack_int[x]= y;
  }
  void rpeek_int(long &x, long y) const
  {
    if (y < 0 || y >= (long) stack_int.size())
      {
        throw stack_error();
      }
    x= stack_int[stack_int.size() - 1 - y];
  }
  void rpoke_int(long x, long y)
  {
    if (x < 0 || x >= (long) stack_int.size())
      {
        throw stack_error();
      }
    stack_int[stack_int.size() - 1 - x]= y;
  }
  void getsp_int(long &x) const
  {
    x= (long) stack_int.size() - 1;
  }

  void push_srint(const SRegint &x)
  {
    stack_srint.push_back(x);
  }
  void pop_srint(SRegint &x)
  {
    if (stack_srint.size() < 1)
      {
        throw stack_error();
      }
    x= stack_srint.back();
    stack_srint.pop_back();
  }
  void peek_srint(SRegint &x, long y) const
  {
    if (y < 0 || y >= (long) stack_srint.size())
      {
        throw stack_error();
      }
    x= stack_srint[y];
  }
  void poke_srint(long x, const SRegint &y)
  {
    if (x < 0 || x >= (long) stack_srint.size())
      {
        throw stack_error();
      }
    stack_srint[x]= y;
  }
  void rpeek_srint(SRegint &x, long y) const
  {
    if (y < 0 || y >= (long) stack_srint.size())
      {
        throw stack_error();
      }
    x= stack_srint[stack_srint.size() - 1 - y];
  }
  void rpoke_srint(long x, const SRegint &y)
  {
    if (x < 0 || x >= (long) stack_srint.size())
      {
        throw stack_error();
      }
    stack_srint[stack_srint.size() - 1 - x]= y;
  }
  void getsp_srint(long &x) const
  {
    x= (long) stack_srint.size() - 1;
  }

  void push_Cp(const gfp &x)
  {
    stack_Cp.push_back(x);
  }
  void pop_Cp(gfp &x)
  {
    if (stack_Cp.size() < 1)
      {
        throw stack_error();
      }
    x= stack_Cp.back();
    stack_Cp.pop_back();
  }
  void peek_Cp(gfp &x, long y) const
  {
    if (y < 0 || y >= (long) stack_Cp.size())
      {
        throw stack_error();
      }
    x= stack_Cp[y];
  }
  void poke_Cp(long x, const gfp &y)
  {
    if (x < 0 || x >= (long) stack_Cp.size())
      {
        throw stack_error();
      }
    stack_Cp[x]= y;
  }
  void rpeek_Cp(gfp &x, long y) const
  {
    if (y < 0 || y >= (long) stack_Cp.size())
      {
        throw stack_error();
      }
    x= stack_Cp[stack_Cp.size() - 1 - y];
  }
  void rpoke_Cp(long x, const gfp &y)
  {
    if (x < 0 || x >= (long) stack_Cp.size())
      {
        throw stack_error();
      }
    stack_Cp[stack_Cp.size() - 1 - x]= y;
  }
  void getsp_Cp(long &x) const
  {
    x= (long) stack_Cp.size() - 1;
  }
  void push_Sp(const Share &x)
  {
    stack_Sp.push_back(x);
  }
  void pop_Sp(Share &x)
  {
    if (stack_Sp.size() < 1)
      {
        throw stack_error();
      }
    x= stack_Sp.back();
    stack_Sp.pop_back();
  }
  void peek_Sp(Share &x, long y) const
  {
    if (y < 0 || y >= (long) stack_Sp.size())
      {
        throw stack_error();
      }
    x= stack_Sp[y];
  }
  void poke_Sp(long x, const Share &y)
  {
    if (x < 0 || x >= (long) stack_Sp.size())
      {
        throw stack_error();
      }
    stack_Sp[x]= y;
  }
  void rpeek_Sp(Share &x, long y) const
  {
    if (y < 0 || y >= (long) stack_Sp.size())
      {
        throw stack_error();
      }
    x= stack_Sp[stack_Sp.size() - 1 - y];
  }
  void rpoke_Sp(long x, const Share &y)
  {
    if (x < 0 || x >= (long) stack_Sp.size())
      {
        throw stack_error();
      }
    stack_Sp[stack_Sp.size() - 1 - x]= y;
  }
  void getsp_Sp(long &x) const
  {
    x= (long) stack_Sp.size() - 1;
  }
  void push_sbit(const SBit &x)
  {
    stack_sbit.push_back(x);
  }
  void pop_sbit(SBit &x)
  {
    if (stack_sbit.size() < 1)
      {
        throw stack_error();
      }
    x= stack_sbit.back();
    stack_sbit.pop_back();
  }
  void peek_sbit(SBit &x, long y) const
  {
    if (y < 0 || y >= (long) stack_sbit.size())
      {
        throw stack_error();
      }
    x= stack_sbit[y];
  }
  void poke_sbit(long x, const SBit &y)
  {
    if (x < 0 || x >= (long) stack_sbit.size())
      {
        throw stack_error();
      }
    stack_sbit[x]= y;
  }
  void rpeek_sbit(SBit &x, long y) const
  {
    if (y < 0 || y >= (long) stack_sbit.size())
      {
        throw stack_error();
      }
    x= stack_sbit[stack_sbit.size() - 1 - y];
  }
  void rpoke_sbit(long x, const SBit &y)
  {
    if (x < 0 || x >= (long) stack_sbit.size())
      {
        throw stack_error();
      }
    stack_sbit[stack_sbit.size() - 1 - x]= y;
  }
  void getsp_sbit(long &x) const
  {
    x= (long) stack_sbit.size() - 1;
  }

  // Argument operations
  int get_arg() const
  {
    return arg;
  }
  void set_arg(int new_arg)
  {
    arg= new_arg;
  }

  // Increment program counter
  void increment_PC()
  {
    PC++;
  }
  // Perform a jump
  void jump(unsigned int new_pc)
  {
    PC= new_pc;
  }
  void relative_jump(signed int jump)
  {
    PC+= jump;
  }
  unsigned int get_PC() const
  {
    return PC;
  }
  unsigned int program_size() const
  {
    return current_program_size;
  }

/* Read and write the registers */
#ifdef DEBUG
  const gfp &read_Cp(int i) const
  {
    if (rwp[i] == 0)
      {
        throw Processor_Error("Invalid read on clear register");
      }
    return Cp.at(i);
  }
  const Share &read_Sp(int i) const
  {
    if (rwp[i + reg_maxp] == 0)
      {
        throw Processor_Error("Invalid read on shared register");
      }
    return Sp.at(i);
  }
  gfp &get_Cp_ref(int i)
  {
    rwp[i]= 1;
    return Cp.at(i);
  }
  Share &get_Sp_ref(int i)
  {
    rwp[i + reg_maxp]= 1;
    return Sp.at(i);
  }
  void write_Cp(int i, const gfp &x)
  {
    rwp[i]= 1;
    Cp.at(i)= x;
  }
  void write_Sp(int i, const Share &x)
  {
    rwp[i + reg_maxp]= 1;
    Sp.at(i)= x;
  }

  const long &read_Ri(int i) const
  {
    if (rwi[i] == 0)
      {
        throw Processor_Error("Invalid read on integer register");
      }
    return Ri.at(i);
  }
  long &get_Ri_ref(int i)
  {
    rwi[i]= 1;
    return Ri.at(i);
  }
  void write_Ri(int i, const long &x)
  {
    rwi[i]= 1;
    Ri.at(i)= x;
  }

  const SRegint &read_srint(int i) const
  {
    if (rwsr[i] == 0)
      {
        throw Processor_Error("Invalid read on srint register");
      }
    return srint.at(i);
  }
  SRegint &get_srint_ref(int i)
  {
    rwsr[i]= 1;
    return srint.at(i);
  }
  void write_srint(int i, const SRegint &x)
  {
    rwsr[i]= 1;
    srint.at(i)= x;
  }

  const SBit &read_sbit(int i) const
  {
    if (rwsb[i] == 0)
      {
        throw Processor_Error("Invalid read on sbit register");
      }
    return sbit.at(i);
  }
  SBit &get_sbit_ref(int i)
  {
    rwsb[i]= 1;
    return sbit.at(i);
  }
  void write_sbit(int i, const SBit &x)
  {
    rwsb[i]= 1;
    sbit.at(i)= x;
  }

  void write_daBit(int i1, int j1, Player &P)
  {
    SBit aB;
    daBitV.get_daBit(temp.Sansp, aB, *daBitGen, P);
    rwp[i1 + reg_maxp]= 1;
    rwsb[j1]= 1;
    Sp.at(i1)= temp.Sansp;
    sbit.at(j1)= aB;
  }

#else
  const gfp &read_Cp(int i) const
  {
    return Cp[i];
  }
  const Share &read_Sp(int i) const
  {
    return Sp[i];
  }
  gfp &get_Cp_ref(int i)
  {
    return Cp[i];
  }
  Share &get_Sp_ref(int i)
  {
    return Sp[i];
  }
  void write_Cp(int i, const gfp &x)
  {
    Cp[i]= x;
  }
  void write_Sp(int i, const Share &x)
  {
    Sp[i]= x;
  }

  const long &read_Ri(int i) const
  {
    return Ri[i];
  }
  long &get_Ri_ref(int i)
  {
    return Ri[i];
  }
  void write_Ri(int i, const long &x)
  {
    Ri[i]= x;
  }

  const SRegint &read_srint(int i) const
  {
    return srint[i];
  }
  SRegint &get_srint_ref(int i)
  {
    return srint[i];
  }
  void write_srint(int i, const SRegint &x)
  {
    srint[i]= x;
  }

  const SBit &read_sbit(int i) const
  {
    return sbit[i];
  }
  SBit &get_sbit_ref(int i)
  {
    return sbit[i];
  }
  void write_sbit(int i, const SBit &x)
  {
    sbit[i]= x;
  }
  void write_daBit(int i1, int j1, Player &P)
  {
    SBit aB;
    daBitV.get_daBit(temp.Sansp, aB, *daBitGen, P);
    write_Sp(i1, temp.Sansp);
    write_sbit(j1, aB);
  }

  void load_matrix_triples(unsigned int triple_id, unsigned int number_of_times, const Player &P)
  {

    Triple_Type_DTO *TT= nullptr;

    if (base_matrix_triple_store.Triple_Types.empty())
      {
        base_matrix_triple_store.initialize(P.whoami());
      }

    if (base_matrix_triple_store.Triple_Types.count(triple_id) > 0)
      {
        TT= &base_matrix_triple_store.Triple_Types.at(triple_id);
      }
    else
      {
        throw not_implemented();
      }

    int total_shares_per_triple= TT->get_shares_per_matrix_triple();
    int total_shares_per_query= total_shares_per_triple * number_of_times;

    vector<Share_DTO> read_matrices= converter_storage->read(total_shares_per_query, *TT, base_matrix_triple_store.last_retrieved_index_per_channel[TT->get_id()], P.whoami());
    vector<Share_DTO>::iterator itv= read_matrices.begin();

    for (unsigned int i= 0; i < number_of_times; i++)
      {
        vector<Share> matrix_triple;

        for (int j= 0; j < total_shares_per_triple; j++)
          {
            string formatted_data= DTO_Format_Utils::format_database_output_to_share(*itv);

            std::stringstream data_as_stream;
            data_as_stream.str(formatted_data);

            Share S;
            S.input(data_as_stream, true);
            matrix_triple.push_back(S);
            itv++;
          }

        matrix_triples.insert(make_pair(TT->get_id(), matrix_triple));
      }
    if (ignore_share_db_count == 0)
      {
        base_matrix_triple_store.last_retrieved_index_per_channel[TT->get_id()]= read_matrices.end()->get_id();
      }
  }

  vector<Share> obtain_matrix_triple_by_triple_id(unsigned int triple_id, const Player &P)
  {
    Triple_Type_DTO *TT= nullptr;

    if (base_matrix_triple_store.Triple_Types.empty())
      {
        base_matrix_triple_store.initialize(P.whoami());
      }

    if (base_matrix_triple_store.Triple_Types.count(triple_id) > 0)
      {
        TT= &base_matrix_triple_store.Triple_Types.at(triple_id);
      }
    else
      {
        throw not_implemented();
      }
    for (map<int, vector<Share>>::iterator it= matrix_triples.begin(); it != matrix_triples.end(); ++it)
      {
        if ((unsigned int) it->first == TT->get_id())
          {
            vector<Share> matrix_triple= it->second;
            matrix_triples.erase(it);
            return matrix_triple;
          }
      }
    throw triple_not_available();
  }

  vector<Share> obtain_next_matrix_triple([[maybe_unused]] const Player &P)
  {

    map<int, vector<Share>>::iterator it;
    it= matrix_triples.begin();
    vector<Share>
        matrix_triple= it->second;
    matrix_triples.erase(it);
    // we pass it so that it can be used in future release
    return matrix_triple;
  }

#endif

  /* Run interaction with other players */

  /* Direct access to PO class*/
  void Open_To_All_Begin(vector<gfp> &values, const vector<Share> &S, Player &P, int connection)
  {
    P.OP->Open_To_All_Begin(values, S, P, connection);
  }
  void Open_To_All_End(vector<gfp> &values, const vector<Share> &S, Player &P, int connection)
  {
    P.OP->Open_To_All_End(values, S, P, connection);
  }

  /* Open/Close Registers*/
  void POpen_Start(const vector<int> &reg, int size, Player &P);
  void POpen_Stop(const vector<int> &reg, int size, Player &P);
  void POpen_Start_Batched(const vector<int> &reg, int size, Player &P);
  void POpen_Stop_Batched(const vector<int> &reg, int size, Player &P);

  void RunOpenCheck(Player &P, const string &aux, int connection)
  {
    P.OP->RunOpenCheck(P, aux, connection, false);
    P.OP2->RunOpenCheck(P, aux, connection, false);
  }

  // Now the routine to execute a program with given argument
  // and start at position start
  void execute(const Program<SRegint, SBit> &prog, int argument,
               unsigned int start,
               Player &P,
               Machine<SRegint, SBit> &machine,
               offline_control_data &OCD);

  word get_random_word()
  {
    return prng.get_word();
  }

  gfp get_random_gfp()
  {
    gfp a;
    a.randomize(prng);
    return a;
  }

  long get_random_float()
  {
    unsigned int seed= prng.get_uint();
    srand48(seed);
    double f= drand48();
    return convert_to_long(f);
  }

  // Add rounds up and add data sent in
  void increment_counters(unsigned int size)
  {
    sent+= size;
    rounds++;
  }

  // Converts a sint register i0 to a sregint register i1
  //   Uses the daBits
  void convert_sint_to_sregint(int i0, int i1, Player &P);

  // Converts a sregint register i0 to a srint register i1
  //   Uses the daBits
  void convert_sregint_to_sint(int i0, int i1, Player &P);

  // Converts a sregint register i0 to a srint register i1
  // But converts as an unsigned sregint
  //   Uses the daBits
  void convert_suregint_to_sint(int i0, int i1, Player &P);

  // Converts a sint register i0 to a sbit register i1
  // Programmer must guarantee that i0 only contains a bit
  //   Uses the daBits
  void convert_sint_to_sbit(int i0, int i1, Player &P, offline_control_data &OCD);

  // Converts an sbit register i0 to an sint register i1
  //   Uses the daBits
  void convert_sbit_to_sint(int i0, int i1, Player &P);

  // Apply one of the indirect GC's
  void apply_GC(unsigned int n, Player &P);

  void garble_circuit(unsigned int circuit_number, unsigned int number_of_times, Player &P);

  void load_garbled_circuit(unsigned int circuit_number, unsigned int number_of_times, Player &P);

  void evaluate_garbled_circuit(unsigned int circuit_number, Player &P);
};

#endif
