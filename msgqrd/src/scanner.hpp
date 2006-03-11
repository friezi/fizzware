#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <stdio.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <exception.hpp>
#include <storage.hpp>

#define EOS_TOKEN 0
#define BAD_TOKEN 1
#define STRING_TOKEN 2
#define ULONG_TOKEN 3

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

class BadToken : Token{

protected:

  BadToken(const BadToken &){}

  BadToken(){ type = BAD_TOKEN; }
};

class EOSToken : public Token{

protected:

  EOSToken(const EOSToken &){}

public:

  EOSToken(){ type = EOS_TOKEN; }
};

class ULongToken : public Token{

protected:

  unsigned long value;

  ULongToken(const ULongToken &){}

public:

  ULongToken(unsigned long value);

  unsigned long getValue(){ return value; }

};

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

  Token *nextToken() throw (Exception<Scanner>,Exception<BadToken>,Exception_T);

  ULongToken *nextULongToken() throw (Exception<Scanner>,Exception<BadToken>,Exception_T);

  int isblank(char c);
};


#endif
