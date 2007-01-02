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
using namespace util;

char String::blanks[] = {' ','\t'};

bool String::isABlank(char c){

  for ( unsigned int i = 0; i < sizeof(blanks); i++ )
    if ( c == blanks[i] )
      return true;

  return false;
}
    
string String::trunc(){

  unsigned int leftend;
  signed int rightstart;
  string truncstring;

  for ( leftend=0; leftend<this->length(); leftend++ )
    if ( !isABlank((*this)[leftend]) )
      break;

  truncstring = this->erase(0,leftend);

  for ( rightstart=(unsigned int)truncstring.length()-1; rightstart>=0; rightstart-- )
    if ( !isABlank((*this)[rightstart]) )
      break;

  rightstart++;

  return truncstring.erase(rightstart,truncstring.length());

}
