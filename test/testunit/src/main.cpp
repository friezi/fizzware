#include "main.hpp"

using namespace test;

int main(){

  TestUnit testunit;

  testunit.addTestcase(new MyTestCase1());
  testunit.addTestcase(new MyTestCase2());

  testunit.run();
  
  return 0;

}
