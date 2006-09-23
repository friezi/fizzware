#include "main.hpp"

using namespace std;
using namespace ds;

int main(int argc, char ** argv){

  try{

  BitMatrix bitarea(3,7);

  for (int j=0;j<3;j++){
    for (int i=0;i<7;i++)
      cout << (int)bitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  bitarea.setBit(1,5);

  for (int j=0;j<3;j++){
    for (int i=0;i<7;i++)
      cout << (int)bitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  bitarea.clearBit(1,5);

  for (int j=0;j<3;j++){
    for (int i=0;i<7;i++)
      cout << (int)bitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  BitMatrix bigbitarea(21,17);

  for (int j=0;j<21;j++){
    for (int i=0;i<17;i++)
      cout << (int)bigbitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  bigbitarea.setBit(0,0);
  bigbitarea.setBit(20,16);
  bigbitarea.setBit(13,9);
  bigbitarea.setBit(5,3);

  for (int j=0;j<21;j++){
    for (int i=0;i<17;i++)
      cout << (int)bigbitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  for (int j=0;j<21;j++)
    for (int i=0;i<17;i++)
      bigbitarea.setBit(j,i);

  for (int j=0;j<21;j++){
    for (int i=0;i<17;i++)
      cout << (int)bigbitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  for (int j=0;j<21;j++)
    for (int i=0;i<17;i++)
      bigbitarea.clearBit(j,i);

  for (int j=0;j<21;j++){
    for (int i=0;i<17;i++)
      cout << (int)bigbitarea.getBit(j,i);
    cout << endl;
  }

  cout << endl;

  return 0;

  }catch (Exception_T &e){
    e.show();
  }

}
