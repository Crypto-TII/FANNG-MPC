#include "DTO_Format_Utils.h"
#include <regex>
#include "Input_Output/IO_exceptions.h"

string DTO_Format_Utils::format_database_output_to_share(const Share_DTO &share_dto){
    string formatted_values;
    formatted_values.append(std::to_string(share_dto.get_player()));
    formatted_values.append(" ");
    formatted_values.append(share_dto.get_share());
    formatted_values.append(" ");
    formatted_values.append(share_dto.get_mac_share());
    formatted_values.append("\n");

    return formatted_values;
}

void DTO_Format_Utils::validate_rational_number_as_text(const string &gfp_as_text){
    regex rational_number_regex("^(-?[0-9]+)(\\.[0-9]+)?$");
    if (!regex_match (gfp_as_text, rational_number_regex)){
        throw Invalid_format_exception();
    }
}

void DTO_Format_Utils::assign_gfp_from_text(gfp *y, const string &gfp_as_text){
    bool is_decimal = gfp_as_text.find(".") != std::string::npos;
    if (is_decimal){
        double converted_value = stod(gfp_as_text);
        y->assign(converted_value);
    }
    else{
        int converted_value = stoi(gfp_as_text);
        y->assign(converted_value);
    }
}
