#include "Choicebits.h"

bool file_does_not_exist(const string &file_name){
    struct stat buffer{};
    return stat(file_name.c_str(), &buffer) != 0;
}

bool file_is_not_empty(const string& file_name){
    ifstream input(file_name);
    return input.peek() != ifstream::traits_type::eof();
}

string Choicebits::get_config_file_name(unsigned int player_number){
    return "Data/choicebits-P" + std::to_string(player_number);
}

void Choicebits::store_in_file(const vector<int> &bits, const string &file_name) {
    ofstream output_file(file_name);
    ostream_iterator<int> output_iterator(output_file, "\n");
    copy(bits.begin(), bits.end(), output_iterator);
}

Choicebits Choicebits::read_from_file(const string &file_name) {
    ifstream input_file(file_name);
    istream_iterator<int> start(input_file);
    istream_iterator<int> end;
    vector<int> rebuilt_choicebits_from_file(start, end);
    return Choicebits(rebuilt_choicebits_from_file);
}

Choicebits Choicebits::read_from_config_file(unsigned int player_number) {
    return read_from_file(get_config_file_name(player_number));
}

Choicebits Choicebits::create(const string &file_name, const PRNG &pseudo_random_number_generator){
    if(file_does_not_exist(file_name)){
        throw file_error(file_name);
    }

    if(file_is_not_empty(file_name)){
        return read_from_file(file_name);
    }

    Choicebits choicebits = Choicebits::initialize(pseudo_random_number_generator);
    store_in_file(choicebits.to_vector(), file_name);
    return choicebits;
}

Choicebits Choicebits::create_from_config_file(unsigned int player_number, const PRNG &pseudo_random_number_generator) {
    return create(get_config_file_name(player_number),pseudo_random_number_generator);
}

Choicebits Choicebits::initialize(PRNG pseudo_random_number_generator) {
    vector<int> bits(OT_comp_sec);
    for (unsigned int i= 0; i < OT_comp_sec; i++)
    {
        bits[i]= pseudo_random_number_generator.get_uchar() & 1;
    }

    return Choicebits(bits);
}

vector<int> Choicebits::to_vector() const {
    return bits;
}

string Choicebits::to_string() const
{
  std::stringstream result;
  std::copy(bits.begin(), bits.end(), std::ostream_iterator<int>(result, " "));

  return result.str();
}
