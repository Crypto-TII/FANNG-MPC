/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef C_STORAGE_H
#define C_STORAGE_H

#include "DatabaseControllers/MySQL_Controller.h"
#include <cppconn/prepared_statement.h>

#include "DatabaseControllers/MySQL_Controller.h"
#include "Input_Output/Share_DTO.h"
#include "LSSS/Share.h"
#include "Tools/Storage_Config.h"
#include "Triple_Type_DTO.h"
#include <string>
#include <vector>

class Converter_Storage
{

public:
  Converter_Storage()= default;

  /* Prevent use of these */
  Converter_Storage(const Converter_Storage &)= delete;

  Converter_Storage &operator=(const Converter_Storage &)= delete;

  static shared_ptr<Converter_Storage> create_from_config_file(unsigned int player_number);

  virtual vector<Share_DTO> read(unsigned int number_of_elements,
                                 const Triple_Type_DTO &triple_type_dto, unsigned int element_id, unsigned int player_number)= 0;

  virtual void save(const Share &share,
                    const unsigned int &type_id,
                    const unsigned int &player_number)= 0;

  virtual void save_batch(const vector<Share> &shares,
                          const unsigned int &type_id,
                          const unsigned int &player_number)= 0;

  virtual vector<Share_DTO> read(const unsigned int &number_of_elements,
                                 const unsigned int &type_id,
                                 const unsigned int element_id, const unsigned int player_number)= 0;

  virtual vector<Share_DTO> read_then_delete(const unsigned int &number_of_elements,
                                             const unsigned int &type_id,
                                             const unsigned int element_id, const unsigned int player_number)= 0;

  virtual vector<Share_DTO> read_then_delete(unsigned int number_of_elements,
                                             const Triple_Type_DTO &triple_type_dto, unsigned int element_id, unsigned int player_number)= 0;

  virtual ~Converter_Storage()= default;
};

class MySQL_Converter_Storage : public Converter_Storage
{
  shared_ptr<MySQL_Controller> controller;
  int insert_batch_size;

public:
  explicit MySQL_Converter_Storage(shared_ptr<MySQL_Controller> controller, int insert_batch_size) : controller(
                                                                                                         controller),
                                                                                                     insert_batch_size(insert_batch_size) {}
  vector<Share_DTO> read(unsigned int number_of_triples,
                         const Triple_Type_DTO &triple_type_dto, unsigned int element_id,
                         unsigned int player_number) override;

  void save(const Share &share,
            const unsigned int &type_id,
            [[maybe_unused]] const unsigned int &player_number) override;

  void save_batch(const vector<Share> &shares,
                  const unsigned int &type_id,
                  const unsigned int &player_number) override;

  vector<Share_DTO> read(const unsigned int &number_of_elements,
                         const unsigned int &type_id,
                         const unsigned int element_id, const unsigned int player_number) override;

  vector<Share_DTO> read_then_delete(const unsigned int &number_of_elements,
                                     const unsigned int &type_id,
                                     const unsigned int element_id, const unsigned int player_number) override;

  vector<Share_DTO> read_then_delete(unsigned int number_of_triples,
                                     const Triple_Type_DTO &triple_type_dto, unsigned int element_id,
                                     unsigned int player_number) override;
};

#endif // C_STORAGE_H
