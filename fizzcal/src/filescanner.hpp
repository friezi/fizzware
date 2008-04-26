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

#ifndef FILESCANNER_HPP
#define FILESCANNER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

class FileScanner{

private:

  static char delimitors[];

  static char separators[];

  std::ifstream & file;

public:

  FileScanner(std::ifstream & file) : file(file){}

  std::string nextLine();

private:

  bool isDelimitor(char c);

  bool isSeparator(char c);

  bool skipSeparators(char & c);

  bool skipDelimitors(char & c);

  std::string getString(char & c);

};

#endif
