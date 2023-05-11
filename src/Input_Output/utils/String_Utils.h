//
// Created by Jose Angel & Mireia on 25/11/2021.
//

#ifndef _String_Utils
#define _String_Utils

#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;

class String_Utils
{
  static const string whiteSpaces;
  static void trimRight(string &str);
  static void trimLeft(string &str);

public:
  String_Utils();
  static void trim(string &str);
};

#endif