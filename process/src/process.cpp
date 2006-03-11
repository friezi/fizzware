/*
    Copyright (C) 1999-2004 Friedemann Zintel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    For any questions, contact me at
    friezi@cs.tu-berlin.de
*/

#include <process.hpp>

unsigned int Process::pcount=0;

pid_t Process::wait(int *status) throw(Exception_T){
  
  pid_t wpid;
  bool delstat=false;

  if (!pid)
    throw WaitErr(getPnmb());

  if (!status){
    status = new int;
    delstat=true;
  }
  
  wpid=waitpid(pid,status,0);;
  pid=ppid=0;
  pprotect=true;

  if (delstat)
    delete status;

  return wpid;
}

Process::Process(bool daemon) : pid(0),ppid(0),setprot(daemon),pprotect(true){

  pcount++;
  pnmb=pcount;
}

void Process::launch() throw (Exception_T){
  
  pid_t pid;
  
  if (this->pid)
    throw ActiveErr(getPnmb());
  
  pid=fork();
  if (pid==-1)
    throw CreateErr(getPnmb());
  
  if (!pid){   // child
    this->pid=getpid();
    this->ppid=getppid();
    exit(main());
    
  }
  
  this->pid=pid;
  this->ppid=getpid();
  this->pprotect=setprot;
}

void Process::abort() throw(Exception_T){
  
  if (!pid)
    throw AbortErr(getPnmb());
  
  kill(pid,SIGKILL);
}

void Process::terminate() throw(Exception_T){
  
  if (!pid)
    throw TerminateErr(getPnmb());
  
  kill(pid,SIGTERM);
}

void Process::sendSignal(int signal) throw(Exception_T){
  
  if (!pid)
    throw SignalErr(getPnmb());
  
  kill(pid,signal);
  pid=ppid=0;
}
