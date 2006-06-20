/*
  this sourcefile belongs to the programm focal,
  a mathematical formular-calculator
  Copyright (C) 1999-2006 Friedemann Zintel

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

#ifndef LINESCANNER_HPP
#define LINESCANNER_HPP

#include <string>

class LineScanner{

private:

  static char delimitors[];

  static char separators[];
  
  unsigned int index;

  char * line;

public:

  LineScanner(char * line) : index(0), line(line){}

  std::string nextToken();

private:

  bool isDelimitor(char c);

  bool isSeparator(char c);

  void skipSeparators();

};

#endif

    
