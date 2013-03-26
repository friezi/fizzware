#include "testmain.hpp"

using namespace test;
using namespace std;
using namespace exc;

static unsigned long testcaseNmb;

static TestResults *testresults;

static unsigned long global_success = 0;
static unsigned long global_errors = 0;

static void statisticHelper(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){

  (*testresults)[testcaseNmb].setTestCase(testcase);
  cout << " success: " << (*testresults)[testcaseNmb].getSuccess() << " failure: " << (*testresults)[testcaseNmb].getFailure() << endl;
  testcaseNmb++;

}

static void localSuccessHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  (*testresults)[testcaseNmb].incSuccess();
}

static void localErrorHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){

  (*testresults)[testcaseNmb].incFailure();
  cerr << " " << msg << endl;

}

static void globalErrorHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  global_errors++;
}

static void globalSuccessHandler(TestCaseBase *testcase, string msg){
  global_success++;
}

static void testcaseStartHandler(TestCaseBase *testcase, string msg) throw (Exception<TestCaseBase>){
  cout << "testcase: " << testcase->getTestCaseName() << endl;
}

TestResults * constructTestResults(TestUnit *mainTestUnit){

  TestResults *testresults = new TestResults(mainTestUnit->getNmbTestCases());
  for ( TestResults::iterator it = testresults->begin(); it != testresults->end(); it++ ){
    (*it) = TestCaseResult();
  }
  
  return testresults;

}

int main(){

  testcaseNmb = 0;
  testresults = 0;

  TestUnit *mainTestUnit = 0;

  try{

    mainTestUnit = createMainTestUnit();

    testresults = constructTestResults(mainTestUnit);
    mainTestUnit->pushErrorHandler(localErrorHandler);
    mainTestUnit->pushErrorHandler(globalErrorHandler);
    mainTestUnit->pushTestCaseStartHandler(testcaseStartHandler);
    mainTestUnit->pushTestCaseEndHandler(statisticHelper);
    mainTestUnit->pushSuccessHandler(localSuccessHandler);
    mainTestUnit->pushSuccessHandler(globalSuccessHandler);
    
    cout << "executing unittests" << endl;
    
    (*mainTestUnit)();
    
    unsigned long tests = mainTestUnit->getNmbTests();
    
    cout << endl;

    for ( TestResults::iterator it = testresults->begin(); it != testresults->end(); it++ ){
      if ( (*it).getFailure() > 0 ){
	cout << (*it).getFailure() << "/" << (*it).getTestCase()->getNmbTests() << " tests failed in "
	     << (*it).getTestCase()->getTestCaseName() << "!" << endl;
      }
    }
    
    cout << "total: " << tests << " success: " << global_success << " failures: " << global_errors << endl;
    
  }catch (ExceptionBase &e){
    e.show();
  }

  if ( testresults != 0 ){
    delete testresults;
  }

  if ( mainTestUnit != 0 ){
    delete mainTestUnit;
  }

  return ( global_errors > 0 ? 1 : 0 );

}
