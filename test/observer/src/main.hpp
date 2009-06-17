#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <fztooltempl/utils.hpp>

class ValueObserver : public utils::SmartObserver<int>{

public:

  void update(utils::Observable<int> *observable, int value);

};

#endif
