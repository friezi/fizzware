#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <set>
#include <stdlib.h>
#include <exception.hpp>
#include <cmdlparser.hpp>

class Error{};

std::string setupParser(cmdl::CmdlParser &parser);
void game(int iterations, bool rand) throw (ExceptionBase);
void initDoors(std::set<long> &doors);
long getDoor(std::set<long> &doors, long nmb) throw (ExceptionBase);
long percent(long value, long total);

#endif
