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

#include <tcpclient.hpp>

using namespace sys;

void TCPClient::init() throw (Exception<TCPClient>,Exception<Socket>){

  struct hostent *host;

  if (sock)
    delete sock;  // falls vor initialize=true erneut init() aufgerufen wird

  if ( family == AF_INET ){

  if (!(host = gethostbyname(hostname.c_str())))
    throw Exception<TCPClient>("ERROR: could not find address of host!");
  
  sock = new TCPSocket(port,host,socket_opt_level,socket_opt_name,0);

  } else{ // family == AF_LOCAL

    ::unlink(stream_path.c_str());

    sock = new TCPSocket(stream_path,socket_opt_level,socket_opt_name,0);

  }
  
  sock->init();

  initialized = true;

}

int TCPClient::buildUpConnection() throw (Exception<TCPClient>,Exception<Socket>){

  if (initialized == true)
    return sock->connect();
  else
    throw Exception<TCPClient>("ERROR: not initialized!");
  
}

void TCPClient::shutdown(){

  if (sock)
    delete sock;

  if ( family == AF_LOCAL )
    ::unlink(stream_path.c_str());

  sock = 0;
  initialized = false;
}
