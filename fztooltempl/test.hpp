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

  /**
     @brief a testcase which can contain several tests
  */
  class TestCase{

    friend class TestUnit;

  private:

    std::list<void (*)()> tests;

    std::string testcasename;

    static std::string testname;

  public:

    TestCase(){}

    virtual ~TestCase(){}

  private:

    virtual void setUp() = 0;

  public:

    virtual std::string getTestcasename() = 0;

  protected:

    void addTest(void (*test)()){ tests.push_back(test); }

    static void setTestname(std::string name){ testname = name; }

  public:

    static std::string getTestname(){ return testname; }

  private:

    void run();

    void startTests();

  };

  /**
     @brief Main class for executing testcases. Can contain several Testcases.
  */
  class TestUnit{

  private:

    std::list<TestCase *> testcases;

  public:

    TestUnit(){}

    virtual ~TestUnit();

    void addTestcase(TestCase * testcase){ testcases.push_back(testcase); }

    void run();

  };

}

#endif
