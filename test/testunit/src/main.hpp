#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <test.hpp>

class MyTestCase1 : public test::TestCase<MyTestCase1>{

private:

  std::string NAME;

public:

  MyTestCase1() : test::TestCase<MyTestCase1>(){

    NAME = "MyTestCase1";
    addTest(&MyTestCase1::test1);
    addTest(&MyTestCase1::test2);

  }

  void setUp(){ std::cout << "starting " << NAME << std::endl; }
  void tearDown(){ std::cout << "stopping " << NAME << std::endl; }

  std::string getTestcasename(){ return NAME; }

  void test1() throw (ExceptionBase){

    setTestname("test1()");

    throw Exception<MyTestCase1>("nee du!");

  }

  void test2() throw (ExceptionBase){

    setTestname("test2()");

    throw Exception<MyTestCase1>("nee du!");

  }

};

class MyTestCase2 : public test::TestCase<MyTestCase2>{

public:

  MyTestCase2() : test::TestCase<MyTestCase2>(){

    addTest(&MyTestCase2::test1);
    addTest(&MyTestCase2::test2);

  }

  void setUp(){}

  std::string getTestcasename(){ return "MyTestCase2"; }

  void test1() throw (ExceptionBase){

    setTestname("test1()");

    throw Exception<MyTestCase2>("nee du!");

  }

  void test2() throw (ExceptionBase){

    setTestname("test2()");

    throw Exception<MyTestCase2>("nee du!");

  }

};

#endif
