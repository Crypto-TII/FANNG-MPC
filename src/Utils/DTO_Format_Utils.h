/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/


//
// Created by Abdelrahaman Aly on 15/02/2023.
//

#ifndef _DTO_Format_Utils
#define _DTO_Format_Utils

#include <string.h>
#include <vector>

#include "Input_Output/Share_DTO.h"
#include "LSSS/Share.h"
#include "Math/gfp.h"

class DTO_Format_Utils
{

  public:
    static string format_database_output_to_share(const string &player, const string &value_share,
                                                  const string &mac_share);
    static string format_database_output_to_share(const Share_DTO &share_dto);
    static void validate_rational_number_as_text(const string &gfp_as_text);
    static void assign_gfp_from_text(gfp *y, const string &gfp_as_text);
    static vector<string> split_share(const Share &share);
    static string reduced_format_share_output(const Share &share);

    static string word_to_string(const word &w);
    static vector<string> word_vector_to_string_vector(const vector<word> &words);

    static word string_to_word(const string &s);
    static vector<word> string_vector_to_word_vector(const vector<string> &strings);

    static Share inputs_to_share(const string &player, const string &value_share, const string &mac_share);
};

#endif
