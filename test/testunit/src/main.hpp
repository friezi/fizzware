#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <test.hpp>

class MyTestCase1 : public test::TestCase<MyTestCase1>{

private:

  std::string NAME;

  static unsigned long nmb;

  static void myErrorHandler1(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>);
  static void mySuccessHandler1(test::TestCaseBase *testcase, std::string msg);

public:

  MyTestCase1() : test::TestCase<MyTestCase1>(){

    NAME = "MyTestCase1";
    addTest(&MyTestCase1::test1);
    addTest(&MyTestCase1::test2);
    addTest(&MyTestCase1::test3);
    addTest(&MyTestCase1::test4);
    pushErrorHandler(myErrorHandler1);
    pushSuccessHandler(mySuccessHandler1);

  }

  void setUp(){ std::cout << "starting " << NAME << std::endl; }
  void tearDown(){
    
    std::cout << "stopping " << NAME << std::endl;
    std::cout << nmb << "/" << getNmbTests() << " tests failed in MyTestCase1!" << std::endl;

  }

  std::string getTestcasename(){ return NAME; }

  void test1() throw (ExceptionBase){

    setTestname("test1()");

    assertTrue(false);

  }

  void test2() throw (ExceptionBase){

    setTestname("test2()");

    assertEquals(1,2);

  }

  void test3() throw (ExceptionBase){

    setTestname("test3()");

    assertEquals(std::string("1,2,3"),std::string("1,2,3"));

  }

  void test4() throw (ExceptionBase){

    setTestname("test4()");

    assertEquals(std::string("1,2,3"),std::string("2,2,3"));

  }

};

class MyTestCase2 : public test::TestCase<MyTestCase2>{

private:

  static unsigned long nmb;

  static void myErrorHandler2(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>);
  
public:

  MyTestCase2() : test::TestCase<MyTestCase2>(){

    addTest(&MyTestCase2::test1);
    addTest(&MyTestCase2::test2);
    clearErrorHandlers();
    pushErrorHandler(myErrorHandler2);

  }

  void setUp(){ nmb = 0; }
  void tearDown(){ std::cout << nmb << "/" << getNmbTests() << " tests failed in MyTestCase2!" << std::endl; }

  std::string getTestcasename(){ return "MyTestCase2"; }

  void test1() throw (ExceptionBase){

    setTestname("test1()");
    assertFalse(true);

  }

  void test2() throw (ExceptionBase){

    setTestname("test2()");
    assertTrue(false);

  }

};

unsigned long MyTestCase1::nmb;

void MyTestCase1::myErrorHandler1(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  nmb++;
}

void MyTestCase1::mySuccessHandler1(test::TestCaseBase *testcase, std::string msg){
  std::cout << "success: " << testcase->getTestname() << std::endl;
}

unsigned long MyTestCase2::nmb;

void MyTestCase2::myErrorHandler2(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  nmb++;
}

unsigned long global_errors = 0;

void globalErrorHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  global_errors++;
}

unsigned long global_success = 0;

void globalSuccessHandler(test::TestCaseBase *testcase, std::string msg){
  global_success++;
}

#endif
