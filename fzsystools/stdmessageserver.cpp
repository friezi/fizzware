/*
    Copyright (C) 1999-2004 Friedemann Zintel

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    For any questions, contact me at
    friezi@cs.tu-berlin.de
*/

#include <stdmessageserver.hpp>

using namespace std;
using namespace sys;

StdMessageServer::PIDSet StdMessageServer::clienthandlers;

StdMessageServer::StdMessageServer(bool daemon, unsigned short int port, const string procname, const bool &createpidfile) :
  mqid(0), pid(-1), port(port), stream_path(""), family(AF_INET), daemon(daemon), procname(procname),
  createpidfile(createpidfile), pidfilecreated(false){

  procpidfile = PIDPATH + procname + ".pid";
}

StdMessageServer::StdMessageServer(bool daemon, string stream_path, const string procname, const bool &createpidfile) :
  mqid(0), pid(-1), port(port), stream_path(stream_path), family(AF_LOCAL), daemon(daemon), procname(procname),
  createpidfile(createpidfile), pidfilecreated(false){

  procpidfile = PIDPATH + procname + ".pid";
}

void StdMessageServer::childhandler(int signr){
  
  int status;
  
  wait(&status);
}

void StdMessageServer::termhandler(int signr){
}

bool StdMessageServer::createPidFile(const bool &createpidfile){

  // PID-File nur erstellen, falls gewünscht
  if (createpidfile == false)
    return true;

  // falls PID-file existiert, laeuft schon ein Prozess -> false zurueckgeben 
  {
    ifstream pidfile(procpidfile.c_str());

    if (pidfile != NULL){
      log(LOG_DEBUG,"PID-file already exists!");
      return false;
    }
  }
  
  ofstream pidfile(procpidfile.c_str(),ios::trunc);
  
  if (pidfile == NULL){
    log(LOG_DEBUG,strerror(errno));
    return false;
  }

  pidfilecreated = true;

  pidfile << getpid();

  return true;
}

void StdMessageServer::erasePidFile(const bool &pidfilecreated){

  if (pidfilecreated == true)
    remove(procpidfile.c_str());
}

void StdMessageServer::start() throw(Exception<StdMessageServer>){

  SignalHandler sigchild(SIGCHLD,StdMessageServer::childhandler,false,SA_RESTART|SA_NOCLDSTOP);
  SignalHandler sigterm(SIGTERM,StdMessageServer::termhandler,false,SA_RESTART);
  SignalHandler sigabort(SIGABRT,StdMessageServer::termhandler,false,SA_RESTART);
  SignalHandler sigquit(SIGQUIT,StdMessageServer::termhandler,false,SA_RESTART);
  SignalHandler sigint(SIGINT,StdMessageServer::termhandler,false,SA_RESTART);
  MessageQueue messagequeue;
  Message *message;
  
  try{

    // evtl. als Daemon laufen lassen
    if (daemon == true)
      daemonize();
   
    log(LOG_NOTICE,"daemon startet!");
    
    if (!createPidFile(createpidfile)){
      log(LOG_ERR,"could not create PID-File!\n");
      goto error1;
    }
    
    // Signalhandler einrichten
    sigchild.init();
    sigterm.init();
    sigabort.init();
    sigquit.init();
    sigint.init();
   
    // MessageQueue einrichten
    messagequeue.init();
    mqid = messagequeue.getId();  // fuer die Child-Prozesse
    
    // Handlerprozess einrichten
    if ((pid = fork()) == 0){
      connectionHandler();
      exit(0);
    }

    for(;;){
      
      // auf Nachrichten warten
      try{

	message = messagequeue.receive();
	handleMessage(message);
	delete message;

	// SIGTERM fuehrt zum Abbruch von receive()
	// -> normal beenden

      } catch (Exception<MessageQueue> &mqe){
	break;
      }
    }
    
  } catch (Exception_T &e){
    log(LOG_ERR,e.getMsgCharPtr());
    goto error1;
  }
  
  // Ende: normal
  log(LOG_NOTICE,"Exiting");

  // Child beenden
  if (pid != -1)
    log(LOG_DEBUG,string(string("Killed child: ")+string(strerror(kill(pid,SIGTERM)))).c_str());

  erasePidFile(pidfilecreated);

  return;

  // Ende im Fehlerfall
 error1:

  log(LOG_ERR,"terminated abnormally!");
  erasePidFile(pidfilecreated);

  // ev. Child beenden
  if ( pid != -1 )
    log(LOG_DEBUG,string(string("Killed child: ")+string(strerror(kill(pid,SIGTERM)))).c_str());

  throw Exception<StdMessageServer>("terminated abnormally!");
}

void StdMessageServer::daemonize(void) throw(Exception<StdMessageServer>){

  pid_t pid;

  if ((pid = fork()) < 0)
    throw Exception<StdMessageServer>("ERROR: could not invoke daemon!");

  else if (pid != 0) // Elternprozess
    exit(0);

  setsid();

  chdir("/");

  umask(0);
}

void StdMessageServer::ch_childhandler(int signr){
  
  pid_t pid;
  int status;
  
  pid = wait(&status);

  StdMessageServer::clienthandlers.erase(pid);
}

void StdMessageServer::connectionHandler(){
  
  pid_t pid;
  int msgserver_fd;
  TCPServer *msgserver = (family == AF_INET ? new TCPServer(port) : new TCPServer(stream_path));
  SignalHandler sigterm(SIGTERM,StdMessageServer::termhandler,false);
  SignalHandler sigchild(SIGCHLD,StdMessageServer::ch_childhandler,false,SA_RESTART|SA_NOCLDSTOP);
 
  try{

    // Signalhandler einrichten
    sigterm.init();
    sigterm.siginterrupt(1);
    sigchild.init();
   
    // Server-Socket einrichten
    msgserver->init();
    
    for(;;){
      
      // warten auf Verbindung
      msgserver_fd = msgserver->waitForConnection();
      
      // Client bedienen: eigener Prozess
      if ((pid = fork()) == 0)
	handleClient(msgserver_fd);

      // PIDs der ClientHandler merken
      // WERDEN MOMENTAN NICHT VERWENDET
      clienthandlers.insert(pid);

      close(msgserver_fd);
    }
  } catch (Exception_T &e){

    delete msgserver;

    log(LOG_ERR,e.getIdMsgCharPtr());
    // Socket konnte nicht eingerichtet werden

    // alle beteiligten Prozesse stoppen
    kill(0,SIGTERM);
//     kill(getppid(),SIGTERM);
//     functions::map_unchg(killClientHandler,clienthandlers);

  }
}

void StdMessageServer::killClientHandler(pid_t pid){

  kill(pid,SIGTERM);
}

void StdMessageServer::handleClient(int fd){

  int status;
  SignalHandler sigterm(SIGTERM,SIG_DFL);
  
  log(LOG_INFO,"client has connected!\n");

  try{
    
    // SIGTERM-handler auf Standard setzen
    sigterm.init();
    
    status = parse(fd);
    
    close(fd);
    
    if (status & STDMSGS_ERROR)
      log(LOG_DEBUG,"parse() failed!");
    else if (status & STDMSGS_TIMEOUT)
      log(LOG_INFO,"Timeout!");
    else  
      log(LOG_INFO,"client has disconnected!");
    
  } catch (Exception_T &e){
    log(LOG_ERR,e.getIdMsgCharPtr());
  }
  
  exit(0);
}
