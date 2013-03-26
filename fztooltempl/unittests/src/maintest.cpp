#include "maintest.hpp"

using namespace std;
using namespace exc;
using namespace utils;
using namespace test;

TestUnit * createMainTestUnit(){

  TestUnit *mainTestUnit = new TestUnit();

  mainTestUnit->addTestCase(new ObserverTest(),"ObserverTest");
  mainTestUnit->addTestCase(new RingbufferTest(),"RingbufferTest");
  mainTestUnit->addTestCase(new UtilsTest(),"UtilsTest");

  return mainTestUnit;

}
