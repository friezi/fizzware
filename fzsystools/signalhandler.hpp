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
   @file signalhandler.hpp
   @author Friedemann Zintel
*/

#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <exception.hpp>
#include <string>
#include <signal.h>

/**
   @class SignalHandler
   @brief For installing Unix/Linux-signalhandlers
*/
class SignalHandler{

private:

  struct sigaction handler, oldhandler;
  int signr;

  SignalHandler(const SignalHandler&){}

public:

  // the flag init tells, if the handler should be directly setup, or later via init()
  /**
     @param signr the signal-number
     @param handler the handler-routine
     @param init if true, the method init() will be called directly, otherwise not
     @param flags flags for system-call sigaction()
     @exception Exception<SignalHandler>;
     @note if init == false, you have to call init() by yourself
  */
  SignalHandler(int signr, void (*handler)(int), bool init = false, int flags = 0) throw(Exception<SignalHandler>);

  /**
     @brief setup the handler, will call sigaction()
     @exception Exception<SignalHandler>;
  */
  void init() throw(Exception<SignalHandler>);

  /**
     @brief wrap-around for sigemptyset
     @exception Exception<SignalHandler>;
  */
  void emptyset() throw(Exception<SignalHandler>);

  /**
     @brief wrap-around for sigfillset
     @exception Exception<SignalHandler>;
  */
  void fillset() throw(Exception<SignalHandler>);

  /**
     @brief wrap-around for sigaddset
     @param signr the signal-number
     @exception Exception<SignalHandler>;
  */
  void addset(int signr) throw(Exception<SignalHandler>);

  /**
     @brief wrap-around for sigdelset
     @param signr the signal-number
     @exception Exception<SignalHandler>;
  */
  void delset(int signr) throw(Exception<SignalHandler>);

  /**
     @brief wrap-around for sigismember
     @return value of system-call sigismember()
  */
  int ismember(int signr);

  /**
     @brief wrap-around for system-call siginterrupt
     @param flag the flag
     @exception Exception<SignalHandler>;
  */
  void siginterrupt(int flag) throw(Exception<SignalHandler>);
};

#endif
