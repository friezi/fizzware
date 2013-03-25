#include "maintest.hpp"

TestUnit * createMainTestUnit(){

  TestUnit *mainTestUnit = new TestUnit();

  mainTestUnit->addTestCase(new LLTest(),"LLTest");

  return mainTestUnit;

}
