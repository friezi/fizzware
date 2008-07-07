#include "testmain.hpp"

using namespace test;
using namespace std;

static unsigned long testcaseNmb;

static FailedTestsVector *failedTests;

static unsigned long global_errors = 0;

static void statisticHelper(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){

  (*failedTests)[testcaseNmb].second = testcase;
  testcaseNmb++;

}

static void localErrorHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){

  (*failedTests)[testcaseNmb].first++;
  cerr << " ";

}

static void globalErrorHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  global_errors++;
}

static void testcaseStartupHandler(test::TestCaseBase *testcase, std::string msg) throw (Exception<test::TestCaseBase>){
  cout << "executing " << testcase->getTestcaseName() << endl;
}

static unsigned long global_success = 0;

static void globalSuccessHandler(test::TestCaseBase *testcase, std::string msg){
  global_success++;
}

FailedTestsVector * constructFailedTestsVector(){

  FailedTestsVector *failedTests = new FailedTestsVector(mainTestUnit.getNmbTestcases());
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
    mainTestUnit.setTestcaseStartupHandler(testcaseStartupHandler);
    
    mainTestUnit();
    
    unsigned long tests = mainTestUnit.getNmbTests();
    
    for ( FailedTestsVector::iterator it = failedTests->begin(); it != failedTests->end(); it++ ){
      if ( (*it).first > 0 )
	cout << endl << (*it).first << "/" << (*it).second->getNmbTests() << " tests failed in "
	     << (*it).second->getTestcaseName() << "!" << endl << endl;
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
