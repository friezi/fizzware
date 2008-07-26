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

#include <tcpserver.hpp>

using namespace std;
using namespace sys;
using namespace exc;

const int TCPServer::TCPSERVER_QU_LENGTH=5;

void TCPServer::init() throw (Exception<TCPServer>,Exception<Socket>){

  struct hostent *host;

  if (sock)
    delete sock;  // falls vor initialize==true erneut init() aufgerufen wird

  if ( family == AF_INET ){

    if (!(host = gethostbyname("127.0.0.1")))
      throw Exception<TCPServer>("ERROR: could not find address of localhost: " + string(hstrerror(h_errno)));
  
    sock = new TCPSocket(port,host,socket_opt_level,socket_opt_name,socket_backlog);

  } else{ // family == AF_LOCAL

    ::unlink(stream_path.c_str());

    sock = new TCPSocket(stream_path,socket_opt_level,socket_opt_name,socket_backlog);

  }

  sock->init();

  sock->bind();

  sock->listen();

  initialized = true;

}

int TCPServer::waitForConnection() throw (Exception<TCPServer>,Exception<Socket>){

  if (initialized == true)
    return sock->accept();
  else
    throw Exception<TCPServer>("ERROR: not initialized!");
}

void TCPServer::shutdown(){

  if ( sock )
    delete sock;

  if ( family == AF_LOCAL )
    ::unlink(stream_path.c_str());

  sock = 0;
  initialized = false;
}
