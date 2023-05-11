
//
// Created by Abdelrahaman Aly on 15/02/2023.
//

#ifndef _DTO_Format_Utils
#define _DTO_Format_Utils

#include <string.h>

#include "Input_Output/Share_DTO.h"
#include "Math/gfp.h"
class DTO_Format_Utils
{

public:
  static string format_database_output_to_share(const Share_DTO &share_dto);
  static void validate_rational_number_as_text(const string &gfp_as_text);
  static void assign_gfp_from_text(gfp *y, const string &gfp_as_text);
};

#endif
