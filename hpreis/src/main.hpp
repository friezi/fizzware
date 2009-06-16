#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <fztooltempl/exception.hpp>
#include <fztooltempl/cmdlparser.hpp>

class Error{};

std::string setupParser(cmdl::CmdlParser &parser);
void game(int iterations, bool rand) throw (exc::ExceptionBase);
void initDoors(std::set<long> &doors);
long getDoor(std::set<long> &doors, long nmb) throw (exc::ExceptionBase);
long percent(long value, long total);

#endif
