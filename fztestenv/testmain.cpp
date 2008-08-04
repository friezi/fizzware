#include "testmain.hpp"

using namespace test;
using namespace std;
using namespace exc;

static unsigned long testcaseNmb;

static FailedTestsVector *failedTests;

static unsigned long global_errors = 0;

static void statisticHelper(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){

  (*failedTests)[testcaseNmb].second = testcase;
  testcaseNmb++;

}

static void localErrorHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){

  (*failedTests)[testcaseNmb].first++;
  cerr << " ";

}

static void globalErrorHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  global_errors++;
}

static void testcaseStartupHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  cout << "executing " << testcase->getTestCaseName() << endl;
}

static unsigned long global_success = 0;

static void globalSuccessHandler(TestCaseBase *testcase, string msg){
  global_success++;
}

FailedTestsVector * constructFailedTestsVector(){

  FailedTestsVector *failedTests = new FailedTestsVector(mainTestUnit.getNmbTestCases());
  for ( FailedTestsVector::iterator it = failedTests->begin(); it != failedTests->end(); it++ )
    (*it) = pair<unsigned long,TestCaseBase *>(0,0);

  return failedTests;

}

int main(){

  testcaseNmb = 0;
  failedTests = 0;

  try{

    initMainTestUnit();

    failedTests = constructFailedTestsVector();
    mainTestUnit.pushErrorHandler(localErrorHandler);
    mainTestUnit.pushErrorHandler(globalErrorHandler);
    mainTestUnit.pushStatisticHelper(statisticHelper);
    mainTestUnit.pushSuccessHandler(globalSuccessHandler);
    mainTestUnit.setTestCaseStartupHandler(testcaseStartupHandler);
    
    mainTestUnit();
    
    unsigned long tests = mainTestUnit.getNmbTests();
    
    cout << endl;

    for ( FailedTestsVector::iterator it = failedTests->begin(); it != failedTests->end(); it++ ){
      if ( (*it).first > 0 )
	cout << (*it).first << "/" << (*it).second->getNmbTests() << " tests failed in "
	     << (*it).second->getTestCaseName() << "!" << endl;
    }
    
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
