#include "main.hpp"

using namespace std;
using namespace utils;

void ValueObserver::update(Observable<int> *observable, int value){

  cout << this << ": " << value << endl;

}

int main(int argc, char **argv){

  ChangeNotifier<int> notifier;
  ValueObserver o1, o2, o3;

  try {

    o1.addToNotifier(&notifier);
    o2.addToNotifier(&notifier);
    o3.addToNotifier(&notifier);

    notifier.setValue(4);

    notifier.removeObserver(&o2);

    notifier.setValue(5);
    notifier.setValue(5);

    notifier.removeObservers();

    notifier.setValue(6);

  } catch (ExceptionBase &e){

    e.show();

  }

  return 0;

}

