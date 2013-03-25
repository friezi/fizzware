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

#include <fztooltempl/utils.hpp>

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
    
string String::trim(const string &s){

  size_t leftend, rightstart;
  string trimstring = s;

  for ( leftend = 0; leftend < s.length(); leftend++ ){
    if ( !isABlank(trimstring[leftend]) ){
      break;
    }
  }

  trimstring.erase(0,leftend);

  if ( !trimstring.empty() ){
    
    for ( rightstart = trimstring.length()-1; rightstart >= 0; rightstart-- ){
      if ( !isABlank(trimstring[rightstart]) ){
	break;
      }
    }
    
    rightstart++;
    
    trimstring.erase(rightstart,trimstring.length());
    
  }
  
  return trimstring;
  
}

string String::toLower(const std::string &s){

  string lower = s;
  size_t pos = 0;

  for ( string::const_iterator it = s.begin(); it != s.end(); it++, pos++ )
    lower[pos] = latinToLower(*it);
  
  return lower;

}

string String::toUpper(const std::string &s){

  string upper = s;
  size_t pos = 0;

  for ( string::const_iterator it = s.begin(); it != s.end(); it++, pos++ )
    upper[pos] = latinToUpper(*it);
  
  return upper;
  
}

bool String::containsChar(const std::string &s, const char c){

  if ( s.find(c) != string::npos )
    return true;

  return false;

}

const IdentityConverter<std::string> string_identity_converter = IdentityConverter<std::string>();
