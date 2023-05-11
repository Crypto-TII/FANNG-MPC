#ifndef GC_STORAGE_H
#define GC_STORAGE_H

#include <fstream>
#include <string>
#include <vector>
#include "Garbled.h"
#include "Garbled_Circuit_Exceptions.h"
#include "OT/Choicebits.h"
#include "DatabaseControllers/MySQL_Controller.h"
#include <cppconn/prepared_statement.h>
#include <boost/algorithm/string/join.hpp>
#include "Tools/Storage_Config.h"
#include "Garbled_Circuit_Serializer.h"
#include <thread>

class Garbled_Circuit_Storage {
protected:
    static string serialize(const Base_Garbled_Circuit &garbled_circuit) {

        return Garbled_Circuit_Serializer::serialize_to_base64(garbled_circuit);
    }

    static Base_Garbled_Circuit deserialize(const string &serialized_garbled_circuit) {
        return Garbled_Circuit_Serializer::deserialize_from_base64(serialized_garbled_circuit);
    }

public:
    Garbled_Circuit_Storage() = default;

    /* Prevent use of these */
    Garbled_Circuit_Storage(const Garbled_Circuit_Storage &) = delete;

    Garbled_Circuit_Storage &operator=(const Garbled_Circuit_Storage &) = delete;

    static shared_ptr<Garbled_Circuit_Storage> create_from_config_file(unsigned int player_number);

    virtual void save(const vector<Base_Garbled_Circuit> &garbled_circuits,
                      const Choicebits &choicebits,
                      unsigned int circuit_number,
                      unsigned int player_number) = 0;

    virtual vector<Base_Garbled_Circuit> read(unsigned int number_of_circuits,
                                              const Choicebits &choicebits,
                                              unsigned int circuit_number,
                                              unsigned int player_number) = 0;

    virtual ~Garbled_Circuit_Storage() = default;
};

class MySQL_Garbled_Circuit_Storage : public Garbled_Circuit_Storage {
    shared_ptr<MySQL_Controller> controller;
    int insert_batch_size;

    static void mark_circuits_as_used(const vector<string>& circuit_ids, shared_ptr<sql::Connection> connection);

    bool is_batch_incomplete(int current_batch_size) const;

    bool are_circuits_to_insert(const vector<Base_Garbled_Circuit>::const_iterator &current_garbled_circuit_iterator,
                                const vector<Base_Garbled_Circuit>::const_iterator &garbled_circuit_last_element) const;
public:

    explicit MySQL_Garbled_Circuit_Storage(shared_ptr<MySQL_Controller> controller, int insert_batch_size) : controller(
            controller), insert_batch_size(insert_batch_size) {}

    void save(const vector<Base_Garbled_Circuit> &garbled_circuits,
              const Choicebits &choicebits,
              unsigned int circuit_number,
              unsigned int player_number) override;

    vector<Base_Garbled_Circuit> read(unsigned int number_of_circuits,
                                      const Choicebits &choicebits,
                                      unsigned int circuit_number,
                                      unsigned int player_number) override;

    string format_insert_row(const Choicebits &choicebits, unsigned int circuit_number, unsigned int player_number,
                             const Base_Garbled_Circuit &garbled_circuit) const;

};

class File_System_Garbled_Circuit_Storage : public Garbled_Circuit_Storage {
    const string FILES_BASE_NAME = "Garbled-Circuits-Storage-";
    const string directory_name;

    void write_in_file(unsigned int circuit_id,
                       unsigned int player_number,
                       unsigned int circuit_number,
                       const string &serialized_choicebits,
                       const string &serialized_garbled_circuit);

    virtual string build_file_path(unsigned int player_number, unsigned int circuit_number);

    unsigned int get_next_circuit_id(unsigned int player_number, unsigned int circuit_number);

    void overwrite_file_with_lines(vector<string> lines, const string &path) const;

public:
    explicit File_System_Garbled_Circuit_Storage(const string &directory_name) : directory_name(directory_name) {};

    void save(const vector<Base_Garbled_Circuit> &garbled_circuits,
              const Choicebits &choicebits,
              unsigned int circuit_number,
              unsigned int player_number) override;

    vector<Base_Garbled_Circuit> read(unsigned int number_of_circuits,
                                      const Choicebits &choicebits,
                                      unsigned int circuit_number,
                                      unsigned int player_number) override;
};

#endif //GC_STORAGE_H
