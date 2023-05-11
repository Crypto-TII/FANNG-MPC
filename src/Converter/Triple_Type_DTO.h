//
// Created by Abdelrahaman ALY on 09/02/2023.
//

#ifndef _TripleTypeDTO
#define _TripleTypeDTO

#include <stdlib.h>
#include <vector>
class Triple_Type_DTO
{
private:
  unsigned int id_;
  int rows_A_;
  int columns_A_;
  int rows_B_;
  int columns_B_;
  int rows_C_;
  int columns_C_;

public:
  Triple_Type_DTO()= default;
  Triple_Type_DTO(unsigned int id, int rows_A, int columns_A, int rows_B, int columns_B, int rows_C, int columns_C) : id_(id), rows_A_(rows_A), columns_A_(columns_A), rows_B_(rows_B),
                                                                                                                      columns_B_(columns_B), rows_C_(rows_C), columns_C_(columns_C){};
  
  static std::vector<int>  get_NN_triple_dimensions(int w, int h, int s, int kh, int kw, int s_, int stride, int padding);

  unsigned int get_id() const;
  void set_id(unsigned int id);

  int get_rows_A() const;
  void set_rows_A(int rows_A);

  int get_columns_A() const;
  void set_columns_A(int columns_A);

  int get_rows_B() const;
  void set_rows_B(int rows_B);

  int get_columns_B() const;
  void set_columns_B(int columns_B);

  int get_rows_C() const;
  void set_rows_C(int rows_C);

  int get_columns_C() const;
  void set_columns_C(int columns_C);
  
  int get_shares_per_matrix_triple() const;
};

class NN_Triple_Type_DTO : public Triple_Type_DTO
{
public:
  NN_Triple_Type_DTO()= default;
  NN_Triple_Type_DTO(unsigned int id, int w, int h, int s, int kh, int kw, int s_, int stride, int padding);
};
#endif