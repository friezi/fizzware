#ifndef MAINTEST_HPP
#define MAINTEST_HPP

#include <iostream>
#include <fztooltempl/utils.hpp>
#include <fztooltempl/test.hpp>

using namespace std;
using namespace exc;
using namespace utils;
using namespace test;

class ObserverTest : public TestCase<ObserverTest>{

public:

  static int testval1;
  static int testval2;
  static int testval3;

  class TestObserver1 : public SmartObserver<int>{

    void update(Observable<int> *observable, int value){
      testval1 = value;
    }

  };

  class TestObserver2 : public SmartObserver<int>{

    void update(Observable<int> *observable, int value){
      testval2 = value;
    }

  };

  class TestObserver3 : public SmartObserver<int>{

    void update(Observable<int> *observable, int value){
      testval3 = value;
    }

  };
  
  ObserverTest() : TestCase<ObserverTest>(){

    addTest(&ObserverTest::straight,"straight");
    addTest(&ObserverTest::reversed,"reversed");

  }

  void init(){

    testval1 = 0;
    testval2 = 0;
    testval3 = 0;

  }

  void straight() throw(ExceptionBase){

    init();
    
    ChangeNotifier<int> notifier(4);
    ChangeNotifier<int> notifier2;
    
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

  void reversed() throw(ExceptionBase){

    init();

    TestObserver1 testobserver1;
    TestObserver2 testobserver2;
    TestObserver3 testobserver3;
    
    ChangeNotifier<int> notifier(4);
    ChangeNotifier<int> notifier2;

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
