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

#ifndef FZTOOLTEMPL_TEST_HPP
#define FZTOOLTEMPL_TEST_HPP

#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <utility>
#include <algorithm>
#include <fztooltempl/exception.hpp>
#include <fztooltempl/utils.hpp>

/**
   @brief for testing code and classes
*/
namespace test{

  class TestCaseBase;
  class TestUnit;
  
  typedef void (*HandlerType)(TestCaseBase *testcase, std::string msg);
  typedef std::list<HandlerType> HandlerStack;
  
  /**
     @brief the base of all TestCases. Not for direct use.
  */
  class TestCaseBase{

    friend class TestUnit;

  private:

    std::string currentTestName;
    HandlerStack testStartHandlers;
    HandlerStack errorHandlers;
    HandlerStack successHandlers;
    std::string testcasename;
 
  public:

    TestCaseBase();

    virtual ~TestCaseBase(){}

    virtual void operator()(std::set<std::string> *selection = 0) = 0;

    /**
       @brief returns the set of names of all registered tests
       @return pointer to a set of names
    */
    virtual std::set<std::string> * getTestNames() = 0;

    /**
       @brief returns the number of all tests.
       @return number
     **/
    virtual unsigned long getNmbTests() = 0;

    std::string getTestCaseName(){ return testcasename; }

    std::string getCurrentTestName(){ return currentTestName; }

    void pushTestStartHandler(HandlerType handler){
      testStartHandlers.push_front(handler);
    }

    void pushErrorHandler(HandlerType handler){
      errorHandlers.push_front(handler);
    }

    void pushSuccessHandler(HandlerType handler){
      successHandlers.push_front(handler);
    }

    void clearTestStartHandlers(){ testStartHandlers.clear(); }

    void clearErrorHandlers(){ errorHandlers.clear(); }

    void clearSuccessHandlers(){ successHandlers.clear(); }
    
    void assertTrue(bool value) throw (exc::Exception<TestCaseBase>);
    
    void assertTrue(bool value, std::string id) throw (exc::Exception<TestCaseBase>);
    
    void assertFalse(bool value) throw (exc::Exception<TestCaseBase>);
    
    void assertFalse(bool value, std::string id) throw (exc::Exception<TestCaseBase>);

    template <typename TE, typename TR>
    void assertEquals(TE expected, TR reference) throw (exc::Exception<TestCaseBase>){
      assertEquals(expected,reference,"");
    }
    
    template <typename TE, typename TR>
    void assertEquals(TE expected, TR reference, std::string info) throw (exc::Exception<TestCaseBase>){
      assertEquals(static_cast<TR>(expected),reference,info);
    }
    
    template <typename T>
    void assertEquals(T expected, T reference) throw (exc::Exception<TestCaseBase>){
      assertEquals(expected,reference,"");
    }
    
    template <typename T>
    void assertEquals(T expected, T reference, std::string info) throw (exc::Exception<TestCaseBase>){

      if ( expected != reference ){

	std::ostringstream err;
	err << "Equals failed! expected: \"" << expected << "\" got: \"" << reference << "\"\t info: " << info;
	throw exc::Exception<TestCaseBase>(err.str());

      }	
    }
    
  protected:
    
    void callErrorHandlers(char msg[]) throw (exc::Exception<TestCaseBase>);
    
    void callErrorHandlers(std::string msg) throw (exc::Exception<TestCaseBase>);

    void callTestStartHandlers();
    
    void callSuccessHandlers();

    void setTestCaseName(std::string name){ testcasename = name; }
    
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

    std::set<std::string> * getTestNames(){

      std::set<std::string> *names = new std::set<std::string>();

      for ( typename Tests::iterator it = tests.begin(); it != tests.end(); it++ ){
	names->insert((*it).second);
      }

      return names;
    }

    unsigned long getNmbTests(){ return tests.size(); }

  private:

    /**
       @brief will be executed before execution of all tests in TestCase
     **/
    virtual void beforeClass(){};

    /**
       @brief will be executed after execution of all tests in TestCase
    **/
    virtual void afterClass(){};

    /**
       @brief will be executed before execution of each test in TestCase
     **/
    virtual void beforeTest(){};

    /**
       @brief will be executed after execution of each test in TestCase
     **/
    virtual void afterTest(){};

  protected:

    void addTest(void (T::*test)(), std::string id){ tests.push_back(std::pair<void (T::*)(),std::string>(test,id)); }

    void operator()(std::set<std::string> *selection) throw (exc::Exception< TestCase<T> >){

      beforeClass();
      startTests(selection);
      afterClass();

    }

  private:

    /**
       @brief starts all resp. selected tests in testcase
       @param selection if != 0: only tests contained in selection will be executed; otherwise all tests
     **/
    void startTests(std::set<std::string> *selection = 0) throw (exc::Exception< TestCase<T> >){

      T * self;
      std::set<std::string>::iterator eit;

      if ( (self = dynamic_cast<T *>(this)) == 0 ){
	throw exc::Exception< TestCase<T> >("ERROR: wrong class-instantiation! ");
      }      

      typename Tests::iterator it;
      for ( it = tests.begin(); it != tests.end(); it++ ){
	
	if ( selection != 0 ){
	  // execute only selected tests
	  if ( (eit = std::find(selection->begin(),selection->end(),(*it).second)) == selection->end() ){
	    continue;
	  }
	}
	
	try{
	  
	  setCurrentTestName((*it).second);
	  callTestStartHandlers();

	  beforeTest();
	  (self->*((*it).first))();
	  afterTest();

	  callSuccessHandlers();
	  
	} catch (exc::ExceptionBase &e){
	  
	  try{
	    
	    std::ostringstream msg;
	    msg << getTestCaseName() + ":" + (*it).second + "() failed!: " + e.getMsg();
	    callErrorHandlers(msg.str());
	    
	  } catch (exc::ExceptionBase &ee){
	    std::cerr << getTestCaseName() + ":" + (*it).second + "() failed!: " + ee.getMsg() << std::endl;
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

    HandlerStack testcaseStartHandlers;
    HandlerStack testcaseEndHandlers;

  public:

    TestUnit();

    virtual ~TestUnit();

    void addTestCase(TestCaseBase * testcase, std::string name);

    /**
       If parameter 'execute' is given, only the testcases and tests found in the map/set are exsecuted. Otherwise
       all will be executed.
       @brief executes testcases
       @param selection if not 0, contains a map with key = TestCaseName and value = set of tests to be executed for this testcase
    */
    void operator()(std::map< std::string,std::set<std::string> *> *selection = 0) throw (exc::ExceptionBase);

    unsigned long getNmbTestCases(){ return testcases.size(); }

    unsigned long getNmbTests();

    const std::list<TestCaseBase *> & getTestCases(){ return testcases; }

    void callTestCaseStartHandlers(TestCaseBase *testcase);

    void callTestCaseEndHandlers(TestCaseBase *testcase);

    void pushTestCaseStartHandler(HandlerType handler);

    void pushTestCaseEndHandler(HandlerType handler);

    void pushTestStartHandler(HandlerType handler);

    void pushErrorHandler(HandlerType handler);

    void pushSuccessHandler(HandlerType handler);

    void clearTestCaseStartHandlers();

    void clearTestCaseEndHandlers();

    void clearTestStartHandlers();

    void clearErrorHandlers();

    void clearSuccessHandlers();

  };

}

#endif
