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

#ifndef MSGQUEUELIST_HPP
#define MSGQUEUELIST_HPP

#include <iostream>
#include <string>
#include <list>
#include <algorithm>

class MsgQueueEntry{
  
private:
  
  std::string name;
  int id;
  
public:
  
  // Konstruktor:
  MsgQueueEntry(const std::string &name, const int &id);
  
  // Destruktor:
  ~MsgQueueEntry(){}
  
  int getId() const { return id; }
  const std::string &getName() const { return name; }

  // wichtig für std::find() !!!
  int operator!=(const std::string &s) const { return (this->getName() != s); }
  int operator==(const std::string &s){ return (this->getName() == s); }
  int operator>(const std::string &s) const { return (this->getName() > s); }
  int operator<(const std::string &s) const { return (this->getName() < s); }
  int operator>=(const std::string &s) const { return (this->getName() >= s); }
  int operator<=(const std::string &s) const { return (this->getName() <= s); }
  
};

class MsgQueueList : public std::list<MsgQueueEntry>{
  
private:
  
  //Kopierkonstruktor: unerlaubt
  MsgQueueList(const MsgQueueList&){}
  
public:

  MsgQueueList(){}
  ~MsgQueueList(){}
  
  // neue MsgQueue eintragen: alphabetisch
  void insert(const std::string &name, const int id);
  
  // MsgQueue aus Liste entfernen
  void erase(const std::string &name);
  
  // Suche nach MsgQueue mit Namen
  // Rückgabe: id, falls Erfolg oder -1
  int find(const std::string &name);
  
  void show ();
  
};

#endif
