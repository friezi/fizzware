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

#include "bashautomat.hpp"

using namespace exc;

Automat::Automat(char *pattern, SpecSymbols *spec_symbols)
  : statelist(0), nos(0){

  this->build(pattern,spec_symbols);
}

Automat::Automat(char *pattern)
  : statelist(0), nos(0){

  SpecSymbols spec_symbols('*','?','[',']','!','\\');

  this->build(pattern,&spec_symbols);
}

Automat::Automat(char *pattern, SpecSymbols *spec_symbols, State *mwcstate,
		 State *firstsymbolstate, Charlist *firstsymbolset,
		 Charlist *symbolset, bool mwcflag, bool firstsymbolflag)
  : statelist(0), nos(0){
  
  this->spec_symbols.multi_wildc=spec_symbols->multi_wildc;
  this->spec_symbols.one_wildc=spec_symbols->one_wildc;
  this->spec_symbols.br_open=spec_symbols->br_open;
  this->spec_symbols.br_close=spec_symbols->br_close;
  this->spec_symbols.exclude=spec_symbols->exclude;
  this->spec_symbols.quote=spec_symbols->quote;

  bildSubautomat(pattern,mwcstate,firstsymbolstate,firstsymbolset,symbolset,
		 mwcflag,firstsymbolflag);
  
}

void Automat::build(char *pattern, SpecSymbols *spec_symbols){

  this->spec_symbols.multi_wildc=spec_symbols->multi_wildc;
  this->spec_symbols.one_wildc=spec_symbols->one_wildc;
  this->spec_symbols.br_open=spec_symbols->br_open;
  this->spec_symbols.br_close=spec_symbols->br_close;
  this->spec_symbols.exclude=spec_symbols->exclude;
  this->spec_symbols.quote=spec_symbols->quote;

  bildSubautomat(pattern,0,0,0,0,false,false);
}

void Automat::bildSubautomat(char *pattern, State *mwcstate,
			     State *firstsymbolstate,
			     Charlist *firstsymbolset, Charlist *symbolset,
			     bool mwcflag, bool firstsymbolflag){

  State *startstate, *newstate=0;
  Statelist *currstatelist;
  Transresult *transresult;
  Charlist *resultlist=0;
  char symbol=0, prevsymbol=0;
  bool endflag=false, quoteflag=false;

  // following flags are important in the case of s.th. like this: [!...],
  // they indicate, that the symbols in the brackets should be excluded
  // from the fitting symbols.
  bool symbolsexclflag=false, mwcexclflag=false;

  char mwc=this->spec_symbols.multi_wildc;
  char owc=this->spec_symbols.one_wildc;
  char openbr=this->spec_symbols.br_open;
  char closebr=this->spec_symbols.br_close;
  char excl=this->spec_symbols.exclude;
  char quote=this->spec_symbols.quote;
  
  currstatelist = new Statelist();

  if (!(startstate = new State))
    throw OutOfMemException();

  this->appendState(startstate);

  currstatelist->appendState(startstate);
  
  do{
    symbol=*pattern++;

    if (!symbol){  // if there's no more symbol

      currstatelist->setMode(STATE_END);
      endflag=true;

    } else if (symbol==quote && !quoteflag){  // quoting

      quoteflag=true;

      continue;

    } else if (symbol==mwc && !quoteflag){  // multi_wildcard?

      if (firstsymbolflag)
	continue;

      // if ea new "*" occurs, we've to delete all the connections to any
      // possible former mwcstate or firstsymbolstate 
      currstatelist->deleteTransresults(); // delete all entrys
//        currstate->transresult=0; // reset pointer
      
      // also reset the firstsymbolset
      delete firstsymbolset;
      firstsymbolset=0;

      mwcflag=true;  // set multi_wildcard-flag
      mwcexclflag=false;
      firstsymbolflag=true; // next symbol is the first after wildc.

      mwcstate = new State;
      this->appendState(mwcstate);

      prevsymbol=symbol;

      continue;

    } else if (symbol==owc && !quoteflag){

      symbolsexclflag=true;
      if (firstsymbolflag)
	mwcexclflag=symbolsexclflag;
      
    } else if (symbol==openbr && !quoteflag){

      pattern+=this->getAltRegion(pattern,&symbolset,closebr,
				  excl,symbolsexclflag);

      if (firstsymbolflag)
	mwcexclflag=symbolsexclflag;

    } else if (symbol!=owc || quoteflag){

      if (!(symbolset = new Charlist(symbol))){
	delete this->statelist;
	throw OutOfMemException();
      }

    }

    if (firstsymbolflag){

      resultlist=symbolset->copy();

      if (!firstsymbolset)
	firstsymbolset=resultlist;
      else
	firstsymbolset->append(resultlist);

    }

    if (symbol!=owc || quoteflag){
      // not when one_wildcard

      // we're somewhere behind a mwc, thus we've to make a connection to
      // the mwcstate
      if (mwcflag){

	if (!(transresult = new Transresult)){
	  delete this->statelist;
	  throw OutOfMemException();
	}

	if (!mwcexclflag && !symbolsexclflag){
	  // e.g. "...*[eu][fu]..."
  
	  resultlist=symbolset->merge(firstsymbolset);
	  transresult->invert=1;

	} else if (!mwcexclflag && symbolsexclflag){
	  // e.g. "...*[eu][!fu]..."
  
	  resultlist=symbolset->substract(firstsymbolset);
	  transresult->invert=0;

	} else if (mwcexclflag && !symbolsexclflag){
	  // e.g. "...[!eu][fu]..."

	  resultlist=firstsymbolset->substract(symbolset);
	  transresult->invert=0;

	} else if (mwcexclflag && symbolsexclflag){
	  // e.g. "...[!eu][!fu]..."

	  resultlist=symbolset->cut(firstsymbolset);
	  transresult->invert=0;

	}
	
	// only if we have symbols for the transition we should draw
	// a connection
	if (transresult->getInvert() || resultlist){
	  
	  transresult->resultstate=mwcstate;
	  transresult->appendCharlist(resultlist);
	  
	  currstatelist->appendTransresult(transresult);
	}
	delete transresult;
	transresult=0;
	
      }
      
    }
    
    if (!endflag){
      
      if (!(newstate = new State)){
	delete this->statelist;
	throw OutOfMemException();
      }

      this->appendState(newstate);

      if (!(transresult = new Transresult)){
	delete this->statelist;
	throw OutOfMemException();
      }

      if (symbol!=owc || quoteflag){

	resultlist=symbolset->copy();
	transresult->appendCharlist(resultlist);

	if (symbolsexclflag)
	  transresult->invert=1;

      }
      else
	transresult->invert=1;

      transresult->resultstate=newstate;
      
      currstatelist->appendTransresult(transresult);
      delete transresult;
      transresult=0;

      if (firstsymbolflag && symbol!=mwc)
	firstsymbolstate=newstate;

    }
    
    // if we're inside an mwc region we've to draw a symbolconnection
    // to the firstsymbolstate, but only if we're at least one step
    // behind the firstsymbol
    // this has also to be done if endflag is set already, because from
    // the last state has to be drawn a connection to firstsymbolstate, too

    if (mwcflag && !firstsymbolflag && !symbolsexclflag){

      if (!(transresult = new Transresult)){
	delete this->statelist;
	throw OutOfMemException();
      }
      
      if (!mwcexclflag && !symbolsexclflag){
	// e.g. "...*[eu][fu]..."

	resultlist=firstsymbolset->substract(symbolset);
	transresult->invert=0;

      } else if (!mwcexclflag && symbolsexclflag){
	// e.g. "...*[eu][!fu]..."
	
	resultlist=symbolset->cut(firstsymbolset);
	transresult->invert=0;

      } else if (mwcexclflag && !symbolsexclflag){
	// e.g. "...[!eu][fu]..."

	resultlist=symbolset->merge(firstsymbolset);
	transresult->invert=1;

      } else if (mwcexclflag && symbolsexclflag){
	// e.g. "...[!eu][!fu]..." 

	resultlist=symbolset->substract(firstsymbolset);
	transresult->invert=0;
	
      }
      
      // only if we have symbols for the transition we should draw
      // a connection
      if (transresult->getInvert() || resultlist){

	transresult->resultstate=firstsymbolstate;
	transresult->appendCharlist(resultlist);
	
	currstatelist->appendTransresult(transresult);
      }
      delete transresult;
      transresult=0;
      
    }
    
    // the mwcstate shall be a copy of the currstate, thus
    // it has to equalized
    if (mwcstate && firstsymbolflag)
      mwcstate->equalize(currstatelist->getState());
    
    currstatelist->replace(newstate);

    // if a "*" is followed by a "?" (e.g. "...*?..."), the firstsymbolstate
    // has to "fall-through" to the current state 
    if (symbol==owc && !quoteflag && mwcflag
	&& (prevsymbol==owc || prevsymbol==mwc))
      firstsymbolstate=currstatelist->getState();
    
    delete symbolset;
    symbolset=0;

    // reset flags
    firstsymbolflag=false;
    symbolsexclflag=false;
    quoteflag=false;

    prevsymbol=symbol;
    
  } while (!endflag);

  delete firstsymbolset;

}

Automat::~Automat(){
  
  delete this->getStatelist();
}

void Automat::appendState(State *state){

  int nmb;
  Statelist *statelist=0;

  if (!this)
    throw NullPointerException("class: Automat\tmethod: appendState");
  
  nmb=this->getNos()+1;

  if (!this->getStatelist()){

    if (!(statelist = new Statelist))
      throw OutOfMemException();

    this->statelist=statelist;
  }
  
  state->setNmb(nmb);
  this->getStatelist()->appendState(state);
  this->nos++;
}

// get all the symbols in the brackets (e.g. "[...]") and set the exclflag
// if it started with the excl-symbol (e.g. "[!...]")
int Automat::getAltRegion(char *strng, Charlist **slist, char closebr,
		 char excl, bool& exclflag){

  int count=0;
  Charlist *cl;

  if (*strng==excl){

    exclflag=true;
    count=1;

    strng++;

  }
  
  if (*strng==closebr)
    return count+1;
  else if (!(*strng))
    return count;
  
  while (*strng && *strng!=closebr){
    if (!(*slist)->isMember(*strng)){

      if (!(cl = new Charlist(*strng))){
	delete this->statelist;
	throw OutOfMemException();
      }

      if (!*slist)
	*slist=cl;
      else
	(*slist)->append(cl);

    }

    strng++;
    count++;

  }

  if (*strng==closebr)
    return count+1;
  else
    return count;
}

char Automat::accept(char *word){

  State *currstate;
  char symbol;

  if(!this)
    return 0;

  currstate=this->getStatelist()->getState();

  while ((symbol=*word++) && currstate)
    currstate=currstate->getResultstate(symbol);

  if (!currstate)
    return 0;
  else
    return (currstate->getMode()==STATE_END);
}

void Automat::show(){

  State *curr;
  Statelist *currstl;
  Transresult *transresult;
  Charlist *transsymbol;

  if (!this)
    return;

  currstl=this->getStatelist();
  if (!currstl)
    throw NullPointerException("class: Automat\tmethod: show");
  curr=currstl->getState();

  while (curr){

    if (curr->getMode()==STATE_END)
      std::cout << "end";
    std::cout << "state: " << curr->nmb << "\n";

    transresult=curr->getTransresult();

    while (transresult){

      switch (transresult->getInvert()){
      case 0: std::cout << "go with:\t"; break;
      case 1: std::cout << "all except:\t";break;
      }

      transsymbol=transresult->getTranssymbol();

      while (transsymbol){

	std::cout << transsymbol->getSymbol();
	transsymbol=transsymbol->getNext();

      }

      std::cout << "\t-> state " << transresult->getResultstate()->getNmb()
	   << "\n";
      transresult=transresult->getNext();

    }

    std::cout << "\n";

    currstl=currstl->getNext();
    curr=currstl ? currstl->getState() : 0;

  }
}

Automat::SpecSymbols::SpecSymbols(const SpecSymbols& spec_symbols){


  multi_wildc=spec_symbols.multi_wildc;
  one_wildc=spec_symbols.one_wildc;
  br_open=spec_symbols.br_open;
  br_close=spec_symbols.br_close;
  exclude=spec_symbols.exclude;
  quote=spec_symbols.quote;
}

Automat::SpecSymbols::SpecSymbols(char mwc, char owc, char bro,
			 char brc, char ex, char qu){
  multi_wildc=mwc;
  one_wildc=owc;
  br_open=bro;
  br_close=brc;
  exclude=ex;
  quote=qu;
}


Automat::State::~State(){

  delete this->getTransresult();
}

void Automat::State::appendTransresult(Transresult *transresult){

  Transresult *currtr;

  if (!this)
    throw NullPointerException("class: State\tmethod: appendTransresult\t");

  if (!this->getTransresult()){
    this->transresult=transresult;
    return;
  }

  currtr=this->getTransresult();
  while (currtr->getNext())
    currtr=currtr->getNext();
  currtr->next=transresult;
}

void Automat::State::equalize(State *state){

  this->mode=state->getMode();

  if (this->getTransresult())
    delete this->getTransresult();
  this->transresult=0;

  if (state->getTransresult())
    this->transresult = new Transresult(*state->getTransresult());
}

Automat::State *Automat::State::getResultstate(char symbol){

  Transresult *transresult;
  Charlist *transsymbol;

  transresult=this->getTransresult();

  while (transresult){

    transsymbol=transresult->getTranssymbol();

    if (!transresult->getInvert()){

      while (transsymbol){

	if (transsymbol->getSymbol()==symbol)
	  return transresult->getResultstate();

	transsymbol=transsymbol->getNext();

      }
    }
    else {

      while (transsymbol){

	if (transsymbol->getSymbol()==symbol)
	  break;

	transsymbol=transsymbol->getNext();

      }

      if (!transsymbol)
	return transresult->getResultstate();

    }

    transresult=transresult->getNext();

  }

  return 0;
}

Automat::Statelist::~Statelist(){
  
  delete this->getNext();
  delete this->getState();
}

void Automat::Statelist::appendState(State *state){

  Statelist *currstl, *newstl=0;

  if (!this)
    throw NullPointerException("class: Statelist\tmethod: appendState");

  currstl=this;

  while (currstl->getNext())
    currstl=currstl->getNext();

  if (currstl->getState()){

    if (!(newstl = new Statelist))
      throw OutOfMemException();

    currstl->next=newstl;
    currstl->getNext()->appendState(state);

  } else
    currstl->state=state;
  
}

void Automat::Statelist::deleteEntrys(){

  if (this->getNext()){
    this->getNext()->deleteEntrys();
    delete this->getNext();
  }

  this->state=0;
}

void Automat::Statelist::replace(State *state){

  this->deleteEntrys();
  this->appendState(state);
}

void Automat::Statelist::setMode(char mode){

  if (this->getNext())
    this->getNext()->setMode(mode);

  if (this->getState())
    this->getState()->setMode(mode);
}

void Automat::Statelist::appendTransresult(Transresult *tr){

  Transresult *transresult;

  if (this->getNext())
    this->getNext()->appendTransresult(tr);

  if (this->getState()){
    transresult = new Transresult(*tr);
    this->getState()->appendTransresult(transresult);
  }
}

void Automat::Statelist::deleteTransresults(){

  if (this->getNext())
    this->getNext()->deleteTransresults();

  if (this->getState()){
    delete this->getState()->getTransresult();
    this->getState()->transresult=0;
  }
}

Automat::Transresult::Transresult(const Transresult& tr)
  : next(0),invert(0),transsymbol(0),resultstate(0){

  if (tr.getNext())
    this->next = new Transresult(*tr.getNext());

  this->invert=tr.getInvert();

  if (tr.getTranssymbol())
    this->transsymbol = new Charlist(*tr.getTranssymbol());

  this->resultstate=tr.getResultstate();
}

Automat::Transresult::~Transresult(){

  delete this->getNext();
  delete this->getTranssymbol();
}

void Automat::Transresult::appendCharlist(Charlist *transsymbol){

     if (!this)
       throw NullPointerException
	 ("class: Transresult\tmethod: appendCharlist");
     
     if (!this->getTranssymbol()){
       this->transsymbol=transsymbol;
       return;
     }
     this->getTranssymbol()->append(transsymbol);

}

Automat::Charlist::Charlist(const Charlist& cl)
  : next(0), symbol(0){

  if (cl.getNext())
    this->next = new Charlist(*cl.getNext());

  this->symbol=cl.getSymbol();
}

Automat::Charlist::~Charlist(){

  delete this->getNext();
}


void Automat::Charlist::append(Charlist *symbol){

  Charlist *curr;

  if (!this)
    throw NullPointerException("class: Charlist\tmethod: append");
    
  curr=this;
  while (curr->getNext())
    curr=curr->getNext();
  curr->next=symbol;
}

char Automat::Charlist::isMember(char symbol){

  Charlist *curr;

  curr=this;

  while(curr){
    if (symbol==curr->getSymbol())
      return 1;
    curr=curr->getNext();
  }
  return 0;
}

char Automat::Charlist::listIsMember(Charlist *checklist){

  Charlist *curr;

  curr=checklist;

  while (curr){
    if (!this->isMember(curr->getSymbol()))
      return 0;
    curr=curr->getNext();
  }
  return 1;
}

Automat::Charlist *Automat::Charlist::merge(Charlist *list){

  Charlist *resultlist, *resultlist2;

  resultlist=this->copy();
  resultlist2=list->substract(this);

  if (!resultlist)
    resultlist=resultlist2;
  else
    resultlist->append(resultlist2);

  return resultlist;
}

Automat::Charlist *Automat::Charlist::cut(Charlist *list){

  Charlist *resultlist=0, *cl_elem, *curr;

  curr=this;

  while (curr){

    if (list->isMember(curr->getSymbol())){

      if (!(cl_elem = new Charlist(curr->getSymbol()))){
	delete resultlist;
	throw OutOfMemException();
      }

      if (!resultlist)
	resultlist=cl_elem;
      else
	resultlist->append(cl_elem);
  
    }
    
    curr=curr->getNext();

  }
  return resultlist;
}

Automat::Charlist *Automat::Charlist::substract(Charlist *list){

  Charlist *resultlist=0, *cl_elem, *curr;
  
  curr=this;

  while (curr){

    if (!list->isMember(curr->getSymbol())){

      if (!(cl_elem = new Charlist(curr->getSymbol()))){
	delete resultlist;
	throw OutOfMemException();
      }

      if (!resultlist)
	resultlist=cl_elem;
      else
	resultlist->append(cl_elem);

    }

    curr=curr->getNext();

  }
  return resultlist;
}

Automat::Charlist *Automat::Charlist::copy(){

  Charlist *resultlist=0, *cl_elem, *curr;

  curr=this;

  while (curr){

    if (!(cl_elem = new Charlist(curr->getSymbol()))){
      delete resultlist;
      throw OutOfMemException();
    }

    if (!resultlist)
      resultlist=cl_elem;
    else
      resultlist->append(cl_elem);

    curr=curr->getNext();

  }

  return resultlist;
}
