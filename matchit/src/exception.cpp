/*
    Copyright (C) 1999-2004 Friedemann Zintel

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

/**
   @file exception.cpp
   @author Friedemann Zintel
*/

#include <exception.hpp>
  
Exception_T::Exception_T(const std::string &id){
    this->errormsg = "";
    this->spc_id = id;
    this->id_errormsg = this->spc_id;
  }
  
Exception_T::Exception_T(const std::string &id, const std::string &errormsg){
    this->errormsg = errormsg;
    this->spc_id = id;
    this->id_errormsg = this->spc_id + ": " + this->errormsg;
  }

std::string Exception_T::skipDigits(const std::string &s){

  const char *sptr = s.c_str();

  while (isdigit(*sptr))
    sptr++;

  return std::string(sptr);
}

std::string Exception_T::skipLetters(const std::string &s){

  const char *sptr = s.c_str();

  while (isalpha(*sptr))
    sptr++;

  return std::string(sptr);
}

std::ostream& operator<<(std::ostream& ostr, const Exception_T& e){

  ostr << e.getIdMsg();
  return ostr;
}

void Exception_T::show() const{

  std::cerr << *this << std::endl;
}
