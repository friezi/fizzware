#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <utils.hpp>

class ValueObserver : public utils::Observer<int>{

public:

  void update(utils::Observable<int> &observable, int value);

};

#endif
