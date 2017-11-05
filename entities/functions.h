#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <ostream>

#include <string>
#include "../colors.h"

int realLength(std::string s);

void printCenter(std::string msg, int count, const std::string& color = "");
void printCenter(std::ostream &stream, std::string msg, int count, const std::string& color = "");

void printCenterCell(std::string msg, int count, const char* color = "");

#endif