#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <stdio.h>
#include <readline.h>
#include <history.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <exception.hpp>
#include <mathexpression.hpp>

#define I_SIZE 1000
#define QUIT "quit"
#define FON "fon"   // show formular
#define FOFF "foff" // hide formular
#define HELP "help"
#define GPL "gpl"
#define VARS "vars"    // show variables
#define REMVAR "remvar" // remove variable
#define UNDEF "undef"  // undefine functions
#define FUNCS "funcs"  // show functiondefinitions
#define SHOWHELP "less" // program to show help
#define SHOWHELP2 "more" // program to show help

void header(char *appname);
void show(char *pname, void (*what)(char *));
void printHelp(char *pname);
void gpl(char *nix);
void undefineFunction(mexp::FunctionList *fl);
void removeVariable(mexp::VarList *vl);

#endif
