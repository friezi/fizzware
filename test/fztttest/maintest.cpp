#include "maintest.hpp"

TestUnit mainTestUnit;

void initMainTestUnit(){

  mainTestUnit.addTestCase(new StackTest(),"StackTest");

  mainTestUnit.showTests();

}
