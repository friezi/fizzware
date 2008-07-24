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
#include <utility>
#include <exception.hpp>
#include <utils.hpp>

/**
   @brief for testing code and classes
*/
namespace test{

  class TestCaseBase;
  class TestUnit;

  /**
     @brief the base of all TestCases. Not for direct use.
  */
  class TestCaseBase{

  public:

    typedef void (*HandlerType)(TestCaseBase *testcase, std::string msg);
    typedef std::list<HandlerType> HandlerStack;

  private:

    std::string currentTestName;
    HandlerStack errorHandlers;
    HandlerStack successHandlers;
    HandlerStack statisticHelpers;

    static void defaultErrorHandler(TestCaseBase *testcase, std::string msg) throw (excpt::Exception<TestCaseBase>);
 
  public:

    TestCaseBase();

    virtual ~TestCaseBase(){}

    virtual void operator()() = 0;

    virtual std::string getTestCaseName() = 0;

    virtual unsigned long getNmbTests() = 0;

    std::string getCurrentTestName(){ return currentTestName; }

    void pushErrorHandler(HandlerType errorHandler){
      errorHandlers.push_front(errorHandler);
    }

    void pushSuccessHandler(HandlerType successHandler){
      successHandlers.push_front(successHandler);
    }

    void pushStatisticHelper(HandlerType statisticHelper){
      statisticHelpers.push_front(statisticHelper);
    }

    void clearErrorHandlers(){ errorHandlers.clear(); }

    void clearSuccessHandlers(){ successHandlers.clear(); }

    void clearStatisticHelpers(){ statisticHelpers.clear(); }

    HandlerType getDefaultErrorHandler();

    void callStatisticHelpers();
    
    void assertTrue(bool value) throw (excpt::Exception<TestCaseBase>);
    
    void assertTrue(bool value, std::string id) throw (excpt::Exception<TestCaseBase>);
    
    void assertFalse(bool value) throw (excpt::Exception<TestCaseBase>);
    
    void assertFalse(bool value, std::string id) throw (excpt::Exception<TestCaseBase>);

    template <typename TE, typename TR>
    void assertEquals(TE expected, TR reference) throw (excpt::Exception<TestCaseBase>){
      assertEquals(static_cast<TR>(expected),reference);
    }
    
    template <typename T>
    void assertEquals(T expected, T reference) throw (excpt::Exception<TestCaseBase>){

      if ( expected != reference ){

	std::ostringstream err;
	err << "Equals failed! expected: \"" << expected << "\" got: \"" << reference << "\"";
	throw excpt::Exception<TestCaseBase>(err.str());

      }	
    }
    
  protected:
    
    void error(char msg[]) throw (excpt::Exception<TestCaseBase>);
    
    void error(std::string msg) throw (excpt::Exception<TestCaseBase>);
    
    void success();
    
    void setCurrentTestName(std::string id){ currentTestName = id; }

  };

  /**
     @brief a testcase which can contain several tests
  */
  template <typename T> class TestCase : public TestCaseBase{

  private:

    typedef std::list< std::pair<void (T::*)(),std::string> > Tests;

    Tests tests;

  public:

    TestCase() : TestCaseBase(){}

    virtual ~TestCase(){}

    unsigned long getNmbTests(){ return tests.size(); }

  private:

    virtual void setUp(){};
    virtual void tearDown(){};

  protected:

    void addTest(void (T::*test)(), std::string id){ tests.push_back(std::pair<void (T::*)(),std::string>(test,id)); }

    void operator()() throw (excpt::Exception< TestCase<T> >){

      setUp();
      startTests();
      tearDown();
      callStatisticHelpers();

    }

  private:

    void startTests() throw (excpt::Exception< TestCase<T> >){

      T * self;

      if ( (self = dynamic_cast<T *>(this)) == 0 )
	throw excpt::Exception< TestCase<T> >("ERROR: wrong class-instantiation! ");
  
      typename Tests::iterator it;
      for ( it = tests.begin(); it != tests.end(); it++ ){
	
	try{
	  
	  setCurrentTestName((*it).second);
	  (self->*((*it).first))();
	  success();
	  
	} catch (excpt::ExceptionBase &e){
	  
	  try{
	    
	    error(e.getMsg());
	    
	  } catch (excpt::ExceptionBase &ee){
	    std::cerr << getTestCaseName() + ":" + (*it).second + " failed!: " + ee.getMsg() << std::endl;
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

    TestCaseBase::HandlerType testcaseStartupHandler;

    static void defaultTestCaseStartupHandler(TestCaseBase::TestCaseBase *testcase, std::string msg);

  public:

    TestUnit();

    virtual ~TestUnit();

    void addTestCase(TestCaseBase * testcase){ testcases.push_back(testcase); }

    void operator()() throw (excpt::ExceptionBase);

    unsigned long getNmbTestCases(){ return testcases.size(); }

    unsigned long getNmbTests();

    void setTestCaseStartupHandler(TestCaseBase::HandlerType testcaseStartupHandler){ this->testcaseStartupHandler = testcaseStartupHandler; }

    void pushErrorHandler(TestCaseBase::HandlerType errorHandler);

    void pushSuccessHandler(TestCaseBase::HandlerType successHandler);

    void pushStatisticHelper(TestCaseBase::HandlerType statisticHelper);

    void clearErrorHandlers();

    void clearSuccessHandlers();

    void clearStatisticHelpers();

  };

}

#endif
