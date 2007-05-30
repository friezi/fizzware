#include "main.hpp"

#define SEMKEY 100

using namespace std;
using namespace sys;

class MyProcess : public Process{
  
private:
  
  int main(){
    
    cout << "this is a process: pid=" << this->getPid()
	 << "  ppid=" << this->getPpid() << "\n";

    for (int i=0;;i++){
      sleep(1);
      cout << i << "\n";}
    
    
    return 0;
  }
  
public:
  
  MyProcess(bool p) : Process(p){}
};

void sig_term(int sig){
    
    cout << "exception! " << 0 << "\n";
    //     abort=1;
  }
  
class GvProcess : public Process{

 private:
  
  int check;

  int abort;
  
  int main(){

    Semaphor s(1,SEMKEY,Semaphor::S_OLD);

//     signal(SIGTERM,(void (*)(int))sig_term);

//     if (getPnmb()==51)
//       throw ProcessRuntimeException(getPnmb());
//     execlp("gv","gv",NULL);

    s.P();

    cout << "process: " << getPnmb() << "\n";
    
    sleep(2);

    s.V();
   
    sleep(10);
    cout << "beende process " << getPnmb() << "\n";
    return 0;
  }

public:

  GvProcess(bool daemon=false) : Process(daemon),check(0){
  abort=0;
  }
};


class Mysem : public Semaphor{

public:


  Mysem(int initval, key_t key=IPC_PRIVATE, char flag=S_NEW)
    : Semaphor(initval,key,flag){}
  ~Mysem(){}
};

int main(){
 
  try{
    
    const int PROCS=50;

    Semaphor sem(2,SEMKEY);
    MyProcess myproc(false);
    GvProcess *gvprocess[PROCS] ;
  
    cout << "Elternprocess\n";
    

    myproc.launch();
    cout << "child gestartet\n";
    for (int i=0;i<PROCS;i++){
      gvprocess[i] = new GvProcess();
      gvprocess[i]->launch();
    }
    sleep(8);

    myproc.terminate();
    myproc.wait();

//     sleep(5);


    
    cout << "child beendet\n";

    sem.block();
    
    cout << "sem blockiert\n";

//     sem.remove();

    for (int i=0;i<PROCS;i++){
      delete gvprocess[i];
//       if (!(i%5)){
// 	sleep(1);
// 	cout << "\n\n";
//       }
    }


   } catch (ExceptionBase &e){
    e.show();
  }
  
  
  return 0;
}
