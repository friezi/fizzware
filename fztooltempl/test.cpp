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

TestCaseBase::TestCaseBase(){
  pushErrorHandler(getDefaultErrorHandler());
  setTestname("");
}

void TestCaseBase::defaultErrorHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  throw Exception<TestCaseBase>(msg);
}

TestCaseBase::ErrorHandlerPtr TestCaseBase::getDefaultErrorHandler(){
  return defaultErrorHandler;
}

void TestCaseBase::error(TestCaseBase *testcase, char msg[]) throw (Exception<TestCaseBase>){
  error(testcase,string(msg));
}

void TestCaseBase::error(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){

  for ( ErrorHandlerStack::iterator it = errorHandlers.begin(); it != errorHandlers.end(); it++ )
    (*it)(testcase,msg);

}

void TestCaseBase::assertTrue(bool value) throw (Exception<TestCaseBase>){
  if ( value != true )
    throw Exception<TestCaseBase>("value not true");
}

void TestCaseBase::assertTrue(bool value, string id) throw (Exception<TestCaseBase>){
  if ( value != true )
    throw Exception<TestCaseBase>(id + " not true");
}

void TestCaseBase::assertFalse(bool value) throw (Exception<TestCaseBase>){
  if ( value != false )
    throw Exception<TestCaseBase>("value not false");
}

void TestCaseBase::assertFalse(bool value, string id) throw (Exception<TestCaseBase>){
  if ( value != false )
    throw Exception<TestCaseBase>(id + " not false");
}

TestUnit::~TestUnit(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    delete *it;

}

void TestUnit::operator()() throw (ExceptionBase){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*(*it))();

}

unsigned long TestUnit::getNmbTests(){

  unsigned long nmb = 0;

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    nmb+=(*it)->getNmbTests();

  return nmb;

}

void TestUnit::pushErrorHandler(TestCaseBase::ErrorHandlerPtr errorHandler){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it)->pushErrorHandler(errorHandler);

}

void TestUnit::clearErrorHandlers(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it)->clearErrorHandlers();

}
