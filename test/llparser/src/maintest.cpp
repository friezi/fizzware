#include "maintest.hpp"

TestUnit mainTestUnit;

void initMainTestUnit(){

  mainTestUnit.addTestCase(new LLTest(),"LLTest");
  mainTestUnit.showTests();

}
