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

#include <msgqueueregclient.hpp>

using namespace std;
using namespace sys;

const string MsgQueueRegClient::default_socket_path = "/var/run/msgqrd_socket";

MsgQueueRegClient::MsgQueueRegClient(string socket_path) throw(Exception<MsgQueueRegClient>,Exception_T){
 
//   struct servent *msgqrd_entry;

//   // Portnummer abfragen
//   msgqrd_entry = getservbyname("msgqrd","tcp");
//   if (!msgqrd_entry){
//     throw Exception<MsgQueueRegClient>("in /etc/services kein Eintrag fuer msgqrd Protokoll:tcp!");
//   }

//   msgqueueregclient = new TCPClient("localhost",ntohs(msgqrd_entry->s_port));

  msgqueueregclient = new TCPClient(socket_path);

}

MsgQueueRegClient::~MsgQueueRegClient(){

  delete msgqueueregclient;
}


bool MsgQueueRegClient::newPort(std::string name) throw (Exception<MsgQueueRegClient>,Exception_T){

//   int sockfd;

//   msgqueueregclient->init();

//   sockfd = msgqueueregclient->buildUpConnection();

//   std::string commandadd = "CMD_NEW_MSGQUEUE";

//   if ((write(sockfd,commandadd.c_str(),commandadd.size()+1)) == -1)
//     throw Exception<MsgQueueRegClient>(strerror(errno));

// //    if ((write(sockfd,&nix,1)) == -1)
//     throw Exception<MsgQueueRegClient>(strerror(errno));

//   if ((write(sockfd,name.c_str(),name.size()+1)) == -1)
//     perror("Fehler in write()");
  
//   // do whatever

//   msgqueueregclient->shutdown();
//   cout << "runtergefahren";

    return false;   /// !!!!!!!!!!!!!! ///

}

bool MsgQueueRegClient::addPort(std::string name, int id) throw (Exception<MsgQueueRegClient>,Exception_T){

  std::string commandadd = "CMD_ADD_MSGQUEUE";
  int sockfd;
  unsigned long nid = htonl((unsigned long)id);
  bool result;

  msgqueueregclient->init();

  sockfd = msgqueueregclient->buildUpConnection();

  // Kommando
  if ((write(sockfd,commandadd.c_str(),commandadd.size()+1)) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));

  // Portname
  if ((write(sockfd,name.c_str(),name.size()+1)) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));
 
  // PortID
  if ((write(sockfd,&nid,sizeof(unsigned long))) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));
  
  // do whatever

  Scanner scanner(sockfd);
  Token *rslttoken = NULL;

  rslttoken = scanner.nextToken();

  result = (*rslttoken == "OK_ADD_MSGQUEUE") ? true : false;

  delete rslttoken;
  msgqueueregclient->shutdown();

  return result;
}

int MsgQueueRegClient::findPort(std::string name) throw (Exception<MsgQueueRegClient>,Exception_T){

  std::string commandfind = "CMD_FIND_MSGQUEUE";
  int sockfd;
  int id = -1;

  msgqueueregclient->init();

  sockfd = msgqueueregclient->buildUpConnection();

  // Kommando
  if ((write(sockfd,commandfind.c_str(),commandfind.size()+1)) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));

  // Portname
  if ((write(sockfd,name.c_str(),name.size()+1)) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));

  Scanner scanner(sockfd);
  Token *token = NULL;
  ULongToken *idtoken = NULL;

  token = scanner.nextToken();

  if (*token == "OK_FIND_MSGQUEUE"){

    idtoken = scanner.nextULongToken();
    id = (int)idtoken->getValue();
    delete idtoken;
  }

  delete token;
  msgqueueregclient->shutdown();

  return id;
}

bool MsgQueueRegClient::removePort(std::string name) throw (Exception<MsgQueueRegClient>,Exception_T){

  std::string commandfind = "CMD_REMOVE_MSGQUEUE";
  int sockfd;
  bool result;

  msgqueueregclient->init();

  sockfd = msgqueueregclient->buildUpConnection();

  // Kommando
  if ((write(sockfd,commandfind.c_str(),commandfind.size()+1)) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));

  // Portname
  if ((write(sockfd,name.c_str(),name.size()+1)) == -1)
    throw Exception<MsgQueueRegClient>(strerror(errno));

  Scanner scanner(sockfd);
  Token *rslttoken = NULL;

  rslttoken = scanner.nextToken();

  result = (*rslttoken == "OK_REMOVE_MSGQUEUE") ? true : false;

  delete rslttoken;
  msgqueueregclient->shutdown();

  return result;
}


MsgQueueRegClient::Token::~Token(){
  
  tsize = 0;
  free(buffer);
}

unsigned long MsgQueueRegClient::Token::toLong(){
  
  unsigned long value = 0;
  size_t size = sizeof(unsigned long);
  
  if (this->size() < sizeof(unsigned long))
    size = this->size();
  
  memcpy((char *)&value,this->get(),size);
  
  return value;
}

int MsgQueueRegClient::Token::operator==(char *s){

  return (strncmp(this->buffer,s,tsize) == 0);
}

int MsgQueueRegClient::Token::operator==(const std::string &s){

  return (strncmp(this->buffer,s.data(),tsize) == 0);
}

MsgQueueRegClient::ULongToken::ULongToken(unsigned long value){

  this->value = value;
  this->buffer = NULL;
  this->tsize = (sizeof(unsigned long));
  this->type = ULONG_TOKEN;
}

const unsigned long MsgQueueRegClient::Scanner::BLKSIZE = 10;

int MsgQueueRegClient::Scanner::isblank(char c){

  return ((c == ' ') || (c == '\t'));
}

MsgQueueRegClient::Token *MsgQueueRegClient::Scanner::nextToken() throw (Exception<Scanner>,Exception<BadToken>,Exception_T){

  stor::Buffer<char> buffer(BLKSIZE);
  stor::MemPointer<char> input(false);
  unsigned int count;
  char tmp;

  if (fd == 0)
    throw Exception<Scanner>("Filedescriptor is zero!");
  
  unsigned long size = 0;
  while (true){

    ssize_t r;

    r = read(fd,&tmp,1);

    if (r == 0){
      if (size) // es wurde was gelesen, nun ist EOS 
	break;
      else  // EOS ohne vorheriges lesen
	return new EOSToken();
    }
    
    if (r == -1) //  Fehler
      throw Exception<BadToken>(strerror(errno));

    if ((tmp=='\0'))
      break;
    else if(isspace(tmp) && !(isblank(tmp)))
      continue;
    
    buffer.put(tmp);
    size++;
  }
  
  input = buffer.merge();
  count = buffer.elements();
 
  return new Token(input.get(),count);
}

MsgQueueRegClient::ULongToken *MsgQueueRegClient::Scanner::nextULongToken()
  throw (Exception<Scanner>,Exception<BadToken>,Exception_T){
  
  unsigned long tmp = 0;
  
  if (fd == 0)
    throw Exception<Scanner>("Filedescriptor is zero!");
  
  ssize_t r;
  
  r = read(fd,&tmp,sizeof(unsigned long));
  
  if (r == 0)
    throw Exception<EOSToken>();
  
  if (r == -1)
    throw Exception<BadToken>(strerror(errno));
  
  return new ULongToken(ntohl(tmp));
}
