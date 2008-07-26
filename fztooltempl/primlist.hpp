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
   @file primlist.hpp
   @author Friedemann Zintel
*/

#ifndef _FZTT_PRIMLIST_HPP_
#define _FZTT_PRIMLIST_HPP_

#include <iostream>
#include <string>
#include <exception.hpp>

/**
   @class PrimList
   @brief a class to hold primenumbers in a list 
*/
class PrimList{

public:

  //constants:
  static const int PRIMLIST_START=2;
  static const int FIRSTPRIM=2;
  
private:
  long number; // the highest number
  char *numbers;
  long nsize;  //size of array
  long pos; 
  char soc;  //size of char;
  long currentnmb;
  long nmbprim;  // number of prims

  /**
     set the position in the bitfield to the next primenumber
     @exception Exception<PrimList>
  */
  void setPosToNextPrim(void) throw(exc::Exception<PrimList>);

  /**
     set the position in the bitfield to the previous primenumber
     @exception Exception<PrimList>
  */
  void setPosToPrevPrim(void) throw(exc::Exception<PrimList>);
 

public:

  /**
     @param number the maximum number to which all prims should be generated
  */
  PrimList(long number);
  ~PrimList();

  /**
     @brief get the primenumber at position 'index'
     @param index the index for the primenumber
     @return the primenumber
     @exception Exception<PrimList>
  */
  long get(long index) throw(exc::Exception<PrimList>);

  /**
     @brief print the primelist
  */
  void show(void);
  
  /**
     @brief get the number of elements
     @return number of primes in the list
  */
  int elements(void){
    return nmbprim;
  }
};

#endif

