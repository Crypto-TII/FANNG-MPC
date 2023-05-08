#ifndef C_STORAGE_H
#define C_STORAGE_H

#include "DatabaseControllers/MySQL_Controller.h"
#include <cppconn/prepared_statement.h>

#include "DatabaseControllers/MySQL_Controller.h"
#include "Input_Output/Share_DTO.h"
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
  

  virtual ~Converter_Storage()= default;
};

class MySQL_Converter_Storage : public Converter_Storage
{
  shared_ptr<MySQL_Controller> controller;

public:
  explicit MySQL_Converter_Storage(shared_ptr<MySQL_Controller> controller) : controller(
                                                                                  controller) {}

  vector<Share_DTO> read(unsigned int number_of_triples,
                         const Triple_Type_DTO &triple_type_dto, unsigned int element_id,
                         unsigned int player_number) override;
};

#endif // C_STORAGE_H
