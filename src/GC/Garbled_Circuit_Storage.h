/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef GC_STORAGE_H
#define GC_STORAGE_H

#include "DatabaseControllers/MySQL_Controller.h"
#include "Garbled.h"
#include "Garbled_Circuit_Exceptions.h"
#include "Garbled_Circuit_Serializer.h"
#include "Mod2Engine/Dabit.h"
#include "OT/Choicebits.h"
#include "Tools/Storage_Config.h"
#include <boost/algorithm/string/join.hpp>
#include <cppconn/prepared_statement.h>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

class Garbled_Circuit_Storage
{
  protected:
    static string serialize_garbled_circuit(const Base_Garbled_Circuit &garbled_circuit)
    {

        return Garbled_Circuit_Serializer<aBit>::serialize_garbled_circuit_to_base64(garbled_circuit);
    }

    static Base_Garbled_Circuit deserialize_garbled_circuit(const string &serialized_garbled_circuit)
    {
        return Garbled_Circuit_Serializer<aBit>::deserialize_garbled_circuit_from_base64(serialized_garbled_circuit);
    }

    template <class SBit> static string serialize_dabit(const Dabit<SBit> &dabit)
    {
        return Garbled_Circuit_Serializer<SBit>::serialize_dabit_to_base64(dabit);
    }

    template <class SBit> static Dabit<SBit> deserialize_dabit(const string &serialized_dabit)
    {
        return Garbled_Circuit_Serializer<SBit>::deserialize_dabit_from_base64(serialized_dabit);
    }

  public:
    Garbled_Circuit_Storage() = default;

    /* Prevent use of these */
    Garbled_Circuit_Storage(const Garbled_Circuit_Storage &) = delete;

    Garbled_Circuit_Storage &operator=(const Garbled_Circuit_Storage &) = delete;

    static shared_ptr<Garbled_Circuit_Storage> create_from_config_file(unsigned int player_number);

    virtual void save(const vector<Base_Garbled_Circuit> &garbled_circuits, const Choicebits &choicebits,
                      unsigned int circuit_number, unsigned int player_number) = 0;

    virtual vector<Base_Garbled_Circuit> read(unsigned int number_of_circuits, const Choicebits &choicebits,
                                              unsigned int circuit_number, unsigned int player_number) = 0;

    virtual ~Garbled_Circuit_Storage() = default;
};

class MySQL_Garbled_Circuit_Storage : public Garbled_Circuit_Storage
{
    shared_ptr<MySQL_Controller> controller;
    int insert_batch_size;

    static void mark_materials_as_used(const vector<string> &material_ids, const string &table,
                                       shared_ptr<sql::Connection> connection);

  public:
    const std::string GARBLED_CIRCUIT_TABLE = "garbled_circuit";
    const std::string AUTHENTICATED_MATERIAL_TABLE = "authenticated_material";

    explicit MySQL_Garbled_Circuit_Storage(shared_ptr<MySQL_Controller> controller, int insert_batch_size)
        : controller(controller), insert_batch_size(insert_batch_size)
    {
    }

    void save(const vector<Base_Garbled_Circuit> &garbled_circuits, const Choicebits &choicebits,
              unsigned int circuit_number, unsigned int player_number) override;

    template <class SBit>
    void save_dabits(const vector<Dabit<SBit>> &dabits, const Choicebits &choicebits, unsigned int material_type,
                     unsigned int player_number);

    template <class SBit>
    vector<Dabit<SBit>> read_dabits(unsigned int number_of_dabits, const Choicebits &choicebits,
                                    unsigned int material_type, unsigned int player_number);

    vector<Base_Garbled_Circuit> read(unsigned int number_of_circuits, const Choicebits &choicebits,
                                      unsigned int circuit_number, unsigned int player_number) override;

    string format_circuit_insert_row(const Choicebits &choicebits, unsigned int circuit_number,
                                     unsigned int player_number, const Base_Garbled_Circuit &garbled_circuit) const;
    template <class SBit>
    string format_dabit_insert_row(const Choicebits &choicebits, unsigned int material_type, unsigned int player_number,
                                   const Dabit<SBit> &dabit) const;
};

class File_System_Garbled_Circuit_Storage : public Garbled_Circuit_Storage
{
    const string FILES_BASE_NAME = "Garbled-Circuits-Storage-";
    const string directory_name;

    void write_in_file(unsigned int circuit_id, unsigned int player_number, unsigned int circuit_number,
                       const string &serialized_choicebits, const string &serialized_garbled_circuit);

    virtual string build_file_path(unsigned int player_number, unsigned int circuit_number);

    unsigned int get_next_circuit_id(unsigned int player_number, unsigned int circuit_number);

    void overwrite_file_with_lines(vector<string> lines, const string &path) const;

  public:
    explicit File_System_Garbled_Circuit_Storage(const string &directory_name) : directory_name(directory_name){};

    void save(const vector<Base_Garbled_Circuit> &garbled_circuits, const Choicebits &choicebits,
              unsigned int circuit_number, unsigned int player_number) override;

    vector<Base_Garbled_Circuit> read(unsigned int number_of_circuits, const Choicebits &choicebits,
                                      unsigned int circuit_number, unsigned int player_number) override;
};

#endif // GC_STORAGE_H
