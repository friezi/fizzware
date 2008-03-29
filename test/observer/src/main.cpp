#include "main.hpp"

using namespace std;
using namespace utils;

void ValueObserver::update(Observable<int> *observable, int value){

  cout << this << ": " << value << endl;

}

int main(int argc, char **argv){

  ChangeNotifier<int> notifier(0);
  ChangeNotifier<int> notifier2(2);
  ValueObserver o1, o2, o3;

  try {

    o1.addToNotifier(&notifier);
    o2.addToNotifier(&notifier);
    o3.addToNotifier(&notifier);

    notifier = 4;

    o2.removeFromNotifier(&notifier);

    notifier = 5;
    notifier = 5;

    notifier = 6;

    notifier = notifier2;

    notifier.removeObservers();

    notifier = 7;

  } catch (ExceptionBase &e){

    e.show();

  }

  return 0;

}

