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

#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <exception.hpp>

class Process;
class CreateErr;
class ActiveErr;
class AbortErr;
class TerminateErr;
class SignalErr;
class WaitErr;
class RuntimeErr;

/**
   @class Process
   You can't define objects from this class directly, define subclasses instead from which you can
   instantiate objects
   @brief For invoking programs as child-processes
   @note Don't forget to define the abstract method main()
*/
class Process{
  
  
private:
  
  static unsigned int pcount;
  
  pid_t pid;
  pid_t ppid;
  unsigned int pnmb;
  bool setprot;
  bool pprotect;

protected:
  
  Process(const Process&){}
  
  /// constructor: must be called from subclasses!
  Process(bool daemon = false);
  
  // destructor:
  virtual ~Process(){
    if (!pprotect){
      this->terminate();
      this->wait();
    }
  }

  /**
     @brief Will be called from launch(); you have to overwrite this method!
     @return what ever you want to return
  */
  virtual int main() = 0;
  
public:

  /**
     Launches the process as a seperate process.
     @brief Launches the process
     @exception CreateErr
     @exception ActiveErr
     @exception RuntimeErr
  */
  void launch() throw (ExceptionBase);

  /**
     @brief Sends the signal SIGKILL to process
     @exception AbortErr
  */
  void abort() throw(ExceptionBase);

  /**
     @brief sends the signal SIGTERM to process
     @exception TerminateErr
  */
  void terminate() throw(ExceptionBase);

  /**
     @brief Sends a signal to the process
     @param signal the signal
     @exception SignalErr
  */
  void sendSignal(int signal) throw(ExceptionBase);

  /**
     @brief Waits for process to exit
     @param status will be filled with exit-status of the process, if status != 0
     @exception WaitErr
  */
  pid_t wait(int *status = 0) throw(ExceptionBase);

  /**
     @brief Get the Process-ID
     @return Process-ID
  */
  pid_t getPid(){ return pid; }

  /**
     @brief Get the Parent-Process-ID
     @return Parent-Process-ID
  */
  pid_t getPpid(){ return ppid; }

  /**
     @brief Get the class-internal Process-number
     @return class-internal Process-number
  */
  unsigned int getPnmb(){ return pnmb; }
  
};

class CreateErr : public Exception<Process>{
  
private:
  
  unsigned int pnmb;
  
public:
  
  CreateErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  CreateErr(const char * const errormsg,unsigned int pnmb)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  CreateErr(const std::string &errormsg,unsigned int pnmb)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  CreateErr(const std::string &id, const std::string &errormsg,unsigned int pnmb)
    : Exception<Process>(id,errormsg),pnmb(pnmb){}

  virtual void show() const {
    std::cout << this->getIdMsg() << ": couldn't create process! "
	 << "internal processobjectnumber: " << pnmb << "\n";
  }
};

class ActiveErr : public Exception<Process>{
   
private:
  
  unsigned int pnmb;
 
public:
  
  ActiveErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  ActiveErr(const char * const errormsg, unsigned int pnmb)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  ActiveErr(const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  ActiveErr(const std::string &id, const std::string &errormsg)
    : Exception<Process>(id,errormsg),pnmb(pnmb){}
  virtual void show() const {
    std::cout << this->getIdMsg() << ": process is active or zombie! "
	 << "internal processobjectnumber: " << pnmb << "\n";
  }
};

class AbortErr : public Exception<Process>{
  
private:
  
  unsigned int pnmb;
  
public:
  
  AbortErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  AbortErr(const char * const errormsg)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  AbortErr(const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  AbortErr(const std::string &id, const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  virtual void show() const {
    std::cout << this->getIdMsg() << ": process to abort is not active! "
	 << "internal processobjectnumber: " << pnmb << "\n";
  }
};

class TerminateErr : public Exception<Process>{
  
private:
  
  unsigned int pnmb;
  
public:
  
  TerminateErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  TerminateErr(const char * const errormsg)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  TerminateErr(const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  TerminateErr(const std::string &id, const std::string &errormsg)
    : Exception<Process>(id,errormsg),pnmb(pnmb){}

  virtual void show() const {
    std::cout << this->getIdMsg() << ": process to terminate is not active! "
	 << "internal processobjectnumber: " << pnmb << "\n";
  }
};

class SignalErr : public Exception<Process>{
  
private:
  
  unsigned int pnmb;
  
public:
  
  SignalErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  SignalErr(const char * const errormsg)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  SignalErr(const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  SignalErr(const std::string &id, const std::string &errormsg)
    : Exception<Process>(id,errormsg),pnmb(pnmb){}

  virtual void show() const {
    std::cout << this->getIdMsg() << ": process to send signal to is not active! "
	 << "internal processobjectnumber: " << pnmb << "\n";
  }
};

class WaitErr : public Exception<Process>{
  
private:
  
  unsigned int pnmb;

public:
  
  WaitErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  WaitErr(const char * const errormsg)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  WaitErr(const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  WaitErr(const std::string &id, const std::string &errormsg)
    : Exception<Process>(id,errormsg),pnmb(pnmb){}

  virtual void show() const {
    std::cout << this->getIdMsg() << ": process to wait for is not active! "
	 << "internal processobjectnumber: " << pnmb << "\n";
  }
};

class RuntimeErr : public Exception<Process>{
  
private:
  
  unsigned int pnmb;

public:
  
  RuntimeErr(unsigned int pnmb)
    : Exception<Process>(""),pnmb(pnmb){}

  RuntimeErr(const char * const errormsg)
    : Exception<Process>(std::string(errormsg)),pnmb(pnmb){}
  
  RuntimeErr(const std::string &errormsg)
    : Exception<Process>(errormsg),pnmb(pnmb){}

  RuntimeErr(const std::string &id, const std::string &errormsg)
    : Exception<Process>(id,errormsg),pnmb(pnmb){}

  virtual void show() const {
    std::cout << this->getIdMsg() << ": internal processobjectnumber: "
	 << pnmb << "\n";
  }
};

#endif
