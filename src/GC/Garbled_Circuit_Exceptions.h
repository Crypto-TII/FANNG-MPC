#ifndef _GC_Exceptions
#define _GC_Exceptions

#include <exception>
#include <sstream>
#include <string>

class Garbled_circuit_file_not_found : public exception
{
  std::string message;

public:
  explicit Garbled_circuit_file_not_found(const std::string &file_name) : message(
                                                                              "Could not find garbled circuit: " + file_name) {}

private:
  const char *what() const throw() override
  {
    return message.c_str();
  }
};

class Garbled_Circuit_Storage_Type_Not_Found : public exception
{
  std::string message;

public:
  explicit Garbled_Circuit_Storage_Type_Not_Found(const std::string &type) : message(
                                                                                 type + " storage type not supported") {}

private:
  const char *what() const throw() override
  {
    return message.c_str();
  }
};

class Not_Enough_Garbled_Circuits : public exception
{
  string message;

  const char *what() const throw() override
  {
    return message.c_str();
  }

public:
  explicit Not_Enough_Garbled_Circuits(unsigned long available, unsigned int requested) : message("Requested " + to_string(requested) + " but " + to_string(available) + "available circuits for your choicebits") {}
};

class No_available_circuits_to_evaluate_exception : public exception
{
  string message;

  const char *what() const throw() override
  {
    return message.c_str();
  }

public:
  explicit No_available_circuits_to_evaluate_exception(int circuit_number) : message("No available circuits with ID " + to_string(circuit_number) + " in memory to evaluate. Use LOADGC first") {}
};

class Error_updating_loaded_circuits_exception : public exception
{
  string message;

  const char *what() const throw() override
  {
    return message.c_str();
  }

public:
  explicit Error_updating_loaded_circuits_exception(const string &error) : message("Error updating loaded circuits from background thread: " + error) {}
};

class Batch_insert_size_too_big : public exception
{
  string message;

  const char *what() const throw() override
  {
    return message.c_str();
  }

public:
  explicit Batch_insert_size_too_big(int batch_size) : message("Batch size " + to_string(batch_size) + " too big. Make sure to use a value below 1000") {}
};

#endif