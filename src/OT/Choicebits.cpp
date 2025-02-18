/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Choicebits.h"


std::shared_ptr<Choicebits> Choicebits::choicebits;

bool file_does_not_exist(const string &file_name)
{
    struct stat buffer
    {
    };
    return stat(file_name.c_str(), &buffer) != 0;
}

bool file_is_not_empty(const string &file_name)
{
    ifstream input(file_name);
    return input.peek() != ifstream::traits_type::eof();
}

string Choicebits::get_config_file_name(unsigned int player_number)
{

    return "Data/choicebits-P" + std::to_string(player_number);
}

void Choicebits::store_in_file(const vector<int> &bits, const string &file_name)
{
    ofstream output_file(file_name);
    ostream_iterator<int> output_iterator(output_file, "\n");
    copy(bits.begin(), bits.end(), output_iterator);
}

std::shared_ptr<Choicebits> Choicebits::read_from_file(const string &file_name)
{
    ifstream input_file(file_name);
    istream_iterator<int> start(input_file);
    istream_iterator<int> end;
    vector<int> rebuilt_choicebits_from_file(start, end);
    return std::make_shared<Choicebits>(rebuilt_choicebits_from_file);
}

std::shared_ptr<Choicebits> Choicebits::read_from_config_file(unsigned int player_number)
{
    std::shared_ptr<Choicebits> choicebits_l = read_from_file(get_config_file_name(player_number));
    if (Share::SD.Etype != HSS)
    {
        choicebits_l->erase();
    }
    return choicebits_l;
}

std::shared_ptr<Choicebits> Choicebits::create(const string &file_name, const PRNG &pseudo_random_number_generator)
{
    if (file_does_not_exist(file_name))
    {
        throw file_error(file_name);
    }

    if (file_is_not_empty(file_name))
    {
        return read_from_file(file_name);
    }

    std::shared_ptr<Choicebits> choicebits_l = Choicebits::initialize(pseudo_random_number_generator);
    store_in_file(choicebits_l->to_vector(), file_name);
    return choicebits_l;
}

std::shared_ptr<Choicebits>  Choicebits::create_from_config_file(unsigned int player_number, const PRNG &pseudo_random_number_generator)
{
    return create(get_config_file_name(player_number), pseudo_random_number_generator);
}

std::shared_ptr<Choicebits> Choicebits::initialize(PRNG pseudo_random_number_generator)
{
    vector<int> bits(OT_comp_sec);
    for (unsigned int i = 0; i < OT_comp_sec; i++)
    {
        bits[i] = pseudo_random_number_generator.get_uchar() & 1;
    }

    return std::make_shared<Choicebits>(bits);
}

vector<int> Choicebits::to_vector() const
{
    return bits;
}

string Choicebits::to_string() const
{
    std::stringstream result;
    std::copy(bits.begin(), bits.end(), std::ostream_iterator<int>(result, " "));

    return result.str();
}

void Choicebits::erase()
{
    bits.clear();
}

shared_ptr<Choicebits> Choicebits::get_choicebits(const unsigned int player_number)
{
    if (!Choicebits::choicebits)
    {
        Choicebits::choicebits = Choicebits::read_from_config_file(player_number);
    }
    return Choicebits::choicebits;
}