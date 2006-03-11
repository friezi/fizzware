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
   @file tcpclient.hpp
   @author Friedemann Zintel
*/

#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <string.h>
#include <string>
#include <socket.hpp>
#include <exception.hpp>

namespace sys{

  /**
     @class TCPClient
     Order of method-calls:\n
     1. TCPClient() (constructor)\n
     2. init()\n
     3. one or more of the set... -methods (optional) to adjust parameters\n
     4. buildUpConnection()\n
     5. shutdown()
     @brief For installing a client, which can build up a TCP/IP-connection to an internet-host
  */
  class TCPClient{

    std::string hostname;
    unsigned short int port;
    std::string stream_path;  // path for UDS
    TCPSocket *sock;
    int family;
    //   int socket_domain;
    //   int socket_type;
    //   int socket_protocol;
    int socket_opt_level;
    int socket_opt_name;
    bool initialized;

    TCPClient(const TCPClient&){}

    //   void closeSocket();

  public:

    // Konstruktor:
    /**
       @param hostname name of the host to connect to
       @param port the desired portnumber
       @remark You have to call init() manually after calling the constructor
    */
    TCPClient(std::string hostname, unsigned short int port) :
      hostname(hostname), port(port), stream_path(""),sock(0), family(AF_INET),socket_opt_level(SOL_SOCKET),
      socket_opt_name(SO_REUSEADDR),initialized(false){}

    /**
       @param stream_path path for UDS
       @remark You have to call init() manually after calling the constructor
    */
    TCPClient(std::string stream_path) :
      hostname(""), port(0), stream_path(stream_path), sock(0), family(AF_LOCAL), socket_opt_level(SOL_SOCKET),
      socket_opt_name(SO_REUSEADDR),initialized(false){}

    // Destruktor:
    ~TCPClient(){ this->shutdown(); }

    // einrichten des Clienten
    /**
       @brief Initializing and setting up the client
       @exception Exception<TCPServer>
       @exception Exception<Socket>
       @note this function must be called manually after calling the constructor
    */
    void init() throw (Exception<TCPClient>,Exception<Socket>);

    // Verbindung zum Server herstellen: liefert socket-filedeskriptor zurueck
    /**
       @brief builds up a connection to a server
       @return the socket-filedeskriptor
       @exception Exception<TCPClient>
       @exception Exception<Socket>
    */
    int buildUpConnection() throw (Exception<TCPClient>,Exception<Socket>);

    // Client herunterfahren
    /**
       @brief shutdown the client
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
    //      Will modifies the port-family (PF_... )
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

  };

}

#endif
