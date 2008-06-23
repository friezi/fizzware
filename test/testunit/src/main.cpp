#include "main.hpp"

using namespace test;
using namespace std;

int main(){

  try{

    TestUnit testunit;

    testunit.addTestcase(new MyTestCase1());
    testunit.addTestcase(new MyTestCase2());
    testunit.pushErrorHandler(globalErrorHandler);

    testunit();
    cout << global_nmb << "/" << testunit.getNmbTests() << " failed global" << endl;

  }catch (ExceptionBase &e){
    e.show();
  }
  
  return 0;

}
