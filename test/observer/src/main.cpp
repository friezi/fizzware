#include "main.hpp"

using namespace std;
using namespace utils;

void ValueObserver::update(Observable<int> &observable, int value){

  cout << this << ": " << value << endl;

}

int main(int argc, char **argv){

  ValueObserver o1, o2, o3;
  ChangeNotifier<int> notifier;

  notifier.addObserver(&o1);
  notifier.addObserver(&o2);
  notifier.addObserver(&o3);

  notifier.setValue(4);

  notifier.removeObserver(&o2);

  notifier.setValue(5);
  notifier.setValue(5);

  notifier.removeObservers();

  notifier.setValue(6);

  return 0;

}

