#include "msgqueueregdaemon.hpp"

using namespace std;
using namespace sys;
using namespace ds;

// Kommandostrings vom Clienten
const string CMD_ADD_MSGQUEUE = "CMD_ADD_MSGQUEUE";
const string CMD_REMOVE_MSGQUEUE = "CMD_REMOVE_MSGQUEUE";
const string CMD_OPEN_MSGQUEUE = "CMD_OPEN_MSGQUEUE";
const string CMD_CLOSE_MSGQUEUE = "CMD_CLOSE_MSGQUEUE";
const string CMD_FIND_MSGQUEUE = "CMD_FIND_MSGQUEUE";
const string OK_ADD_MSGQUEUE = "OK_ADD_MSGQUEUE";
const string OK_REMOVE_MSGQUEUE = "OK_REMOVE_MSGQUEUE";
const string OK_OPEN_MSGQUEUE = "OK_OPEN_MSGQUEUE";
const string OK_CLOSE_MSGQUEUE = "OK_CLOSE_MSGQUEUE";
const string OK_FIND_MSGQUEUE = "OK_FIND_MSGQUEUE";
const string ERROR_PORT_EXISTS = "ERROR_PORT_EXISTS";
const string ERROR_REMOVE_MSGQUEUE = "ERROR_REMOVE_MSGQUEUE";
const string ERROR_OPEN_MSGQUEUE = "ERROR_OPEN_MSGQUEUE";
const string ERROR_CLOSE_MSGQUEUE = "ERROR_CLOSE_MSGQUEUE";
const string ERROR_PORT_NOTSUBSCRIBED = "ERROR_PORT_NOTSUBSCRIBED";
const string ERROR_QUEUE_NOTEXISTS = "ERROR_QUEUE_NOTEXISTS";
const string ERROR_MSGPD_TIMEOUT = "ERROR_MSGPD_TIMEOUT";

unsigned int MsgQueueRegDaemon::timeout = 2;

unsigned int MsgQueueRegDaemon::status = 0;

void MsgQueueRegDaemon::pipehandler(int signr){
}

void MsgQueueRegDaemon::alarmhandler(int signr){
  MsgQueueRegDaemon::status |= STDMSGS_TIMEOUT;
}

unsigned int MsgQueueRegDaemon::parse(int fd){

  Scanner scanner(fd);
  Token *token = NULL;
  MsgQueueRegCommand *command = NULL;
  SignalHandler sigpipe(SIGPIPE,MsgQueueRegDaemon::pipehandler);
  SignalHandler sigalarm(SIGALRM,MsgQueueRegDaemon::alarmhandler);
  MessageQueue messagequeue;
  Message *recvmsg = NULL;

  try{  
    
    sigpipe.init(); // damit write() den Prozess nicht beendet, falls die Verbindung unterbrochen wurde
    sigalarm.init(); // normale Beendigung nach timeout
    messagequeue.init();
    
    alarm(MsgQueueRegDaemon::timeout); // nach timeout wird prozess beendet
    
    MsgQueueRegDaemon::status = 0;
    command = NULL;
    
    // Daten scannen
    token = scanner.nextToken();
    
    if (token->getType() == EOS_TOKEN)
      return MsgQueueRegDaemon::status;
 
    // ist überhaupt was angekommen?
    if (token->size()) {
      
      // Kommandos überprüfen
      if (*token == CMD_ADD_MSGQUEUE) {
	// port hinzufügen

	Token *portname;
	ULongToken *portID;
	
	if ((portname = scanner.nextToken())->getType() == EOS_TOKEN)
	  return MsgQueueRegDaemon::status;
	
	if ((portID = scanner.nextULongToken())->getType() == EOS_TOKEN){
	  log(LOG_DEBUG,"No Value given for Port!");
	  return MsgQueueRegDaemon::status;
	}
	
	command = new AddCommand(portname->get(),(int)portID->getValue());
	
	delete portID;
	delete portname;

      } else if (*token == CMD_REMOVE_MSGQUEUE) {
	// Port entfernen
	
	Token *portname;
	
	if ((portname = scanner.nextToken())->getType() == EOS_TOKEN)
	  return MsgQueueRegDaemon::status;
	
	command = new RemoveCommand(portname->get());
	
	delete portname;

      } else if (*token == CMD_FIND_MSGQUEUE) {
	// port finden
	
	Token *portname;
	
	if ((portname = scanner.nextToken())->getType() == EOS_TOKEN)
	  return MsgQueueRegDaemon::status;
	
	command = new FindCommand(portname->get());
	
	delete portname;
      }
      
      if (command) {
	
	// Message bauen
	MsgQuMessage msg(messagequeue.getId(),command->getCommandline(),command->size());
	// Kommando senden
	msg.send(getMsgQuId());
	delete command;
	
      // Rückgabe-Message vom Server
	recvmsg = messagequeue.receive();
	if (loglvl == LOG_DEBUG)
	  cout << "returnmessage: " << recvmsg->getData() << " size: " << recvmsg->size() << "\n";
	
	// an Client weiterleiten
	write(fd,recvmsg->getData(),recvmsg->size());
	delete recvmsg;
      }
    }
    delete token;
    
  } catch(Exception_T &e) {
    log(LOG_DEBUG,e.getIdMsgCharPtr());
  }
  
  if (MsgQueueRegDaemon::status & STDMSGS_TIMEOUT)
    write(fd,ERROR_MSGPD_TIMEOUT.data(),ERROR_MSGPD_TIMEOUT.size());
    
  return MsgQueueRegDaemon::status;
  
}

void MsgQueueRegDaemon::log(const int &loglevel, const char *text){

  if (loglevel <= loglvl){
    if (daemon == true)
      syslog(loglevel,text);
    else
      cout << text << endl;
  }
}

void MsgQueueRegDaemon::handleMessage(Message *message){

  CommandMsgScanner messagescanner(*message);

  if (isCommand(messagescanner.getCommand())) {
 
    log(LOG_DEBUG,"got command!");
   
    Token *porttoken = messagescanner.nextToken();
    
    if (messagescanner.getCommand() == CMD_ADD) {

      // MsgQueue-ID auslesen
      Token *idtoken = messagescanner.nextToken();
      unsigned long id = idtoken->toLong();

      // nur Eintragen, falls message-queue auch existiert
      if ( MessageQueue::exists(id) == true ) {

	if (messagequeuelist.find(porttoken->get()) == -1) {
	  // Queue noch nicht vorhanden

	  // MsgQueue hinzufügen
	  messagequeuelist.insert(porttoken->get(),id);

	  // Antwort an Handler zurückliefern
	  Message returnmsg(OK_ADD_MSGQUEUE.data(),OK_ADD_MSGQUEUE.size());
	  returnmsg.send(messagescanner.getMsgQuId());

	} else { // Queue schon eingetragen

	  // Antwort an Handler zurückliefern
	  Message returnmsg(ERROR_PORT_EXISTS.data(),ERROR_PORT_EXISTS.size());
	  returnmsg.send(messagescanner.getMsgQuId());

	}
      } else {

	Message returnmsg(ERROR_QUEUE_NOTEXISTS.data(),ERROR_QUEUE_NOTEXISTS.size());
	returnmsg.send(messagescanner.getMsgQuId());

      }

      delete idtoken;

    } else if (messagescanner.getCommand() == CMD_FIND) {

      int id;

      if ((id = messagequeuelist.find(porttoken->get())) == -1) {
	// zu findender Port existiert nicht

	Message returnmsg(ERROR_PORT_NOTSUBSCRIBED.data(),ERROR_PORT_NOTSUBSCRIBED.size());
	returnmsg.send(messagescanner.getMsgQuId());

      } else {
	// Port existiert
	
	FindMessage returnmsg(htonl((unsigned long)id),OK_FIND_MSGQUEUE.data(),OK_FIND_MSGQUEUE.size());
	returnmsg.send(messagescanner.getMsgQuId());
	
      }
    } else if (messagescanner.getCommand() == CMD_REMOVE) {

      int id;

      if ((id = messagequeuelist.find(porttoken->get())) == -1) {
	// zu löschender Port existiert nicht

	Message returnmsg(ERROR_PORT_NOTSUBSCRIBED.data(),ERROR_PORT_NOTSUBSCRIBED.size());
	returnmsg.send(messagescanner.getMsgQuId());

      } else {
	// Port existiert

	messagequeuelist.erase(porttoken->get());
	
	Message returnmsg(OK_REMOVE_MSGQUEUE.data(),OK_REMOVE_MSGQUEUE.size());
	returnmsg.send(messagescanner.getMsgQuId());

      }
    }
    
    delete porttoken;
    
  } else {
    
    log(LOG_DEBUG,"got message!");
    log(LOG_DEBUG,message->getData());
    
    string text = "MSG_OK";
    Message returnmsg(text.data(),text.size());
    
    returnmsg.send(messagescanner.getMsgQuId());
  }
}

MsgQueueRegCommand::~MsgQueueRegCommand(){
  
  if (commandline)
    free(commandline);
  commandline = 0;
}

size_t MsgQueueRegCommand::size(){
  
  return sizeof(getCommand()) + sizeof(size_t) + getPortname().size();
}

void MsgQueueRegCommand::printCommandline(){
  
  for (unsigned int i = 0; i < size(); i++)
    cout << getCommandline()[i];
  cout << "\n";
}

AddCommand::AddCommand(string portname, unsigned long id) : MsgQueueRegCommand(CMD_ADD,portname), id(id){
  
  commandline = buildCommandline();
}

size_t AddCommand::size(){
  
  return sizeof(getCommand()) + sizeof(size_t) + getPortname().size() + sizeof(size_t) + sizeof(getId());
}

char *AddCommand::buildCommandline() throw(OutOfMemException){
  
  char *array;
  size_t portnamesize = getPortname().size();
  size_t portidsize = sizeof(getId());

  if (!(array = (char *)calloc(1,size())))
    throw OutOfMemException();
 
  // 1 Byte Kommando  
  memcpy(array,(char *)&getCommand(),sizeof(getCommand()));
  // size_t  Laenge des Portnamen
  memcpy(array+sizeof(getCommand()),&portnamesize,sizeof(size_t));
  // Portname
  memcpy(array+sizeof(getCommand())+sizeof(size_t),getPortname().data(),portnamesize);
  // size_t  Laenge der PortID
  memcpy(array+sizeof(getCommand())+sizeof(size_t)+portnamesize,&portidsize,sizeof(size_t));
  // Port-ID
  memcpy(array+sizeof(getCommand())+sizeof(size_t)+portnamesize+sizeof(size_t),(char *)&getId(),sizeof(getId()));
  
  return array;
}

OneArgCommand::OneArgCommand(unsigned short cmd, string portname) : MsgQueueRegCommand(cmd,portname){
  
  commandline = buildCommandline();
}

char *OneArgCommand::buildCommandline() throw(OutOfMemException){
  
  char *array;
  size_t portnamesize = getPortname().size();

  if (!(array = (char *)calloc(1,size())))
    throw OutOfMemException();
  
  // 1 Byte Kommando  
  memcpy(array,(char *)&getCommand(),sizeof(getCommand()));
  // size_t  Laenge des Portnamen
  memcpy(array+sizeof(getCommand()),&portnamesize,sizeof(size_t));
  // Portname
  memcpy(array+sizeof(getCommand())+sizeof(size_t),getPortname().data(),portnamesize);
  
  return array;
}

int CommandMsgScanner::getMsgQuId(){

  int id;

  memcpy((char *)&id,message->getData(),sizeof(int));

  return id;
}

unsigned short CommandMsgScanner::getCommand(){
  
  unsigned short command;
  
  memcpy((char *)&command,message->getData()+sizeof(int),sizeof(unsigned short));
  
  return command;
}

size_t CommandMsgScanner::shiftTokenSize(){

  size_t s = 0;

  memcpy((char *)&s,&(message->getData()[count]),sizeof(size_t));
  count+= sizeof(size_t);

  return s;
}

Token *CommandMsgScanner::nextToken(){
  
  Buffer<char> buffer(TOKENSIZE);
  MemPointer<char> input(false);
  unsigned int sz;
  // Grössenangabe shiften und Grösse zurückliefern
  size_t tsize = shiftTokenSize();
  
  for (size_t i = 0; i < tsize; i++) {
    
    if (count >= message->size())
      break;
    
    buffer.put(message->getData()[count]);
    count++;

  }
  
  input = buffer.merge();
  sz = buffer.elements();

  return new Token(input.get(),sz);
}

MQMBody::MQMBody(int mqid, const char * const rest, size_t rsize) throw(OutOfMemException){

  if (!(body = (char *)calloc(1,rsize+sizeof(int))))
    throw OutOfMemException();

  // ID der MessageQueue für Rückantwort eintragen
  memcpy(body,(char *)&mqid,sizeof(int));
  // Kommandostring
  memcpy(body+sizeof(int),rest,rsize);
  bsize = rsize+sizeof(int);
}

MQMBody::~MQMBody(){

  if (body)
    free(body);
}

MsgQuMessage::MsgQuMessage(int mqid, const char * const buffer, size_t msize, long type) throw(Exception_T){

  this->msg = 0;

  MQMBody msgbody(mqid,buffer,msize);

  this->msize = msgbody.size();

  init(msgbody.get(),type);
}

FMBody::FMBody(unsigned long id, const char * const start, size_t ssize) throw(OutOfMemException){

  bsize = ssize+sizeof(id)+1;

  if (!(body = (char *)calloc(1,bsize)))
    throw OutOfMemException();

  // Kommandostring
  memcpy(body,start,ssize);
  // ID der MessageQueue
  memcpy(body+ssize+1,(char *)&id,sizeof(id));
}

FMBody::~FMBody(){

  if (body)
    free(body);
}

FindMessage::FindMessage(unsigned long id, const char * const buffer, size_t ssize, long type) throw(Exception_T){

  this->msg = 0;

  FMBody fmbody(id,buffer,ssize);

  this->msize = fmbody.size();

  init(fmbody.get(),type);
}
