#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <utils.hpp>

class ValueObserver : public utils::SmartObserver<int>{

public:

  void update(utils::Observable<int> *observable, int value);

};

#endif
