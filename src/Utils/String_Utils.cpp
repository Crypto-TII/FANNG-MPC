/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "String_Utils.h"

String_Utils::String_Utils() {}
const string String_Utils::whiteSpaces= " \f\n\r\t\v";
void String_Utils::trimRight(string &str)
{
  string::size_type pos= str.find_last_not_of(String_Utils::whiteSpaces);
  str.erase(pos + 1);
}

void String_Utils::trimLeft(string &str)
{
  string::size_type pos= str.find_first_not_of(String_Utils::whiteSpaces);
  str.erase(0, pos);
}

void String_Utils::trim(string &str)
{
  trimRight(str);
  trimLeft(str);
}
