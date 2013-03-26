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

#include <fztooltempl/test.hpp>

using namespace test;
using namespace std;
using namespace exc;

TestCaseBase::TestCaseBase(){
  setCurrentTestName("");
}

void TestCaseBase::callErrorHandlers(char msg[]) throw (Exception<TestCaseBase>){
  callErrorHandlers(string(msg));
}

void TestCaseBase::callErrorHandlers(string msg) throw (Exception<TestCaseBase>){

  for ( HandlerStack::iterator it = errorHandlers.begin(); it != errorHandlers.end(); it++ ){
    (*it)(this,msg);
  }

}

void TestCaseBase::callTestStartHandlers(){

  for ( HandlerStack::iterator it = testStartHandlers.begin(); it != testStartHandlers.end(); it++ ){
    (*it)(this,"");
  }

}

void TestCaseBase::callSuccessHandlers(){

  for ( HandlerStack::iterator it = successHandlers.begin(); it != successHandlers.end(); it++ ){
    (*it)(this,"");
  }

}

void TestCaseBase::assertTrue(bool value) throw (Exception<TestCaseBase>){
  if ( value != true ){
    throw Exception<TestCaseBase>("value not true");
  }
}

void TestCaseBase::assertTrue(bool value, string id) throw (Exception<TestCaseBase>){
  if ( value != true ){
    throw Exception<TestCaseBase>(id + " not true");
  }
}

void TestCaseBase::assertFalse(bool value) throw (Exception<TestCaseBase>){
  if ( value != false ){
    throw Exception<TestCaseBase>("value not false");
  }
}

void TestCaseBase::assertFalse(bool value, string id) throw (Exception<TestCaseBase>){
  if ( value != false ){
    throw Exception<TestCaseBase>(id + " not false");
  }
}

TestUnit::TestUnit(){}

TestUnit::~TestUnit(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    delete *it;

}

void TestUnit::addTestCase(TestCaseBase * testcase, string name){

  testcase->setTestCaseName(name);
  testcases.push_back(testcase);

}

void TestUnit::operator()(map< string,set<string> *> *selection) throw (ExceptionBase){

  map< string,set<string> *>::iterator eit;

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){

    callTestCaseStartHandlers(*it);

    if ( selection != 0 ){
      // execute only selected testcases
      if ( (eit = selection->find((*it)->getTestCaseName())) == selection->end() ){
	continue;
      } else {
	(*(*it))((*eit).second);
      }
    } else{
      (*(*it))();
    }

    callTestCaseEndHandlers(*it);

  }
}

unsigned long TestUnit::getNmbTests(){

  unsigned long nmb = 0;

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    nmb+=(*it)->getNmbTests();
  }

  return nmb;

}

void TestUnit::callTestCaseStartHandlers(TestCaseBase *testcase){

  for ( HandlerStack::iterator it = testcaseStartHandlers.begin(); it != testcaseStartHandlers.end(); it++ ){
    (*it)(testcase,"");
  }

}

void TestUnit::callTestCaseEndHandlers(TestCaseBase *testcase){

  for ( HandlerStack::iterator it = testcaseEndHandlers.begin(); it != testcaseEndHandlers.end(); it++ ){
    (*it)(testcase,"");
  }

}

void TestUnit::pushTestCaseStartHandler(HandlerType handler){
  testcaseStartHandlers.push_front(handler);
}

void TestUnit::pushTestCaseEndHandler(HandlerType handler){
  testcaseEndHandlers.push_front(handler);
}

void TestUnit::pushErrorHandler(HandlerType handler){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    (*it)->pushErrorHandler(handler);
  }

}

void TestUnit::clearErrorHandlers(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    (*it)->clearErrorHandlers();
  }

}

void TestUnit::pushTestStartHandler(HandlerType handler){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    (*it)->pushTestStartHandler(handler);
  }

}

void TestUnit::clearTestCaseStartHandlers(){
  testcaseStartHandlers.clear();
}

void TestUnit::clearTestCaseEndHandlers(){
  testcaseEndHandlers.clear();
}

void TestUnit::clearTestStartHandlers(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    (*it)->clearTestStartHandlers();
  }

}

void TestUnit::pushSuccessHandler(HandlerType handler){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    (*it)->pushSuccessHandler(handler);
  }

}

void TestUnit::clearSuccessHandlers(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ ){
    (*it)->clearSuccessHandlers();
  }

}
