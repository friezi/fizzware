/*
  Copyright (C) 1999-2006 Friedemann Zintel

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

#include "utils.hpp"

using namespace std;
using namespace exc;
using namespace utils;

char String::blanks[] = {' ','\t'};

bool String::isABlank(char c){

  for ( unsigned int i = 0; i < sizeof(blanks); i++ )
    if ( c == blanks[i] )
      return true;

  return false;

}
    
String String::trunc() const {

  size_t leftend, rightstart;
  String truncstring = *this;

  for ( leftend = 0; leftend < this->length(); leftend++ )
    if ( !isABlank(truncstring[leftend]) )
      break;

  truncstring.erase(0,leftend);

  if ( truncstring.empty() == false ){
    
    for ( rightstart = truncstring.length()-1; rightstart >= 0; rightstart-- )
      if ( !isABlank(truncstring[rightstart]) )
	break;
    
    rightstart++;
    
    truncstring.erase(rightstart,truncstring.length());
    
  }
  
  return truncstring;
  
}

String String::toLower() const {

  String lower = *this;
  size_t pos = 0;

  for ( String::const_iterator it = this->begin(); it != this->end(); it++, pos++ )
    lower[pos] = latinToLower(*it);
  
  return lower;

}

String String::toUpper() const {

  String upper = *this;
  size_t pos = 0;

  for ( String::const_iterator it = this->begin(); it != this->end(); it++, pos++ )
    upper[pos] = latinToUpper(*it);
  
  return upper;
  
}

bool String::containsChar(const char & c) const {

  if ( this->find(c) != string::npos )
    return true;

  return false;

}

String & String::operator+(const bool & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const unsigned char & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const char & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const unsigned int & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const int & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const unsigned long & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const long & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const double & value){

  ostringstream stream;

  stream << value;

  (*this) += stream.str();

  return *this;

}

String & String::operator+(const string & value){

  (*this) += value;

  return *this;

}

String & String::operator+(const char *value){

  (*this) += value;

  return *this;

}
