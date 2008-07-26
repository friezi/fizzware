#include "scanner.hpp"

using namespace std;
using namespace exc;
using namespace ds;

Token::~Token(){
  
  tsize = 0;
  free(buffer);
}

unsigned long Token::toLong(){
  
  unsigned long value = 0;
  size_t size = sizeof(unsigned long);
  
  if (this->size() < sizeof(unsigned long))
    size = this->size();
  
  memcpy((char *)&value,this->get(),size);
  
  return value;
}

int Token::operator==(char *s){

  return (strncmp(this->buffer,s,tsize) == 0);
}

int Token::operator==(const string &s){

  return (strncmp(this->buffer,s.data(),tsize) == 0);
}

ULongToken::ULongToken(unsigned long value){

  this->value = value;
  this->buffer = NULL;
  this->tsize = (sizeof(unsigned long));
  this->type = ULONG_TOKEN;
}

const unsigned long Scanner::BLKSIZE = 10;

int Scanner::isblank(char c){

  return ((c == ' ') || (c == '\t'));
}

Token *Scanner::nextToken() throw (Exception<Scanner>,Exception<BadToken>,ExceptionBase){

  DynamicBuffer<char> buffer(BLKSIZE);
  MemPointer<char> input(false);
  unsigned int count;
  char tmp;

  if (fd == 0)
    throw Exception<Scanner>("Filedescriptor is zero!");
  
  unsigned long size = 0;
  while (true){

    ssize_t r;

    r = read(fd,&tmp,1);

    if (r == 0){
      if (size) // es wurde was gelesen, nun ist EOS 
	break;
      else  // EOS ohne vorheriges lesen
	return new EOSToken();
    }
    
    if (r == -1) //  Fehler
      throw Exception<BadToken>(strerror(errno));

    if ((tmp=='\0'))
      break;
    else if(isspace(tmp) && !(isblank(tmp)))
      continue;
    
    buffer.put(tmp);
    size++;
  }
  
  input = buffer.merge();
  count = buffer.getElements();
  
  return new Token(input.get(),count);
}

ULongToken *Scanner::nextULongToken() throw (Exception<Scanner>,Exception<BadToken>,ExceptionBase){
  
  unsigned long tmp = 0;
  
  if (fd == 0)
    throw Exception<Scanner>("Filedescriptor is zero!");
  
  ssize_t r;
  
  r = read(fd,&tmp,sizeof(unsigned long));
  
  if (r == 0)
    throw Exception<EOSToken>();
  
  if (r == -1)
    throw Exception<BadToken>(strerror(errno));
  
  return new ULongToken(ntohl(tmp));
}
