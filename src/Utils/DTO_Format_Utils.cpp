/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/


#include <regex>

#include "DTO_Format_Utils.h"
#include "Input_Output/IO_exceptions.h"
#include "String_Utils.h"

string DTO_Format_Utils::format_database_output_to_share(const string &player, const string &value_share,
                                                         const string &mac_share)
{

    string formatted_values;
    formatted_values.append(player);
    formatted_values.append(" ");
    formatted_values.append(value_share);
    formatted_values.append(" ");
    formatted_values.append(mac_share);
    formatted_values.append("\n");
    return formatted_values;
}

string DTO_Format_Utils::format_database_output_to_share(const Share_DTO &share_dto)
{
    return DTO_Format_Utils::format_database_output_to_share(std::to_string(share_dto.get_player()),
                                                             share_dto.get_share(), share_dto.get_mac_share());
}

void DTO_Format_Utils::validate_rational_number_as_text(const string &gfp_as_text)
{
    regex rational_number_regex("^(-?[0-9]+)(\\.[0-9]+)?$");
    if (!regex_match(gfp_as_text, rational_number_regex))
    {
        throw Invalid_format_exception();
    }
}

void DTO_Format_Utils::assign_gfp_from_text(gfp *y, const string &gfp_as_text)
{
    bool is_decimal = gfp_as_text.find(".") != std::string::npos;
    if (is_decimal)
    {
        double converted_value = stod(gfp_as_text);
        y->assign(converted_value);
    }
    else
    {
        int converted_value = stoi(gfp_as_text);
        y->assign(converted_value);
    }
}

vector<string> DTO_Format_Utils::split_share(const Share &share)
{
    stringstream output;
    share.output(output, true);
    string token;
    char delimiter = ' ';
    vector<string> share_vector;
    int count = 0;
    while (getline(output, token, delimiter))
    {
        String_Utils::trim(token);
        share_vector.push_back(token);
        count++;
        if (count == 3)
        {
            break;
        }
    }
    return share_vector;
}

string DTO_Format_Utils::reduced_format_share_output(const Share &share)
{

    stringstream output;
    share.output(output, true);
    string token;
    output >> token;
    stringstream simplified_output;
    bool is_first = true;
    while (output >> token)
    {
        if (is_first)
        {
            simplified_output << token;
            is_first = false;
        }
        else
        {
            simplified_output << " " << token;
        }
    }
    simplified_output << ";";
    string response = simplified_output.str();

    return response;
}

string DTO_Format_Utils::word_to_string(const word &w)
{
    return std::to_string(w);
}

vector<string> DTO_Format_Utils::word_vector_to_string_vector(const vector<word> &words)
{
    std::vector<std::string> strings;
    strings.reserve(words.size());
    for (const auto &w : words)
    {
        strings.push_back(DTO_Format_Utils::word_to_string(w));
    }
    return strings;
}

word DTO_Format_Utils::string_to_word(const string &s)
{
    return std::stoul(s);
}

vector<word> DTO_Format_Utils::string_vector_to_word_vector(const vector<string> &strings)
{
    std::vector<word> words;
    words.reserve(strings.size());
    for (const auto &s : strings)
    {
        words.push_back(DTO_Format_Utils::string_to_word(s));
    }
    return words;
}

Share DTO_Format_Utils::inputs_to_share(const string &player, const string &value_share, const string &mac_share)
{

    string formatted_data = DTO_Format_Utils::format_database_output_to_share(player, value_share, mac_share);
    std::stringstream data_as_stream;
    data_as_stream.str(formatted_data);

    Share bp;
    bp.input(data_as_stream, true);

    return bp;
}