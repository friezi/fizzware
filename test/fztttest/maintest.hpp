#ifndef _MAINTEST_HPP_
#define _MAINTEST_HPP_

#include <iostream>
#include <list>
#include <exception.hpp>
#include <test.hpp>
#include <datastructures.hpp>

using namespace std;
using namespace exc;
using namespace test;
using namespace ds;

class StackTest : public TestCase<StackTest>{

public:

  StackTest() : TestCase<StackTest>(){
   
    addTest(&StackTest::emptyTest,"emptyTest");
    addTest(&StackTest::simpleTest,"simpleTest");
    addTest(&StackTest::fillTest,"fillTest");
    addTest(&StackTest::listPrependTest,"listPrependTest");
    addTest(&StackTest::listAppendTest,"listAppendTest");

  }

  void emptyTest() throw (ExceptionBase){

    Stack<int> mystack;

    assertTrue(mystack.begin() == mystack.topIter(),"begin() == topIter()");
    assertTrue(mystack.end() == mystack.topIter(),"end() == topIter()");
    assertTrue(mystack.localEmpty(mystack.topIter()),"localEmpty topIter(): ");
    assertTrue(mystack.localEmpty(mystack.begin()),"localEmpty: begin()");
    assertTrue(mystack.localEmpty(mystack.end()),"localEmpty: end()");

    try{
      mystack.top();
      throw Exception<StackTest>("top() must fail!");
    } catch ( Exception< Stack<int> > & e ){
    }

    try{
      mystack.pop();
      throw Exception<StackTest>("pop() must fail!");
    } catch ( Exception< Stack<int> > & e ){
    }

  }

  void simpleTest() throw (ExceptionBase){

    Stack<int> mystack;

    mystack.push(1);
   
    assertEquals(1,mystack.size(),"size");
    assertTrue(mystack.begin() == mystack.topIter(),"begin() == topIter()");
    assertTrue(mystack.end() != mystack.topIter(),"end() != topIter()");

  }

  void fillTest() throw (ExceptionBase){

    Stack<int> mystack;

    mystack.push(1);
    mystack.push(2);
    mystack.push(3);
    
    assertEquals(3,mystack.size(),"size");
    assertTrue(mystack.begin() != mystack.topIter(),"begin() != topIter()");
    assertTrue(mystack.end() != mystack.topIter(),"end() != topIter()");

    assertEquals(3,mystack.top(),"top()");
    assertEquals(3,mystack.size(),"size() after top()");
    assertEquals(3,mystack.pop(),"pop()");
    assertEquals(2,mystack.size(),"size() after pop()");

    Stack<int>::const_iterator it = mystack.topIter();
    
    assertTrue(mystack.localEmpty(it),"localEmpty():");
    mystack.push(4);
    assertFalse(mystack.localEmpty(it),"localEmpty() after fill:");
    mystack.pop();
    assertTrue(mystack.localEmpty(it),"localEmpty() after pop:");
    

  }

  void listPrependTest() throw (ExceptionBase){

    Stack<int> mystack;
    list<int> l;
    
    Stack<int>::const_iterator bottom = mystack.topIter();    

    l.push_back(0);
    l.push_back(13);

    mystack.push(1);
    mystack.push(2);
    mystack.push(3);
    
    Stack<int>::const_iterator it = mystack.topIter();
    
    mystack.push(4);
    mystack.push(5);

    mystack.popToListPrepend(l,it);

    assertEquals(3,mystack.size(),"mystack size");
    assertEquals(4,l.size(),"l size");

    assertEquals(4,l.front(),"l first");
    l.pop_front();
    assertEquals(5,l.front(),"l second");
    l.pop_front();
    assertEquals(0,l.front(),"l third");
    l.pop_front();

    mystack.popToListPrepend(l,bottom);

    assertEquals(0,mystack.size(),"mystack size empty");
    assertEquals(4,l.size(),"l size bottom");

    assertEquals(1,l.front(),"l first bottom");
    l.pop_front();
    assertEquals(2,l.front(),"l second bottom");
    l.pop_front();
    assertEquals(3,l.front(),"l third bottom");
    l.pop_front();
    assertEquals(13,l.front(),"l last");
    
  }

  void listAppendTest() throw (ExceptionBase){

    Stack<int> mystack;
    list<int> l;
    
    Stack<int>::const_iterator bottom = mystack.topIter();    

    l.push_front(0);
    l.push_front(13);

    mystack.push(1);
    mystack.push(2);
    mystack.push(3);
    
    Stack<int>::const_iterator it = mystack.topIter();
    
    mystack.push(4);
    mystack.push(5);

    mystack.popToListAppend(l,it);

    assertEquals(3,mystack.size(),"mystack size");
    assertEquals(4,l.size(),"l size");

    assertEquals(4,l.back(),"l first");
    l.pop_back();
    assertEquals(5,l.back(),"l second");
    l.pop_back();
    assertEquals(0,l.back(),"l third");
    l.pop_back();

    mystack.popToListAppend(l,bottom);

    assertEquals(0,mystack.size(),"mystack size empty");
    assertEquals(4,l.size(),"l size bottom");

    assertEquals(1,l.back(),"l first bottom");
    l.pop_back();
    assertEquals(2,l.back(),"l second bottom");
    l.pop_back();
    assertEquals(3,l.back(),"l third bottom");
    l.pop_back();
    assertEquals(13,l.back(),"l last");
    
  }

};


#endif
