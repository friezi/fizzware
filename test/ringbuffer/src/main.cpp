#include "main.hpp"

using namespace std;
using namespace exc;
using namespace ds;

int main(int argc, char **argv){
  
  try{

    RingBuffer<int> buffer(10,RingBuffer<int>::QUIET_OVERWRITING);

    cout << "quiet mode:" << endl;
    
    for ( int i = 1; i <= 37; i++)
      buffer.enqueue(i);

    for (int i = 0; i < 10; i++)
      cout << buffer[i] << " ";
    cout << endl;

    buffer[9] = 1;

    for (int i = 0; i < 10; i++)
      cout << buffer[i] << " ";
    cout << endl;

    RingBuffer<int> buffer2(10);

    cout << "alerting mode:" << endl;
    
    for ( int i = 1; i <= 10; i++)
      buffer2.enqueue(i);

    for (int i = 0; i < 10; i++)
      cout << buffer2[i] << " ";
    cout << endl;

    cout << "first: " << buffer2.dequeue() << endl;

    for (size_t i = 0; i < buffer2.getElements(); i++)
      cout << buffer2[i] << " ";
    cout << endl;

    cout << "skip 2" << endl;

    buffer2.skip(2);

    for (size_t i = 0; i < buffer2.getElements(); i++)
      cout << buffer2[i] << " ";
    cout << endl;

    cout << "insert 3: " << endl;

    for (size_t i = 0; i < 3; i++)
      buffer2.enqueue(i);

    for (size_t i = 0; i < buffer2.getElements(); i++)
      cout << buffer2[i] << " ";
    cout << endl;

    buffer2.enqueue(1);
    
    return 0;

  } catch (ExceptionBase &e){
    
    e.show();

  }

}
