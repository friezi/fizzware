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
   @file bashautomat.hpp
   @brief defines classes for constructing automats to match bash-like expressions
   @author Friedemann Zintel
*/

#ifndef BASHAUTOMAT_HPP
#define BASHAUTOMAT_HPP

#include <iostream>
#include <fztooltempl/exception.hpp>

#define STATE_NORMAL 0
#define STATE_END 1

/**
   @class Automat
   @brief for matching words on bash-like patterns
*/
class Automat{

protected:
  class State;
  class Statelist;
  class Transresult;
  class Charlist;
  class SpecSymbols;
  
  class SpecSymbols{
    
  public:
    char multi_wildc;   /* multi_wildcard: wildcard for many symbols */
    char one_wildc;     /* one_wildcard: wildcard for exactly one symbol */
    char br_open;       /* openbracket for alternate chars */
    char br_close;      /* closebracket for alternate chars */
    char exclude;           /* if alternates should be excluded */
    char quote;         /* quoting symbol */
    
    /* Constructors: */
    
    SpecSymbols(){}
    SpecSymbols(const SpecSymbols& spec_symbols);
    SpecSymbols(char mwc, char owc, char bro, char brc, char ex, char qu);
    
  };
  
  class State{
    
  public:
    int nmb;       /* number */
    char mode;     /* STATE_NORMAL or STATE_END */
    Transresult *transresult;  /* transitionsymbol and resultstate */
    
    /* Constructor: */
    State() : nmb(0),mode(0),transresult(0){}
    
    /* Destructor: */
    ~State();
    
    void setNmb(int nmb) { this->nmb=nmb; }
    int getNmb() const{ return nmb; }
    void setMode(char mode) { this->mode=mode; }
    char getMode() const{ return mode; }
    Transresult *getTransresult() const{ return transresult; }
    void appendTransresult(Transresult *transresult);
    
    // equalize copies the properties of state except nmb,
    // all objects are copy-constructed
    void equalize(State *state);
    State *getResultstate(char symbol);
    
  };
  
  class Statelist {
    
  protected:
    Statelist *next;   /* next state in list */
    State *state;
    
  public:
    
    
    /* Constructor: */
    Statelist() : next(0),state(0){}
    
    /* Destructor: */
    ~Statelist();
    
    Statelist *getNext() const{ return next; }
    State *getState() const{ return state; }
    void appendState(State *state);
    void deleteEntrys();
    void replace(State *state);
    void setMode(char mode);
    void appendTransresult(Transresult *tr);
    void deleteTransresults();
  };
  
  class Transresult{
    
  public:
    Transresult *next;   /* next transresult for this state */
    
    /* if invert=1 then all symbols EXCEPT those in List
       will lead to resultstate */
    char invert;
    
    /* a set of symbols which allow, res. not allow the transition */
    Charlist *transsymbol;
    
    State *resultstate;
    
    /* Constructor: */
    Transresult() : next(0),invert(0),transsymbol(0),resultstate(0){}
    
    /* Destructor: */
    ~Transresult();
    
    /* copy-constructor: */
    Transresult(const Transresult& tr);
    
    Transresult *getNext() const{ return next; }
    char getInvert() const{ return invert; }
    Charlist *getTranssymbol() const{ return transsymbol; }
    State *getResultstate() const{ return resultstate; }
    void appendCharlist(Charlist *transsymbol);
    
  };
  
  class Charlist{
    
  public:
    Charlist *next;
    char symbol;
    
    /* Constructor: */
    Charlist(char symbol) : next(0), symbol(symbol){}
    
    /* copy-constructor: */
    Charlist(const Charlist& cl);
    
    /* Destructor: */
    ~Charlist();
    
    Charlist *getNext() const{ return next; }
    char getSymbol() const{ return symbol; }
    void append(Charlist *symbol);
    char isMember(char symbol);
    char listIsMember(Charlist *checklist);
    Charlist *merge(Charlist *list);
    Charlist *cut(Charlist *list);
    Charlist *substract(Charlist *list);
    Charlist *copy();
    
  };
  
private:
  Statelist *statelist;
  int nos;            // number of states
  SpecSymbols spec_symbols;

  // private constructor:
  Automat(char *pattern, SpecSymbols *spec_symbols, State *mwcstate,
	  State *firstsymbolstate, Charlist *firstsymbolset,
	  Charlist *symbolset, bool mwcflag, bool firstsymbolflag);
  
  void build(char *pattern, SpecSymbols *spec_symbols);
  void bildSubautomat(char *pattern, State *mwcstate, State *firstsymbolstate,
		      Charlist *firstsymbolset, Charlist *symbolset,
		      bool mwcflag, bool firstsymbolflag);
  void appendState(State *state);
  Statelist *getStatelist() const{ return statelist; }
  int getAltRegion(char *strng, Charlist **slist, char closebr,
		   char excl, bool& exclflag);

public:
  
  // constructors:
  Automat(char *pattern);
  Automat(char *pattern, SpecSymbols *spec_symbols);

  // destructor:
  ~Automat();

  int getNos() const{ return nos; }
  void show();
  char accept(char *word);

};


#endif
