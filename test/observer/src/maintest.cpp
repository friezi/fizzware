#include "maintest.hpp"

int ObserverTest::testval1 = 0;
int ObserverTest::testval2 = 0;
int ObserverTest::testval3 = 0;

TestUnit * createMainTestUnit(){

  TestUnit *mainTestUnit = new TestUnit();

  mainTestUnit->addTestCase(new ObserverTest(),"ObserverTest");

  return mainTestUnit;

}
