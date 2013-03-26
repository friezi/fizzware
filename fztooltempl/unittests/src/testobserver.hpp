#ifndef TEST_OBSERVER_HPP
#define TEST_OBSERVER_HPP

#include <fztooltempl/exception.hpp>
#include <fztooltempl/utils.hpp>
#include <fztooltempl/test.hpp>

class ObserverTest : public test::TestCase<ObserverTest>{

public:

  int testval1;
  int testval2;
  int testval3;

  class TestObserver1 : public utils::SmartObserver<int>{

    int *testval;

  public:

    TestObserver1(int *testval){
      this->testval = testval;
    }

  private:

    void update(utils::Observable<int> *observable, int value){
      (*testval) = value;
    }

  };

  class TestObserver2 : public utils::SmartObserver<int>{

    int *testval;

  public:

    TestObserver2(int *testval){
      this->testval = testval;
    }

  private:

    void update(utils::Observable<int> *observable, int value){
      (*testval) = value;
    }

  };

  class TestObserver3 : public utils::SmartObserver<int>{

    int *testval;

  public:

    TestObserver3(int *testval){
      this->testval = testval;
    }

  private:

    void update(utils::Observable<int> *observable, int value){
      (*testval) = value;
    }

  };
  
  ObserverTest() : test::TestCase<ObserverTest>(){

    addTest(&ObserverTest::straight,"straight");
    addTest(&ObserverTest::reversed,"reversed");

  }

  void init(){

    testval1 = 0;
    testval2 = 0;
    testval3 = 0;

  }

  void beforeTest(){
    init();
  }

  void straight() throw(exc::ExceptionBase){
    
    utils::ChangeNotifier<int> notifier(4);
    utils::ChangeNotifier<int> notifier2;
    
    TestObserver1 testobserver1(&testval1);
    TestObserver2 testobserver2(&testval2);
    TestObserver3 testobserver3(&testval3);

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

  void reversed() throw(exc::ExceptionBase){

    TestObserver1 testobserver1(&testval1);
    TestObserver2 testobserver2(&testval2);
    TestObserver3 testobserver3(&testval3);
    
    utils::ChangeNotifier<int> notifier(4);
    utils::ChangeNotifier<int> notifier2;

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
