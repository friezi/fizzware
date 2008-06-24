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
#include <sstream>
#include <list>
#include <exception.hpp>
#include <utils.hpp>

/**
   @brief for testing code and classes
*/
namespace test{

  class TestCaseBase;
  class TestUnit;

  class TestCaseBase{

  public:

    typedef void (*ErrorHandlerPtr)(TestCaseBase *testcase, std::string msg);
    typedef std::list<ErrorHandlerPtr> ErrorHandlerStack;

  private:

    std::string testname;

    ErrorHandlerStack errorHandlers;

    static void defaultErrorHandler(TestCaseBase *testcase, std::string msg) throw (Exception<TestCaseBase>);

  public:

    TestCaseBase();

    virtual ~TestCaseBase(){}

    virtual void operator()() = 0;

    virtual std::string getTestcasename() = 0;

    virtual unsigned long getNmbTests() = 0;

    std::string getTestname(){ return testname; }

    void pushErrorHandler(ErrorHandlerPtr errorHandler){
      errorHandlers.push_front(errorHandler);
    }

    void clearErrorHandlers(){ errorHandlers.clear(); }

    ErrorHandlerPtr getDefaultErrorHandler();
    
    void assertTrue(bool value) throw (Exception<TestCaseBase>);
    
    void assertTrue(bool value, std::string id) throw (Exception<TestCaseBase>);
    
    void assertFalse(bool value) throw (Exception<TestCaseBase>);
    
    void assertFalse(bool value, std::string id) throw (Exception<TestCaseBase>);
    
    template <typename T>
    void assertEquals(T expected, T reference) throw (Exception<TestCaseBase>){

      if ( expected != reference ){

	std::ostringstream err;
	err << "Equals failed! expected was: \"" << expected << "\" got \"" << reference << "\"";
	throw Exception<TestCaseBase>(err.str());

      }	
    }
    
  protected:
    
    void setTestname(std::string name){ testname = name; }
    
  public:
    
    void error(TestCaseBase *testcase, char msg[]) throw (Exception<TestCaseBase>);
    
    void error(TestCaseBase *testcase, std::string msg) throw (Exception<TestCaseBase>);

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

    unsigned long getNmbTests(){ return tests.size(); }

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
  
      typename std::list<void (T::*)()>::iterator it;
      for ( it = tests.begin(); it != tests.end(); it++ ){
	
	try{
	  
	  (self->*(*it))();
	  
	} catch (ExceptionBase &e){
	  
	  try{
	    
	    error(this,e.getMsg());
	    
	  } catch (ExceptionBase &ee){
	    std::cout << getTestcasename() + ":" + getTestname() + " failed!: " + ee.getMsg() << std::endl;
	  }
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

    unsigned long getNmbTests();

    void pushErrorHandler(TestCaseBase::ErrorHandlerPtr errorHandler);

    void clearErrorHandlers();

  };

}

#endif
