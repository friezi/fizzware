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
   @file messagequeue.hpp
   @author Friedemann Zintel
*/

#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <errno.h>
#include <exception.hpp>

/**
   the defines from <sys/stat.h>
*/
#define MSG_UR S_IRUSR
#define MSG_UW S_IWUSR
#define MSG_GR S_IRGRP
#define MSG_GW S_IWGRP
#define MSG_OR S_IROTH
#define MSG_OW S_IWOTH

// doxygen has problems with multiple defnitions of the same namespace
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace sys{

#endif

  class Message;
  class MessageQueue;

  /**
     @class Message
     @brief Messages which can be sent to a MessageQueue (note: belongs to the namespace sys)
     @note belongs to the namespace sys
  */
  class Message{
  
    friend class MessageQueue;

  protected:

    char *msg;
    size_t msize;

    Message(){}
    Message(const Message&){}

    // Kopiert Inhalt von Buffer und schreibt MessageID davor
    /**
       The content of buffer will be copied and the MessageID will be set on top of this.
       @brief Initialization of the message with the message-content
       @param buffer the buffer which contains the message-data
       @param type the message-type according to unix-standard
       @exception Exception<Message>
    */
    void init(const char * const buffer, long type) throw(Exception<Message>);

  public:

    /**
       @param buffer contains the data
       @param msize the size of the data in bytes
       @param type the message-type !!! PLEASE MORE INFORMATION !!!
       @exception Exception<Message>
    */
    Message(const char * const buffer, size_t msize, long type = getpid()) throw(Exception<Message>);

    ~Message();

    // Sende Message an MessageQueue
    /**
       @brief send a message to a unix/linux-messagequeue (see MessageQueue)
       @param id the id of the messagequeue (see MessageQueue)
       @param flag kommt noch
       @exception Exception<Message>
    */
    void send(int id, int flag = 0) const throw(Exception<Message>);

    // Setze Typ nachträglich
    /**
       @brief Modify the type after construction
       @param type the type to be set
    */
    void setType(long type);

    // liefere Typ
    /**
       @brief return the type
       @return the type
    */
    long getType() const ;

    //liefere Typlaenge
    /**
       @brief return the length, i.e. size of the type
       @return the size
    */
    size_t sizeOfType() const ;

    // liefere Laenge der Message
    /**
       @brief return the size of the message
       @return the size
    */
    size_t size() const { return msize; }

    // liefere msg
    /**
       @brief return the whole message
       @return pointer to the message
    */
    char *getMsg() const { return msg; }

    // liefere nur Daten ohne Typ
    /**
       @brief return only data whithout typeinfo of the message
       @return pointer to data
    */
    char *getData() const { return &msg[sizeof(long)]; }

    // string zurueckliefern
    /**
       @brief return the data as a string
       @return well, the string
       @exception Exception<Message>
    */
    std::string getStringMsg() const throw(Exception<Message>);

    /**
       @brief return the mesage as ascii-string-line
       @return the contents of the message (including newline)
    */
    std::string contents() const ;
  };

  /**
     @class MessageQueue
     @brief installs a unix/linux-messagequeue (note: belongs to the namespace sys)
     @note belongs to the namespace sys
  */
  class MessageQueue{

  private:

    key_t key;
    int id;
    int flag;

    // Copyconstructor: not allowed
    MessageQueue(const MessageQueue&){}

    // Einrichten oder oeffnen
    void get() throw(Exception<MessageQueue>);

  public:

    static const int MSGMAX;

    /**
       @brief checks, if a message-queue with the given id exists
       @param id the id for the message-queue
       @return true, if message-queue exists, false otherwise
    */
    static bool exists(int id);

    /**
       will set: key = IPC_PRIVATE
       flag = MSG_UR|MSG_UW
    */
    MessageQueue() : key(IPC_PRIVATE), id(0), flag(MSG_UR|MSG_UW) {}

    /**
       @param key !!! INFOS !!!
       @param flag
       @param init
       @exception Exception<MessageQueue>
       @todo Infos for parameter "key" 
    */
    MessageQueue(key_t key, int flag = 0, bool init = false) throw(Exception<MessageQueue>);

    ~MessageQueue() throw(Exception<MessageQueue>);

    // Initialisieren: Oeffnen/Einrichten
    /**
       @brief Initializes the MessageQueue
       @exception Exception<MessageQueue>
       @remark Has to be called manually if init == false in constructor-call MessageQueue(...)
    */
    void init() throw(Exception<MessageQueue>);

    /**
       @brief Return the id of the unix/linux-messagequeue
       @return the id*/
    int getId() { return id; }

    // Message erhalten ; typ == 0 : nimm erste Message aus der Queue
    /**
       @brief Receive a message
       @param maxlength the maximum length the message-data is allowed to be
       @param typ the type of the message (see the unix/linux-documentation on messagequeues
       @param flag the flag of the message (see the unix/linux-documentation on messagequeues
       @return pointer to the message
       @exception Exception<MessageQueue>
       @exception ExceptionBase
    */
    Message *receive(size_t maxlength = 4096, long typ = 0, int flag = MSG_NOERROR) throw(Exception<MessageQueue>,ExceptionBase);

    // Abfragen/Aendern/Loeschen
    /**
       @brief For asking, modifying or erasing (calls directly the msgctl() function)
       @param kdo the kommando
       @param buffer !!!TO BE FILLED!!!
       @exception Exception<MessageQueue>
    */
    void control(int kdo, struct msqid_ds *buffer) throw(Exception<MessageQueue>);
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

}

#endif

#endif
