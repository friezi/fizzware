/*
  Copyright (C) 1999-2008 Friedemann Zintel

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
  friezi@cs.tu-berlin.de
*/

#include <test.hpp>

using namespace test;
using namespace std;

string TestCase::testname = "";

void TestCase::run(){

  setUp();
  startTests();

}
    
void TestCase::startTests(){
  
  for (list<void (*)()>::iterator it = tests.begin(); it != tests.end(); it++ ){

    try{
      (*it)();
    } catch (ExceptionBase &e){
      cout << getTestcasename() + ":" + getTestname() + " failed!: " + e.getMsg() << endl;
    }    
  }
}

TestUnit::~TestUnit(){

  for ( list<TestCase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    delete *it;

}

void TestUnit::run(){

  for ( list<TestCase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it)->run();

}
