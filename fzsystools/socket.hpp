/**
   @mainpage
   This page documents the static-library 'Friedemann-Zintel`s System Tools'.\n
   It depends on the library 'Friedemann-Zintel`s-Tools`n-Templates' (fztooltempl).
   @author Friedemann Zintel
   
   Copyright (C) 1999-2007 Friedemann Zintel

   Compilation is successfully tested with g++ v.2.95, v.3.3 and v.4.0 .
   
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
   @file socket.hpp
   @author Friedemann Zintel
*/

#ifndef _FZST_SOCKET_HPP_
#define _FZST_SOCKET_HPP_

#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <exception.hpp>

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif

#ifndef PF_LOCAL
#define PF_LOCAL PF_UNIX
#endif


/**
   @namespace sys
   @brief for Linux/Unix-systemstructures and -functions
*/
namespace sys{

  /**
     @class Socket
     @brief A class for Unix/Linux-sockets of any type
     @remark so far only AF_INET and AF_LOCAL are supported
  */

  class Socket{

  private:
  
    // Exception-Classes
    /** @internal */
    class BindErr{};
  
    /** @internal */
    class ListenErr{};
  
    /** @internal */
    class AcceptErr{};
  
    /** @internal */
    class ConnectErr{};

    unsigned short int port;
    std::string spath;
    int sock;
    int family;
    int socket_domain;
    int socket_type;
    int socket_protocol;
    int socket_opt_level;
    int socket_opt_name;
    int socket_backlog;
    bool initialized;
    struct hostent *host;
    struct sockaddr_un sock_addr;  // sicher ist sicher
    socklen_t sockaddr_size;
  
    Socket(const Socket&){}

  protected:

    /// The system-error
    std::string err(){ return std::string(strerror(errno)); }

  public:
  
    /**
       @brief installs an INET-socket
       @param port the address-port
       @param host the host (given for ex. by gethostbyname())
       @param family the address-family
       @param socket_domain the port-family
       @param socket_type type of the socket
       @param socket_protocol the desired protocol
       @param socket_opt_level option-level for socket
       @param socket_opt_name options for socket
       @param socket_backlog option for listen()
       @exception Exception<Socket>
       @remark You have to call init() manually after calling the constructor
    */
    Socket(unsigned short int port, struct hostent *host, int family, int socket_domain,
	   int socket_type, int socket_protocol, int socket_opt_level, int socket_opt_name,
	   int socket_backlog) throw(exc::ExceptionBase);
  
  
    /**
       @brief installs an UDS (unix-domain-socket)
       @param spath the path and filename for an Unix-Domain-Socket
       @param family the address-family
       @param socket_domain the port-family
       @param socket_type type of the socket
       @param socket_protocol the desired protocol
       @param socket_opt_level option-level for socket
       @param socket_opt_name options for socket
       @param socket_backlog option for listen()
       @exception Exception<Socket>
       @remark You have to call init() manually after calling the constructor
    */
    Socket(std::string spath, int family, int socket_domain,
	   int socket_type, int socket_protocol, int socket_opt_level, int socket_opt_name,
	   int socket_backlog) throw(exc::ExceptionBase);
    /**
       @remark close() will be called
    */
    ~Socket();
  
    /**
       @brief Initializes and sets up the socket
       @exception Exception<Socket>
    */
    void init() throw (exc::Exception<Socket>);

    /**
       @brief binds a socket to an address
       @exception SubException<BindErr,Socket>
       @note wrap-around for Unix/Linux bind-function
    */
    void bind() throw (exc::SubException<BindErr,Socket>);

    /**
       @brief Waits for a connection
       @exception SocketListenException
       @note wrap-around for Unix/Linux listen-function
    */
    void listen() throw (exc::SubException<ListenErr,Socket>);

    /**
       @brief Accepts a connection
       @exception SubException<AcceptErr,Socket>
       @note wrap-around for Unix/Linux accept-function
    */
    int accept() throw (exc::SubException<AcceptErr,Socket>);

    /**
       @brief Connects with a server
       @return the socket-filedeskriptor
       @exception SubException<ConnectErr,Socket>
       @note wrap-around for Unix/Linux connect-function
    */
    int connect() throw (exc::SubException<ConnectErr,Socket>);

    /**
       @brief closes an open socket
    */
    void close();

    /**
       @brief returns socket filedescriptor
       @return filedescriptor
    */
    int getSocket(){ return sock; }

  protected:
  
    /**
       @brief sets the correct values in sockaddr according to family
       @exception Exception<Socket>
    */
    void setSockaddr() throw(exc::Exception<Socket>);
  
  };

  /**
     The following inherited methods are public:\n
     - init()
     - bind()
     - listen()
     - accept()
     - connect()
     - close()

     All others are protected.
     @brief for installing a TCP-based socket.
  */
  class TCPSocket : protected Socket{

  public:

    /**
       @brief installs an INET-socket based on TCP
       @param port the address-port
       @param host the host (given for ex. by gethostbyname())
       @param socket_opt_level option-level for socket
       @param socket_opt_name options for socket
       @param socket_backlog option for listen()
       @exception Exception<Socket>
       @remark You have to call init() manually after calling the constructor
    */
    TCPSocket(unsigned short int port, struct hostent *host, int socket_opt_level, int socket_opt_name,
	      int socket_backlog) throw(exc::ExceptionBase)
      : Socket(port,host,AF_INET,PF_INET,SOCK_STREAM,0,socket_opt_level,
	       socket_opt_name,socket_backlog){}
  
    /**
       @brief installs a Unix-Domain-Socket based on TCP
       @param spath the path and filename for an Unix-Domain-Socket
       @param socket_opt_level option-level for socket
       @param socket_opt_name options for socket
       @param socket_backlog option for listen()
       @exception Exception<Socket>
       @remark You have to call init() manually after calling the constructor
    */
    TCPSocket(std::string spath, int socket_opt_level, int socket_opt_name,
	      int socket_backlog) throw(exc::ExceptionBase)
      : Socket(spath,AF_LOCAL,PF_LOCAL,SOCK_STREAM,0,socket_opt_level,
	       socket_opt_name,socket_backlog){}

    /**
       @brief public
    */
    Socket::init;
    Socket::bind;
    Socket::listen;
    Socket::accept;
    Socket::connect;
    Socket::close;
    Socket::getSocket;

  };

  /**
     @brief for installing an UDP-based socket.
  */
  class UDPSocket : protected Socket{

    /**
       @brief installs an INET-socket based on UDP
       @param port the address-port
       @param host the host (given for ex. by gethostbyname())
       @param socket_opt_level option-level for socket
       @param socket_opt_name options for socket
       @param socket_backlog option for listen()
       @exception Exception<Socket>
       @remark You have to call init() manually after calling the constructor
    */
    UDPSocket(unsigned short int port, struct hostent *host, int socket_opt_level, int socket_opt_name,
	      int socket_backlog) throw(exc::ExceptionBase)
      : Socket(port,host,AF_INET,PF_INET,SOCK_DGRAM,0,socket_opt_level,
	       socket_opt_name,socket_backlog){}
  
    /**
       @brief installs a Unix-Domain-Socket based on UDP
       @param spath the path and filename for an Unix-Domain-Socket
       @param socket_opt_level option-level for socket
       @param socket_opt_name options for socket
       @param socket_backlog option for listen()
       @exception Exception<Socket>
       @remark You have to call init() manually after calling the constructor
    */
    UDPSocket(std::string spath, int socket_opt_level, int socket_opt_name,
	      int socket_backlog) throw(exc::ExceptionBase)
      : Socket(spath,AF_LOCAL,PF_LOCAL,SOCK_DGRAM,0,socket_opt_level,
	       socket_opt_name,socket_backlog){}


  };

}

#endif
