#ifndef MAIN_HPP
#define MAIN_HPP

#include <test.hpp>

class MyTestCase1 : public test::TestCase{

public:

  MyTestCase1() : TestCase(){

    addTest(test1);
    addTest(test2);

  }

  void setUp(){}

  std::string getTestcasename(){ return "MyTestCase1"; }

  static void test1() throw (ExceptionBase){

    setTestname("test1()");

    throw Exception<MyTestCase1>("nee du!");

  }

  static void test2() throw (ExceptionBase){

    setTestname("test2()");

    throw Exception<MyTestCase1>("nee du!");

  }

};

class MyTestCase2 : public test::TestCase{

public:

  MyTestCase2() : TestCase(){

    addTest(test1);
    addTest(test2);

  }

  void setUp(){}

  std::string getTestcasename(){ return "MyTestCase2"; }

  static void test1() throw (ExceptionBase){

    setTestname("test1()");

    throw Exception<MyTestCase2>("nee du!");

  }

  static void test2() throw (ExceptionBase){

    setTestname("test2()");

    throw Exception<MyTestCase2>("nee du!");

  }

};

#endif
