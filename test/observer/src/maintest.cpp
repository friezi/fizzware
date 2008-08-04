#include "maintest.hpp"

int ObserverTest::testval1 = 0;
int ObserverTest::testval2 = 0;
int ObserverTest::testval3 = 0;

TestUnit mainTestUnit;

void initMainTestUnit(){

  mainTestUnit.addTestCase(new ObserverTest(),"ObserverTest");

}
