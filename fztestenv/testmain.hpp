/**
   @mainpage
   This page documents the static-library 'Friedemann-Zintel\`s Testenvironment'. It provides the programmer with a standard testenvironment
   based on the test-classes in fztooltempl.
   If linked to a module it provides a main-method wich will execute all user-defined testcases in the module.
   To work correctly the module must provide the non-class-method

   test::TestUnit * createMainTestUnit()

   which creates a TestUnit, fills it with all TestCases and returns it.

   @author Friedemann Zintel
   
   Copyright (C) 1999-2013 Friedemann Zintel

   Compilation is successfully tested with g++ v.2.95, v.3.3 and v.4.0 .
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   For any questions, contact me at
   FriedemannZintel@gmx.de
*/

/**
   @file testmain.hpp
   @author Friedemann Zintel
*/

#ifndef FZTESTENV_TESTMAIN_HPP
#define FZTESTENV_TESTMAIN_HPP

#include <vector>
#include <utility>
#include <fztooltempl/test.hpp>
#include <fztooltempl/exception.hpp>

typedef std::vector< std::pair<unsigned long, test::TestCaseBase *> > FailedTests;

/**
   @brief creates a TestUnit and sets up all testcases whithin the TestUnit
   @note This method must be provided by the module which setsup the testcases.
   @return the main TestUnit to be executed
 **/
extern test::TestUnit * createMainTestUnit();

#endif
