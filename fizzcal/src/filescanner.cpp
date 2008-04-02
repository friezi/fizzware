/*
  this sourcefile belongs to the programm focal,
  a mathematical formula-calculator
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

#include "filescanner.hpp"

using namespace std;

char FileScanner::delimitors[] = {'\0','\n','\r'};

char FileScanner::separators[] = {' ','\t'};

string FileScanner::nextLine(){

  char c = 0;
  bool emptyline = true;

  while ( emptyline == true ){

    if ( file.eof() == true )
      return "";

    file.read(&c,1);

    if ( skipDelimitors(c) == false )
      return "";

    if ( skipSeparators(c) == false )
      return "";

    if ( isSeparator(c) == false && isDelimitor(c) == false )
      return getString(c);
    
  }
  
  return "";

}

string FileScanner::getString(char & c){

  ostringstream content;

  content << c;

  file.read(&c,1);

  while ( isDelimitor(c) == false && file.eof() == false ){

    content << c;
    file.read(&c,1);

  }

  return content.str();

}


bool FileScanner::isDelimitor(char c){

  for ( unsigned int i = 0; i<(sizeof(delimitors)/sizeof(char)); i++)
    if ( c == delimitors[i] )
      return true;

  return false;

}

bool FileScanner::isSeparator(char c){

  for ( unsigned int i = 0; i<(sizeof(separators)/sizeof(char)); i++)
    if ( c == separators[i] )
      return true;

  return false;

}

bool FileScanner::skipSeparators(char & c){

  while ( true ){
    
    if ( isSeparator(c) == false )
      return true;
    
    if ( file.eof() == true )
      return false;
    
    file.read(&c,1);

  }

}

bool FileScanner::skipDelimitors(char & c){

  while ( true ){
    
    if ( isDelimitor(c) == false )
      return true;
    
    if ( file.eof() == true )
      return false;
    
    file.read(&c,1);

  }

}
