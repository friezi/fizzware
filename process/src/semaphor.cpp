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

#include <semaphor.hpp>

// need extra.qualifier for doxygen (why ever)
#ifndef DOXYGEN_SHOULD_SKIP_THIS

using namespace sys;

#endif

Semaphor::Semaphor(int initval, key_t key, char flag)
  throw (Exception<Semaphor>,SubException<GetErr,Semaphor>,SubException<CtrlErr,Semaphor>)
  : flag(flag){

  union semun arg;
  struct semid_ds buf;

  if (flag==S_NEW){
    if ((id=semget(key,1,IPC_CREAT|IPC_EXCL))==-1)
      throw SubException<GetErr,Semaphor>("exists already: " + err());
    
    buf.sem_perm.uid=geteuid();
    buf.sem_perm.gid=getegid();
    buf.sem_perm.mode=S_IRUSR|S_IWUSR;
    
    arg.buf=&buf;
    
    if ((semctl(id,0,IPC_SET,arg))==-1){
      this->remove();
      throw SubException<CtrlErr,Semaphor>("IPC_SET: " + err());
    }
    
    arg.val=initval;
    
    if ((semctl(id,0,SETVAL,arg))==-1){
      this->remove();
      throw SubException<CtrlErr,Semaphor>("SETVAL: " + err());
    }
  } else if (flag==S_OLD){
    if ((id=semget(key,1,0))==-1)
      throw SubException<GetErr,Semaphor>("doesn't exist -> " + err());
  } else{
    throw Exception<Semaphor>("unsupported flag!");
  }

}

Semaphor::~Semaphor(){

  if (flag==S_NEW)
    this->remove();
}

void Semaphor::block() throw(SubException<CtrlErr,Semaphor>){

  union semun arg;

  arg.val=0;
  
  if ((semctl(id,0,SETVAL,arg))==-1)
    throw SubException<CtrlErr,Semaphor>("SETVAL: " + err());
}
  

void Semaphor::remove(){

  union semun arg;

  arg.val=0;
  semctl(id,0,IPC_RMID,arg);
}

void Semaphor::PV(int op) throw (SubException<OpErr,Semaphor>){

  struct sembuf sb;

  sb.sem_num=0;
  sb.sem_op=op;
  sb.sem_flg=SEM_UNDO;

  if ((semop(id,&sb,1))==-1)
    throw SubException<OpErr,Semaphor>(err());
}



