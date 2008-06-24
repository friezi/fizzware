#include "main.hpp"

using namespace test;
using namespace std;

int main(){

  try{

    TestUnit testunit;

    testunit.addTestcase(new MyTestCase1());
    testunit.addTestcase(new MyTestCase2());
    testunit.pushErrorHandler(globalErrorHandler);
    testunit.pushSuccessHandler(globalSuccessHandler);

    testunit();

    unsigned long tests = testunit.getNmbTests();
    cout << global_errors << "/" << tests << " failed global" << endl;
    cout << global_success << "/" << tests << " succeeded global" << endl;

  }catch (ExceptionBase &e){
    e.show();
  }
  
  return 0;

}
