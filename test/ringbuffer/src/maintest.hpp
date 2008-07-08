#ifndef MAINTEST_HPP
#define MAINTEST_HPP

#include <iostream>
#include <test.hpp>
#include <datastructures.hpp>
#include <exception.hpp>

class RingbufferTest : public test::TestCase<RingbufferTest>{

private:
  
  ds::RingBuffer<int> *buffer;

public:

  RingbufferTest() : test::TestCase<RingbufferTest>(){

    addTest(&RingbufferTest::quietTest,"quietTest()");
    addTest(&RingbufferTest::insertTest,"insertTest()");

  }

  void setUp(){

    buffer = new ds::RingBuffer<int>(10,ds::RingBuffer<int>::QUIET_OVERWRITING);

  }

  void tearDown(){

    delete buffer;

  }

  std::string getTestCaseName(){ return "RingbufferTest"; }

  void quietTest() throw (ExceptionBase){
    
    for ( int i = 1; i <= 37; i++)
      buffer->enqueue(i);

    for (int i = 0; i < 10; i++)
      assertEquals(28+i,(*buffer)[i]);

  }

  void insertTest() throw (ExceptionBase){
   
    (*buffer)[9] = 1;

    for (int i = 0; i < 10; i++){

      if ( i != 9 )
	assertEquals(28+i,(*buffer)[i]);
      else
	assertEquals(1,(*buffer)[i]);
	
    }

  }
  

};

#endif
