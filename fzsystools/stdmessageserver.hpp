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

/**
   @file stdmessageserver.hpp
   @author Friedemann Zintel
*/

#ifndef STDMESSAGESERVER_HPP
#define STDMESSAGESERVER_HPP

#include <string>
#include <fstream>
#include <set>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <tcpserver.hpp>
#include <signalhandler.hpp>
#include <messagequeue.hpp>
#include <functions.hpp>

#define STDMSGS_STOP 1L<<0
#define STDMSGS_QUIT 1L<<1
#define STDMSGS_ERROR 1L<<2
#define STDMSGS_TIMEOUT 1L<<3
#define PIDPATH "/var/run/"

// abstrakte Klasse:
/**
    @interface StdMessageServer
    If a Client builds up a connection to the standard-message-server, the sms opens a socket and passes
    its descriptor to the abstract method parse(), which parses the output of the socket. Within parse() an
    appropriet message should be send to the sms via Message::send(). The handling of the message will be
    executed by handleMessage().So you have to overwrite the following abstract methods:\n
    - parse()\n
    - handleMessage()\n
    - log()
    @brief A standard-server based on message-communication
*/
class StdMessageServer{

  /// a set of process-ids
  typedef std::set< pid_t,std::less<pid_t> > PIDSet;

private:

  static void childhandler(int signr);
 
  static void termhandler(int signr);

  static void ch_childhandler(int signr);

  static void killClientHandler(pid_t pid);

  bool createPidFile(const bool &createpidfile);
  void erasePidFile(const bool &pidfilecreated);

  /// wartet auf Verbindung zum Server-Socket
  void connectionHandler();

  // Kommuniziert mit verbundenem Client
  void handleClient(int fd);

  // Nur von eigenen Prozessen aus zugreifen, sonst gibt's Müll
  static PIDSet clienthandlers;  // PIDs der ClientHandlers

  int mqid;  // Kennung der MessaqueQueue
  pid_t pid;
  unsigned short int port;
  std::string stream_path;
  int family;
  bool daemon;
  std::string procname;
  bool createpidfile;
  bool pidfilecreated;
  std::string procpidfile;

protected:

  StdMessageServer(const StdMessageServer&){}

public:

  /**
     @brief a server type INET
     @param daemon if true, then starting as backgroundprocess
     @param port portnumber
     @param procname name of server
     @param createpidfile if true, then a file \<file\>.pid in directory PIDPATH is created, containing the PID
     @remark After calling the constructor you have to call start() manually to get the server startet. 
  */
  StdMessageServer(bool daemon, unsigned short int port, const std::string procname = "", const bool &createpidfile = false);

  /**
     @brief a server type Unix/Local
     @param daemon if true, then starting as backgroundprocess
     @param stream_path path for a Unix-Domain-Socket
     @param procname name of server
     @param createpidfile if true, then a file \<file\>.pid in directory PIDPATH is created, containing the PID
     @remark After calling the constructor you have to call start() manually to get the server startet. 
  */
  StdMessageServer(bool daemon, std::string stream_path, const std::string procname = "", const bool &createpidfile = false);

  virtual ~StdMessageServer() {}

  // starten des Daemons: solange Daemon läuft, kehrt start nicht zurück
  /**
     @brief Starts the server
     @exception Exception<StdMessageServer>
     @remark Will return immediately, if daemon == true in constructor-call StdMessageServer().
   */
  void start() throw(Exception<StdMessageServer>);

  // zurückliefern der ID der MessageQueue
  /**
     @brief Returns the ID of the MessageQueue
     @return the ID of the messagequeue
     @exception Exception<StdMessageServer>
  */
  int getMsgQuId() { return mqid; }

  // läßt aufrufenden Prozess als Daemon im Hintergrund laufen
  /**
     @brief the calling prozess will become a daemon running in the background.
     @exception Exception<StdMessageServer>
  */
  void daemonize(void) throw(Exception<StdMessageServer>);

  // zu überschreibende Funktionen:

  // was soll mit log-informationen geschehen?
  /**
     @brief to write logging-information
     @param loglevel the current loglevel
     @param text the text to be printed
  */
  virtual void log(const int &loglevel, const char *text) = 0;

  // fd ist Socket-Filedeskriptor
  // es können nun Daten vom Socket gelesen werden
  // nach Beendigung von parse wird fd automatisch geschlossen
  /**
     parsing, parsing, parsing
     @brief to parse the output of a socket
     @param fd is a socket-filedescriptor
     @return the status: the STDMSGS_ERROR-flag should be set for an occurred error, the STDMSGS_TIMEOUT-flag should be set for timeout
     @remark Within this method you can send a message to the main-server with Message::send().
  */
  virtual unsigned int parse(int fd) = 0;

  // wird vom Server bei erhalt einer Message aufgerufen
  /**
     This is the method the server calls after receiving a message.
     So, you have to define what to do with it.
     @brief to handle the message sent to the standard-message-server
     @param message is the message (must be parsed as well)
  */
  virtual void handleMessage(sys::Message *message) = 0;

};


#endif
