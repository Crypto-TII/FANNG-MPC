/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Abdelrahaman ALY on 09/02/2023.
//
#include "Triple_Type_DTO.h"
#include <iostream>

unsigned int Triple_Type_DTO::get_id() const
{
  return this->id_;
}

void Triple_Type_DTO::set_id(unsigned int id)
{
  this->id_= id;
}

int Triple_Type_DTO::get_rows_A() const
{
  return this->rows_A_;
}

void Triple_Type_DTO::set_rows_A(int rows_A)
{
  this->rows_A_= rows_A;
}

int Triple_Type_DTO::get_columns_A() const
{
  return this->columns_A_;
}

void Triple_Type_DTO::set_columns_A(int columns_A)
{
  this->columns_A_= columns_A;
}

int Triple_Type_DTO::get_rows_B() const
{
  return this->rows_B_;
}

void Triple_Type_DTO::set_rows_B(int rows_B)
{
  this->rows_B_= rows_B;
}

int Triple_Type_DTO::get_columns_B() const
{
  return this->columns_B_;
}

void Triple_Type_DTO::set_columns_B(int columns_B)
{
  this->columns_B_= columns_B;
}

int Triple_Type_DTO::get_rows_C() const
{
  return this->rows_C_;
}
void Triple_Type_DTO::set_rows_C(int rows_C)
{
  this->rows_C_= rows_C;
}

int Triple_Type_DTO::get_columns_C() const
{
  return this->columns_C_;
}

void Triple_Type_DTO::set_columns_C(int columns_C)
{
  this->columns_C_= columns_C;
}

int Triple_Type_DTO::get_shares_per_matrix_triple() const
{
  return this->rows_A_ * this->columns_A_ + this->rows_B_ * this->columns_B_ + this->rows_C_ * this->columns_C_;
}
std::vector<int> Triple_Type_DTO::get_NN_triple_dimensions(int w, int h, int s, int kh, int kw, int s_, int stride, int padding)
{
  std::vector<int> response;
  int w_in= w + (padding * 2);
  int h_in= h + (padding * 2);

  int w_out= (w_in - kw + 1);
  int h_out= (h_in - kh + 1);

  if (stride == 2)
    {

      int w_out_p= (w_out / stride);
      int h_out_p= (h_out / stride);

      if ((w_in % stride) > 0)
        {
          w_out_p+= 1;
        }

      if ((h_in % stride) > 0)
        {
          h_out_p+= 1;
        }
      w_out= w_out_p;
      h_out= h_out_p;
    }
  // A
  response.push_back(w_in * h_in);
  response.push_back(s);
  // B
  response.push_back(kh * kw);
  response.push_back(s * s_);
  // C
  response.push_back(w_out * h_out);
  response.push_back(s_);
  return response;
}

NN_Triple_Type_DTO::NN_Triple_Type_DTO(unsigned int id, int w, int h, int s, int kh, int kw, int s_, int stride, int padding)
{
  this->set_id(id);
  std::vector<int> response= Triple_Type_DTO::get_NN_triple_dimensions(w, h, s, kh, kw, s_, stride, padding);

  this->set_rows_A(response.front());
  response.erase(response.begin());

  this->set_columns_A(response.front());
  response.erase(response.begin());

  this->set_rows_B(response.front());
  response.erase(response.begin());

  this->set_columns_B(response.front());
  response.erase(response.begin());

  this->set_rows_C(response.front());
  response.erase(response.begin());

  this->set_columns_C(response.front());
  response.erase(response.begin());
  // this param might be used in some future release to calculate the size of C.
}
