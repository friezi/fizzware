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

#include <socket.hpp>

using namespace std;
using namespace sys;

Socket::Socket(unsigned short int port, struct hostent *host, int family, int socket_domain,
	       int socket_type, int socket_protocol, int socket_opt_level, int socket_opt_name,
	       int socket_backlog) throw(Exception_T) :
  port(port), spath(""), sock(0), family(family), socket_domain(socket_domain), socket_type(socket_type),
  socket_protocol(socket_protocol), socket_opt_level(socket_opt_level), socket_opt_name(socket_opt_name),
  socket_backlog(socket_backlog),initialized(false), host(host){

  setSockaddr();
}

Socket::Socket(string spath, int family, int socket_domain,
	       int socket_type, int socket_protocol, int socket_opt_level, int socket_opt_name,
	       int socket_backlog) throw(Exception_T) :
  port(0), spath(spath), sock(0), family(family), socket_domain(socket_domain), socket_type(socket_type),
  socket_protocol(socket_protocol), socket_opt_level(socket_opt_level), socket_opt_name(socket_opt_name),
  socket_backlog(socket_backlog),initialized(false), host(0){

  setSockaddr();
}

void Socket::setSockaddr() throw(Exception<Socket>){
  
  switch (family){

  case AF_INET:
    sockaddr_size = sizeof(struct sockaddr_in);
    break;
  case AF_LOCAL:
    sockaddr_size = sizeof(struct sockaddr_un);
    break;
  default:
    throw Exception<Socket>("Error: unsupported adress-family!");
    break;
  }
}

Socket::~Socket(){

  this->close();
}

void Socket::close(){
  
  if (sock)
    ::close(sock);
  sock = 0;
  initialized = false;
}

void Socket::init() throw (Exception<Socket>){

  if (initialized == true)
    throw Exception<Socket>("ERROR: init() called, but Socket already initialized!");

  // Adress-Parameter für den Socket eintragen
  switch (family){

  case AF_INET:
    
    ((struct sockaddr_in *)&sock_addr)->sin_family = family;
    ((struct sockaddr_in *)&sock_addr)->sin_port = htons(port);
    memcpy(&((struct sockaddr_in *)&sock_addr)->sin_addr,host->h_addr,host->h_length);
    break;

  case AF_LOCAL:

    ((struct sockaddr_un *)&sock_addr)->sun_family = family;
    strcpy(((struct sockaddr_un *)&sock_addr)->sun_path,spath.c_str());
    break;

  default:
    throw Exception<Socket>("ERROR: unsupported address-family for socket!");
    break;
  }
  
  // Socket einrichten
  if ((sock = socket(socket_domain,socket_type,socket_protocol)) == -1)
    throw Exception<Socket>("ERROR: could not install socket: " + err());

  // Socket-optionen setzen
  {
    int i = 1;
    if ((setsockopt(sock,socket_opt_level,socket_opt_name,&i,sizeof(i))) == -1)
      throw Exception<Socket>("ERROR: setsockopt() failed: " + err());
  }
  
  initialized = true;
}

void Socket::bind() throw (SubException<BindErr,Socket>){

  // Socket mit Adresse verknüpfen
  if ((::bind(sock,(struct sockaddr *)&sock_addr,sockaddr_size)) == -1)
    throw SubException<BindErr,Socket>("ERROR: could not bind socket to requested address: " + err());
}

void Socket::listen() throw (SubException<ListenErr,Socket>){

  // zum Abhören anmelden
  if ((::listen(sock,socket_backlog)) == -1)
    throw SubException<ListenErr,Socket>("ERROR:listen failed: " + err());
}

int Socket::accept() throw (SubException<AcceptErr,Socket>){
  
  int fd;

  // Verbindung annehmen
  if ((fd = ::accept(sock,(struct sockaddr *)&sock_addr,&sockaddr_size)) == -1)
    throw SubException<AcceptErr,Socket>("ERROR: accept failed: " + err());

  return fd;
}

int Socket::connect() throw (SubException<ConnectErr,Socket>){
  
  if (::connect(sock,(struct sockaddr *)&sock_addr,sockaddr_size) == -1)
    throw SubException<ConnectErr,Socket>("ERROR: connect failed: " + err());

  return sock;
}
