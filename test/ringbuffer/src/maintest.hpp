#ifndef MAINTEST_HPP
#define MAINTEST_HPP

#include <iostream>
#include <fztooltempl/test.hpp>
#include <fztooltempl/datastructures.hpp>
#include <fztooltempl/exception.hpp>

class RingbufferTest : public test::TestCase<RingbufferTest>{

private:
  
  ds::RingBuffer<int> *buffer;
  ds::RingBuffer<int> *buffer2;

public:

  RingbufferTest() : test::TestCase<RingbufferTest>(){

    addTest(&RingbufferTest::quietTest,"quietTest()");
    addTest(&RingbufferTest::quietInsertTest,"quietInsertTest()");
    addTest(&RingbufferTest::alertingTest,"alertingTest()");
    addTest(&RingbufferTest::alertingDequeueTest,"alertingDequeueTest()");
    addTest(&RingbufferTest::alertingSkipTest,"alertingSkipTest()");
    addTest(&RingbufferTest::alertingEnqueueTest,"alertingEnqueueTest()");

  }

  void setUp(){

    buffer = new ds::RingBuffer<int>(10,ds::RingBuffer<int>::QUIET_OVERWRITING);
    buffer2 = new ds::RingBuffer<int>(10);

  }

  void tearDown(){

    delete buffer;
    delete buffer2;

  }

  std::string getTestCaseName(){ return "RingbufferTest"; }

  void quietTest() throw (exc::ExceptionBase){
    
    for ( int i = 1; i <= 37; i++)
      buffer->enqueue(i);

    for (int i = 0; i < 10; i++)
      assertEquals(28+i,(*buffer)[i]);

  }

  void quietInsertTest() throw (exc::ExceptionBase){
   
    (*buffer)[9] = 1;

    for (int i = 0; i < 10; i++){

      if ( i != 9 )
	assertEquals(28+i,(*buffer)[i]);
      else
	assertEquals(1,(*buffer)[i]);
	
    }

  }

  void alertingTest() throw (exc::ExceptionBase){
    
    for ( int i = 1; i <= 10; i++)
      buffer2->enqueue(i);

    for (int i = 0; i < 10; i++)
      assertEquals(i+1,(*buffer2)[i]);

  }

  void alertingDequeueTest() throw (exc::ExceptionBase){

    assertEquals(1,buffer2->dequeue());
    assertEquals(9,buffer2->getElements());

    for (size_t i = 0; i < buffer2->getElements(); i++)
      assertEquals(i+2,(*buffer2)[i]);

  }

  void alertingSkipTest() throw (exc::ExceptionBase){

    buffer2->skip(2);

    for (size_t i = 0; i < buffer2->getElements(); i++)
      assertEquals(i+4,(*buffer2)[i]);

    for (size_t i = 0; i < 3; i++)
      buffer2->enqueue(i);

    assertEquals(10,buffer2->getElements());

    for (size_t i = 0; i < buffer2->getElements(); i++){

      if ( i < 7 )
	assertEquals(i+4,(*buffer2)[i]);
      else
	assertEquals(i-7,(*buffer2)[i]);

    }

  }

  void alertingEnqueueTest() throw (exc::ExceptionBase){
  
    try{
      buffer2->enqueue(1);
    } catch (exc::ExceptionBase &e){
      return;
    }

    throw exc::Exception<test::TestCaseBase>("expected Exception in alerting-mode on inserting into full buffer!");
    
  }
  

};

#endif
