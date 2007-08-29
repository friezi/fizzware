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

#include <mathexpression.hpp>

#define SUM "Sum"
#define PROD "Prod"
#define FLIST "sin","cos","tan","asin","acos","atan","sinh","cosh","tanh", \
    "asinh","acosh","atanh","ln","ld","log","exp","sgn","tst",SUM,PROD
#define OPLIST '+','-','*','/','\\','%','^','!','@','=','(',')','[',']',',',';'
  
  using namespace std;
using namespace mexp;

Variable::Variable(const char *name, Value *value, char protect)
  throw (ExceptionBase) : value(value), protect(protect), next(0){

  if (!(this->name = new char[strlen(name)+1]))
    throw OutOfMemException();
  strcpy(this->name,name);
}

Variable::~Variable(){

  delete [] this->name;
  delete value;

}

void Variable::setValue(Value *value){
  
  if ( this->value )
    delete this->value;
  
  this->value = value;
  
}

VariableList::VariableList(const VariableList& vl) throw (ExceptionBase) : first(0), last(0), modified(false){
  
  const Variable *curr;

  curr = vl.first;
  while (curr){

    this->insert(curr->getName(),curr->getValue()->clone(),curr->getProtect());
    curr = curr->getNext();

  }
}

VariableList::~VariableList(){

  Variable *curr, *next;

  curr = first;
  while (curr){
    next = curr->getNext();
    delete curr;
    curr = next;
  }
}

void VariableList::insert(const char *name, Value *value, char protect)
  throw (ExceptionBase){

  Variable *ve;

  if ((ve = isMember(name))){
    if (!ve->getProtect()){

      ve->setValue(value);

    }else
      throw EvalException("redefinition not possible!",
			  ve->getName());
  } else {
    if (!(ve = new Variable(name,value,protect)))
      throw OutOfMemException();
      
    if (!first)
      first = ve;
    if (last)
      last->setNext(ve);
    last = ve;
  }
}

void VariableList::remove(const char *name) throw (Exception<VariableList>){

  Variable *curr, *prev = 0;

  curr = first;
  while (curr){
    if (!strcmp(name,curr->getName())){
      if (curr == first)
	first=curr->getNext();
      if (curr == last)
	last=prev;
      if (prev)
	prev->setNext(curr->getNext());
      delete curr;
      return;
    }
    prev = curr;
    curr = curr->getNext();
  }
  throw Exception<VariableList>("not defined!");
}

Value *VariableList::getValue(const char *name) const throw (ExceptionBase){

  const Variable *ve;

  ve = first;
  while (ve){
    if (!strcmp(ve->getName(),name))
      return ve->getValue()->clone();
    ve = ve->getNext();
  }
  throw EvalException("unknown variable!",name);
}

Variable *VariableList::isMember(const char *name) const{

  const Variable *ve;
  
  ve=first;
  while (ve){
    if (!strcmp(ve->getName(),name))
      return const_cast<Variable *>(ve);
    ve=ve->getNext();
  }
  return 0;
}

void VariableList::unprotect(const char *name){

  Variable *curr=0;

  if (name)
    this->isMember(name)->setProtect(0);
  else{
    curr=this->first;
    while (curr){
      curr->setProtect(0);
      curr=curr->getNext();
    }
  }
  return;
}

void VariableList::print(streamsize precision) const{
  cout << toString(true,precision);
}

string VariableList::toString(bool include_protected, streamsize precision) const{

  ostringstream vars;

  const Variable *ve;

  ve=first;

  while(ve){

    if ( ve->getProtect() == false || include_protected == true )
      vars << ve->getName() << "=" << ve->getValue()->toString(precision) << endl;

    ve = ve->getNext();

  }

  return vars.str();

}

Function::Function(const char *name, MathExpression *paramlist,
		   MathExpression *body)
  : paramlist(paramlist), body(body), next(0){

  this->name = new char[strlen(name)+1];
  strcpy(this->name,name);
}

Function::~Function(){

  delete [] this->name;
  delete this->paramlist;
  delete this->body;
}

FunctionList::~FunctionList(){

  Function *curr, *next;

  curr=this->first;
  while (curr){
    next=curr->next;
    delete curr;
    curr=next;
  }
}

bool FunctionList::isMember(const char *name) const{

  return (bool)this->get(name);
}

Function *FunctionList::get(const char *name) const{

  const Function *fe;
  
  fe=first;
  while (fe){
    if (!strcmp(fe->getName(),name))
      return const_cast<Function *>(fe);
    fe=fe->next;
  }
  return 0;
}


void FunctionList::insert(Function *fe) throw(Exception<FunctionList>){

  if (this->isMember(fe->getName()))
    throw Exception<FunctionList>("already in list!");

  if (!first)
    first=fe;
  if (last)
    last->next=fe;
  last=fe;
}

void FunctionList::remove(const char *name) throw(Exception<FunctionList>){

  Function *curr, *prev=0;

  curr=first;
  while (curr){
    if (!strcmp(name,curr->getName())){
      if (curr==first)
	first=curr->next;
      if (curr==last)
	last=prev;
      if (prev)
	prev->next=curr->next;
      delete curr;
      return;
    }
    prev=curr;
    curr=curr->next;
  }
  throw Exception<FunctionList>("not defined!");
}

void FunctionList::print(streamsize precision, const char *name){

  Function *curr;
  bool match=true, stop=false;

  if ( name )
    match = false;

  curr = this->first;
  while ( curr ){

    if ( name )

      if ( !strcmp(curr->getName(),name) ){

	match=true;
	stop=true;

      }

    if ( match ){
      std::cout << curr->getName();

      if ( curr->paramlist )
	curr->paramlist->print(precision);
      else
	std::cout << "()";

      std::cout << "=";
      curr->body->print(precision);
      std::cout << "\n";
    }
    curr = curr->next;
    if ( stop )
      break;
  }
}

string FunctionList::toString(streamsize precision) const{

  ostringstream funs;

  Function *curr;

  funs.setf(ios::fixed);
  funs.precision(precision);

  curr = this->first;

  while ( curr ){

    funs << curr->getName();

    if (curr->paramlist)
      funs << curr->paramlist->toString(precision);
    else
      funs << "()";

    funs << "=";
    funs << curr->body->toString(precision);
    funs << endl;

    curr = curr->next;

  }

  return funs.str();

}

string Complex::toString(std::streamsize precision) const{
  
  ostringstream value;
  ostringstream re;
  
  re.setf(ios::fixed);
  re.precision(precision);
  
  if ( this->getRe() != 0 || this->isReal() )
    re << this->getRe();
  
  value << MathExpression::skipTrailingZeros(re.str());

  if ( !this->isReal() ){
    
    if ( this->getIm() > 0 )
      if ( this->getRe() != 0 )
	value << "+";
    
    ostringstream im;
  
    im.setf(ios::fixed);
    im.precision(precision);

    if ( this->getIm() == -1 )
      im << "-";
    else if ( this->getIm() == 1 )
      ; // no output necessary
    else
      im << this->getIm();

    // TODO
    // imaginary unit
    value << MathExpression::skipTrailingZeros(im.str()) << 'i';

  }

  return value.str();
  
}

Complex *Complex::assertComplex(Value *value){
  
  if ( value->getType() != Value::COMPLEX )
    throw EvalException("operand to complex must be complex-type!");
  
  return (Complex *)value;
  
}

Complex *Complex::assertReal(Value *value){
  
  if ( value->getType() != Value::COMPLEX )
    throw EvalException("operand to complex must be complex-type!");

  if ( ((Complex *)value)->getIm() != 0 )
    throw EvalException("operation only allowed for real-numbers!");
  
  return (Complex *)value;
  
}

Value *Complex::operator+(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);;
  return new Complex(this->getRe()+rc->getRe(),this->getIm()+rc->getIm());
      
}

Value *Complex::operator-(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);
  return new Complex(getRe()-rc->getRe(),getIm()-rc->getIm());

}
Value *Complex::operator*(Value *right) throw (ExceptionBase){ 
      
  Complex *rc = assertComplex(right);
  return new Complex(getRe()*rc->getRe()-getIm()*rc->getIm(),getRe()*rc->getIm()+getIm()*rc->getRe());
      
}
Value *Complex::operator/(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);
  return new Complex((getRe()*rc->getRe()+getIm()*rc->getIm())/(::pow(rc->getRe(),2)+::pow(rc->getIm(),2)),
		     (getIm()*rc->getRe()-getRe()*rc->getIm())/(::pow(rc->getRe(),2)+::pow(rc->getIm(),2)));
      
}

Value *Complex::integerDivision(Value *right) throw (ExceptionBase){
  
  Complex *rc = assertComplex(right);
  return new Complex((double)((int)((getRe()*rc->getRe()+getIm()*rc->getIm())/(::pow(rc->getRe(),2)+::pow(rc->getIm(),2)))),
		     (double)((int)((getIm()*rc->getRe()-getRe()*rc->getIm())/(::pow(rc->getRe(),2)+::pow(rc->getIm(),2)))));
      
}

Value *Complex::operator%(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);

  if ( this->isReal() && rc->isReal() )
    return new Complex((double)(((int)getRe())%((int)rc->getRe())),0);
  else{
    
    Complex *div = (Complex *)integerDivision(rc);
    Complex * prod = (Complex *)right->operator*(div);
    Complex *diff = (Complex *)this->operator-(prod);

    delete prod;
    delete div;

    return diff;

  }

}

Value *Complex::faculty() throw (ExceptionBase){
  
  assertReal(this);

  return new Complex(MathExpression::faculty(getRe()),0);
      
}

MathExpression::MathExpression(int abs_pos, VariableList *vl, FunctionList *fl) :
  varlist(vl),functionlist(fl),left(0), right(0), pred(0), value(0), type(EMPTY), abs_pos(abs_pos), imaginary_unit('i'){
  
  for (int i=0;i<OP_LEN;i++)
    oprtr[i]=0;
  for (int i=0;i<VARLEN;i++)
    variable[i]=0;

}

MathExpression::MathExpression(const char *expression, VariableList *vl,
			       FunctionList *fl)
  throw (ParseException,ExceptionBase)
  : varlist(vl), functionlist(fl), left(0), right(0), pred(0), value(0), type(EMPTY), abs_pos(1), imaginary_unit('i'){

  MathExpression *top;
  VariableList locals;
  
  for (int i=0;i<OP_LEN;i++)
    oprtr[i]=0;
  for (int i=0;i<VARLEN;i++)
    variable[i]=0;
  
  top=this->parse(expression,locals);
  if (top){
    this->left=top->left;
    this->right=top->right;
    this->pred=top->pred;
    switch (top->getType()){
    case OP:
      this->setOperatorType(top->getOperator());
      break;
    case VAR:
      this->setVariableType(top->getVariable());
      break;
    case VAL:
      // must be a flat copy because 'top' will only be freed and not deleted
      this->setValueType(top->getValue());
      break;
    case EMPTY:
    default:
      this->setType(top->getType());
      break;
    }
  }
  
  try{

    this->checkSyntaxAndOptimize();

  }catch (ParseException &pe){

    delete top;
    throw pe;

  }

  // Es wird nur free() aufgerufen, da die Unteraeste schon in den
  // Klassenvariablen gespeischert wurden und nicht geloescht werden sollen
  free(top);
}

MathExpression::MathExpression(MathExpression *me, VariableList *vl, FunctionList *fl, int abs_pos)
  throw (ParseException,ExceptionBase)
  : varlist(vl), functionlist(fl), left(0), right(0), pred(0), value(0), type(EMPTY), abs_pos(abs_pos){
  
  for (int i=0;i<OP_LEN;i++)
    oprtr[i]=0;
  for (int i=0;i<VARLEN;i++)
    variable[i]=0;
  
  if (!me)
    return;

  setImaginaryUnit(me->getImaginaryUnit());
  
  switch (me->getType()){
  case OP:
    this->setOperatorType(me->getOperator());
    break;
  case VAR:
    this->setVariableType(me->getVariable());
    break;
  case VAL:
    this->setValueType(me->getValue()->clone());
    break;
  case EMPTY:
  default:
    this->setType(EMPTY);
    break;
  }
  if (me->left){
    this->left = new MathExpression(me->left,vl,fl,abs_pos);
    this->left->pred=this;
  }
  if (me->right){
    this->right = new MathExpression(me->right,vl,fl,abs_pos);
    this->right->pred=this;
  }
}

MathExpression::~MathExpression(){
  
  if ( !this )
    return;
  if ( this->left )
    delete this->left;
  if ( this->right )
    delete this->right;
  eraseElements();
  if ( this->value )
    delete this->value;

}

void MathExpression::eraseElements(){

  for( list<MathExpression *>::iterator it = elements.begin(); it != elements.end(); it++ )
    delete *it;
  elements.clear();

}

MathExpression *MathExpression::parse(const char *expr, VariableList& locals)
  throw (ParseException,ExceptionBase){

  int e_indx, priority, offset;
  char exprstring[OP_LEN]={0}, bracketstring[BR_LEN]={0};
  MathExpression *TopNode=0, *ActualNode=0, *PrevNode=0, *actn, *prevn;

  if ( !expr[0] )
    return 0;

  for ( e_indx = 0; expr[e_indx]; ){

    // check for blanks
    if ( isABlank(expr[e_indx]) == true ){

      offset = skipBlanks(expr,e_indx);

      e_indx+=offset;
      abs_pos+=offset;

      // leave loop in case of end of string
      if ( expr[e_indx] == '\0' )
	break;

    }

    if ( expr[e_indx] == ')' ){

      delete TopNode;
      throw ParseException(abs_pos, "invalid syntax!");

    }

    if ( expr[e_indx] == '(' ){
      if ( PrevNode ){
	if ( PrevNode->isOperator() ){
	  if ( PrevNode->pred ){
	    if ( PrevNode->right ){
	      /* hier handelt es sich um Ausdruecke der Form
		 "1+sin2(...), also um eine implizite Multipikation,
		 es muss also ein "*" in den Baum eingefuegt werden*/

	      if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

		delete TopNode;
		throw OutOfMemException();

	      }

	      ActualNode->setOperatorType("*");

	    } else{      /* Prev hat kein ->right */
	      
	      if ( (offset = this->copyBracketContent(bracketstring,
						      &expr[e_indx],'(',')')) == -1 ){
		
		delete TopNode;
		throw ParseException(abs_pos, "missing bracket!");

	      }	      

	      e_indx+=offset;
	      abs_pos-=offset-1;

	      // if nothing's in the brackets, we connect an empty element
	      if ( !(ActualNode = this->parse(bracketstring,locals)) )
		ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	      abs_pos++;

	      this->clearString(bracketstring);

	    }
	  } else{          /* PrevNode has no predecessor */

	    if ( PrevNode->right ){

	      if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

		delete TopNode;
		throw OutOfMemException();

	      }

	      ActualNode->setOperatorType("*");

	    } else{      /* Prev hat kein ->right */

	      if ( (offset = this->copyBracketContent(bracketstring,
						      &expr[e_indx],'(',')')) == -1 ){

		delete TopNode;
		throw ParseException(abs_pos, "missing bracket!");

	      }

	      e_indx+=offset;
	      abs_pos-=offset-1;

	      if ( !(ActualNode=this->parse(bracketstring,locals)) )
		ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	      abs_pos++;

	      this->clearString(bracketstring);

	    }
	  }
	} else{  /* Prev no operator -> digit or variable */
	  if ( PrevNode->isVariable() ){

	    if ( !varlist->isMember(PrevNode->getVariable() )
		 && !locals.isMember(PrevNode->getVariable())){
	      // a functioncall/-definition is following, thus variable
	      // must be operator

	      PrevNode->setOperatorType(PrevNode->getVariable());
	      continue;

	    }

	  }

	  if (!(ActualNode = new MathExpression(abs_pos,varlist,functionlist))){

	    delete TopNode;
	    throw OutOfMemException();

	  }

	  ActualNode->setOperatorType("*");

	}
      } else{ /* kein Prev vorhanden */
	if ( (offset = this->copyBracketContent(bracketstring,&expr[e_indx],'(',')')) == -1 ){

	  delete TopNode;
	  throw ParseException(abs_pos, "missing bracket!");
	  
	}
	
	e_indx+=offset;
	abs_pos-=offset-1;

	if ( !(ActualNode=this->parse(bracketstring,locals)) )
	  ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	abs_pos++;
	
	this->clearString(bracketstring);

      }
    } else if( expr[e_indx]=='[' ){

      if ( !PrevNode ){

	delete TopNode;
	throw ParseException(abs_pos, "invalid operator \"[\"!");

      }
      if ( !PrevNode->isOperator() ){

	delete TopNode;
	throw ParseException(abs_pos, "invalid operator \"[\"!");

      }
      
      if ( !strcmp(PrevNode->getOperator(),"log")
	   || !strcmp(PrevNode->getOperator(),SUM)
	   || !strcmp(PrevNode->getOperator(),PROD) ){

	if ( PrevNode->left ){

	  delete TopNode;
	  throw ParseException(abs_pos, "invalid syntax!");

	}

	if ( (offset = this->copyBracketContent(bracketstring,&expr[e_indx],'[',']')) == -1 ){

	  delete TopNode;
	  throw ParseException(abs_pos, "missing bracket!");

	}

	e_indx+=offset;
	abs_pos-=offset-1;

	if ( !(ActualNode=this->parse(bracketstring,locals)) )
	  ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	abs_pos++;

	// variables defined in Sum or Prod must be added to local Scope
	ActualNode->addVariablesToList(&locals);

	PrevNode->left=ActualNode;
	ActualNode->pred=PrevNode;
	this->clearString(bracketstring);
	continue;

      } else{

	delete TopNode;
	throw ParseException(abs_pos, "invalid operator \"[\"!");

      }
    } else{        /* kein Bracket */

      if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

	delete TopNode;
	throw OutOfMemException();

      }

      if ( checkDigit(expr[e_indx]) ){
	if ( PrevNode ){
	  if ( PrevNode->isOperator() ){
	    if ( PrevNode->right ){

	      ActualNode->setOperatorType("*");

	    } else{ /* Prev hat kein right */

	      offset = this->copyFloatContent(exprstring,&expr[e_indx]);
	      e_indx+=offset;

	      ActualNode->setValueType(new Complex(atof(exprstring)));
	      this->clearString(exprstring);

	    }
	  } else{  /* Prev kein operator
		      ( Actual=Zahl, Prev=Zahl => Prev->pred=ln o.a") */
	    ActualNode->setOperatorType("*");

	  }

	} else{ /* kein Prev */

	  offset = this->copyFloatContent(exprstring,&expr[e_indx]);
	  e_indx+=offset;

	  ActualNode->setValueType(new Complex(atof(exprstring)));
	  this->clearString(exprstring);

	}

      } else{               /* keine Zahl */
	if ( PrevNode ){
	  if ( checkLetter(expr[e_indx]) ){
	    if ( PrevNode->isOperator() ){
	      if ( PrevNode->pred ){
		if ( PrevNode->right ){

		  ActualNode->setOperatorType("*");

		} else{      /* Prev hat kein ->right */

		  offset = this->copyOperatorContent(exprstring,&expr[e_indx]);
		  e_indx+=offset;

		  if ( isBuiltinFunction(exprstring) || checkOperator(exprstring[0]) )
		    ActualNode->setOperatorType(exprstring);
		  else if ( functionlist ){

		    if ( functionlist->isMember(exprstring) )
		      ActualNode->setOperatorType(exprstring);
		    else
		      ActualNode->setVariableType(exprstring);

		  } else
		    ActualNode->setVariableType(exprstring);

		  this->clearString(exprstring);

		}
	      } else{          /* Prev hat kein ->pred */

		if ( PrevNode->right ){

		  ActualNode->setOperatorType("*");

		} else{      /* Prev hat kein ->right */

		  offset = this->copyOperatorContent(exprstring,&expr[e_indx]);
		  e_indx+=offset;

		  if ( isBuiltinFunction(exprstring) || checkOperator(exprstring[0]) )
		    ActualNode->setOperatorType(exprstring);
		  else if ( functionlist ){

		    if ( functionlist->isMember(exprstring) )
		      ActualNode->setOperatorType(exprstring);
		    else
		      ActualNode->setVariableType(exprstring);

		  } else
		    ActualNode->setVariableType(exprstring);

		  this->clearString(exprstring);
		}
	      }
	    } else{  /* Prev kein operator also Zahl */

	      ActualNode->setOperatorType("*");

	    }
	  } else{ /* Arithmetischer Operator */

	    if ( PrevNode->isOperator() ){
	      /* behandelt werden sollen Ausdruecke der Form: "sin^2(...)": */
	      /* ist das  aktuelle Zeichen ein "^" und der PrevNode ein
		 Operator, handelt es sich um die Form wie z.B. "sin^2",
		 allerdings nur, wenn der PrevNode noch keinen rechten
		 Unterast hat, denn dann muss es sich um eine Form wie
		 "(sin2)^3" handeln */
	      if ( expr[e_indx]=='^'
		   && (pri(&expr[e_indx],PrevNode->getOperator())==POT_PRI-SIN_PRI)
		   && !PrevNode->right ){
		ActualNode->setOperatorType("^");
		/* im Folgenden wird geprueft, ob eine fehlerhafte Eingabe
		   der Form "sin^^ ..." gemacht wurde. Dann ist der PrevNode
		   (immer noch der sin) schon der linke Unterast eines
		   "^"-Operators. Nun wird absichtlich ein fehlerhafter Baum
		   aufgebaut, indem der ActualNode einfach als linker Unterast
		   an den "sin"-Node gehaengt wird. Dies wird beim checken
		   automatisch zu einem Syntaxfehler fuehren. */
		if ( PrevNode->pred ){
		  if ( PrevNode==PrevNode->pred->left ){

		    prevn=PrevNode;

		    while ( prevn->left )
		      prevn=prevn->left;

		    prevn->left=ActualNode;
		    ActualNode->pred=prevn;
		    e_indx++;
		    abs_pos++;

		    continue;

		  }
		}
		/* hier ist alles ok (es handelt sich also um das erste "^"),
		   der "sin"-Node muss also zum linken Unterast des ActualNode
		   werden */
		ActualNode->left=PrevNode;
		/* hat der "sin"-Node einen Vorgaenger, muss diese Verbindung
		   auch verbogen werden */
		if ( PrevNode->pred){

		  ActualNode->pred=PrevNode->pred;
		  PrevNode->pred->right=ActualNode;

		} else
		  TopNode=ActualNode;

		PrevNode->pred=ActualNode;
		e_indx++;
		abs_pos++;

		/* Auswertung der Potenz: handelt es sich um einen
		   Klammerausdruck? ... */
		if ( !expr[e_indx] )
		  continue;
		else if ( expr[e_indx]=='(' ){

		  if ( (offset = this->copyBracketContent(bracketstring,&expr[e_indx],
							  '(',')')) == -1 ){

		    delete TopNode;
		    throw ParseException(abs_pos, "missing bracket!");

		  }
		  
		  e_indx+=offset;
		  abs_pos-=offset-1;
		  
		  if ( !(actn=this->parse(bracketstring,locals)) )
		    actn = new MathExpression(abs_pos,varlist,functionlist);

		  abs_pos++;

		  this->clearString(bracketstring);

		} else{ /* oder um eine simple Zahl? */

		  if ( !(actn = new MathExpression(abs_pos,varlist,functionlist)) ){

		    delete TopNode;
		    throw OutOfMemException();

		  }

		  offset = this->copyFloatContent(exprstring,&expr[e_indx]);
		  e_indx+=offset;

		  actn->setValueType(new Complex(atof(exprstring)));
		  this->clearString(exprstring);

		}

		actn->pred=ActualNode;
		ActualNode->right=actn;
		actn=PrevNode->pred;
		continue;

	      } else{

		if ( expr[e_indx]=='!' ){

		  if ( !(actn = new MathExpression(abs_pos,varlist,functionlist)) ){

		    delete TopNode;
		    throw OutOfMemException();

		  }

		  actn->setOperatorType("!");
		  e_indx++;
		  abs_pos++;

		  if ( PrevNode->pred ){

		    PrevNode->pred->right=actn;
		    actn->pred=PrevNode->pred;

		  }  else
		    TopNode=actn;

		  actn->right=PrevNode;
		  PrevNode->pred=actn;
		  continue;

		} else if ( expr[e_indx]=='=' ){

		  // it's a functiondefinition
		  // to build a correct tree, the local vars must be remembered
		  if ( PrevNode )
		    PrevNode->addVariablesToList(&locals);

		}

		offset = this->copyOperatorContent(exprstring,&expr[e_indx]);
		e_indx+=offset;

		if ( isBuiltinFunction(exprstring) || checkOperator(exprstring[0]) )
		  ActualNode->setOperatorType(exprstring);
		else if ( functionlist ){

		  if ( functionlist->isMember(exprstring) )
		    ActualNode->setOperatorType(exprstring);
		  else
		    ActualNode->setVariableType(exprstring);

		} else
		  ActualNode->setVariableType(exprstring);

		this->clearString(exprstring);

	      }
	    } else{ /* der Vorgaenger ist kein Operator, also "normale"
		       Verhaeltnisse */
	      if ( expr[e_indx]=='!' ){

		if ( !(actn = new MathExpression(abs_pos,varlist,functionlist)) ){

		  delete TopNode;
		  throw OutOfMemException();

		}

		actn->setOperatorType("!");
		e_indx++;
		abs_pos++;

		if ( PrevNode->pred ){

		  PrevNode->pred->right=actn;
		  actn->pred=PrevNode->pred;

		} else
		  TopNode=actn;

		actn->right=PrevNode;
		PrevNode->pred=actn;
		continue;

	      } else if ( expr[e_indx]=='=' ){

		// it's a functiondefinition
		// to build a correct tree, the local vars must be remembered
		if ( PrevNode )
		  PrevNode->addVariablesToList(&locals);

	      }

	      offset = this->copyOperatorContent(exprstring,&expr[e_indx]);
	      e_indx+=offset;

	      if ( isBuiltinFunction(exprstring) || checkOperator(exprstring[0]) )
		ActualNode->setOperatorType(exprstring);
	      else if (functionlist){

		if ( functionlist->isMember(exprstring) )
		  ActualNode->setOperatorType(exprstring);
		else
		  ActualNode->setVariableType(exprstring);

	      }
	      else
		ActualNode->setVariableType(exprstring);

	      this->clearString(exprstring);

	    }
	  }
	} else{ /* kein PrevNode */

	  offset = this->copyOperatorContent(exprstring,&expr[e_indx]);
	  e_indx+=offset;

	  if ( isBuiltinFunction(exprstring) || checkOperator(exprstring[0]) )
	    ActualNode->setOperatorType(exprstring);
	  else if ( functionlist){

	    if ( functionlist->isMember(exprstring) )
	      ActualNode->setOperatorType(exprstring);
	    else
	      ActualNode->setVariableType(exprstring);

	  } else
	    ActualNode->setVariableType(exprstring);

	  this->clearString(exprstring);

	}
      }
    }
    if ( PrevNode ){
      if ( ActualNode->isOperator() ){
	if ( PrevNode->isOperator() ){

	  if ( (ActualNode->oprtr[0] == '-' || ActualNode->oprtr[0] == '+')
	       && !ActualNode->left && !PrevNode->right ){

	    /* falls unaerer minus- oder plusoperator */
	    /* wird eine Mult. mit - bzw. +1 in den Baum eingefuegt */
	    if ( ActualNode->oprtr[0] == '-' )         
	      ActualNode->setValueType(new Complex(-1));
	    else                                   
	      ActualNode->setValueType(new Complex(1));

	    prevn=ActualNode;

	    if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

	      delete TopNode;
	      throw OutOfMemException();

	    }

	    ActualNode->setOperatorType("*");
	    ActualNode->left=prevn;
	    prevn->pred=ActualNode;

	    PrevNode->right = ActualNode;
	    ActualNode->pred = PrevNode;

	  } else if ( PrevNode->right ){
	    if ( PrevNode->pred ){ /* Da Prev pred hat, muss prioritaet des
				      operators gecheckt werden */
	      actn=ActualNode, prevn=PrevNode->pred;

	      while ( (priority = pri(actn->getOperator(),prevn->getOperator())) <= 0
		      && prevn->pred )
		prevn=prevn->pred;

	      if ( priority <= 0 ){  /* hier muss prevn Top sein */

		actn->left=prevn;
		prevn->pred=actn;
		TopNode=actn;

	      }
	      else{               /* actn hat hoehere Prioritaet */

		actn->left = prevn->right;
		actn->pred = prevn;
		prevn->right = actn;

	      }
	    } else{

	      ActualNode->left = PrevNode;
	      PrevNode->pred = ActualNode;
	      TopNode = ActualNode;

	    }
	  } else{   /* Prev hat keinen rechten Unterbaum, also wird hier neuer
		       Ast angefuegt */
	    // z.B "3+sin2"
	    // 	    if (pri(ActualNode->oprtr,PrevNode->oprtr)){
	    PrevNode->right = ActualNode;
	    ActualNode->pred = PrevNode;
	    // 	    }
	  }
	} else{      /* Prev ist Zahl */

	  if (PrevNode->pred  ){

	    actn = ActualNode, prevn=PrevNode->pred;

	    while ( (priority = pri(actn->getOperator(),prevn->getOperator())) <= 0
		    && prevn->pred )
	      prevn = prevn->pred;

	    if ( priority <= 0 ){  /* hier muss prevn Top sein */

	      actn->left = prevn;
	      prevn->pred = actn;
	      TopNode = actn;

	    } else{               /* actn hat hoehere Prioritaet */

	      actn->left=prevn->right;
	      actn->pred=prevn;
	      prevn->right=actn;

	    }
	  } else{    /* Prev ist Top */

	    ActualNode->left=PrevNode;
	    PrevNode->pred=ActualNode;
	    TopNode=ActualNode;

	  }
	}
      } else{        /* Act ist Zahl */

	PrevNode->right=ActualNode;
	ActualNode->pred=PrevNode;

      }

      PrevNode=ActualNode;

    } else{               /* erstes Zeichen */

      if ( (ActualNode->oprtr[0] == '-' || ActualNode->oprtr[0] == '+')
	   && !ActualNode->left ){

	/* falls unaerer minus- oder plusoperator */
	/* wird eine Mult. mit - bzw. +1 in den Baum eingefuegt */
	if ( ActualNode->oprtr[0] == '-' )         
	  ActualNode->setValueType(new Complex(-1));
	else                                   
	  ActualNode->setValueType(new Complex(1));

	prevn=ActualNode;

	if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

	  delete TopNode;
	  throw OutOfMemException();

	}

	ActualNode->setOperatorType("*");
	ActualNode->left=prevn;
	prevn->pred=ActualNode;

      }

      PrevNode=ActualNode;
      TopNode=ActualNode;

    }
  }
  return(TopNode);
}

inline bool MathExpression::checkOperator(char x){ 
  return (((unsigned char)(x)>=1 && (unsigned char)(x)<='/')
	  || ((unsigned char)(x)>=':' && (unsigned char)(x)<='@')
	  || ((unsigned char)(x)>='[' && (unsigned char)(x)<='`')
	  || ((unsigned char)(x)>='{') );
}

inline bool MathExpression::checkDigit(char x){
  return (((unsigned char)(x)>='0' && (unsigned char)(x)<='9')
	  || (unsigned char)(x)=='.');
}
 
inline bool MathExpression::checkLetter(char x){
  return (((unsigned char)(x)>='A' && (unsigned char)(x)<='Z')
	  || ((unsigned char)(x)>='a' && (unsigned char)(x)<='z'));
}

bool MathExpression::isBuiltinFunction(const char *strname){

  char *flist[]={FLIST};

  for (unsigned int i=0;i<sizeof(flist)/sizeof(char *);i++)
    if (!strcmp(strname,flist[i]))
      return true;
  return false;
}

void MathExpression::setOperatorType(const char *name){

  clearString(oprtr);
  strcpy(oprtr,name);
  if ( value )
    delete value;
  value=0;
  clearString(variable);
  type=OP;

}

void MathExpression::setVariableType(const char *name){

  clearString(variable);
  strcpy(variable,name);
  clearString(oprtr);
  if ( value )
    delete value;
  value = 0;
  type = VAR;

}
  
void MathExpression::setValueType(Value *value){
    
  if ( this->value )
    delete this->value;

  this->value = value;
  clearString(oprtr);
  clearString(variable);
  type=VAL;
    
}

void MathExpression::setValue(Value *value){
    
  if ( this->value )
    delete this->value;

  this->value = value;

}

bool MathExpression::isABlank(char c){

  return ( c == ' ' || c == '\t' );

}

int MathExpression::skipBlanks(const char *expr, int indx){

  int offset = 0;

  while ( isABlank(expr[indx++]) == true )
    offset++;

  return offset;

}

void MathExpression::addVariablesToList(VariableList *varlist){

  if (isVariable())
    varlist->insert(getVariable(),new Complex(0));

  if (right)
    right->addVariablesToList(varlist);
  if (left)
    left->addVariablesToList(varlist);
}
 
bool MathExpression::isBuiltinOperator(char op){

  char oplist[]={OPLIST};

  for (unsigned int i=0;i<sizeof(oplist)/sizeof(char);i++)
    if (op==oplist[i])
      return true;
  return false;
}

int MathExpression::copyBracketContent(char *exprstring, const char *arg, char open,
				       char close){

  int br_cnt=0, indx=0, eindx=0;

  if ( open && (arg[indx] != open) )
    return (-1);

  do{
    if ( arg[indx] == open || ( !open && !indx) ){  // auch moeglich: alles bis ...
      if ( br_cnt )
	exprstring[eindx] = arg[indx];
      else if ( !open && !indx )
	exprstring[0] = arg[0];
      else
	eindx--;
      br_cnt++;
    } else if (arg[indx] == close ){
      br_cnt--; 
      if ( br_cnt )
	exprstring[eindx] = arg[indx];
    } else
      exprstring[eindx] = arg[indx];
    indx++;
    eindx++;
    abs_pos++;
  } while ( br_cnt&&arg[indx] );
  if ( br_cnt )
    return (-1);
  return(indx);
}
  
int MathExpression::copyFloatContent(char *exprstring, const char *arg){

  int indx=0;
  while (checkDigit(arg[indx])){ 
    exprstring[indx]=arg[indx];
    indx++;
    abs_pos++;
  }
  return(indx);
}

int MathExpression::copyOperatorContent(char *exprstring, const char *arg){

  int indx=0;

  switch (arg[indx]){
  case '(':
  case ')':
    break;
  default:
    if (checkOperator(arg[indx])){
      exprstring[indx]=arg[indx];
      indx++;
      abs_pos++;
    } else{
      while (arg[indx] && indx<OP_LEN){ 
	if (!(checkLetter(arg[indx])))
	  break;
	exprstring[indx]=arg[indx];
	indx++;
	abs_pos++;
      }
    }
    break;
  }
  return(indx);
}

void MathExpression::clearString(char *exprstring){
  while (*exprstring)
    *exprstring++=0;
}

int MathExpression::pri(const char *c0, const char *c1){

  int p[2],i;
  const char *c[2]={&c0[0],&c1[0]};

  for (i=0;i<2;i++){
    switch(*c[i]){
    case ',':
    case ';':
      p[i]=KOMMA_PRI;
      break;
    case '=':
      p[i]=EQUAL_PRI;
      break;
    case '+':
    case '-':
      p[i]=ADDSUB_PRI;
      break;
    case '*':
    case '/':
    case '\\':
    case '%':
      p[i]=MULTDIV_PRI;
      break;
    case '^':
      p[i]=POT_PRI;
      break;
    case '!':
      p[i]=FAC_PRI;
      break;
    case '@':
      p[i]=BINOM_PRI;
      break;
    default :
      if (!strcmp(c[i],SUM)){
	p[i]=ADDSUB_PRI;
	break;
      } else if (!strcmp(c[i],PROD)){
	p[i]=MULTDIV_PRI;
	break;
      } else{
	p[i]=SIN_PRI;
	break;
      }
    }
  }
  return(p[0]-p[1]);
}

bool MathExpression::checkSyntaxAndOptimize(void) throw (ParseException){

  if (this->isOperator()){
    if (checkOperator(this->oprtr[0])){
      if (this->oprtr[0]=='='){
	if (this->left && this->right)
	  if (!this->left->empty() && !this->right->empty()){
	    if (this->left->isVariable()){
	      if (!this->left->left && !this->left->right)
		if (this->right->checkSyntaxAndOptimize())
		  return(true);
	    } else if (this->left->isOperator()){
	      if (!this->left->left && this->left->right)
		if (this->right->checkSyntaxAndOptimize())
		  return true;
	    }
	    throw ParseException(abs_pos, "invalid syntax!");
	  }
      } else if (this->oprtr[0]=='!'){
	if (!this->left && this->right)
	  if (!this->right->empty())
	    return (true);
      } else if (this->left && this->right){
	if (!this->left->empty() && !this->right->empty())
	  if (this->left->checkSyntaxAndOptimize() && this->right->checkSyntaxAndOptimize()){

	    // optimization: reduction  of "({-,+}1)*({-,+}1)" to "{-,+}1"
	    if ( this->oprtr[0] == '*' )
	      if ( this->left->isValue() == true && this->right->isValue() == true )
		if ( this->left->getValue()->getType() == Value::COMPLEX && this->right->getValue()->getType() == Value::COMPLEX){
		  
		  Complex *left = (Complex *)this->left->getValue();
		  Complex *right = (Complex *)this->right->getValue();
		  
		  if ( left->getIm() == 0 && right->getIm() == 0 ){
		    
		    if ( abs(left->getRe()) == 1 && abs(right->getRe()) == 1 ){
		      
		      this->setOperatorType("");
		      this->setValueType(new Complex(sgn(left->getRe()) * right->getRe()));
		      
		      delete this->left;
		      delete this->right;
		      this->left = this->right = 0;
		      
		    }
		    
		  }
		  
		}
	    
	    return(true);

	  }
      }

      throw ParseException(abs_pos, "invalid syntax!");

    } else{
      if (!strcmp(this->getOperator(),"log")){
	if (this->left && this->right)
	  if (!this->left->empty() && !this->right->empty())
	    if (this->left->checkSyntaxAndOptimize() && this->right->checkSyntaxAndOptimize())
	      return(true);
	throw ParseException(abs_pos, "invalid syntax in function log!");
      } else if (!strcmp(this->getOperator(),SUM)
		 || !strcmp(this->getOperator(),PROD)){
	if (this->left && this->right)
	  if (!this->left->empty() && !this->right->empty())
	    if (this->left->oprtr[0]==';')
	      if (this->left->left && this->left->right)
		if (!this->left->left->empty() && !this->left->right->empty())
		  if (this->left->left->oprtr[0]=='='){
		    try{
		      if (this->left->left->checkSyntaxAndOptimize())
			if (this->left->right->checkSyntaxAndOptimize())
			  if (this->right->checkSyntaxAndOptimize())
			    return(true);
		    } catch (ParseException pe){
		      throw ParseException(abs_pos, "invalid syntax in function Sum/Prod!");
		    }
		  }
	throw ParseException(abs_pos, "invalid syntax in function Sum/Prod!");
      } else if (isBuiltinFunction(this->getOperator())){
	if (!this->left && this->right)
	  if (this->right->empty()==false)
	    if (this->right->checkSyntaxAndOptimize())
	      return(true);
	throw ParseException(abs_pos, "invalid syntax!");
      } else if (functionlist->isMember(this->getOperator())){
	if (!this->left && this->right)
	  if (this->right->checkSyntaxAndOptimize())
	    if (this->right->countArgs() ==
		functionlist->get(this->getOperator())->getParameterList()->countArgs())
	      return(true);
	throw ParseException(abs_pos, "invalid syntax!");
      } else
	return true;
    }
  }
  return(true);
}

void MathExpression::print(streamsize precision) const{
  cout << toString(precision);
}

string MathExpression::toString(streamsize precision) const throw (Exception<MathExpression>,ExceptionBase){

  ostringstream exp;

  exp << "(";
  if (!this->empty()){
    if (this->isOperator()){
      if (this->oprtr[0]=='!'){

	if (this->right)
	  exp << this->right->toString(precision);

	exp << this->getOperator();

      } else if (!strcmp(this->getOperator(),"log")
		 || !strcmp(this->getOperator(),SUM)
		 || !strcmp(this->getOperator(),PROD)){

	exp << this->getOperator();
	exp << "[";
	exp << this->left->toString(precision);
	exp << "]";
	exp << this->right->toString(precision);

      } else{

	if (this->left)
	  exp << this->left->toString(precision);

	exp << this->getOperator();

	if (this->right)
	  exp << this->right->toString(precision);

      }
    } else if (this->isVariable())
      exp << this->getVariable();
    else if ( this->isValue() ){

      exp << this->getValue()->toString(precision);

    } else
      throw Exception<MathExpression>("internal error in MathExpression::toString()!");
  }
  exp << ")";

  return exp.str();

}

string MathExpression::builtinsToString(){

  ostringstream builtins;
  
  char oplist[]={OPLIST};
  char *flist[]={FLIST};

  for (unsigned int i=0;i<sizeof(oplist)/sizeof(char);i++)
    builtins << oplist[i] << endl;

  for (unsigned int i=0;i<sizeof(flist)/sizeof(char *);i++)
    builtins << flist[i] << endl;

  return builtins.str();

}


Value *MathExpression::eval() throw (ExceptionBase){

  //   double result;
  if ( isOperator() ){
    switch ( oprtr[0] ){
    case '+':
      this->setValue(left->eval()->operator+(right->eval()));
      break;
    case '-':
      this->setValue(left->eval()->operator-(right->eval()));
      break;
    case '*':
      this->setValue(left->eval()->operator*(right->eval()));
      break;
    case '/':
      this->setValue(left->eval()->operator/(right->eval()));
      break;
    case '\\':
      this->setValue(left->eval()->integerDivision(right->eval()));
      break;
    case '%':
      this->setValue(left->eval()->operator%(right->eval()));
      break;
    case '^':
      /*
	if (left->eval()<=0 && (result/(int)result)){
	fprintf(stderr,"negative root!\n");
	exit(1);
	}
	else*/
      this->setValue(left->eval()->pow(right->eval()));
      break;
    case '!':
      this->setValue(right->eval()->faculty());
      break;
    case '@':
      //       return (faculty(left->eval())/(faculty(right->eval())
      // 				     *faculty(left->eval() - right->eval())));
      this->setValue(left->eval()->choose(right->eval()));
      break;
    case '=':
      if (left->isVariable()){

	this->setValue(assignValue());
	break;

      } else if (left->isOperator()){

	defineFunction();
	this->setValue(new Complex(0));
	break;

      }

      throw EvalException("invalid use of assignment!");
      break;

    default:
      if (!strcmp("sin",getOperator()))
        this->setValue(right->eval()->sin());
      else if (!strcmp("cos",getOperator()))
	this->setValue(right->eval()->cos());
      else if (!strcmp("tan",getOperator()))
	this->setValue(right->eval()->tan());
      else if (!strcmp("asin",getOperator()))
	this->setValue(right->eval()->asin());
      else if (!strcmp("acos",getOperator()))
	this->setValue(right->eval()->acos());
      else if (!strcmp("atan",getOperator()))
	this->setValue(right->eval()->atan());
      else if (!strcmp("sinh",getOperator()))
	this->setValue(right->eval()->sinh());
      else if (!strcmp("cosh",getOperator()))
	this->setValue(right->eval()->cosh());
      else if (!strcmp("tanh",getOperator()))
	this->setValue(right->eval()->tanh());
      else if (!strcmp("asinh",getOperator()))
	this->setValue(right->eval()->asinh());
      else if (!strcmp("acosh",getOperator()))
	this->setValue(right->eval()->acosh());
      else if (!strcmp("atanh",getOperator()))
	this->setValue(right->eval()->atanh());
      else if (!strcmp("ln",getOperator())){
	// 	if ((result=right->eval())<=0)
	// 	  throw EvalException("argument of ln negative!");
	// 	else
	this->setValue(right->eval()->ln());
      } else if (!strcmp("ld",getOperator())){
	// 	if ((result=right->eval())<=0)
	// 	  throw EvalException("argument of ld negative!");
	// 	else
	// 	  return(log(right->eval())/log(2.0));
	this->setValue(right->eval()->ld());
      } else if (!strcmp("log",getOperator())){
	// 	if ((result=right->eval())<=0)
	// 	  throw EvalException("argument of log negative!");
	// 	else
	// 	  return(log(right->eval())/log(left->eval()));
	this->setValue(right->eval()->log(left->eval()));
      } else if (!strcmp("exp",getOperator()))
	this->setValue(right->eval()->exp());
      else if (!strcmp(SUM,getOperator()) || !strcmp(PROD,getOperator())){
	
	//TODO
	throw EvalException("SumProd not supported yet!");

	// 	this->setValue(sumProd());

      } else if (!strcmp("sgn",getOperator())){

	this->setValue(right->eval()->sgn());

      } else if (!strcmp("tst",getOperator())){

	this->setValue(right->eval()->tst());

      } else{  // user defined function

	if (functionlist){
	  if (functionlist->isMember(getOperator())){

	    this->setValue(evalFunction());
	    break;

	  }
	}

	throw EvalException("unknown operator/function!",getOperator());

      }

      break;

    }

  } else if ( isVariable() ){

    if ( varlist )
      this->setValue(varlist->getValue(getVariable()));
    else
      throw EvalException("unauthorized use of variables!");
  }

  return value;

}

double MathExpression::sgn(double value){

  if ( value < 0 )
    return -1;
  else
    return 1;

}

double MathExpression::abs(double value){
  return sgn(value) * value;
}

string MathExpression::skipTrailingZeros(string value){

  int stop = 0;
  int pos = 0;
  bool pflag = false;

  for ( string::iterator it = value.begin(); it != value.end(); it++, pos++ ){

    if ( *it == '.' ){

      pflag = true;
      stop = pos;

    } else if ( ( pflag == false ) || ( *it != '0' && pflag == true ) )
      stop = pos+1;

  }
  
  return value.substr(0,stop);
}

double MathExpression::faculty(double fac) throw (ExceptionBase){
  
  if (fac!=(double)((int)(fac)) || fac<0)
    throw EvalException
      ("Argument of Faculty not a natural number or negative!");
  if (fac==0)
    return(1);
  for (int i=(int)fac-1;i>0;i--)
    fac*=i;
  return(fac);
}

#ifdef NO_COMPILE
double MathExpression::sumProd(void)
  throw (ExceptionBase){

  VariableList vl = *this->varlist;  // lokaler Scope: Kopie der globalen Varlist
  MathExpression me(this,&vl,this->functionlist,abs_pos);
  Variable *currve;
  Value *c_from,*c_to;
  double from, to;
  Complex *value, res;
  char p=0;
  
  if (!strcmp(PROD,this->getOperator())){
    res=1;
    p=1;
  }
  
  // Indexvariable zum lokalen Scope hinzufuegen
  vl.insert(this->left->left->left->getVariable(),
	    this->left->left->right->eval()->clone());

  //Zaehlbereich
  cfrom=vl.getValue(me.left->left->left->getVariable());
  cto=me.left->right->eval()->clone();

  Complex::assertReal(c_from);
  Complex::assertReal(c_to);

  from = ((Complex *)c_from)->getRe();
  to = ((Complex *)c_to)->getRe();

  if ( from!=(int)from || from<0 || to!=(int)to )
    throw EvalException("indices in Sum/Prod not natural or negative!");

  if ( to<from )
    return 0;

  value = me.right->eval()->clone();

  for ( int i=(int)from;i<=(int)to; ){

    if (!p)
      res += me.right->eval();  // Summe
    else
      res *= me.right->eval();  // Produkt

    i++;

    // neuen Wert der Zaehlvariablen eintragen
    vl.insert(this->left->left->left->getVariable(),new Complex((double)i));

  }

  delete value;

  // neu definierte Variablen in globale Varlist eintragen
  currve = vl.first;
  while ( currve ){

    if (strcmp(currve->getName(),this->left->left->left->getVariable())
	&& !currve->protect)
      this->varlist->insert(currve->getName(),currve->getValue()->clone());

    currve=currve->next;

  }

  return res;
}
#endif

Value *MathExpression::assignValue(void) throw (ExceptionBase){

  Value *result;

  result = right->eval()->clone();

  if ( functionlist )
    if ( functionlist->isMember(left->getOperator()) )
      throw EvalException ("variable already defined as function");

  if ( varlist ){

    try{

      varlist->insert(left->getVariable(),result);

    }catch ( OutOfMemException &oome ){

      delete varlist;
      throw OutOfMemException();

    }

    varlist->setModified(true);
    return result->clone();

  }

  throw EvalException("invalid use of assignment!");

}

Value *MathExpression::evalFunction(void) throw (ExceptionBase){

  VariableList vl = *this->varlist; // local scope
  MathExpression *args =  0,*params = 0;
  Function *f_template = functionlist->get(this->getOperator());

  vl.unprotect();  // remove protection-status for all variables

  args = this->right;
  params = f_template->getParameterList();

  // eval args and insert them in varlist
  if (!args->empty()){

    while (args->oprtr[0] == ','){

      vl.insert(params->right->getVariable(),args->right->eval()->clone());
      args = args->left;
      params = params->left;

    }

    vl.insert(params->getVariable(),args->eval()->clone());

  }

  MathExpression function(f_template->getBody(),&vl,functionlist,abs_pos);

  return function.eval()->clone();

}

void MathExpression::defineFunction(void) throw (ParseException){

  const char *functionname;
  MathExpression *paramlist=0, *body=0;
  Function *fe=0;
  VariableList locals;

  functionname = this->left->getOperator();
  
  // already declared as variable? (impossible to happen)
  if (varlist->isMember(functionname))
    throw ParseException(abs_pos, string(functionname)
			 + " already declared as variable, definition "
			 + "not possible!");
  
  // built-in function?
  if (isBuiltinFunction(functionname))
    throw ParseException(abs_pos, string(functionname)
			 + " is built-in function and can't be replaced!");
  
  // already declared as function?
  if (functionlist->isMember(functionname))
    throw ParseException(abs_pos, string(functionname)
			 + " already declared as function, to redefine it"
			 + " first undefine it!");

  // build parameterlist
  paramlist = new MathExpression(this->left->right,varlist,functionlist,abs_pos);

  // must be a variabletree
  if (paramlist->checkForVariableTree()==false){

    delete paramlist;
    throw ParseException(abs_pos, string("only letters and non-built-in functionnames allowed,")
			 + " variables must be seperated by commas!");

  }
  
  // build functionbody
  body = new MathExpression(this->right,varlist,functionlist,abs_pos);
  
  // body must only contain defined variables/functions/operators
  if (!checkBody(body,paramlist,&locals)){

    delete body;
    delete paramlist;
    throw ParseException(abs_pos, string("undefined variables/functions/operators used, ")
			 + "no definition possible!");

  }
  
  // build functionelement
  fe = new Function(functionname,paramlist,body);
  
  this->functionlist->insert(fe); // insert in functionlist;

  functionlist->setModified(true);
  
}

bool MathExpression::checkBody(MathExpression *body, MathExpression *pl,
			       VariableList *lvl){

  // pl: parameterTree
  // lvl: local VariableList

  bool inparam=false;
  
  if (!body)
    return true;

  if (*(body->getOperator())){
    if (!(isBuiltinOperator(*(body->getOperator()))))
      if (!(isBuiltinFunction(body->getOperator())))
	if (!(functionlist->isMember(body->getOperator())))
	  return false;
  } else if (*(body->getVariable())){
    if (pl)
      if (pl->isVariableInTree(body->getVariable()))
	inparam=true;
    if (!inparam){
      if (!varlist->isMember(body->getVariable())){
	if (!lvl->isMember(body->getVariable())){
	  if (!body->getPred())
	    return false;
	  else{
	    if ((body->getPred()->getOperator())[0]!='=')
	      return false;
	    else
	      lvl->insert(body->getVariable(),new Complex(0));
	  }
	}
      }
    }
  }
  if (!checkBody(body->getLeft(),pl,lvl))
    return false;
  if (!checkBody(body->getRight(),pl,lvl))
    return false;
  return true;
  
}

bool MathExpression::checkForVariableTree(){

  if (this->left){
    if (this->left->checkForVariableTree()==false){
      return false;
    }
    if (this->oprtr[0]!=',')
      return false;
  } else{
    if (!this->isVariable()){
      return false;
    }
  }
  if (this->right){
    if (this->right->checkForVariableTree()==false){
      return false;
    }
  }
  return true;
}

bool MathExpression::isVariableInTree(const char *name){

  if (!this)
    return false;

  if (strcmp(name,this->getVariable())){
    if (this->left->isVariableInTree(name))
      return true;
    else if (this->right->isVariableInTree(name))
      return true;
    else
      return false;
  }
  return true;
}
    
unsigned int MathExpression::countArgs(void){
  
  if (!this)
    return 0;

  if (this->empty())
    return 0;

  if (this->oprtr[0]==',')
    return (1+this->left->countArgs());
  else
    return 0;
}
