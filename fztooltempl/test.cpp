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

void test::assertTrue(bool value) throw (Exception<TestCaseBase>){
  if ( value != true )
    throw Exception<TestCaseBase>("value not true");
}

void test::assertTrue(bool value, string id) throw (Exception<TestCaseBase>){
  if ( value != true )
    throw Exception<TestCaseBase>(id + " not true");
}

void test::assertFalse(bool value) throw (Exception<TestCaseBase>){
  if ( value != false )
    throw Exception<TestCaseBase>("value not false");
}

void test::assertFalse(bool value, string id) throw (Exception<TestCaseBase>){
  if ( value != false )
    throw Exception<TestCaseBase>(id + " not false");
}

TestCaseBase::TestCaseBase(){
  setTestname("");
}

TestUnit::~TestUnit(){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    delete *it;

}

void TestUnit::operator()() throw (ExceptionBase){

  for ( list<TestCaseBase *>::iterator it = testcases.begin(); it != testcases.end(); it++ )
    (*(*it))();

}