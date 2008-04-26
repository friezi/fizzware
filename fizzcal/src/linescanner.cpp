/*
  this sourcefile belongs to the programm fizzcal,
  a calculator/evaluator for arithmetic expresions.
  Copyright (C) 1999-2008 Friedemann Zintel

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

#include "linescanner.hpp"

using namespace std;

char LineScanner::delimitors[] = {'\0','\n','\r'};

char LineScanner::separators[] = {' ','\t'};

string LineScanner::nextToken(){

  unsigned int count = 0;

  skipSeparators();

  unsigned int startpos = index;

  while ( isDelimitor(line[index]) == false && isSeparator(line[index]) == false ){

    index++;
    count++;

  }
  
    return string(&line[startpos],count);

} 

bool LineScanner::isDelimitor(char c){

  for ( unsigned int i = 0; i<(sizeof(delimitors)/sizeof(char)); i++)
    if ( c == delimitors[i] )
      return true;

  return false;

}

bool LineScanner::isSeparator(char c){

  for ( unsigned int i = 0; i<(sizeof(separators)/sizeof(char)); i++)
    if ( c == separators[i] )
      return true;

  return false;

}

void LineScanner::skipSeparators(){

  while ( isSeparator(line[index]) )
    index++;

}

