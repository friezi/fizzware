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
   @file tcpserver.hpp
   @author Friedemann Zintel
*/

#ifndef FZSYSTOOLS_TCPSERVER_HPP
#define FZSYSTOOLS_TCPSERVER_HPP

#include <string.h>
#include <string>
#include <fzsystools/socket.hpp>
#include <fztooltempl/exception.hpp>

namespace sys{

  /**
     @class TCPServer
     Order of method-calls:\n
     1. TCPServer() (constructor)\n
     2. init()\n
     3. one or more of the set... -methods (optional) to adjust parameters\n
     4. waitForConnection()\n
     5. shutdown()
     @brief For setting up an Server which can wait for TCP/IP or UDP connections
     @todo handling UDP properly
  */
  class TCPServer{

  private:

    unsigned short int port;
    std::string stream_path;
    int family;
    TCPSocket *sock;
    int socket_opt_level;
    int socket_opt_name;
    int socket_backlog;
    bool initialized;

    static const int TCPSERVER_QU_LENGTH;

    // unerlaubter Kopierkonstruktor
    TCPServer(const TCPServer&){}

  public:

    // Konstruktor: fuer Typ TCP
    /**
       @brief an INET-server
       @param port the desired portnumber
       @remark You have to call init() manually after calling the constructor
    */ 
    TCPServer(unsigned short int port) :
      port(port), stream_path(""),family(AF_INET),sock(0), socket_opt_level(SOL_SOCKET),
      socket_opt_name(SO_REUSEADDR),socket_backlog(TCPSERVER_QU_LENGTH),initialized(false){}
    /**
       @brief a Unix/Local-server
       @param stream_path the path for the stream-socket file
       @remark You have to call init() manually after calling the constructor
    */ 
    TCPServer(std::string stream_path) :
      port(0), stream_path(stream_path),family(AF_LOCAL), sock(0), socket_opt_level(SOL_SOCKET),
      socket_opt_name(SO_REUSEADDR), socket_backlog(TCPSERVER_QU_LENGTH),initialized(false){}
  
    // Destruktor:
    ~TCPServer(){ this->shutdown(); }

    // einrichten des Servers
    /**
       @brief initializing and setting up the server
       @exception Exception<TCPServer>
       @exception Exception<Socket>
       @note this function must be called manually after calling the constructor
    */
    void init() throw (exc::Exception<TCPServer>,exc::Exception<Socket>);

    // auf Verbindung durch client warten, Rueckgabe: Filedeskriptor zum Schreiben und Lesen
    /**
       @brief waits for a client-connection
       @return a filedescriptor for reading and writing
       @throw Exception<TCPServer>
       @throw Exception<Socket>
    */
    int waitForConnection() throw (exc::Exception<TCPServer>,exc::Exception<Socket>);

    // Server herunterfahren
    /**
       @brief shutdown the server
    */
    void shutdown();

    //    // parameter fuer struct sockaddr
    //    void setFamily(int family){ this->family = family; }

    //   /**
    //      @brief For getting the address-family
    //      @return address-family
    //   */
    //   int getFamily(){ return family; }

    //    // Parameter fuer socket()
    //   /**
    //      Will modify the port-family (PF_... )
    //      @brief Change the port-family
    //      @param domain port-family
    //   */
    //   void setSocketDomain(int domain){ socket_domain = domain; }

    //   /**
    //      @brief Returns the port-family
    //      @return the port-family
    //   */
    //   int getSocketDomain(){ return socket_domain; }

    //    // Parameter fuer socket()
    //   /**
    //      @brief Modify the socket-type
    //      @param type socket-type
    //   */
    //   void setSocketType(int type){ socket_type = type; }
  
    //   /**
    //      @brief returns the socket-type
    //      @return the socket-type
    //   */
    //   int getsocketType(){ return socket_type; }
  
    //   // Parameter fuer socket()
    //   /**
    //      @brief Modifies the protocol-type
    //      @param protocol the protocol-type
    //   */
    //   void setSocketProtocol(int protocol){ socket_protocol = protocol; }
  
    //   /**
    //      @brief to get the protocol-type
    //      @return the protocol-type
    //   */
    //   int getSocketProtocol(){ return socket_protocol; }
  
    // Parameter fuer setsockopt()
    /**
       @brief For modifying the socket-option-level
       @param level the socket-option-level
    */
    void setSocketOptLevel(int level){ socket_opt_level = level; }
  
    /**
       @brief for getting the socket-option-level
       @return the socket-option-level
    */
    int getSocketOptLevel(){ return socket_opt_level; }
  
    // Parameter fuer setsockopt()
    /**
       @brief Modifies the socket-option-name
       @param name the socket-option-name
    */
    void setSocketOptName(int name){ socket_opt_name = name; }
  
    /**
       @brief for getting the socket-option-name
       @return the socket-option-name
    */
    int getSocketOptName(){ return socket_opt_name; }
  
    // Parameter fuer listen()
    /**
       @brief Changes the socket_backlog
       @param backlog the socket_backlog
    */
    void setSocketBacklog(int backlog){ socket_backlog = backlog; }

    /**
       @brief For getting the socket_backlog
       @return the socket_backlog
    */
    int getSocketBacklog(){ return socket_backlog; }
  };

}

#endif
