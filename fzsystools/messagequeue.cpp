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

#include <fzsystools/messagequeue.hpp>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

using namespace sys;
using namespace exc;

#endif

const int MessageQueue::MSGMAX = 4096;

bool MessageQueue::exists(int id){

  struct msqid_ds buffer;

  if ( msgctl(id,IPC_STAT,&buffer) == 0 )
    return true;
  else
    return false;
}

Message::Message(const char * const buffer, size_t msize, long type) throw(Exception<Message>) : msg(0), msize(msize){

  init(buffer,type);
}

void Message::init(const char * const buffer, long type) throw(Exception<Message>){
    
    if (!(msg = (char *)calloc(1,msize+sizeof(long))))
      throw Exception<Message>("calloc() failed!");
    
    if (buffer){
      memcpy(msg,(char *)&type,sizeof(long));
      memcpy(msg+sizeof(long),buffer,msize);
    }
}

Message::~Message(){

  free(msg);
  msg = 0;
  msize = 0;
}

void Message::setType(long type){

  memcpy(msg,(char *)&type,sizeof(long));
}

long Message::getType() const {

  long type;

  memcpy((char *)&type,msg,sizeof(long));
  return type;
}

size_t Message::sizeOfType() const {

  return sizeof(long);
}

void Message::send(int id, int flag) const throw(Exception<Message>){

  if (msgsnd(id,msg,msize,flag) == -1){
    perror("error:");
    throw Exception<Message>("send() failed!");
  }
}

std::string Message::getStringMsg() const throw(Exception<Message>){

  char *buf;
  std::string s;

  if (!(buf = (char *)calloc(1,msize+1)))
    throw Exception<Message>("getStringMsg(): calloc() failed");

  memcpy(buf,getData(),msize);
  s = buf;
  free(buf);
  return s;
}
  

std::string Message::contents() const {

  std::ostringstream collect;

  for (unsigned int i = 0; i < size()+sizeof(long); i++)
    collect << getMsg()[i] << "|";
  collect << "\n";
  
  return collect.str();
}

MessageQueue::MessageQueue(key_t key, int flag, bool init) throw(Exception<MessageQueue>) : key(key), id(0), flag(flag){
  
  if (init == true)
    this->init();
}

MessageQueue::~MessageQueue() throw(Exception<MessageQueue>){
  
  control(IPC_RMID,NULL);
}

void MessageQueue::init() throw(Exception<MessageQueue>){
  
  get();
}

void MessageQueue::get() throw(Exception<MessageQueue>){
  
  if ((id = msgget(key,flag)) == -1){
    throw Exception<MessageQueue>("get() failed!");}
}

// Message erhalten
Message *MessageQueue::receive(size_t maxlength, long typ, int flag)
  throw(Exception<MessageQueue>,ExceptionBase){
  
  int length;
  Message *msg = new Message(NULL,maxlength);

  if ((length = msgrcv(id,msg->msg,maxlength,typ,flag)) == -1)
    throw Exception<MessageQueue>("receive() failed!");

  msg->msize = length;
  return msg;
}

// Abfragen/Aendern/Loeschen
void MessageQueue::control(int kdo, struct msqid_ds *buffer) throw(Exception<MessageQueue>){
  
  if (msgctl(id,kdo,buffer) == -1)
    throw Exception<MessageQueue>("control() failed!");
}

