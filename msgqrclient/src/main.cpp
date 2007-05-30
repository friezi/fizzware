#include "main.hpp"

using namespace std;
using namespace sys;

int main(int argc, char **argv){

  try{

    MsgQueueRegClient client(MsgQueueRegClient::default_socket_path);
    
    if (client.addPort("nochgu",1212))
      cout << "Queue 'nochgu' eingetragen\n";
    else
      cout << "konnte 'nochgu' nicht eintragen, existiert wohl schon\n";
    if (client.addPort("noch einer",1234))
      cout << "Queue 'noch einer' eingetragen\n";
    else
      cout << "konnte 'noch einer' nicht eintragen, existiert wohl schon\n";
    if (client.addPort("nochgu",1212))
      cout << "Queue 'nochgu' eingetragen\n";
    else
      cout << "konnte 'nochgu' nicht eintragen, existiert wohl schon\n";

    MessageQueue mq;
    mq.init();

    if (client.addPort("MessageQueue",mq.getId()))
      cout << "Queue 'MessageQueue' eingetragen\n";
    else
      cout << "konnte 'MessageQueue' nicht eintragen, existiert wohl schon\n";

    cout << "Port 'nochgu' bei " << client.findPort("nochgu") << endl;
    cout << "Port 'MessageQueue' bei " << client.findPort("MessageQueue") << endl;

    if (client.removePort("nochgu"))
      cout << "Queue 'nochgu' entfernt\n";
    else
      cout << "Konnte Queue 'nochgu' nicht entfernen, wohl nicht existent\n";
    cout << "Port 'nochgu' bei " << client.findPort("nochgu") << endl;
    if (client.removePort("nppie"))
      cout << "Queue 'nochgu' entfernt\n";
    else
      cout << "Konnte Queue 'nochgu' nicht entfernen, wohl nicht existent\n";
    if (client.removePort("MessageQueue"))
      cout << "Queue 'MessageQueue' entfernt\n";
    else
      cout << "Konnte Queue 'MessageQueue' nicht entfernen, wohl nicht existent\n";

  }catch(ExceptionBase &e){
    e.show();
  }
  
  return 0;
}
