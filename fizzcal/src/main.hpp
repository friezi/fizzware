/*
    this sourcefile belongs to the programm focal,
    a mathematical formula-calculator
    Copyright (C) 1999-2006 Friedemann Zintel

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
#include <exception.hpp>
#include <mathexpression.hpp>
#include <cmdlparser.hpp>
#include "linescanner.hpp"
#include "filescanner.hpp"

#define I_SIZE 1000
#define QUIT "quit"
#define FON "fon"   // show formula
#define FOFF "foff" // hide formula
#define HELP "help"
#define GPL "gpl"
#define VARS "vars"    // show variables
#define REMVAR "undefvar" // remove variable
#define UNDEF "undeffun"  // undefine functions
#define FUNCS "funs"  // show functiondefinitions
#define SAVE "save" // save the variables and commands to file
#define LOAD "load" // load variables and commands from file
#define SHOWHELP "less" // program to show help
#define SHOWHELP2 "more" // program to show help

std::string setupParser(cmdl::CmdlParser& parser);
void header(const char *appname);
void show(const char *pname, void (*what)(const char *));
void printHelp(const char *pname);
void gpl(const char *nix);
void undefineFunctions(mexp::FunctionList *fl, LineScanner & lscanner);
void removeVariables(mexp::VarList *vl, LineScanner & lscanner);
void save(mexp::VarList *vl, mexp::FunctionList *fl, LineScanner & lscanner);
void load(mexp::VarList *vl, mexp::FunctionList *fl, LineScanner & lscanner);

#endif
