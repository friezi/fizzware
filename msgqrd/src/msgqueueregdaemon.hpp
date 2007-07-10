#ifndef MSGQUEUEREGDAEMON_HPP
#define MSGQUEUEREGDAEMON_HPP

#include <iostream>
#include <string>
#include <syslog.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <exception.hpp>
#include <signalhandler.hpp>
#include <stdmessageserver.hpp>
#include <datastructures.hpp>
#include <messagequeue.hpp>
#include "msgqueuelist.hpp"
#include "scanner.hpp"

#define MSGQRD_STOP 1L<<0
#define MSGQRD_QUIT 1L<<1
#define MSGQRD_ERROR 1L<<2

#define TOKENSIZE 20

// Kommandos an Server-MessageQueue
#define CMD_NONE 0
#define CMD_ADD 1
#define CMD_REMOVE 2
#define CMD_OPEN 3
#define CMD_CLOSE 4
#define CMD_FIND 5
#define ADD_OK 6
#define REMOVE_OK 7
#define OPEN_OK 8
#define CLOSE_OK 9
#define FIND_OK 10
#define ADD_ERROR 11
#define REMOVE_ERROR 12
#define OPEN_ERROR 13
#define CLOSE_ERROR 14
#define FIND_ERROR 15
#define NMB_CMDS 15

class MsgQueueRegDaemon : public sys::StdMessageServer{
  
private:

  MsgQueueList messagequeuelist;

  static unsigned int timeout;

  // Nur von eigenen Prozessen aus zugreifen, sonst
  // könnte es Probleme geben
  static unsigned int status;  // Beendigungsstatus

  static void pipehandler(int signr);
  static void alarmhandler(int signr);

  bool daemon;  
  int loglvl;

public:

  MsgQueueRegDaemon(bool daemon, unsigned short int port, const std::string name, int loglvl = LOG_NOTICE)
    : StdMessageServer(daemon,port,name,true), daemon(daemon),loglvl(loglvl){}

  MsgQueueRegDaemon(bool daemon,const std::string stream_path, const std::string name, int loglvl = LOG_NOTICE)
    : StdMessageServer(daemon,stream_path,name,true), daemon(daemon),loglvl(loglvl){}
  
  void log(const int &loglevel, const char *text);
  
  unsigned int parse(int fd);
  
  void handleMessage(sys::Message *message);
  
  bool isCommand(unsigned short command) { return ((command > CMD_NONE) && (command < NMB_CMDS)); }

};

// Die verschiedenen Kommandos an den Server
class MsgQueueRegCommand{
  
private:
  
  unsigned short command;
  std::string portname;
  
  // Zusammensetzen der Kommandozeile: Kommando, Laenge des Folgestrings, Folgestring ...
  virtual char *buildCommandline() throw(OutOfMemException) { return NULL; }

protected:

  // falls commandline != 0 wird der Speicher automatisch
  // durch den Destruktor freigegeben
  char *commandline;
  
  MsgQueueRegCommand(MsgQueueRegCommand &){}
  
public:
  
  MsgQueueRegCommand(unsigned short command, std::string portname): command(command), portname(portname), commandline(0){}
  
  unsigned short &getCommand() { return command; }
  
  std::string getPortname() { return portname; }

  char * const getCommandline() { return commandline; }

  void printCommandline();
  
  virtual ~MsgQueueRegCommand();

  virtual size_t size();
};

class AddCommand : public MsgQueueRegCommand{
  
private:

  // die ID der MessageQueue, an die das Kommando gesendet wird
  unsigned long id;

  // liefert einen NICHT 0-terminiertes char-array, welches beinhaltet:
  // getCommand() + getPortname().size() + 1 + getId()
  char *buildCommandline() throw(OutOfMemException);

protected:

  AddCommand(AddCommand &c) : MsgQueueRegCommand(c){}

public:

  AddCommand(std::string portname, unsigned long id);

  unsigned long &getId() { return id; }

  size_t size();

};

class OneArgCommand : public MsgQueueRegCommand{

private:

  // liefert einen NICHT 0-terminiertes char-array, welches beinhaltet:
  // getCommand() + getPortname().size()
  char *buildCommandline() throw(OutOfMemException);

protected:

  OneArgCommand(OneArgCommand &c) : MsgQueueRegCommand(c){}

public:

  OneArgCommand(unsigned short cmd, std::string portname);
};

class RemoveCommand : public OneArgCommand{

protected:

  RemoveCommand(RemoveCommand &c) : OneArgCommand(c){}

public:

  RemoveCommand(std::string portname) : OneArgCommand(CMD_REMOVE,portname){}
};

class FindCommand : public OneArgCommand{

protected:

  FindCommand(RemoveCommand &c) : OneArgCommand(c){}

public:

  FindCommand(std::string portname) : OneArgCommand(CMD_FIND,portname){}
};

// Scanner für die Kommandomessages
class CommandMsgScanner{
  
private:
  
  const sys::Message * message; 
  size_t count;
  
protected:
  
  CommandMsgScanner(const CommandMsgScanner & cms){
    
    this->message = cms.message;
    this->count = cms.count;
}

  // zurückliefern der Länge des nächsten Blocks in der Message
  size_t shiftTokenSize();
  
public:

  // die ersten Einträge einer Message sind: Antwort-MessageQueueID:int und Kommando 
  CommandMsgScanner(sys::Message &message) : message(&message), count(sizeof(int) + sizeof(unsigned short)){} 

  // zurückliefern der Antwort-MessagequeueID
  int getMsgQuId();

  // liefert das Kommando, welches an erster Stelle steht (nach ID und MsgQuID in der Message): ist unsigned short
  unsigned short getCommand();

  // liefert das nächste Token (ohne Command)
  Token *nextToken();

};

// Message-Body für MsgQuMessages: MessageQueue-id, Kommando
class MQMBody{

private:
  
  char *body;
  size_t bsize;

protected:
  
  MQMBody(const MQMBody &){}

public:

  MQMBody(int mqid, const char * const rest, size_t rsize) throw(OutOfMemException);

  ~MQMBody();

  char *get() { return body; }
  size_t size() { return bsize; }
};

// Message, die mit MessageQueue für Rückantwort versehen wird
class MsgQuMessage : public sys::Message{

protected:
  
  MsgQuMessage(const MsgQuMessage &){}

public:

  MsgQuMessage(int mqid, const char * const buffer, size_t msize, long type = getpid()) throw(ExceptionBase);
};

class FMBody{

private:
  
  char *body;
  size_t bsize;

protected:
  
  FMBody(const FMBody &){}

public:

  FMBody(unsigned long id, const char * const start, size_t ssize) throw(OutOfMemException);

  ~FMBody();

  char *get() { return body; }
  size_t size() { return bsize; }
};


class FindMessage : public sys::Message{

protected:

  FindMessage(const FindMessage &){}

public:

  FindMessage(unsigned long id, const char * const buffer, size_t ssize, long type = getpid()) throw(ExceptionBase);
};

#endif
