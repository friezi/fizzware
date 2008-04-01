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
using namespace ds;

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

Tuple::~Tuple(){

  for ( list<Value *>::iterator it = elements.begin(); it != elements.end(); it++ )
    delete (*it);

}

string Tuple::toString(std::streamsize precision) const {

  ostringstream value;

  value << "(";

  for ( list<Value *>::const_iterator it = elements.begin(); it != elements.end(); it++ )
    value << ( it == elements.begin() ? (*it)->toString(precision) : string(",") + (*it)->toString(precision) );

  value << ")";

  return value.str();

}

Value *Tuple::clone() const {

  Tuple *value = new Tuple();

  for ( list<Value *>::const_iterator it = elements.begin(); it != elements.end(); it++ )
    value->addElement((*it)->clone());

  return value;

}

Tuple *Tuple::assertTuple(Value *value) throw(EvalException){

  if ( value->getType() != Value::TUPLE )
    throw EvalException(string("operand to tuple is no tuple-type: ") + value->toString() + string(" !"));

  return dynamic_cast<Tuple *>(value);

}
void Tuple::assertKind(const Tuple *t1, const Tuple *t2) throw(EvalException){

  if ( t1->elements.size() != t2->elements.size() )
    throw EvalException(string("tuples are not of same kind: ") + t1->Value::toString() + string(" <-> ") + t2->Value::toString() + string(" !"));

}

Value *Tuple::operator+(Value *right) throw (ExceptionBase){

  Tuple *rt = assertTuple(right);

  assertKind(this,rt);

  Tuple *value = new Tuple();

  for ( list<Value *>::iterator lit = elements.begin(), rit = rt->elements.begin(); lit != elements.end(); lit++, rit++ )
    value->addElement((*lit)->operator+((*rit)));

  return value;

}

string Complex::toString(std::streamsize precision) const {
  
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

Complex *Complex::assertComplex(Value *value) throw(EvalException,ExceptionBase){
  
  if ( value->getType() != Value::COMPLEX )
    throw EvalException(string("operand to complex is not complex-type: ") + value->toString() + string(" !"));
  
  return dynamic_cast<Complex *>(value);
  
}

Complex *Complex::assertReal(Value *value) throw(EvalException,ExceptionBase){

  try{
  
    Complex *cmplx = assertComplex(value);
  
    if ( cmplx->getIm() != 0 )
      throw EvalException("");
  
    return cmplx;

  } catch (EvalException &e){

    throw EvalException("operation only allowed for real-numbers! ");

  }
  
}

Complex *Complex::assertInteger(Value *value) throw(EvalException,ExceptionBase){

  try{

    Complex *cmplx = assertReal(value);

    if ( cmplx->getRe() != (double)((int)(cmplx->getRe())) )
      throw EvalException("");

    return cmplx;

  } catch (EvalException &e){

    throw EvalException("operation only allowed for integer-numbers! ");

  }


}

Complex *Complex::assertNatural(Value *value) throw(EvalException,ExceptionBase){

  try{

    Complex *cmplx = assertInteger(value);

    if ( cmplx->getRe() < 0 )
      throw EvalException("");

    return cmplx;

  } catch (EvalException &e){

    throw EvalException("operation only allowed for natural-numbers! ");

  }


}

Value *Complex::neutralAddition() const {

  return new Complex(0);

}

Value *Complex::neutralMultiplikation() const {

  return new Complex(1,1);

}

Value *Complex::operator+(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);
  return new Complex(this->getRe()+rc->getRe(),this->getIm()+rc->getIm());
      
}

Value *Complex::operator-(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);
  return new Complex(getRe()-rc->getRe(),getIm()-rc->getIm());

}

Value *Complex::operator*(Value *right) throw (ExceptionBase){ 
      
  if ( right->getType() == Value::COMPLEX ){

    Complex *rc = dynamic_cast<Complex *>(right);
    return new Complex(getRe()*rc->getRe()-getIm()*rc->getIm(),getRe()*rc->getIm()+getIm()*rc->getRe());

  } else if ( right->getType() == Value::TUPLE ){

    Tuple *rt = dynamic_cast<Tuple *>(right);
    Tuple *value = new Tuple();

    for ( list<Value *>::iterator it = rt->elements.begin(); it != rt->elements.end(); it++ )
      value->addElement(this->operator*(*it));

    return value;

  } else
    throw EvalException(string("unsupported type for '*': ") + right->toString() + string(" !"));
    
}

Value *Complex::operator/(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);

  double divisor = (::pow(rc->getRe(),2)+::pow(rc->getIm(),2));
  
  if ( divisor == 0 )
    throw EvalException("division by zero!");
  
  return new Complex((getRe()*rc->getRe()+getIm()*rc->getIm())/divisor,
		     (getIm()*rc->getRe()-getRe()*rc->getIm())/divisor);
      
}

void Complex::operator+=(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);

  (*static_cast< complex<cmplx_tp> *>(this)) += static_cast< complex<cmplx_tp> >(*rc);

}

void Complex::operator*=(Value *right) throw (ExceptionBase){

  Complex *rc = assertComplex(right);

  (*static_cast< complex<cmplx_tp> *>(this)) *= static_cast< complex<cmplx_tp> >(*rc);
      
}

Value *Complex::integerDivision(Value *right) throw (ExceptionBase){
  
  Complex *rc = assertComplex(right);

  double divisor = (::pow(rc->getRe(),2)+::pow(rc->getIm(),2));
  
  if ( divisor == 0 )
    throw EvalException("division by zero!");

  return new Complex((double)((int)((getRe()*rc->getRe()+getIm()*rc->getIm())/divisor)),
		     (double)((int)((getIm()*rc->getRe()-getRe()*rc->getIm())/divisor)));
      
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
  
  assertNatural(this);

  if ( this->getRe() == 0 )
    return new Complex(1);

  cmplx_tp fac = this->getRe();

  for ( unsigned long i = (unsigned long)fac-1; i > 0; i-- )
    fac *= i;

  return new Complex(fac);
      
}

Value *Complex::choose(Value *right) throw (ExceptionBase){

  assertNatural(this);
  Complex *cr = assertNatural(right);

  return new Complex( MathExpression::faculty(getRe()) / ( MathExpression::faculty(cr->getRe()) * MathExpression::faculty(getRe() - cr->getRe())));

}

Value *Complex::pow(Value *right) throw (ExceptionBase){
  
  Complex *rc = assertComplex(right);

  return new Complex(std::pow(static_cast< complex<cmplx_tp> >(*this),static_cast< complex<cmplx_tp> >(*rc)));
      
}

Value *Complex::sin() throw (ExceptionBase){

  return new Complex(std::sin(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::cos() throw (ExceptionBase){

  return new Complex(std::cos(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::tan() throw (ExceptionBase){

  return new Complex(std::tan(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::asin() throw (ExceptionBase){

  assertReal(this);
  return new Complex(::asin(this->getRe()));
      
}

Value *Complex::acos() throw (ExceptionBase){

  assertReal(this);
  return new Complex(::acos(this->getRe()));
      
}

Value *Complex::atan() throw (ExceptionBase){

  assertReal(this);
  return new Complex(::atan(this->getRe()));
      
}

Value *Complex::sinh() throw (ExceptionBase){

  return new Complex(std::sinh(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::cosh() throw (ExceptionBase){

  return new Complex(std::cosh(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::tanh() throw (ExceptionBase){

  return new Complex(std::tanh(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::asinh() throw (ExceptionBase){

  assertReal(this);
  return new Complex(::asinh(this->getRe()));
      
}

Value *Complex::acosh() throw (ExceptionBase){

  assertReal(this);
  return new Complex(::acosh(this->getRe()));
      
}

Value *Complex::atanh() throw (ExceptionBase){

  assertReal(this);
  return new Complex(::atanh(this->getRe()));
      
}

Value *Complex::ln() throw (ExceptionBase){

  return new Complex(std::log(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::ld() throw (ExceptionBase){

  return new Complex(std::log(static_cast< complex<cmplx_tp> >(*this))/std::log((cmplx_tp)2.0));
      
}

Value *Complex::log(Value *base) throw (ExceptionBase){

  Complex *cmplx = assertComplex(base);

  return new Complex(std::log(static_cast< complex<cmplx_tp> >(*this))/std::log(static_cast< complex<cmplx_tp> >(*cmplx)));

}

Value *Complex::exp() throw (ExceptionBase){

  return new Complex(std::exp(static_cast< complex<cmplx_tp> >(*this)));
      
}

Value *Complex::sgn() throw (ExceptionBase){

  assertReal(this);

  return new Complex(this->getRe() < 0 ? -1 : 1);

}

Value *Complex::tst() throw (ExceptionBase){

  return new Complex(this->getRe() != 0 or this->getIm() != 0);

}

MathExpression::MathExpression(int abs_pos, VariableList *vl, FunctionList *fl) :
  varlist(vl),functionlist(fl),left(0), right(0), pred(0),
  value(0), type(ET_EMPTY), operator_type(OT_EMPTY), abs_pos(abs_pos), imaginary_unit('i'), delete_flat(false){
  
  oprtr = string("");
  variable = string("");

}

MathExpression::MathExpression(const char *expression, VariableList *vl,
			       FunctionList *fl)
  throw (ParseException,ExceptionBase)
  : varlist(vl), functionlist(fl), left(0), right(0), pred(0),
    value(0), type(ET_EMPTY), operator_type(OT_EMPTY), abs_pos(1), imaginary_unit('i'), delete_flat(false){
  
  oprtr = string("");
  variable = string("");

  MathExpression *top;
  VariableList locals;
  Context context;
  
  top = this->parse(expression,context,locals);


  if ( top ){

    this->setLeft(top->getLeft());
    this->setRight(top->getRight());
    this->elements = top->elements;
    this->pred=top->pred;

    switch (top->getEType()){

    case ET_OP:
      this->setETOperator(top->getOperator());
      this->setOType(top->getOType());
      break;

    case ET_VAR:
      this->setETVariable(top->getVariable());
      break;

    case ET_VAL:
      // must be a flat copy because 'top' will only be freed and not deleted
      this->setETValue(top->getValue());
      break;

    case ET_EMPTY:
    default:
      this->setEType(top->getEType());
      break;

    }

  }
  
  try{

    this->checkSyntaxAndOptimize();

  }catch (ParseException &pe){

    delete top;
    throw pe;

  }

  // must be only flat-destruction because the referred objects have been stored in this
  top->delete_flat = true;
  delete top;

}

MathExpression::MathExpression(MathExpression *me, VariableList *vl, FunctionList *fl, int abs_pos)
  throw (ParseException,ExceptionBase)
  : varlist(vl), functionlist(fl), left(0), right(0), pred(0),
    value(0), type(ET_EMPTY), operator_type(OT_EMPTY), abs_pos(abs_pos), imaginary_unit('i'), delete_flat(false){

  oprtr = string("");
  variable = string("");

  if ( !me )
    return;

  setImaginaryUnit(me->getImaginaryUnit());
  
  switch (me->getEType()){
  case ET_OP:
    this->setETOperator(me->getOperator());
    this->setOType(me->getOType());
    break;
  case ET_VAR:
    this->setETVariable(me->getVariable());
    break;
  case ET_VAL:
    this->setETValue(me->getValue()->clone());
    break;
  case ET_EMPTY:
  default:
    this->setEType(ET_EMPTY);
    break;
  }
  
  // functions mostly don't have a left parameter. In this case we must not add the elements to the
  // element-list otherwise the left-pointer will be set which will result in a malformed tree.
  if ( me->getLeft() ){
    // a left-argument exists: we can safely add all arguments to element-list
    
    for ( list<MathExpression *>::iterator it = me->elements.begin(); it != me->elements.end(); it++ ){
      
      addElement(new MathExpression(*it,vl,fl,abs_pos));
      elements.back()->pred = this;
      
    }
    
  } else if ( me->getRight() )
    // a left-argument doesn't exist: we must only set the right-pointer
    this->setRight(new MathExpression(me->getRight(),vl,fl,abs_pos));

}

MathExpression::~MathExpression(){

  if ( delete_flat == false){
    
    eraseElements();
    if ( this->value )
      delete this->value;
    
  }
  
}

void MathExpression::addElement(MathExpression *element){
  
  if ( elements.empty() )
    left = element;
  else
    right = element;
  
  elements.push_back(element);
  
}

void MathExpression::setRight(MathExpression *right){

  if ( getRight() )
    elements.pop_back();

  if ( right )
    elements.push_back(right);

  this->right = right;

}

void MathExpression::setLeft(MathExpression *left){

  if ( getLeft() )
    elements.pop_front();

  if ( left )
    elements.push_front(left);

  this->left = left;

}

void MathExpression::eraseElements(){

  for( list<MathExpression *>::iterator it = elements.begin(); it != elements.end(); it++ )
    delete *it;
  elements.clear();

}

MathExpression *MathExpression::parse(const char *expr, const Context & context, VariableList& locals)
  throw (ParseException,ExceptionBase){

  int e_indx, offset;
  char *exprstring=0, *bracketstring=0;
  MathExpression *TopNode=0, *ActualNode=0, *PrevNode=0, *actn, *prevn;
  Context new_context;

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

      new_context.setWithinBrackets(true);

      if ( PrevNode ){
	if ( PrevNode->isOperator() ){
	  if ( PrevNode->pred ){
	    if ( PrevNode->getRight() ){
	      /* hier handelt es sich um Ausdruecke der Form
		 "1+sin2(...), also um eine implizite Multipikation,
		 es muss also ein "*" in den Baum eingefuegt werden*/

	      if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

		delete TopNode;
		throw OutOfMemException();

	      }

	      ActualNode->setETOperator("*");
	      ActualNode->setOTOperation();

	    } else{      /* Prev hat kein ->right */
	      
	      if ( (offset = this->copyBracketContent(bracketstring,
						      &expr[e_indx],'(',')')) == -1 ){
		
		delete TopNode;
		throw ParseException(abs_pos, "missing bracket!");

	      }	      

	      e_indx+=offset;
	      abs_pos-=offset-1;

	      // if nothing's in the brackets, we connect an empty element
	      if ( !(ActualNode = this->parse(bracketstring,new_context,locals)) )
		ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	      abs_pos++;
	      new_context.setWithinBrackets(false);

	      if ( bracketstring )
		free(bracketstring);

	    }
	  } else{          /* PrevNode has no predecessor */

	    if ( PrevNode->getRight() ){

	      if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

		delete TopNode;
		throw OutOfMemException();

	      }

	      ActualNode->setETOperator("*");
	      ActualNode->setOTOperation();

	    } else{      /* Prev hat kein ->right */

	      if ( (offset = this->copyBracketContent(bracketstring,
						      &expr[e_indx],'(',')')) == -1 ){

		delete TopNode;
		throw ParseException(abs_pos, "missing bracket!");

	      }

	      e_indx+=offset;
	      abs_pos-=offset-1;

	      if ( !(ActualNode=this->parse(bracketstring,new_context,locals)) )
		ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	      abs_pos++;
	      new_context.setWithinBrackets(false);

	      if ( bracketstring )
		free(bracketstring);

	    }
	  }
	} else{  /* Prev no operator -> digit or variable */
	  if ( PrevNode->isVariable() ){

	    if ( !varlist->isMember(PrevNode->getVariable() )
		 && !locals.isMember(PrevNode->getVariable())){
	      // a functioncall/-definition is following, thus variable
	      // must be operator

	      PrevNode->setETOperator(PrevNode->getVariable());
	      PrevNode->setOTFunction();
	      continue;

	    }

	  }

	  if (!(ActualNode = new MathExpression(abs_pos,varlist,functionlist))){

	    delete TopNode;
	    throw OutOfMemException();

	  }

	  ActualNode->setETOperator("*");
	  ActualNode->setOTOperation();

	}
      } else{ /* kein Prev vorhanden */

	if ( (offset = this->copyBracketContent(bracketstring,&expr[e_indx],'(',')')) == -1 ){

	  delete TopNode;
	  throw ParseException(abs_pos, "missing bracket!");
	  
	}
	
	e_indx+=offset;
	abs_pos-=offset-1;

	if ( !(ActualNode=this->parse(bracketstring,new_context,locals)) )
	  ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	abs_pos++;
	new_context.setWithinBrackets(false);

	if ( bracketstring )
	  free(bracketstring);

      }

    } else if( expr[e_indx]=='[' ){

      new_context.setWithinBrackets(true);

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

	if ( PrevNode->getLeft() ){

	  delete TopNode;
	  throw ParseException(abs_pos, "invalid syntax!");

	}

	if ( (offset = this->copyBracketContent(bracketstring,&expr[e_indx],'[',']')) == -1 ){

	  delete TopNode;
	  throw ParseException(abs_pos, "missing bracket!");

	}

	e_indx+=offset;
	abs_pos-=offset-1;

	if ( !(ActualNode=this->parse(bracketstring,new_context,locals)) )
	  ActualNode = new MathExpression(abs_pos,varlist,functionlist);

	abs_pos++;
	new_context.setWithinBrackets(false);

	// variables defined in Sum or Prod must be added to local Scope
	ActualNode->addVariablesToList(&locals);

	PrevNode->setLeft(ActualNode);
	ActualNode->pred=PrevNode;

	if ( bracketstring )
	  free(bracketstring);

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
	    if ( PrevNode->getRight() ){

	      ActualNode->setETOperator("*");
	      ActualNode->setOTOperation();

	    } else{ /* Prev hat kein right */

	      offset = this->copyFloatContent(exprstring,&expr[e_indx]);
	      e_indx+=offset;

	      ActualNode->setETValue(new Complex(atof(exprstring)));

	      if ( exprstring )
		free(exprstring);

	    }
	  } else{  /* Prev kein operator
		      ( Actual=Zahl, Prev=Zahl => Prev->pred=ln o.a") */
	    ActualNode->setETOperator("*");
	    ActualNode->setOTOperation();

	  }

	} else{ /* kein Prev */

	  offset = this->copyFloatContent(exprstring,&expr[e_indx]);
	  e_indx+=offset;

	  ActualNode->setETValue(new Complex(atof(exprstring)));
	  
	  if ( exprstring )
	    free(exprstring);

	}

      } else{               /* keine Zahl */
	if ( PrevNode ){
	  if ( checkLetter(expr[e_indx]) ){
	    if ( PrevNode->isOperator() ){
	      if ( PrevNode->pred ){
		if ( PrevNode->getRight() ){

		  ActualNode->setETOperator("*");
		  ActualNode->setOTOperation();

		} else{      /* Prev hat kein ->right */

		  e_indx = determineAndSetOperatorOrVariable(expr,e_indx,ActualNode,functionlist);

		}
	      } else{          /* Prev hat kein ->pred */

		if ( PrevNode->getRight() ){

		  ActualNode->setETOperator("*");
		  ActualNode->setOTOperation();

		} else{      /* Prev hat kein ->right */

		  e_indx = determineAndSetOperatorOrVariable(expr,e_indx,ActualNode,functionlist);

		}
	      }
	    } else{  /* Prev kein operator also Zahl */

	      ActualNode->setETOperator("*");
	      ActualNode->setOTOperation();

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
		   && (priCompare(&expr[e_indx],PrevNode->getOperator())==POT_PRI-SIN_PRI)
		   && !PrevNode->getRight() ){
		ActualNode->setETOperator("^");
		ActualNode->setOTOperation();
		/* im Folgenden wird geprueft, ob eine fehlerhafte Eingabe
		   der Form "sin^^ ..." gemacht wurde. Dann ist der PrevNode
		   (immer noch der sin) schon der linke Unterast eines
		   "^"-Operators. Nun wird absichtlich ein fehlerhafter Baum
		   aufgebaut, indem der ActualNode einfach als linker Unterast
		   an den "sin"-Node gehaengt wird. Dies wird beim checken
		   automatisch zu einem Syntaxfehler fuehren. */
		if ( PrevNode->pred ){
		  if ( PrevNode==PrevNode->pred->getLeft() ){

		    prevn=PrevNode;

		    while ( prevn->getLeft() )
		      prevn=prevn->getLeft();

		    prevn->setLeft(ActualNode);
		    ActualNode->pred=prevn;
		    e_indx++;
		    abs_pos++;

		    continue;

		  }
		}
		/* hier ist alles ok (es handelt sich also um das erste "^"),
		   der "sin"-Node muss also zum linken Unterast des ActualNode
		   werden */
		ActualNode->setLeft(PrevNode);
		/* hat der "sin"-Node einen Vorgaenger, muss diese Verbindung
		   auch verbogen werden */
		if ( PrevNode->pred){

		  ActualNode->pred=PrevNode->pred;
		  PrevNode->pred->setRight(ActualNode);

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

		  Context subsequent_context;
		  subsequent_context.setWithinBrackets(true);

		  if ( (offset = this->copyBracketContent(bracketstring,&expr[e_indx],
							  '(',')')) == -1 ){

		    delete TopNode;
		    throw ParseException(abs_pos, "missing bracket!");

		  }
		  
		  e_indx+=offset;
		  abs_pos-=offset-1;
		  
		  if ( !(actn=this->parse(bracketstring,subsequent_context,locals)) )
		    actn = new MathExpression(abs_pos,varlist,functionlist);

		  abs_pos++;
		  subsequent_context.setWithinBrackets(false);

		  if ( bracketstring )
		    free(bracketstring);

		} else{ /* oder um eine simple Zahl? */

		  if ( !(actn = new MathExpression(abs_pos,varlist,functionlist)) ){

		    delete TopNode;
		    throw OutOfMemException();

		  }

		  offset = this->copyFloatContent(exprstring,&expr[e_indx]);
		  e_indx+=offset;

		  actn->setETValue(new Complex(atof(exprstring)));

		  if ( exprstring )
		    free(exprstring);

		}

		actn->pred=ActualNode;
		ActualNode->setRight(actn);
		actn=PrevNode->pred;
		continue;

	      } else {
		
		if ( expr[e_indx]=='!' ){
		  
		  if ( !(actn = new MathExpression(abs_pos,varlist,functionlist)) ){
		    
		    delete TopNode;
		    throw OutOfMemException();
		    
		  }
		  
		  actn->setETOperator("!");
		  actn->setOTOperation();
		  e_indx++;
		  abs_pos++;
		  
		  if ( PrevNode->pred ){
		    
		    PrevNode->pred->setRight(actn);
		    actn->pred=PrevNode->pred;
		    
		  }  else
		    TopNode=actn;
		  
		  actn->setRight(PrevNode);
		  PrevNode->pred=actn;
		  continue;
		  
		} else if ( expr[e_indx] == ',' ){
		  // Tuple-Operator

		  e_indx = parseCommaOperator(expr,e_indx,ActualNode,PrevNode,TopNode,context,new_context,locals);
		  
		  continue;
		  
		} else if ( expr[e_indx] == '=' ){
		  
		  // it's a functiondefinition
		  // to build a correct tree, the local vars must be remembered
		  if ( PrevNode )
		    PrevNode->addVariablesToList(&locals);
		  
		}

		e_indx = determineAndSetOperatorOrVariable(expr,e_indx,ActualNode,functionlist);
		
	      }
	    } else{ /* der Vorgaenger ist kein Operator, also "normale"
		       Verhaeltnisse */
	      if ( expr[e_indx] == '!' ){
		
		if ( !(actn = new MathExpression(abs_pos,varlist,functionlist)) ){
		  
		  delete TopNode;
		  throw OutOfMemException();
		  
		}

		actn->setETOperator("!");
		actn->setOTOperation();
		e_indx++;
		abs_pos++;
		
		if ( PrevNode->pred ){

		  PrevNode->pred->setRight(actn);
		  actn->pred=PrevNode->pred;

		} else
		  TopNode=actn;

		actn->setRight(PrevNode);
		PrevNode->pred=actn;
		continue;
		  
	      } else if ( expr[e_indx] == ',' ){
		// Tuple-Operator

		e_indx = parseCommaOperator(expr,e_indx,ActualNode,PrevNode,TopNode,context,new_context,locals);
		
		continue;
		
	      } else if ( expr[e_indx] == '=' ){

		// it's a functiondefinition
		// to build a correct tree, the local vars must be remembered
		if ( PrevNode )
		  PrevNode->addVariablesToList(&locals);

	      }

	      e_indx = determineAndSetOperatorOrVariable(expr,e_indx,ActualNode,functionlist);

	    }
	  }
	} else{ /* kein PrevNode */

	  e_indx = determineAndSetOperatorOrVariable(expr,e_indx,ActualNode,functionlist);

	}
      }
    }

    if ( PrevNode ){
      if ( ActualNode->isOperator() ){
	if ( PrevNode->isOperator() ){

	  if ( (ActualNode->oprtr[0] == '-' || ActualNode->oprtr[0] == '+')
	       && !ActualNode->getLeft() && !PrevNode->getRight() ){

	    /* falls unaerer minus- oder plusoperator */
	    /* wird eine Mult. mit - bzw. +1 in den Baum eingefuegt */
	    if ( ActualNode->oprtr[0] == '-' )         
	      ActualNode->setETValue(new Complex(-1));
	    else                                   
	      ActualNode->setETValue(new Complex(1));

	    prevn=ActualNode;

	    if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

	      delete TopNode;
	      throw OutOfMemException();

	    }

	    ActualNode->setETOperator("*");
	    ActualNode->setOTOperation();
	    ActualNode->setLeft(prevn);
	    prevn->pred=ActualNode;

	    PrevNode->setRight(ActualNode);
	    ActualNode->pred = PrevNode;

	  } else if ( PrevNode->getRight() ){

	    searchAndSetLowerPriNode(ActualNode,PrevNode,TopNode);

	  } else{   /* Prev hat keinen rechten Unterbaum, also wird hier neuer
		       Ast angefuegt */
	    // z.B "3+sin2"
	    // 	    if (priCompare(ActualNode->oprtr,PrevNode->oprtr)){
	    PrevNode->setRight(ActualNode);
	    ActualNode->pred = PrevNode;
	    // 	    }
	  }
	} else{      /* Prev ist Zahl */

	  searchAndSetLowerPriNode(ActualNode,PrevNode,TopNode);

	}
      } else{        /* Act ist Zahl */

	PrevNode->setRight(ActualNode);
	ActualNode->pred=PrevNode;

      }

      PrevNode=ActualNode;

    } else{               /* erstes Zeichen */

      if ( (ActualNode->oprtr[0] == '-' || ActualNode->oprtr[0] == '+')
	   && !ActualNode->getLeft() ){

	/* falls unaerer minus- oder plusoperator */
	/* wird eine Mult. mit - bzw. +1 in den Baum eingefuegt */
	if ( ActualNode->oprtr[0] == '-' )         
	  ActualNode->setETValue(new Complex(-1));
	else                                   
	  ActualNode->setETValue(new Complex(1));

	prevn=ActualNode;

	if ( !(ActualNode = new MathExpression(abs_pos,varlist,functionlist)) ){

	  delete TopNode;
	  throw OutOfMemException();

	}

	ActualNode->setETOperator("*");
	ActualNode->setOTOperation();
	ActualNode->setLeft(prevn);
	prevn->pred=ActualNode;

      }

      PrevNode=ActualNode;
      TopNode=ActualNode;

    }
  }

  return TopNode;
}

int MathExpression::parseCommaOperator(const char *expr, int e_indx, MathExpression * & ActualNode, MathExpression * & PrevNode, MathExpression * & TopNode,
				       const Context & context, const Context & new_context, VariableList & locals) throw (ParseException, ExceptionBase){

  int offset;
  char *commastring=0;
		  
  ActualNode->setETOperator(",");
  
  if ( context.isWithinBrackets() == true )
    ActualNode->setOTTuple();
  else
    ActualNode->setOTParameter();

  searchAndSetLowerPriNode(ActualNode,PrevNode,TopNode);
  
  // add all further elements of comma to its list
  while ( expr[e_indx] == ',' ){
    
    ++e_indx,++abs_pos;
    offset = copyCommaContent(commastring,&expr[e_indx]);
    
    if ( offset == 0 )
      throw ParseException(abs_pos,"missing operand for comma-operator!");
    
    abs_pos-=offset;
    e_indx+=offset;
    
    ActualNode->addElement(parse(commastring,new_context,locals));
    ActualNode->elements.back()->pred = ActualNode;
    
    if ( commastring )
      free(commastring);
    
  }
  
  return e_indx;
  
}

void MathExpression::searchAndSetLowerPriNode(MathExpression * & ActualNode, MathExpression * & PrevNode, MathExpression * & TopNode){

  MathExpression *actn=0, *prevn=0;
  int priority;
  
  if (PrevNode->pred  ){
  
    actn = ActualNode, prevn=PrevNode->pred;
    
    // search for node with lower priority in hierarchy
    while ( (priority = priCompare(actn->getOperator(),prevn->getOperator())) <= 0
	    && prevn->pred )
      prevn = prevn->pred;
    
    if ( priority <= 0 ){  /* hier muss prevn Top sein */
      
      actn->setLeft(prevn);
      prevn->pred = actn;
      TopNode = actn;
      
    } else{               /* actn hat hoehere Prioritaet */
      
      actn->setLeft(prevn->getRight());
      actn->pred = prevn;
      prevn->setRight(actn);
      
    }
  } else{    /* Prev ist Top */
    
    ActualNode->setLeft(PrevNode);
    PrevNode->pred = ActualNode;
    TopNode = ActualNode;
  }
  
}

int MathExpression::determineAndSetOperatorOrVariable(const char *expr, int e_indx, MathExpression * & ActualNode, const FunctionList *functionlist){

  int offset;
  char *exprstring = 0;

  offset = this->copyOperatorContent(exprstring,&expr[e_indx]);
  e_indx+=offset;
  
  bool is_builtin = false;
  if ( (is_builtin = isBuiltinFunction(exprstring)) || checkOperator(exprstring[0]) ){
    
    ActualNode->setETOperator(exprstring);
    
    if ( is_builtin == true )
      ActualNode->setOTFunction();
    else
      ActualNode->setOTOperation();
    
  } else if ( functionlist ){
    
    if ( functionlist->isMember(exprstring) ){
      
      ActualNode->setETOperator(exprstring);
      ActualNode->setOTFunction();
      
    } else
      ActualNode->setETVariable(exprstring);
    
  } else
    ActualNode->setETVariable(exprstring);
  
  if ( exprstring )
    free(exprstring);
  
  return e_indx;

}

inline bool MathExpression::checkOperator(char x){ 
  return (((unsigned char)(x) >= 1 && (unsigned char)(x) <= '/')
	  || ((unsigned char)(x) >= ':' && (unsigned char)(x) <= '@')
	  || ((unsigned char)(x) >= '[' && (unsigned char)(x) <= '^')
	  || ((unsigned char)(x) == '`')
	  || ((unsigned char)(x) >= '{') );
}

inline bool MathExpression::checkDigit(char x){
  return (((unsigned char)(x) >= '0' && (unsigned char)(x) <= '9')
	  || (unsigned char)(x) == '.');
}
 
inline bool MathExpression::checkLetter(char x){
  return (((unsigned char)(x) >= 'A' && (unsigned char)(x) <= 'Z')
	  || ((unsigned char)(x) >= 'a' && (unsigned char)(x) <= 'z')
	  || ((unsigned char)(x) == '_'));
}

bool MathExpression::isBuiltinFunction(const char *strname){

  char *flist[]={FLIST};

  for (unsigned int i=0;i<sizeof(flist)/sizeof(char *);i++)
    if (!strcmp(strname,flist[i]))
      return true;
  return false;
}

void MathExpression::setETOperator(const char *name){

  oprtr = string(name);

  if ( value )
    delete value;
  value=0;
  variable = string("");
  type=ET_OP;

}

void MathExpression::setETVariable(const char *name){

  variable = string(name);

  oprtr = string("");
  setOType(OT_EMPTY);

  if ( value )
    delete value;
  value = 0;

  type = ET_VAR;

}
  
void MathExpression::setETValue(Value *value){
    
  if ( this->value )
    delete this->value;

  this->value = value;

  oprtr = string("");
  setOType(OT_EMPTY);

  variable = string("");

  type=ET_VAL;
    
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

  if ( isVariable() )
    varlist->insert(getVariable(),new Complex(0));

  if ( getRight() )
    getRight()->addVariablesToList(varlist);
  if ( getLeft() )
    getLeft()->addVariablesToList(varlist);
}
 
bool MathExpression::isBuiltinOperator(char op){

  char oplist[]={OPLIST};

  for (unsigned int i=0;i<sizeof(oplist)/sizeof(char);i++)
    if (op==oplist[i])
      return true;
  return false;
}

bool MathExpression::isOpenBrace(char c){
  return ( c == '(' || c == '[' );
}

bool MathExpression::isCloseBrace(char c){
  return ( c == ')' || c == ']' );
}

int MathExpression::copyCommaContent(char * & exprstring, const char *arg){

  int br_cnt = 0, indx=0;
  DynamicBuffer<char> buffer(BUFSIZE);

  exprstring = 0;

  while ( (br_cnt || (arg[indx] != ',' && !isCloseBrace(arg[indx]))) && arg[indx] ){

    if ( isOpenBrace(arg[indx]) )
      br_cnt++;
    else if ( isCloseBrace(arg[indx]) )
      br_cnt--;

    buffer.put(arg[indx++]);
    abs_pos++;

  } 

  buffer.put('\0');

  exprstring = buffer.merge();

  return indx;

}

int MathExpression::copyBracketContent( char * & exprstring, const char *arg, char open,
					char close){

  int br_cnt=0, indx=0;
  DynamicBuffer<char> buffer(BUFSIZE);

  exprstring = 0;

  if ( open && (arg[indx] != open) )
    return (-1);

  do{

    if ( arg[indx] == open || ( !open && !indx) ){  // auch moeglich: alles bis ...

      if ( br_cnt )
	buffer.put(arg[indx]);
      else if ( !open && !indx )
	buffer.put(arg[0]);

      br_cnt++;

    } else if (arg[indx] == close ){

      br_cnt--; 

      if ( br_cnt )
	buffer.put(arg[indx]);

    } else
      buffer.put(arg[indx]);

    indx++;
    abs_pos++;

  } while ( br_cnt && arg[indx] );

  buffer.put('\0');

  if ( br_cnt )
    return (-1);

  exprstring = buffer.merge();

  return(indx);

}
  
int MathExpression::copyFloatContent(char * & exprstring, const char *arg){

  DynamicBuffer<char> buffer(BUFSIZE);

  int indx=0;
  while (checkDigit(arg[indx])){ 

    buffer.put(arg[indx]);
    indx++;
    abs_pos++;

  }

  buffer.put('\0');

  exprstring = buffer.merge();

  return(indx);
}

int MathExpression::copyOperatorContent(char * & exprstring, const char *arg){

  DynamicBuffer<char> buffer(BUFSIZE);
  bool concat_mode = false;

  int indx=0;

  exprstring = 0;

  switch (arg[indx]){
  case '(':
  case ')':
    break;
  default:
    if ( checkOperator(arg[indx]) ){

      buffer.put(arg[indx]);
      indx++;
      abs_pos++;

    } else{
 
      while ( arg[indx] ){ 

	if ( arg[indx] == '_' )
	  concat_mode = true;

	if ( !(checkLetter(arg[indx]) || (concat_mode && checkDigit(arg[indx]))) )
	  break;

	buffer.put(arg[indx]);
	indx++;
	abs_pos++;

      }
    }

    break;
  }

  buffer.put('\0');

  exprstring = buffer.merge();

  return(indx);
}

void MathExpression::clearString(char *exprstring){
  while (*exprstring)
    *exprstring++=0;
}

int MathExpression::priCompare(const char *c0, const char *c1){

  int p[2],i;
  const char *c[2]={ &c0[0], &c1[0] };

  for ( i = 0; i < 2; i++ ){
    switch( *c[i] ){
    case ';':
      p[i] = SEMICOLON_PRI;
      break;
    case ',':
      p[i] = KOMMA_PRI;
      break;
    case '=':
      p[i] = EQUAL_PRI;
      break;
    case '+':
    case '-':
      p[i] = ADDSUB_PRI;
      break;
    case '*':
    case '/':
    case '\\':
    case '%':
      p[i] = MULTDIV_PRI;
      break;
    case '^':
      p[i] = POT_PRI;
      break;
    case '!':
      p[i] = FAC_PRI;
      break;
    case '@':
      p[i] = BINOM_PRI;
      break;
    default :
      if ( !strcmp(c[i],SUM) ){
	p[i] = ADDSUB_PRI;
	break;
      } else if ( !strcmp(c[i],PROD) ){
	p[i] = MULTDIV_PRI;
	break;
      } else{
	p[i] = SIN_PRI;
	break;
      }
    }
  }
  return ( p[0] - p[1] );
}

bool MathExpression::checkSyntaxAndOptimize(void) throw (ParseException){

  if (this->isOperator()){
    if (checkOperator(this->oprtr[0])){
      if (this->oprtr[0] == '='){
	
	if (this->getLeft() && this->getRight())
	  if (!this->getLeft()->isEmpty() && !this->getRight()->isEmpty()){
	    if (this->getLeft()->isVariable()){
	      if (!this->getLeft()->getLeft() && !this->getLeft()->getRight())
		if (this->getRight()->checkSyntaxAndOptimize())
		  return(true);
	    } else if (this->getLeft()->isOperator()){
	      if (!this->getLeft()->getLeft() && this->getLeft()->getRight()){
		
		// 		// we have a function-defintion and we have to set the operator-type of the
		// 		// parameter-list as  "OT_PARAMETER"
		// 		this->getLeft()->getRight()->setOTParameter();

		if (this->getRight()->checkSyntaxAndOptimize())
		  return true;
	      
	      }
	    }
	    throw ParseException(abs_pos, "invalid syntax for assignment!");
	  }
      
      } else if (this->oprtr[0] == '!'){
	if (!this->getLeft() && this->getRight())
	  if (!this->getRight()->isEmpty())
	    return (true);
      } else if ( this->oprtr[0] == ',' ){

	if ( !this->elements.size() )
	  throw ParseException(abs_pos, "invalid syntax: comma-operator has no arguments!");
	for ( list<MathExpression *>::iterator it = this->elements.begin(); it != this->elements.end(); it++ )
	  if ( (*it)->checkSyntaxAndOptimize() == false )
	    throw ParseException(abs_pos,"invalid syntax in parameter-list!");

	return true;
	  
      } else if (this->getLeft() && this->getRight()){
	
	if (!this->getLeft()->isEmpty() && !this->getRight()->isEmpty())
	  if (this->getLeft()->checkSyntaxAndOptimize() && this->getRight()->checkSyntaxAndOptimize()){
	    
	    // optimization: reduction  of "({-,+}1)*({-,+}1)" to "{-,+}1"
	    if ( this->oprtr[0] == '*' )
	      if ( this->getLeft()->isValue() == true && this->getRight()->isValue() == true )
		if ( this->getLeft()->getValue()->getType() == Value::COMPLEX
		     && this->getRight()->getValue()->getType() == Value::COMPLEX){
		  
		  Complex *left = (Complex *)this->getLeft()->getValue();
		  Complex *right = (Complex *)this->getRight()->getValue();
		  
		  if ( left->getIm() == 0 && right->getIm() == 0 ){
		    
		    if ( abs(left->getRe()) == 1 && abs(right->getRe()) == 1 ){
		      
		      this->setETValue(new Complex(sgn(left->getRe()) * right->getRe()));
		      
		      delete this->getLeft();
		      delete this->getRight();
		      this->setLeft(0);
		      this->setRight(0);
		      
		    }
		    
		  }
		  
		}
	    
	    return(true);

	  }
      }

      throw ParseException(abs_pos, "invalid syntax for binary operator!");

    } else{

      // so far, it was difficult to determine if a (...)-expression is a tuple or a parameterlist;
      // that's why we decide and set it here properly
      //       if ( this->getRight() )
      // 	this->getRight()->setOTParameter();

      if (!strcmp(this->getOperator(),"log")){
	if (this->getLeft() && this->getRight())
	  if (!this->getLeft()->isEmpty() && !this->getRight()->isEmpty())
	    if (this->getLeft()->checkSyntaxAndOptimize() && this->getRight()->checkSyntaxAndOptimize())
	      return true;
	throw ParseException(abs_pos, "invalid syntax in function log!");
      } else if (!strcmp(this->getOperator(),SUM)
		 || !strcmp(this->getOperator(),PROD)){
	if (this->getLeft() && this->getRight())
	  if (!this->getLeft()->isEmpty() && !this->getRight()->isEmpty())
	    if (this->getLeft()->oprtr[0] == ';')
	      if (this->getLeft()->getLeft() && this->getLeft()->getRight())
		if (!this->getLeft()->getLeft()->isEmpty() && !this->getLeft()->getRight()->isEmpty())
		  if (this->getLeft()->getLeft()->oprtr[0] == '='){
		    try{
		      if (this->getLeft()->getLeft()->checkSyntaxAndOptimize())
			if (this->getLeft()->getRight()->checkSyntaxAndOptimize())
			  if (this->getRight()->checkSyntaxAndOptimize())
			    return true;
		    } catch (ParseException &pe){
		      throw ParseException(abs_pos, pe.getMsg() + "invalid syntax in function Sum/Prod!");
		    }
		  }
	throw ParseException(abs_pos, "invalid syntax in function Sum/Prod!");
      } else if (isBuiltinFunction(this->getOperator())){
	if (!this->getLeft() && this->getRight())
	  if (this->getRight()->isEmpty()==false)
	    if (this->getRight()->checkSyntaxAndOptimize())
	      return(true);
	throw ParseException(abs_pos, "invalid syntax for builtin-function!");
      } else if (functionlist->isMember(this->getOperator())){
	if (!this->getLeft() && this->getRight()){
	  if (this->getRight()->checkSyntaxAndOptimize()){
	    if (this->getRight()->countArgs() ==
		functionlist->get(this->getOperator())->getParameterList()->countArgs())
	      return(true);
	    else{

	      ostringstream out;
	      out << "invalid syntax for user-defined function: invalid number of arguments: " << this->getRight()->countArgs() << " : "
		  << functionlist->get(this->getOperator())->getParameterList()->countArgs();
	      throw ParseException(abs_pos,out.str());
	    }
	  } else
	    throw ParseException(abs_pos,"invalid syntax for user-defined function: checkSyntax for right operand failed!");
	} else
	  throw ParseException(abs_pos,"invalid syntax for user-defined function: left or no right operand!");

	throw ParseException(abs_pos, "invalid syntax for user-defined function!");

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
  if ( !this->isEmpty() ){
    if ( this->isOperator() ){
      if ( this->oprtr[0] == '!' ){

	if (this->getRight())
	  exp << this->getRight()->toString(precision);

	exp << this->getOperator();

      } else if ( this->oprtr[0] == ',' ){

	for ( list<MathExpression *>::const_iterator it = elements.begin(); it != elements.end(); it++ )
	  exp << ( it == elements.begin() ? "" : this->getOperator() ) << (*it)->toString(precision);

      } else if (!strcmp(this->getOperator(),"log")
		 || !strcmp(this->getOperator(),SUM)
		 || !strcmp(this->getOperator(),PROD)){

	exp << this->getOperator();
	exp << "[" << this->getLeft()->toString(precision) << "]";
	exp << this->getRight()->toString(precision);

      } else{

	if (this->getLeft())
	  exp << this->getLeft()->toString(precision);

	exp << this->getOperator();

	if (this->getRight())
	  exp << this->getRight()->toString(precision);

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


Value *MathExpression::eval() throw (ExceptionBase,FunctionDefinition){

  //   double result;
  if ( isOperator() ){

    switch ( oprtr[0] ){

    case '+':

      this->setValue(getLeft()->eval()->operator+(getRight()->eval()));
      break;

    case '-':

      this->setValue(getLeft()->eval()->operator-(getRight()->eval()));
      break;

    case '*':

      this->setValue(getLeft()->eval()->operator*(getRight()->eval()));
      break;

    case '/':

      this->setValue(getLeft()->eval()->operator/(getRight()->eval()));
      break;

    case '\\':

      this->setValue(getLeft()->eval()->integerDivision(getRight()->eval()));
      break;

    case '%':

      this->setValue(getLeft()->eval()->operator%(getRight()->eval()));
      break;

    case '^':

      this->setValue(getLeft()->eval()->pow(getRight()->eval()));
      break;

    case '!':

      this->setValue(getRight()->eval()->faculty());
      break;

    case '@':

      //       return (faculty(getLeft()->eval())/(faculty(getRight()->eval())
      // 				     *faculty(getLeft()->eval() - getRight()->eval())));
      this->setValue(getLeft()->eval()->choose(getRight()->eval()));
      break;

    case '=':

      if (getLeft()->isVariable()){

	this->setValue(assignValue());
	break;

      } else if (getLeft()->isOperator()){

	defineFunction();
	throw FunctionDefinition(getLeft()->getOperator());
	break;

      }

      throw EvalException("invalid use of assignment!");
      break;

    case ',':

      if ( this->isOTTuple() == true )
	this->setValue(evalTupleExpression());
      else if ( this->isOTParameter() == true )
	throw EvalException("parameterlist can't be evaluated!");
      else
	throw EvalException("internal error: getOType() == OT_EMPTY!");
	
      break;

    default:
      if (!strcmp("sin",getOperator()))
        this->setValue(getRight()->eval()->sin());
      else if (!strcmp("cos",getOperator()))
	this->setValue(getRight()->eval()->cos());
      else if (!strcmp("tan",getOperator()))
	this->setValue(getRight()->eval()->tan());
      else if (!strcmp("asin",getOperator()))
	this->setValue(getRight()->eval()->asin());
      else if (!strcmp("acos",getOperator()))
	this->setValue(getRight()->eval()->acos());
      else if (!strcmp("atan",getOperator()))
	this->setValue(getRight()->eval()->atan());
      else if (!strcmp("sinh",getOperator()))
	this->setValue(getRight()->eval()->sinh());
      else if (!strcmp("cosh",getOperator()))
	this->setValue(getRight()->eval()->cosh());
      else if (!strcmp("tanh",getOperator()))
	this->setValue(getRight()->eval()->tanh());
      else if (!strcmp("asinh",getOperator()))
	this->setValue(getRight()->eval()->asinh());
      else if (!strcmp("acosh",getOperator()))
	this->setValue(getRight()->eval()->acosh());
      else if (!strcmp("atanh",getOperator()))
	this->setValue(getRight()->eval()->atanh());
      else if (!strcmp("ln",getOperator())){

	this->setValue(getRight()->eval()->ln());

      } else if (!strcmp("ld",getOperator())){

	this->setValue(getRight()->eval()->ld());

      } else if (!strcmp("log",getOperator())){

	this->setValue(getRight()->eval()->log(getLeft()->eval()));

      } else if (!strcmp("exp",getOperator()))

	this->setValue(getRight()->eval()->exp());

      else if (!strcmp(SUM,getOperator()) || !strcmp(PROD,getOperator())){

	this->setValue(sumProd());

      } else if (!strcmp("sgn",getOperator())){

	this->setValue(getRight()->eval()->sgn());

      } else if (!strcmp("tst",getOperator())){

	this->setValue(getRight()->eval()->tst());

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

Value * MathExpression::evalTupleExpression(){

  Tuple *value = new Tuple();

  for ( list<MathExpression *>::const_iterator it = elements.begin(); it != elements.end(); it++ )
    value->addElement((*it)->eval()->clone());

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

  for (unsigned long i=(unsigned long)fac-1;i>0;i--)
    fac*=i;

  return(fac);

}

Value *MathExpression::sumProd(void) throw (ExceptionBase){

  VariableList vl = *this->varlist;  // lokaler Scope: Kopie der globalen Varlist
  MathExpression me(this,&vl,this->functionlist,abs_pos);
  Variable *currve;
  double from, to;
  Value *value;
  Complex *c_from,*c_to;
  char p = 0;
  
  if ( !strcmp(PROD,this->getOperator()) )
    p = 1;
  
  // Indexvariable zum lokalen Scope hinzufuegen
  vl.insert(this->left->left->left->getVariable(),
	    this->left->left->right->eval()->clone());

  //Zaehlbereich
  c_from = Complex::assertNatural(vl.getValue(me.left->left->left->getVariable()));
  c_to = Complex::assertNatural(me.left->right->eval()->clone());

  from = c_from->getRe();
  to = c_to->getRe();

  if ( from != (int)from || from < 0 || to != (int)to )
    throw EvalException("indices in Sum/Prod not natural or negative!");

  if ( to < from )
    return 0;

  if ( !p )
    value = me.right->eval()->neutralAddition();
  else
    value = me.right->eval()->neutralMultiplikation();

  for ( unsigned long i = (unsigned long)from; i <= (unsigned long)to; ){

    if ( !p )
      (*value) += me.right->eval();  // Summe
    else
      (*value) *= me.right->eval();  // Produkt

    i++;

    // neuen Wert der Zaehlvariablen eintragen
    vl.insert(this->left->left->left->getVariable(),new Complex((cmplx_tp)i));

  }

  // neu definierte Variablen in globale Varlist eintragen
  currve = vl.first;
  while ( currve ){

    if ( strcmp(currve->getName(),this->left->left->left->getVariable())
	&& !currve->getProtect() )
      this->varlist->insert(currve->getName(),currve->getValue()->clone());

    currve = currve->getNext();

  }

  return value;
}

Value *MathExpression::assignValue(void) throw (ExceptionBase){

  Value *result;

  result = getRight()->eval()->clone();

  if ( functionlist )
    if ( functionlist->isMember(getLeft()->getOperator()) )
      throw EvalException ("variable already defined as function");

  if ( varlist ){

    try{

      varlist->insert(getLeft()->getVariable(),result);

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
  Function *f_template = functionlist->get(this->getOperator());

  vl.unprotect();  // remove protection-status for all variables

  // assign values to all variables occuring in function-head and add them to local scope
  assignVariablesInFunctionhead(vl,f_template->getParameterList(),this->getRight());

  // build an instance of the function-template
  MathExpression function(f_template->getBody(),&vl,functionlist,abs_pos);

  // eval the function
  return function.eval()->clone();

}

void MathExpression::assignVariablesInFunctionhead(VariableList & vl, MathExpression * parameter, MathExpression * argument) throw (ExceptionBase){

  // if it's a variable it can be assigned directly
  if ( parameter->isVariable() )
    vl.insert(parameter->getVariable(), argument->eval()->clone());
  else{
    // a tuple of variables
    
    list<MathExpression *>::iterator pit, ait;

    for ( pit = parameter->elements.begin(), ait = argument->elements.begin();
	  pit != parameter->elements.end();
	  pit++, ait++ ){
      
      if ( ait == argument->elements.end() )
	throw EvalException(string("no matching argument to parameter: ") + (*pit)->toString(Value::DFLT_PRECISION) + "!");

      // recursive assigment
      assignVariablesInFunctionhead(vl,(*pit),(*ait));

    }

    if ( ait != argument->elements.end() )
      throw EvalException("too many arguments for function!");
  }

}

void MathExpression::defineFunction(void) throw (EvalException,ParseException){

  const char *functionname;
  MathExpression *paramlist=0, *body=0;
  Function *fe=0;
  VariableList locals;

  functionname = this->getLeft()->getOperator();
  
  // already declared as variable? (impossible to happen)
  if ( varlist->isMember(functionname) )
    throw ParseException(abs_pos, string(functionname)
			 + " already declared as variable, definition "
			 + "not possible!");
  
  // built-in function?
  if ( isBuiltinFunction(functionname) )
    throw ParseException(abs_pos, string(functionname)
			 + " is built-in function and can't be replaced!");
  
  // already declared as function?
  if ( functionlist->isMember(functionname) )
    throw ParseException(abs_pos, string(functionname)
			 + " already declared as function, to redefine it"
			 + " first undefine it!");

  // build parameterlist
  paramlist = new MathExpression(this->getLeft()->getRight(),varlist,functionlist,abs_pos);

  // must be a variabletree
  if ( paramlist->checkForVariableTree() == false ){

    delete paramlist;
    throw ParseException(abs_pos, string("only letters and non-built-in functionnames allowed,")
			 + " variables must be separated by commas and may not occur more then once!");

  }
  
  // build functionbody
  body = new MathExpression(this->getRight(),varlist,functionlist,abs_pos);

  try{
    
    // body must only contain defined variables/functions/operators
    checkBody(body,paramlist,&locals);
    
  } catch (EvalException e){
    
    delete body;
    delete paramlist;
    throw e;
    
  }
  
  // build functionelement
  fe = new Function(functionname,paramlist,body);
  
  functionlist->insert(fe); // insert in functionlist;

  functionlist->setModified(true);
  
}

void MathExpression::checkBody(MathExpression *body, MathExpression *pl,
			       VariableList *lvl) const throw(EvalException){

  // pl: parameterTree
  // lvl: local VariableList

  bool inparam=false;
  
  if (!body)
    return;

  if (*(body->getOperator())){
    if (!(isBuiltinOperator(*(body->getOperator()))))
      if (!(isBuiltinFunction(body->getOperator())))
	if (!(functionlist->isMember(body->getOperator())))
	  throw EvalException("undefined operator:",body->getOperator());
  } else if (*(body->getVariable())){
    if (pl)
      if (pl->isVariableInTree(body->getVariable()))
	inparam=true;
    if (!inparam){
      if (!varlist->isMember(body->getVariable())){
	if (!lvl->isMember(body->getVariable())){
	  if (!body->getPred())
	    throw EvalException("variable undefined:",body->getVariable());
	  else{
	    if ( (body->getPred()->getOperator())[0] != '=' )
	      throw EvalException("variable undefined:",body->getVariable());
	    else
	      lvl->insert(body->getVariable(),new Complex(0));
	  }
	}
      }
    }
  }

  for ( list<MathExpression *>::const_iterator it = body->elements.begin(); it != body->elements.end(); it++ )
    checkBody(*it,pl,lvl);
  
  return;
  
}

bool MathExpression::checkForVariableTree() const {

  set< string,less<string> > variableset;

  return checkForVariableTree(variableset);

}

bool MathExpression::checkForVariableTree(set< string,less<string> > & variableset) const {
  
  if ( !elements.empty() ){
    
    if ( this->oprtr[0] != ',' )
      return false;
    
    for ( list<MathExpression *>::const_iterator it = elements.begin(); it != elements.end(); it++ )
      if ( (*it)->checkForVariableTree(variableset) == false )
	return false;
    
  } else if ( !this->isVariable() ) {
    return false;
  } else if ( (variableset.find(getVariable())) != variableset.end() )
    return false; // variable already occured somewhere
  
  variableset.insert(getVariable());
  
  return true;
  
}

bool MathExpression::isVariableInTree(const char *name) const {
  
  if ( this->getVariable() != string(name) ){
    
    for ( list<MathExpression *>::const_iterator it = elements.begin(); it != elements.end(); it++ )
      if ( (*it)->isVariableInTree(name) )
	return true;
    
    return false;

  }

  return true;

}
    
unsigned int MathExpression::countArgs(void){

  if ( this->isEmpty() )
    return 0;

  if ( this->oprtr[0] == ',' )
    return elements.size();
  else
    return 1;
}
