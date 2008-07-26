#include "main.hpp"

using namespace std;
using namespace exc;

int main(int argc, char *argv[]){
  
  long number;
  PrimList *primlist;
  
  if (argc!=2)
    goto errorexit;
  
  try{
    for (unsigned int i=0;i<strlen(argv[1]);i++)
      if (!isdigit(argv[1][i]))
	goto errorexit;
    
    number=atol(argv[1]);
    
    primlist = new PrimList(number);
    for (long i=1;number>1;){
      if (!(number%primlist->get(i))){
	cout << primlist->get(i);
	if (number/primlist->get(i)!=1)
	  cout << "*";
	number/=primlist->get(i);
      }
      else
	i++;
    }
    cout << "\n";
    
    return 0;
  }catch (ExceptionBase & e){
    e.show();
    exit(1);
  }
  
 errorexit:
  cout << "usage: " << argv[0] << " <number>\n";
  exit(1);
}



