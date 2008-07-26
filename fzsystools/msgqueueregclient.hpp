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
   @file msgqueueregclient.hpp
   @author Friedemann Zintel
*/

#ifndef _FZST_MSGQUEUEREGCLIENT_HPP_
#define _FZST_MSGQUEUEREGCLIENT_HPP_

#include <stdio.h>
#include <errno.h>
#include <exception.hpp>
#include <tcpclient.hpp>
#include <datastructures.hpp>

#define EOS_TOKEN 0
#define BAD_TOKEN 1
#define STRING_TOKEN 2
#define ULONG_TOKEN 3

namespace sys{

  /**
     @class MsgQueueRegClient
     @brief A client which can connect to a messagequeue-registration-server
  */
  class MsgQueueRegClient{

  protected:
  
    /**
       @class Token
       @internal
       @brief A Token, produced by a Scanner
    */
    class Token{
    
    protected:
    
      char *buffer;
      size_t tsize;
    
      int type;
    
      Token(){}
    
      Token(const Token &){}
    
    public:
    
      Token(char *buffer, size_t tsize) : buffer(buffer), tsize(tsize), type(STRING_TOKEN){}
    
      ~Token();
    
      char *get(){ return buffer; }
    
      size_t size(){ return tsize; }
    
      int getType(){ return type; }
    
      // konvertiert die sizeof(long) bytes vom Token in long
      unsigned long toLong();
    
      int operator==(char *s);
      int operator==(const std::string &s);
    
    };
  
    /**
       @class BadToken
       @internal
       @brief an invalid Token
    */
    class BadToken : Token{
    
    protected:
    
      BadToken(const BadToken &){}
    
      BadToken(){ type = BAD_TOKEN; }
    };
  
    /**
       @class EOSToken
       @internal
       @brief for End-Of-Stream
    */
    class EOSToken : public Token{
    
    protected:
    
      EOSToken(const EOSToken &){}
    
    public:
    
      EOSToken(){ type = EOS_TOKEN; }
    };
  
    /**
       @class ULongToken
       @internal
       @brief holds an ulong-value
    */
    class ULongToken : public Token{
    
    protected:
    
      unsigned long value;
    
      ULongToken(const ULongToken &){}
    
    public:
    
      ULongToken(unsigned long value);
    
      unsigned long getValue(){ return value; }
    
    };
  
    /**
       @class Scanner
       @internal
       @brief for scanning the output of a socket, connected to a MsgQueueRegClient
    */
    class Scanner{
    
    private:
    
      static const unsigned long BLKSIZE;
    
      // Filedeskriptor
      int fd;
    
      Scanner(Scanner &){}
    
    public:
    
      // Konstruktor
      Scanner(int fd) : fd(fd){}
    
      ~Scanner() { fd = 0; }
    
      Token *nextToken() throw (exc::Exception<Scanner>,exc::Exception<BadToken>,exc::ExceptionBase);
    
      ULongToken *nextULongToken() throw (exc::Exception<Scanner>,exc::Exception<BadToken>,exc::ExceptionBase);
    
      int isBlank(char c);
    };
  
    sys::TCPClient *msgqueueregclient;
  
    MsgQueueRegClient(const MsgQueueRegClient &){}
  
  public:

    static const std::string default_socket_path;

    /**
       @exception Exception<MsgQueueRegClient>
       @exception ExceptionBase
    */
    MsgQueueRegClient(std::string socket_path) throw(exc::Exception<MsgQueueRegClient>,exc::ExceptionBase);
  
    ~MsgQueueRegClient();

    /**
       @brief Commands the msgpd to install a new MsgQueue
       @param name the desired name for the MsgQueue
       @exception Exception<MsgQueueRegClient>
       @exception ExceptionBase
    */
    bool newPort(std::string name) throw (exc::Exception<MsgQueueRegClient>,exc::ExceptionBase);

    /**
       @brief Commands the msgpd to add an already existing MsgQueue
       @param name the desired name for the MsgQueue
       @param id the id of the message-queue
       @exception Exception<MsgQueueRegClient>
       @exception ExceptionBase
    */
    bool addPort(std::string name, int id) throw (exc::Exception<MsgQueueRegClient>,exc::ExceptionBase);

    /**
       @brief Commands the msgpd to find the ID of a MsgQueue
       @param name the desired name for the MsgQueue
       @exception Exception<MsgQueueRegClient>
       @exception ExceptionBase
    */
    int findPort(std::string name) throw (exc::Exception<MsgQueueRegClient>,exc::ExceptionBase);

    /**
       @brief Commands the msgpd to remove a MsgQueue of the list
       @param name the desired name for the MsgQueue
       @exception Exception<MsgQueueRegClient>
       @exception ExceptionBase
    */
    bool removePort(std::string name) throw (exc::Exception<MsgQueueRegClient>,exc::ExceptionBase);

  };

}

#endif
