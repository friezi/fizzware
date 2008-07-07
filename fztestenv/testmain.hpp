#ifndef __TESTMAIN_HPP__
#define __TESTMAIN_HPP__

#include <vector>
#include <utility>
#include <test.hpp>
#include <exception.hpp>

typedef std::vector< std::pair<unsigned long, test::TestCaseBase *> > FailedTestsVector;

extern test::TestUnit mainTestUnit;

extern void initMainTestUnit();

#endif
