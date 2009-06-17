#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <set>
#include <fztooltempl/functions.hpp>
#include <fztooltempl/exception.hpp>
#include <typeinfo>

class Test{
};

class TestDieZweite : public Test {
};

template <typename T> class Set : public std::set<T,std::less<T> >{
};

typedef std::set<int,std::less<int> > IntSet;


#endif
