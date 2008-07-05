#include "maintest.hpp"

using namespace test;
using namespace std;

TestUnit mainTestUnit;

void initMainTestUnit(){

  mainTestUnit.addTestcase(new MyTestCase1());
  mainTestUnit.addTestcase(new MyTestCase2());
  mainTestUnit.addTestcase(new MyTestCase3());

}
