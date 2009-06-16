/*
  this sourcefile belongs to the programm fizzcal,
  a calculator/evaluator for arithmetic expresions.
  Copyright (C) 1999-2008 Friedemann Zintel

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  For any questions, contact me at
  friezi@cs.tu-berlin.de
*/

#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <libgen.h>
#include <readline.h>
#include <history.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <fztooltempl/exception.hpp>
#include <fztooltempl/mathexpression.hpp>
#include <fztooltempl/cmdlparser.hpp>
#include <fztooltempl/datastructures.hpp>
#include "linescanner.hpp"
#include "filescanner.hpp"

#define I_SIZE 1000
#define QUIT "exit"
#define FON "fon"   // show formula
#define FOFF "foff" // hide formula
#define HELP "help"
#define INFO "info"
#define GPL "gpl"
#define VARS "vars"    // show variables
#define REMVAR "undefvar" // remove variable
#define UNDEF "undeffun"  // undefine functions
#define FUNCS "funs"  // show functiondefinitions
#define BUILTINS "builtins" // show builtin-functions
#define SAVE "save" // save the variables and commands to file
#define LOAD "load" // load variables and commands from file
#define SETPRECISION "setprecision" // set precision for post decimal position for float-values
#define SHOWPRECISION "showprecision" // show pd-precision for float-values
#define SHOWHELP "less" // program to show help
#define SHOWHELP2 "more" // program to show help

std::string setupParser(cmdl::CmdlParser& parser);
void info(const char *appname);
void show(const char *pname, void (*what)(const char *));
void printHelp(const char *pname);
void gpl(const char *nix);
void undefineFunctions(mexp::FunctionList *fl, LineScanner & lscanner);
void removeVariables(mexp::VariableList *vl, LineScanner & lscanner);
void save(mexp::VariableList *vl, mexp::FunctionList *fl, std::streamsize precision, std::string filename, LineScanner & lscanner);
void load(mexp::VariableList *vl, mexp::FunctionList *fl, std::string filename, bool interactive);
bool checkAnswer(const std::string & text);
void printErrorArrow(int pos);

#endif
