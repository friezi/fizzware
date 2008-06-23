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
    addTest(&MyTestCase1::test3);
    addTest(&MyTestCase1::test4);

  }

  void setUp(){ std::cout << "starting " << NAME << std::endl; }
  void tearDown(){ std::cout << "stopping " << NAME << std::endl; }

  std::string getTestcasename(){ return NAME; }

  void test1() throw (ExceptionBase){

    setTestname("test1()");

    test::assertTrue(false);

    throw Exception<MyTestCase1>("nee du!");

  }

  void test2() throw (ExceptionBase){

    setTestname("test2()");

    test::assertEquals(1,2);

    throw Exception<MyTestCase1>("nee du!");

  }

  void test3() throw (ExceptionBase){

    setTestname("test3()");

    test::assertEquals(std::string("1,2,3"),std::string("1,2,3"));

    throw Exception<MyTestCase1>("erfolgreich!");

  }

  void test4() throw (ExceptionBase){

    setTestname("test4()");

    test::assertEquals(std::string("1,2,3"),std::string("2,2,3"));

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
