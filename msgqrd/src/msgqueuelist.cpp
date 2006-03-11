#include "msgqueuelist.hpp"

using namespace std;

MsgQueueEntry::MsgQueueEntry(const string &name, const int &id) : id(id){

  this->name = name;
}

int MsgQueueList::find(const string &name){

  MsgQueueList::iterator result;

  if ( (result = std::find(this->begin(),this->end(),name)) == this->end() )
    return -1;
  return (*result).getId();
}

void MsgQueueList::insert(const string &name, const int id){

  MsgQueueList::iterator curr = this->begin(), end = this->end();
  MsgQueueEntry ne(name,id);
  
  while ( curr != end ){
    if ( (*curr).getName() < name ){
      curr++;
    }
    else
      break;
  }
  list<MsgQueueEntry>::insert(curr,ne);
}

void MsgQueueList::erase(const string &name){

  MsgQueueList::iterator curr = this->begin(), end = this->end();

  while ( curr != end){
    if ( (*curr).getName() != name )
      curr++;
    else{
      list<MsgQueueEntry>::erase(curr);
      break;
    }
  }
}

void MsgQueueList::show(){

  MsgQueueList::iterator curr = this->begin(), end = this->end();

  while ( curr != end ){
    cout << (*curr).getName() << "\t" << (*curr).getId() << "\n";
    curr++;
  }
}
