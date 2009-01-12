#include "maintest.hpp"

using namespace test;
using namespace std;

TestUnit mainTestUnit;

void initMainTestUnit(){

  mainTestUnit.addTestCase(new RingbufferTest(),"RingbufferTest()");

}
