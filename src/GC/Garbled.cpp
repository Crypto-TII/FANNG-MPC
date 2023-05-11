/*
Copyright (c) 2017, The University of Bristol, Senate House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
Copyright (c) 2021, COSIC-KU Leuven, Kasteelpark Arenberg 10, bus 2452, B-3001 Leuven-Heverlee, Belgium.

All rights reserved
*/

#include "Garbled.h"
#include "LSSS/PRSS.h"
#include "OT/OT_Thread_Data.h"

extern OT_Thread_Data OTD;

// Apply the PRF with keys k1 and k2 to the message (g||j) where
// j runs through 1 to n. To get an n-vector of gf2n elements out
void Base_Garbled_Circuit::apply_PRF(vector<gf2n> &ans,
                                     const gf2n &k1, const gf2n &k2, unsigned int g)
{

  gf2n temp1, temp2, x;
  x.assign_x();
  temp1.mul(k1, x);
  temp2.mul(k2, x);
  temp2.mul(temp2, x);
  temp1.add(temp2);

  uint8_t buffer[16], kbuffer[16], ibuffer[4], obuffer[16]; // 16 as gf2n is 128 bits long
  temp1.store_into_buffer(kbuffer);

  memset(buffer, 0, 16 * sizeof(uint8_t));
  INT_TO_BYTES(ibuffer, g);

  // We fix buffer[5...] here, the lower bytes are set in the main loop
  for (unsigned int k= 4; k < 16; k++)
    {
      if (k < 12)
        {
          buffer[k]= kbuffer[k];
        }
      else
        {
          buffer[k]= kbuffer[k] ^ ibuffer[k - 12];
        }
    }

  for (unsigned int j= 0; j < ans.size(); j++)
    {
      INT_TO_BYTES(ibuffer, j);
      for (unsigned int k= 0; k < 4; k++)
        {
          buffer[k]= kbuffer[k] ^ ibuffer[k];
        }
      mmo.hashOneBlock<gf2n>(obuffer, buffer);
      ans[j].load_from_buffer(obuffer);
    }
}

void Base_Garbled_Circuit::Garble(const Circuit &C,
                                  Player &P,
                                  unsigned int online_thread_no)
{
  // This follows the method on page 27 of ePrint 2017/214

  // First agree the per circuit MMO key
  uint8_t mmo_key[AES_BLK_SIZE];
  AgreeRandom(P, mmo_key, AES_BLK_SIZE, 2);
  mmo.setIV(mmo_key);

  // Now do the Garbling
  unsigned int n= P.nplayers();

  lambda.resize(C.get_nWires());
  k.resize(C.get_nWires(), vector<gf2n>(2));
  unsigned int nAG= C.num_AND_gates();
  vector<aBit> lambda_uv(nAG), lambda_u(nAG), lambda_v(nAG);
  gab.resize(nAG);

  // Step 2, 3 and 4
  // Fix the one label
  one_label.resize(n);
  one_label[P.whoami()].randomize(P.G);

  unsigned int cnt= C.num_AND_gates();
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      cnt+= C.num_iWires(i);
    }

  // Getting both lots of data at once as that avoid thread locks
  OTD.check();
  list<aBit> in_AND_aBits= OTD.aBD.get_aShares(online_thread_no, cnt);
  list<aTriple> triples= OTD.aAD.get_aANDs(online_thread_no, nAG);
  gf2n Delta= aBit::get_Delta();
#ifdef BENCH_OFFLINE
  P.aands+= nAG;
  P.abits+= cnt;
#endif

  unsigned int nI= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      for (unsigned int j= 0; j < C.num_iWires(i); j++)
        {
          lambda[nI]= in_AND_aBits.front();
          in_AND_aBits.pop_front();
          k[nI][0].randomize(P.G);
          k[nI][1].add(k[nI][0], Delta);
          nI++;
        }
    }
  for (unsigned int i= 0; i < C.get_nGates(); i++)
    {
      unsigned int iout= C.Gate_Wire_Out(i);
      if (C.get_GateType(i) == XOR)
        {
          lambda[iout].add(lambda[C.Gate_Wire_In(i, 0)],
                           lambda[C.Gate_Wire_In(i, 1)]);
          k[iout][0].add(k[C.Gate_Wire_In(i, 0)][0], k[C.Gate_Wire_In(i, 1)][0]);
        }
      else if (C.get_GateType(i) == AND)
        {
          lambda[iout]= in_AND_aBits.front();
          in_AND_aBits.pop_front();
          k[iout][0].randomize(P.G);
        }
      else if (C.get_GateType(i) == INV)
        {
          lambda[iout]= lambda[C.Gate_Wire_In(i, 0)];
          k[iout][0].add(k[C.Gate_Wire_In(i, 0)][0], one_label[P.whoami()]);
        }
      else if (C.get_GateType(i) == EQW)
        {
          lambda[iout]= lambda[C.Gate_Wire_In(i, 0)];
          k[iout][0]= k[C.Gate_Wire_In(i, 0)][0];
        }
      else
        {
          throw circuit_error();
        }
      k[iout][1].add(k[iout][0], Delta);
    }

  // Step 5
  for (unsigned int g= 0; g < nAG; g++)
    {
      unsigned int i= C.get_nth_AND_Gate(g);
      lambda_u[g]= lambda[C.Gate_Wire_In(i, 0)];
      lambda_v[g]= lambda[C.Gate_Wire_In(i, 1)];
    }
  Mult_aBits(lambda_uv, lambda_u, lambda_v, triples, P);

  // Step 6
  vector<vector<vector<gf2n>>> rho(2, vector<vector<gf2n>>(2, vector<gf2n>(n)));
  vector<gf2n> s_l_u, s_l_v;
  for (unsigned int g= 0; g < nAG; g++)
    {
      unsigned int i= C.get_nth_AND_Gate(g);
      lambda_u[g].get_Share_x_Delta_j(s_l_u);
      lambda_v[g].get_Share_x_Delta_j(s_l_v);
      lambda_uv[g].add(lambda[C.Gate_Wire_Out(i)]);
      lambda_uv[g].get_Share_x_Delta_j(rho[0][0]);
      for (unsigned int j= 0; j < n; j++)
        {
          rho[1][0][j].add(rho[0][0][j], s_l_v[j]);
          rho[0][1][j].add(rho[0][0][j], s_l_u[j]);
          rho[1][1][j].add(rho[1][0][j], s_l_u[j]);
          if (j == P.whoami())
            {
              rho[1][1][j].add(Delta);
            }
        }

      // Now do Step 4 of Figure 6
      gab[g].resize(2);
      for (unsigned int a= 0; a < 2; a++)
        {
          gab[g][a].resize(2);
          for (unsigned int b= 0; b < 2; b++)
            {
              gab[g][a][b].resize(n);
              apply_PRF(gab[g][a][b], k[C.Gate_Wire_In(i, 0)][a], k[C.Gate_Wire_In(i, 1)][b], g);
              for (unsigned int j= 0; j < n; j++)
                {
                  gab[g][a][b][j].add(rho[a][b][j]);
                  if (j == P.whoami())
                    {
                      gab[g][a][b][j].add(k[C.Gate_Wire_Out(i)][0]);
                    }
                }
            }
        }
    }

  // Now opening garbling, altering the one_label
  one_label[P.whoami()].add(Delta);
  Open_Garbling(P);
}

void Garbled_Circuit::Garble(const Circuit &C,
                             const vector<unsigned int> &i_a, const vector<unsigned int> &o_a,
                             Player &P,
                             unsigned int online_thread_no)
{
  Base_Garbled_Circuit::Garble(C, P, online_thread_no);

  extern aBit_Data aBD;

  // This follows the method on page 27 of ePrint 2017/214

  // Step 2, 3 and 4

  i_assign= i_a;
  o_assign= o_a;
  if (C.num_inputs() != i_assign.size())
    {
      throw bad_value();
    }

  if (C.num_outputs() != o_assign.size())
    {
      throw bad_value();
    }

  // Step 7
  // Open output wires to player i
  vector<aBit> data;
  vector<word> dummy;
  unsigned int cnt= C.get_nWires();
  for (unsigned int i= 0; i < C.num_outputs(); i++)
    {
      cnt-= C.num_oWires(i);
    }

  outputs.resize(C.num_outputs());
  for (unsigned int i= 0; i < C.num_outputs(); i++)
    {
      data.resize(C.num_oWires(i));
      for (unsigned int j= 0; j < C.num_oWires(i); j++)
        {
          data[j]= lambda[cnt];
          cnt++;
        }
      if (o_assign[i] == P.whoami())
        {
          P.OP2->Open_Bits_To(outputs[i], o_assign[i], data, P);
        }
      else
        {
          P.OP2->Open_Bits_To(dummy, o_assign[i], data, P);
        }
    }

  // Step 8
  // Do the same with input wires
  inputs.resize(C.num_inputs());
  cnt= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      data.resize(C.num_iWires(i));
      for (unsigned int j= 0; j < C.num_iWires(i); j++)
        {
          data[j]= lambda[cnt];
          cnt++;
        }
      if (i_assign[i] == P.whoami())
        {
          P.OP2->Open_Bits_To(inputs[i], i_assign[i], data, P);
        }
      else
        {
          P.OP2->Open_Bits_To(dummy, i_assign[i], data, P);
        }
    }
}

void Base_Garbled_Circuit::Open_Garbling(Player &P)
{
  vector<string> oo(P.nplayers());

  uint8_t *buff= new uint8_t[(gab.size() * 4 * P.nplayers() + 1) * gf2n::size()];
  unsigned int pos= 0;
  for (unsigned int g= 0; g < gab.size(); g++)
    {
      for (unsigned int a= 0; a < 2; a++)
        {
          for (unsigned int b= 0; b < 2; b++)
            {
              for (unsigned int i= 0; i < P.nplayers(); i++)
                {
                  pos+= gab[g][a][b][i].output(buff + pos);
                }
            }
        }
    }
  pos+= one_label[P.whoami()].output(buff + pos);
  oo[P.whoami()]= string((char *) buff, pos);
  delete[] buff;

  P.Broadcast_Receive(oo, true, 2);

  gf2n temp;
  for (unsigned int k= 0; k < P.nplayers(); k++)
    {
      if (k != P.whoami())
        {
          unsigned int pos= 0;
          for (unsigned int g= 0; g < gab.size(); g++)
            {
              for (unsigned int a= 0; a < 2; a++)
                {
                  for (unsigned int b= 0; b < 2; b++)
                    {
                      for (unsigned int i= 0; i < P.nplayers(); i++)
                        {
                          pos+= temp.input(oo[k], pos);
                          gab[g][a][b][i].add(temp);
                        }
                    }
                }
            }
          pos+= temp.input(oo[k], pos);
          one_label[k]= temp;
        }
    }
}

/* Figure 11 on page 29 of 2017/214 */
void Base_Garbled_Circuit::Evaluate_Core(vector<int> &Gamma,
                                         const Circuit &C, Player &P)
{
  // These hold the opened k values
  vector<vector<gf2n>> ok(C.get_nWires(), vector<gf2n>(P.nplayers()));

  // Line 1
  unsigned int tot_num_iwires= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      tot_num_iwires+= C.num_iWires(i);
    }

  // Line 2
  uint8_t *buff= new uint8_t[tot_num_iwires * gf2n::size()];
  vector<string> o(P.nplayers());
  unsigned int pos= 0;
  for (unsigned int i= 0; i < tot_num_iwires; i++)
    {
      ok[i][P.whoami()]= k[i][Gamma[i]];
      pos+= ok[i][P.whoami()].output(buff + pos);
    }
  o[P.whoami()]= string((char *) buff, pos);
  delete[] buff;

  P.Broadcast_Receive(o, true, 2);

  for (unsigned int i= 0; i < P.nplayers(); i++)
    {
      if (i != P.whoami())
        {
          unsigned int pos= 0;
          for (unsigned int j= 0; j < tot_num_iwires; j++)
            {
              pos+= ok[j][i].input(o[i], pos);
            }
        }
    }

  // Line 4
  unsigned int i0= -1, i1= -1, out;
  vector<vector<gf2n>> ans(P.nplayers(), vector<gf2n>(P.nplayers()));
  for (unsigned int i= 0; i < C.get_nGates(); i++)
    {
      i0= C.Gate_Wire_In(i, 0);
      if (C.get_GateType(i) != INV && C.get_GateType(i) != EQW)
        {
          i1= C.Gate_Wire_In(i, 1);
        }
      out= C.Gate_Wire_Out(i);
      if (C.get_GateType(i) == XOR)
        {
          Gamma[out]= Gamma[i0] ^ Gamma[i1];
          for (unsigned int j= 0; j < P.nplayers(); j++)
            {
              ok[out][j].add(ok[i0][j], ok[i1][j]);
            }
        }
      else if (C.get_GateType(i) == INV)
        {
          Gamma[out]= 1 - Gamma[i0];
          for (unsigned int j= 0; j < P.nplayers(); j++)
            {
              ok[out][j].add(ok[i0][j], one_label[j]);
            }
        }
      else if (C.get_GateType(i) == EQW)
        {
          Gamma[out]= Gamma[i0];
          for (unsigned int j= 0; j < P.nplayers(); j++)
            {
              ok[out][j]= ok[i0][j];
            }
        }
      else if (C.get_GateType(i) == AND)
        { // AND Gate
          unsigned int g= C.map_to_AND_Gate(i);
          for (unsigned int k= 0; k < P.nplayers(); k++)
            {
              apply_PRF(ans[k], ok[i0][k], ok[i1][k], g);
            }
          for (unsigned int j= 0; j < P.nplayers(); j++)
            {
              ok[out][j]= gab[g][Gamma[i0]][Gamma[i1]][j];
              for (unsigned int k= 0; k < P.nplayers(); k++)
                {
                  ok[out][j].add(ans[k][j]);
                }
            }
          // Line 4.3 and 4.4
          if (ok[out][P.whoami()] == k[out][0])
            {
              Gamma[out]= 0;
            }
          else if (ok[out][P.whoami()] == k[out][1])
            {
              Gamma[out]= 1;
            }
          else
            {
              throw circuit_eval_error();
            }
        }
      else
        {
          throw circuit_error();
        }
    }

  P.Check_Broadcast(2);
}

void Base_Garbled_Circuit::Evaluate(vector<vector<aBit>> &output,
                                    const vector<vector<aBit>> &input,
                                    const Circuit &C, Player &P)
{
  if (C.num_inputs() != input.size())
    {
      throw bad_value();
    }

  // These hold the external values
  vector<int> Gamma(C.get_nWires());

  unsigned int tot_num_iwires= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      if (C.num_iWires(i) != input[i].size())
        {
          throw bad_value();
        }
      tot_num_iwires+= C.num_iWires(i);
    }
  // Add the inputs to the lambda and then open
  vector<aBit> v(tot_num_iwires);
  vector<word> dv(tot_num_iwires);
  unsigned int cnt= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      for (unsigned int j= 0; j < input[i].size(); j++)
        {
          v[cnt].add(lambda[cnt], input[i][j]);
          cnt++;
        }
    }

  P.OP2->Open_Bits(dv, v, P);

  for (unsigned int i= 0; i < tot_num_iwires; i++)
    {
      Gamma[i]= dv[i];
    }

  Evaluate_Core(Gamma, C, P);

  // Now map the outputs over to the outputs of this function
  output.resize(C.num_outputs());
  cnt= C.get_nWires();
  for (unsigned int i= 0; i < C.num_outputs(); i++)
    {
      cnt-= C.num_oWires(i);
    }
  for (unsigned int i= 0; i < C.num_outputs(); i++)
    {
      output[i].resize(C.num_oWires(i));
      for (unsigned int k= 0; k < output[i].size(); k++)
        {
          output[i][k]= lambda[cnt + k];
          if (Gamma[cnt + k] == 1)
            {
              output[i][k].negate();
            }
        }
      cnt+= C.num_oWires(i);
    }
}

/* Figure 11 on page 29 of 2017/214 */
void Garbled_Circuit::Evaluate(vector<vector<int>> &output,
                               const vector<vector<int>> &input,
                               const Circuit &C, Player &P)
{
  if (C.num_inputs() != input.size())
    {
      throw bad_value();
    }

  // These hold the external values
  vector<int> Gamma(C.get_nWires());

  // Line 1
  unsigned int tot_num_iwires= 0, total_iwires_sent= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      if (i_assign[i] == P.whoami())
        {
          total_iwires_sent+= input[i].size();
        }
    }
  vector<string> o(P.nplayers());
  uint8_t *buff= new uint8_t[total_iwires_sent];
  unsigned int pos= 0;
  for (unsigned int i= 0; i < C.num_inputs(); i++)
    {
      if (i_assign[i] == P.whoami())
        {
          for (unsigned int j= 0; j < input[i].size(); j++)
            {
              Gamma[j + tot_num_iwires]= inputs[i][j] ^ input[i][j];
              buff[pos]= (uint8_t) Gamma[j + tot_num_iwires];
              pos++;
            }
        }
      tot_num_iwires+= C.num_iWires(i);
    }
  o[P.whoami()]= string((char *) buff, pos);
  delete[] buff;

  P.Broadcast_Receive(o, true, 2);

  for (unsigned int i= 0; i < P.nplayers(); i++)
    {
      if (i != P.whoami())
        {
          unsigned int cnt= 0, pos= 0;
          for (unsigned int j= 0; j < C.num_inputs(); j++)
            {
              if (i_assign[j] == i)
                {
                  for (unsigned int k= 0; k < C.num_iWires(j); k++)
                    {
                      Gamma[k + cnt]= o[i].c_str()[pos];
                      pos++;
                    }
                }
              cnt+= C.num_iWires(j);
            }
        }
    }

  Evaluate_Core(Gamma, C, P);

  // Line 5
  output.resize(C.num_outputs());
  unsigned int cnt= C.get_nWires();
  for (unsigned int i= 0; i < C.num_outputs(); i++)
    {
      cnt-= C.num_oWires(i);
    }
  for (unsigned int i= 0; i < C.num_outputs(); i++)
    {
      if (o_assign[i] == P.whoami())
        {
          output[i].resize(C.num_oWires(i));
          for (unsigned int k= 0; k < output[i].size(); k++)
            {
              output[i][k]= outputs[i][k] ^ Gamma[cnt + k];
            }
        }
      else
        {
          output[i].resize(0);
        }
      cnt+= C.num_oWires(i);
    }
}

MMO Base_Garbled_Circuit::get_mmo() const
{
    return mmo;
}

vector<aBit> Base_Garbled_Circuit::get_lambda() const
{
    return lambda;
}

vector<vector<gf2n>> Base_Garbled_Circuit::get_k() const
{
    return k;
}

vector<vector<vector<vector<gf2n>>>> Base_Garbled_Circuit::get_gab() const
{
    return gab;
}

vector<gf2n> Base_Garbled_Circuit::get_one_label() const
{
    return one_label;
}

void Base_Garbled_Circuit::set_mmo(const MMO &mmo)
{
    this->mmo = mmo;
}

void Base_Garbled_Circuit::set_lambda(const vector<aBit> &lambda)
{
    this->lambda = lambda;
}

void Base_Garbled_Circuit::set_k(const vector<vector<gf2n>> &k)
{
    this->k = k;
}

void Base_Garbled_Circuit::set_gab(vector<vector<vector<vector<gf2n>>>> gab)
{
    this->gab = gab;
}

void Base_Garbled_Circuit::set_one_label(vector<gf2n> one_label)
{
    this->one_label = one_label;
}
