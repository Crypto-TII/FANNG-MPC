#ifndef MPCLIB2P_CHOICEBITS_H
#define MPCLIB2P_CHOICEBITS_H

#include <vector>
#include "config.h"
#include "Tools/Crypto.h"
#include "Tools/parse.h"
#include "Tools/Timer.h"
#include <iterator>
#include <fstream>
#include <sys/stat.h>

bool file_does_not_exist(const string &file_name);

bool file_is_not_empty(const string &file_name);

class Choicebits {
private:
    vector<int> bits;

    static Choicebits initialize(PRNG pseudo_random_number_generator);

    static void store_in_file(const vector<int> &bits, const string &file_name);

public:
    static string get_config_file_name(unsigned int player_number);

    static Choicebits create(const string &file_name, const PRNG &pseudo_random_number_generator);

    static Choicebits create_from_config_file(unsigned int player_number, const PRNG &pseudo_random_number_generator);

    static Choicebits read_from_file(const string &file_name);

    static Choicebits read_from_config_file(unsigned int player_number);

    explicit Choicebits(const vector<int> &choice_bits) : bits(choice_bits) {}

    vector<int> to_vector() const;

    string to_string() const;
};


#endif //MPCLIB2P_CHOICEBITS_H
