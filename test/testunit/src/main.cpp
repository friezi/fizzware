#include "main.hpp"

using namespace exc;
using namespace test;
using namespace std;

static unsigned long testcaseNmb;

static vector<unsigned long> *failedTests;

static unsigned long global_errors = 0;

static void statisticHelper(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){

  if ( (*failedTests)[testcaseNmb] > 0 )
    std::cout << endl << (*failedTests)[testcaseNmb] << "/" << testcase->getNmbTests() << " tests failed in "
	      << testcase->getTestCaseName() << "!" << endl << std::endl;
  testcaseNmb++;

}

static void localErrorHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  (*failedTests)[testcaseNmb]++;
  cout << " ";
}

static void globalErrorHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  global_errors++;
}

static void testcaseStartupHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  cout << "executing " << testcase->getTestCaseName() << endl;
}

static unsigned long global_success = 0;

static void globalSuccessHandler(test::TestCaseBase *testcase, std::string msg){
  global_success++;
}

int main(){

  testcaseNmb = 0;
  failedTests = 0;

  try{

    initMainTestUnit();

    failedTests = new vector<unsigned long>(mainTestUnit.getNmbTestCases());
    for ( vector<unsigned long>::iterator it = failedTests->begin(); it != failedTests->end(); it++ )
      (*it) = 0;

    mainTestUnit.pushErrorHandler(localErrorHandler);
    mainTestUnit.pushErrorHandler(globalErrorHandler);
    mainTestUnit.pushStatisticHelper(statisticHelper);
    mainTestUnit.pushSuccessHandler(globalSuccessHandler);
    mainTestUnit.setTestCaseStartupHandler(testcaseStartupHandler);
    
    mainTestUnit();
    
    unsigned long tests = mainTestUnit.getNmbTests();
    cout << endl;
    cout << "Success: " << global_success << "/" << tests << endl;
    if ( global_errors > 0 )
      cout << "Failure: " << global_errors << "/" << tests << endl;

  }catch (ExceptionBase &e){
    e.show();
  }

  if ( failedTests != 0 )
    delete failedTests;
  
  return 0;

}
