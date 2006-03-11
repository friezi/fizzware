#include "main.hpp"

using namespace sys;

int main(){

  try{
    Semaphor sem1(1,1001),sem2(6,1002);
    
    sem1.P();
    sem2.P();

    sleep(5);

    sem2.V();
    sem1.V();

  } catch (Exception_T &e){
    e.show();
  }
    
  return 0;
}
