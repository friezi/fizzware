#include "maintest.hpp"

using namespace test;
using namespace std;

TestUnit mainTestUnit;

void initMainTestUnit(){

  mainTestUnit.addTestCase(new MyTestCase1());
  mainTestUnit.addTestCase(new MyTestCase2());
  mainTestUnit.addTestCase(new MyTestCase3());

}
