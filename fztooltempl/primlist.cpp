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

#include <fztooltempl/primlist.hpp>
#include <stdio.h>

using namespace exc;

PrimList::PrimList(long number=0) : number(number),numbers(0),
				    nsize(0),pos(0),currentnmb(0),nmbprim(0){
  

  if (number<PRIMLIST_START)
    return;

  soc=sizeof(char)*8;

  // create bitmask for numbers starting with 2
  nsize=(int)((number-1)/soc)+((number-1)%soc ? 1 : 0);

  numbers = new char[nsize];

  // set all numbers
  for (long i=0;i<nsize;i++)
    numbers[i]|=0xffB;

//    primlist=new NumList();

  // Erathostenes:

  while (pos+PRIMLIST_START<=number){
    // number at pos is a prime
    nmbprim++;

    // clear all multiples from primenumber in bitmask
    for (long cpos=2;cpos*(pos+PRIMLIST_START)<=number;cpos++)
      numbers[(long)((cpos*(pos+PRIMLIST_START)-PRIMLIST_START)/soc)]&=
	~(1<<((cpos*(pos+PRIMLIST_START)-PRIMLIST_START)%soc));

    setPosToNextPrim();

  }
  pos=0;currentnmb=1;
}

PrimList::~PrimList(){

  delete [] numbers;
}

void PrimList::setPosToNextPrim(void) throw(Exception<PrimList>){

  char diff;
  
  diff=(pos+PRIMLIST_START==FIRSTPRIM)?1:2;

  pos+=diff;
  
  // position of the next set bit is next prime
  while ((pos+PRIMLIST_START<=number)
	 && !(numbers[(long)pos/soc]&(1<<(pos%soc))))
    pos+=diff;  // difference to next prim must be even
}

void PrimList::setPosToPrevPrim(void) throw(Exception<PrimList>){

  char diff;
  
  if (pos==0)
    return;

  diff=(pos+PRIMLIST_START==FIRSTPRIM+1)?1:2;

  pos-=diff;

    // position of the next set bit is next prime
    while ((pos+PRIMLIST_START > 0)
	   && !(numbers[(long)pos/soc]&(1<<(pos%soc))))
      pos-=diff;  // difference to next prim must be even
}

long PrimList::get(long index) throw(Exception<PrimList>){

  void (PrimList::*spool)(void);
  signed char diff=0;

  if (index<1)
    throw Exception<PrimList>("out of bounds!");

  if (index<currentnmb){
    spool=&PrimList::setPosToPrevPrim;
    diff=-1;
  }
  else if(index>currentnmb){
    if (index>nmbprim)
      throw Exception<PrimList>("out of bounds!");
    spool=&PrimList::setPosToNextPrim;
    diff=1;
  }
  else{
    if (pos>=nsize*soc || pos<0)
      throw Exception<PrimList>("out of bounds!");
    return pos+PRIMLIST_START;
  }

  while (currentnmb!=index){
    (this->*spool)();
    currentnmb+=diff;
  }
  
  if (pos>=nsize*soc || pos<0)
    throw Exception<PrimList>("out of bounds!");
 
  return pos+PRIMLIST_START;
}

void PrimList::show(void){
  pos=0;
  currentnmb=1;

  while (pos+PRIMLIST_START <= number){
    std::cout << pos+PRIMLIST_START << "\n";
    setPosToNextPrim();
  }

  pos=0;
  currentnmb=1;
}
