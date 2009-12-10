#ifndef FZTESTENV_TESTMAIN_HPP
#define FZTESTENV_TESTMAIN_HPP

#include <vector>
#include <utility>
#include <fztooltempl/test.hpp>
#include <fztooltempl/exception.hpp>

typedef std::vector< std::pair<unsigned long, test::TestCaseBase *> > FailedTestsVector;

extern test::TestUnit mainTestUnit;

extern void initMainTestUnit();

#endif
