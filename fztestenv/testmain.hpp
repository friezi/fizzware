#ifndef FZTESTENV_TESTMAIN_HPP
#define FZTESTENV_TESTMAIN_HPP

#include <vector>
#include <utility>
#include <test.hpp>
#include <exception.hpp>

typedef std::vector< std::pair<unsigned long, test::TestCaseBase *> > FailedTestsVector;

extern test::TestUnit mainTestUnit;

extern void initMainTestUnit();

#endif
