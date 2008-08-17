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
using namespace exc;

TestCaseBase::TestCaseBase(){
  show_tests = false;
  pushErrorHandler(getDefaultErrorHandler());
  setCurrentTestName("");
}

void TestCaseBase::defaultErrorHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  throw Exception<TestCaseBase>(msg);
}

TestCaseBase::HandlerType TestCaseBase::getDefaultErrorHandler(){
  return defaultErrorHandler;
}

void TestCaseBase::error(char msg[]) throw (Exception<TestCaseBase>){
  error(string(msg));
}

void TestCaseBase::error(string msg) throw (Exception<TestCaseBase>){

  for ( HandlerStack::iterator it = errorHandlers.begin(); it != errorHandlers.end(); it++ )
    (*it)(this,msg);

}

void TestCaseBase::success(){

  for ( HandlerStack::iterator it = successHandlers.begin(); it != successHandlers.end(); it++ )
    (*it)(this,"");

}

void TestCaseBase::callStatisticHelpers(){

  for (HandlerStack::iterator it = statisticHelpers.begin(); it != statisticHelpers.end(); it++ )
    (*it)(this,"");

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

TestUnit::TestUnit(){
  setTestCaseStartupHandler(defaultTestCaseStartupHandler);
}

TestUnit::~TestUnit(){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    delete (*it).first;

}

void TestUnit::defaultTestCaseStartupHandler(TestCaseBase::TestCaseBase *testcase, string msg){}

void TestUnit::addTestCase(TestCaseBase * testcase, string name){

  testcase->setTestCaseName(name);
  testcases.push_back(TestCaseInfo(testcase,name));

}

void TestUnit::operator()() throw (ExceptionBase){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ ){

    (*testcaseStartupHandler)((*it).first,"");
    (*(*it).first)();

  }

}

unsigned long TestUnit::getNmbTests(){

  unsigned long nmb = 0;

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    nmb+=(*it).first->getNmbTests();

  return nmb;

}

void TestUnit::pushErrorHandler(TestCaseBase::HandlerType errorHandler){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->pushErrorHandler(errorHandler);

}

void TestUnit::clearErrorHandlers(){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->clearErrorHandlers();

}

void TestUnit::pushSuccessHandler(TestCaseBase::HandlerType successHandler){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->pushSuccessHandler(successHandler);

}

void TestUnit::clearSuccessHandlers(){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->clearSuccessHandlers();

}

void TestUnit::pushStatisticHelper(TestCaseBase::HandlerType statisticHelper){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->pushStatisticHelper(statisticHelper);

}

void TestUnit::clearStatisticHelpers(){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->clearStatisticHelpers();

}

void TestUnit::showTests(){

  for ( list<TestCaseInfo>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*it).first->showTests();  

}
