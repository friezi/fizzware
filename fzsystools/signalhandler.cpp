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

#include <signalhandler.hpp>

SignalHandler::SignalHandler(int signr, void (*handler)(int), bool init, int flags)
  throw(Exception<SignalHandler>) : signr(signr){

  this->handler.sa_handler = handler;
  ::sigemptyset(&(this->handler.sa_mask));
  this->handler.sa_flags = flags;

  if (init == true)
    this->init();
}

void SignalHandler::init() throw(Exception<SignalHandler>){

  if ( ::sigaction(signr,&handler,&oldhandler) == -1 )
    throw Exception<SignalHandler>("sigaction failed!");
}

void SignalHandler::emptyset() throw(Exception<SignalHandler>){
  if ( ::sigemptyset(&handler.sa_mask) == -1 )
    throw Exception<SignalHandler>("sigemptyset failed!");
}

void SignalHandler::fillset() throw(Exception<SignalHandler>){
  if ( ::sigfillset(&handler.sa_mask) == -1 )
    throw Exception<SignalHandler>("sigfillset failed!");
}

void SignalHandler::addset(int signr) throw(Exception<SignalHandler>){
  if ( ::sigaddset(&handler.sa_mask, signr) == -1 )
    throw Exception<SignalHandler>("sigaddset failed!");
}

void SignalHandler::delset(int signr) throw(Exception<SignalHandler>){
  if ( ::sigdelset(&handler.sa_mask, signr) == -1 )
    throw Exception<SignalHandler>("sigdelset failed!");
}

int SignalHandler::ismember(int signr){
  return ::sigismember(&handler.sa_mask, signr);
}

void SignalHandler::siginterrupt(int flag) throw(Exception<SignalHandler>){
  if ( ::siginterrupt(signr,flag) == -1 )
    throw Exception<SignalHandler>("siginterrupt: invalid signal-number!");
}
