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
   @file semaphor.hpp
   @author Friedemann Zintel
*/

#ifndef FZSYSTOOLS_SEMAPHOR_HPP
#define FZSYSTOOLS_SEMAPHOR_HPP

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include <fztooltempl/exception.hpp>

// doxygen has problems with multiple defnitions of the same namespace
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace sys{

#endif

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
  /* union semun is defined by including <sys/sem.h> */
#else
  /* according to X/OPEN we have to define it ourselves */
  union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
  };
#endif

  /**
     @class Semaphor
     @brief To setup a semaphor (note: belongs to the namespace sys)
     @note belongs to the namespace sys
  */
  class Semaphor{
  
  private:

    // Exception-Classes
    /** @internal */
    class GetErr{};

    /** @internal */
    class CtrlErr{};

    /** @internal */
    class OpErr{};
  
    // constants:

    static const int OBTAIN=-1;
    static const int RELEASE=1;

    int id;   // ID returned by system
    char flag;
  
    void PV(int op) throw (exc::SubException<OpErr,Semaphor>);

    // copyconstructor:
    Semaphor(const Semaphor&){}

  protected:

    /// The system-error
    std::string err(){ return std::string(strerror(errno)); }
  
  public:

    // for "flag" field
    static const char S_NEW=1;
    static const char S_OLD=2;
  
    /**
       @param initval the initial value
       @param key either a known key for the semaphor or IPC_PRIVATE
       @param flag either S_NEW or S_OLD
       @throw Exception<Semaphor>
       @throw SubException<GetErr,Semaphor>
       @throw SubException<CtrlErr,Semaphor>
    */
    Semaphor(int initval, key_t key=IPC_PRIVATE, char flag=S_NEW)
      throw (exc::Exception<Semaphor>,exc::SubException<GetErr,Semaphor>,exc::SubException<CtrlErr,Semaphor>);
  
    /**
       Removes a semaphor, if flag was "S_NEW"
    */
    ~Semaphor();

    /// to get exclusive access to the semaphor
    void P() throw (exc::SubException<OpErr,Semaphor>){ PV(OBTAIN); }
    /// to release the semaphor
    void V() throw (exc::SubException<OpErr,Semaphor>){ PV(RELEASE); }

    /// blocks a semaphor that no process gets access
    void block() throw(exc::SubException<CtrlErr,Semaphor>);
    /// removes a semaphor
    void remove();
  
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

}

#endif

#endif
