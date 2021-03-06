#include "main.hpp"

using namespace std;
using namespace exc;

int main(int argc, char *argv[]){

  PrimList *primlist;

  if (argc!=2)
    goto errorexit;

  try{
    primlist = new PrimList(atoi(argv[1]));
    primlist->show();
   
    delete primlist;
  } catch (ExceptionBase &e){
    e.show();
  }
  return 0;
  
 errorexit:
  cout << "usage: " << argv[0] <<" <number>\n";
  exit(0);
}
