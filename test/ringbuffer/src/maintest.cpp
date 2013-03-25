#include "maintest.hpp"

using namespace test;
using namespace std;

TestUnit * createMainTestUnit(){
  
  TestUnit *mainTestUnit = new TestUnit();
  
  mainTestUnit->addTestCase(new RingbufferTest(),"RingbufferTest()");
  
  return mainTestUnit;
  
}
