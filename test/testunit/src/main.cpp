#include "main.hpp"

using namespace test;

int main(){

  try{

    TestUnit testunit;

    testunit.addTestcase(new MyTestCase1());
    testunit.addTestcase(new MyTestCase2());

    testunit();

  }catch (ExceptionBase &e){
    e.show();
  }
  
  return 0;

}
