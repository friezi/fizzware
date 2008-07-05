#ifndef MAINTEST_HPP
#define MAINTEST_HPP

#include <iostream>
#include <utils.hpp>
#include <test.hpp>

class ObserverTest : public test::TestCase<ObserverTest>{

public:

  static int testval1;
  static int testval2;
  static int testval3;

  class TestObserver1 : public utils::SmartObserver<int>{

    void update(utils::Observable<int> *observable, int value){
      testval1 = value;
    }

  };

  class TestObserver2 : public utils::SmartObserver<int>{

    void update(utils::Observable<int> *observable, int value){
      testval2 = value;
    }

  };

  class TestObserver3 : public utils::SmartObserver<int>{

    void update(utils::Observable<int> *observable, int value){
      testval3 = value;
    }

  };
  
  ObserverTest() : test::TestCase<ObserverTest>(){

    addTest(&ObserverTest::test1);

  }
    
  std::string getTestcaseName(){ return "ObserverTest"; }

//   void startup(){

//     testval1 = 0;
//     testval2 = 0;

//   }

  void test1() throw(ExceptionBase){
    
    utils::ChangeNotifier<int> notifier(4);
    utils::ChangeNotifier<int> notifier2;

    TestObserver1 testobserver1;
    TestObserver2 testobserver2;
    TestObserver3 testobserver3;

    assertEquals(0,testval1);
    assertEquals(0,testval2);
    assertEquals(0,testval3);

    testobserver1.addToNotifier(&notifier);
    testobserver2.addToNotifier(&notifier);
    testobserver3.addToNotifier(&notifier2);
    
    assertEquals(4,testval1);
    assertEquals(4,testval2);
    assertEquals(0,testval3);

    notifier = 2;
    notifier2 = 2;
 
    assertEquals(2,testval1);
    assertEquals(2,testval2);
    assertEquals(2,testval3);

    notifier.removeObservers();
    notifier2.removeObservers(); 

    notifier = 3;
    notifier2 = 3;
 
    assertEquals(2,testval1);
    assertEquals(2,testval2);
    assertEquals(2,testval3);
    
  }


};

#endif
