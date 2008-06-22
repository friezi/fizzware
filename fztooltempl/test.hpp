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

/**
   @file test.hpp
   @author Friedemann Zintel
   @todo doku!!!
*/

#ifndef _FZTT_TEST_HPP_
#define _FZTT_TEST_HPP_

#include <iostream>
#include <list>
#include <exception.hpp>

/**
   @brief for testing code and classes
*/
namespace test{

  class TestUnit;

  class TestCaseBase{

  private:

    std::string testname;

  public:

    TestCaseBase();

    virtual ~TestCaseBase(){}

    virtual void operator()() = 0;

    virtual std::string getTestcasename() = 0;

    std::string getTestname(){ return testname; }

  protected:

    void setTestname(std::string name){ testname = name; }

  };

  /**
     @brief a testcase which can contain several tests
  */
  template <typename T> class TestCase : public TestCaseBase{

  private:

    std::list<void (T::*)()> tests;

  public:

    TestCase() : TestCaseBase(){}

    virtual ~TestCase(){}

  private:

    virtual void setUp(){};
    virtual void tearDown(){};

  protected:

    void addTest(void (T::*test)()){ tests.push_back(test); }

    void operator()() throw (Exception< TestCase<T> >){

      setUp();
      startTests();
      tearDown();

    }

  private:

    void startTests() throw (Exception< TestCase<T> >){

      T * self = dynamic_cast<T *>(this);

      if ( self == 0 )
	throw Exception< TestCase<T> >("ERROR: wrong class-instantiation! ");
  
      for (typename std::list<void (T::*)()>::iterator it = tests.begin(); it != tests.end(); it++ ){

	try{
	  (self->*(*it))();
	} catch (ExceptionBase &e){
	  std::cout << getTestcasename() + ":" + getTestname() + " failed!: " + e.getMsg() << std::endl;
	}    
      }
    }

  };

  /**
     @brief Main class for executing testcases. Can contain several Testcases.
  */
  class TestUnit{

  private:

    std::list<TestCaseBase *> testcases;

  public:

    TestUnit(){}

    virtual ~TestUnit();

    void addTestcase(TestCaseBase * testcase){ testcases.push_back(testcase); }

    void operator()() throw (ExceptionBase);

  };

}

#endif
