/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef MPCLIB2P_CHOICEBITS_H
#define MPCLIB2P_CHOICEBITS_H

#include "Tools/Crypto.h"
#include "Tools/Timer.h"
#include "Tools/parse.h"
#include "config.h"
#include <fstream>
#include <iterator>
#include <memory>
#include <sys/stat.h>
#include <vector>

#include "LSSS/Share.h"

bool file_does_not_exist(const string &file_name);

bool file_is_not_empty(const string &file_name);

class Choicebits
{
  private:
    vector<int> bits;

    static std::shared_ptr<Choicebits> initialize(PRNG pseudo_random_number_generator);

    static void store_in_file(const vector<int> &bits, const string &file_name);
    static std::shared_ptr<Choicebits> choicebits;

  public:
    static shared_ptr<Choicebits> get_choicebits(const unsigned int player_number);

    static string get_config_file_name(unsigned int player_number);

    static std::shared_ptr<Choicebits> create(const string &file_name, const PRNG &pseudo_random_number_generator);

    static std::shared_ptr<Choicebits> create_from_config_file(unsigned int player_number,
                                                               const PRNG &pseudo_random_number_generator);

    static std::shared_ptr<Choicebits> read_from_file(const string &file_name);

    static std::shared_ptr<Choicebits> read_from_config_file(unsigned int player_number);

    explicit Choicebits(const vector<int> &choice_bits) : bits(choice_bits)
    {
    }

    vector<int> to_vector() const;

    string to_string() const;

    void erase();
};

#endif // MPCLIB2P_CHOICEBITS_H
