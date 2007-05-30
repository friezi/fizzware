#include "main.hpp"

using namespace std;

int main(int argc, char **argv){

  Automat *automat;
//   SpecSymbols spec_symbols('*','?','[',']','!','\\');

  if (argc<3){
    cout << "usage: " << argv[0] << " <pattern> <string> [<string> ...]\n";
    exit(1);
  }

  try{

    if (!(automat = new Automat(argv[1]))){
      cout << "konnte keinen Automaten bauen!\n";
      throw OutOfMemException();
    }

    cout << "pattern: " << argv[1] << "\n";
    
    automat->show();
    
    for (int n=2;n<argc;n++){
      
      if (automat->accept(argv[n]))
	cout << argv[n] << " wurde akzeptiert!\n";
      else
	cout << argv[n] << " wurde nicht akzeptiert!\n";
      
    }
    
    delete automat;
    
  } catch (ExceptionBase &e){
    e.show();
  }

  return 0;
}
