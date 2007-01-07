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
    "asinh","acosh","atanh","ln","ld","log","exp","sgn",SUM,PROD
#define OPLIST '+','-','*','/','\\','%','^','!','@','=','(',')','[',']',',',';'

  using namespace std;
using namespace mexp;

VarElement::VarElement(const char *name, double value, char protect)
  throw (Exception_T) : value(value), protect(protect), next(0){

  if (!(this->name = new char[strlen(name)+1]))
    throw OutOfMemException();
  strcpy(this->name,name);
}

VarElement::~VarElement(){
  delete [] this->name;
}

VarList::VarList(const VarList& vl) throw (Exception_T) : first(0), last(0), modified(false){

  const VarElement *curr;

  curr = vl.first;
  while (curr){

    this->insert(curr->getName(),curr->getValue(),curr->protect);
    curr = curr->next;

  }
}

VarList::~VarList(){

  VarElement *curr, *next;

  curr = first;
  while (curr){
    next = curr->next;
    delete curr;
    curr = next;
  }
}

void VarList::insert(const char *name, double value, char protect)
  throw (Exception_T){

  VarElement *ve;

  if ((ve = isMember(name))){
    if (!ve->protect)
      ve->value = value;
    else
      throw EvalException("redefinition not possible!",
			  ve->getName());
  } else {
    if (!(ve = new VarElement(name,value,protect)))
      throw OutOfMemException();
      
    if (!first)
      first = ve;
    if (last)
      last->next = ve;
    last = ve;
  }
}

void VarList::remove(const char *name) throw (Exception<VarList>){

  VarElement *curr, *prev = 0;

  curr = first;
  while (curr){
    if (!strcmp(name,curr->getName())){
      if (curr == first)
	first=curr->next;
      if (curr == last)
	last=prev;
      if (prev)
	prev->next = curr->next;
      delete curr;
      return;
    }
    prev = curr;
    curr = curr->next;
  }
  throw Exception<VarList>("not defined!");
}

double VarList::getValue(const char *name) const throw (Exception_T){

  const VarElement *ve;

  ve = first;
  while (ve){
    if (!strcmp(ve->getName(),name))
      return ve->getValue();
    ve = ve->next;
  }
  throw EvalException("unknown variable!",name);
}

VarElement *VarList::isMember(const char *name) const{

  const VarElement *ve;
  
  ve=first;
  while (ve){
    if (!strcmp(ve->getName(),name))
      return const_cast<VarElement *>(ve);
    ve=ve->next;
  }
  return 0;
}

void VarList::unprotect(const char *name){

  VarElement *curr=0;

  if (name)
    this->isMember(name)->protect=0;
  else{
    curr=this->first;
    while (curr){
      curr->protect=0;
      curr=curr->next;
    }
  }
  return;
}

void VarList::print() const{
  cout << toString(true);
}

string VarList::toString(bool include_protected) const{

  ostringstream vars;

  const VarElement *ve;

  ve=first;
  while(ve){

    if ( ve->protect == false || include_protected == true )
      vars << ve->getName() << "=" << ve->getValue() << endl;

    ve = ve->next;

  }

  return vars.str();

}

FunctionElement::FunctionElement(const char *name, MathExpression *paramlist,
				 MathExpression *body)
  : paramlist(paramlist), body(body), next(0){

  this->name = new char[strlen(name)+1];
  strcpy(this->name,name);
}

FunctionElement::~FunctionElement(){

  delete [] this->name;
  delete this->paramlist;
  delete this->body;
}

FunctionList::~FunctionList(){

  FunctionElement *curr, *next;

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

FunctionElement *FunctionList::get(const char *name) const{

  const FunctionElement *fe;
  
  fe=first;
  while (fe){
    if (!strcmp(fe->getName(),name))
      return const_cast<FunctionElement *>(fe);
    fe=fe->next;
  }
  return 0;
}


void FunctionList::insert(FunctionElement *fe) throw(Exception<FunctionList>){

  if (this->isMember(fe->getName()))
    throw Exception<FunctionList>("already in list!");

  if (!first)
    first=fe;
  if (last)
    last->next=fe;
  last=fe;
}

void FunctionList::remove(const char *name) throw(Exception<FunctionList>){

  FunctionElement *curr, *prev=0;

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

void FunctionList::print(const char *name){

  FunctionElement *curr;
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
	curr->paramlist->print();
      else
	std::cout << "()";

      std::cout << "=";
      curr->body->print();
      std::cout << "\n";
    }
    curr = curr->next;
    if ( stop )
      break;
  }
}

string FunctionList::toString() const{

  ostringstream funs;

  FunctionElement *curr;

  curr = this->first;

  while ( curr ){

    funs << curr->getName();

    if (curr->paramlist)
      funs << curr->paramlist->toString();
    else
      funs << "()";

    funs << "=";
    funs << curr->body->toString();
    funs << endl;

    curr = curr->next;

  }

  return funs.str();

}

MathExpression::MathExpression(VarList *vl, FunctionList *fl) :
  varlist(vl),functionlist(fl),left(0), right(0), pred(0), value(0),
  type(EMPTY){
  
  for (int i=0;i<OP_LEN;i++)
    oprtr[i]=0;
  for (int i=0;i<VARLEN;i++)
    variable[i]=0;
  }

MathExpression::MathExpression(const char *expression, VarList *vl,
			       FunctionList *fl)
  throw (SubException<SyntaxErr,MathExpression>,Exception_T)
  : varlist(vl), functionlist(fl), left(0), right(0), pred(0), value(0),
    type(EMPTY){

  MathExpression *top;
  VarList locals;
  
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
      this->setOprtr(top->getOprtr());
      break;
    case VAR:
      this->setVariable(top->getVariable());
      break;
    case VAL:
      this->setValue(top->getValue());
      break;
    case EMPTY:
    default:
      this->setType(top->getType());
      break;
    }
  }
  
  try{

    this->checkSyntaxAndOptimize();

  }catch (SubException<SyntaxErr,MathExpression> &mese){

    delete top;
    throw SubException<SyntaxErr,MathExpression>(mese.getMsg());

  }

  // Es wird nur free() aufgerufen, da die Unteraeste schon in den
  // Klassenvariablen gespeischert wurden und nicht geloescht werden sollen
  free(top);
    }

MathExpression::MathExpression(MathExpression *me, VarList *vl,
			       FunctionList *fl)
  throw (SubException<SyntaxErr,MathExpression>,Exception_T)
  : varlist(vl), functionlist(fl), left(0), right(0), pred(0), value(0),
    type(EMPTY){
  
  for (int i=0;i<OP_LEN;i++)
    oprtr[i]=0;
  for (int i=0;i<VARLEN;i++)
    variable[i]=0;

  if (!me)
    return;

  switch (me->getType()){
  case OP:
    this->setOprtr(me->getOprtr());
    break;
  case VAR:
    this->setVariable(me->getVariable());
    break;
  case VAL:
    this->setValue(me->getValue());
    break;
  case EMPTY:
  default:
    this->setType(EMPTY);
    break;
  }
  if (me->left){
    this->left = new MathExpression(me->left,vl,fl);
    this->left->pred=this;
  }
  if (me->right){
    this->right = new MathExpression(me->right,vl,fl);
    this->right->pred=this;
  }
    }
  
MathExpression::~MathExpression(){
  
  if (!this)
    return;
  if (this->left)
    delete this->left;
  if (this->right)
    delete this->right;
}

MathExpression *MathExpression::parse(const char *expr, VarList& locals)
  throw (SubException<SyntaxErr,MathExpression>,Exception_T){

  int e_indx, priority, offs;
  char exprstring[OP_LEN]={0}, bracketstring[BR_LEN]={0};
  MathExpression *TopNode=0, *ActualNode=0, *PrevNode=0, *actn, *prevn;

  if (!expr[0])
    return 0;

  for (e_indx=0;expr[e_indx];){

    if (expr[e_indx]==')'){

      delete TopNode;
      throw SubException<SyntaxErr,MathExpression>("invalid expression!");

    }

    if (expr[e_indx]=='('){
      if (PrevNode){
	if (PrevNode->isOprtr()){
	  if (PrevNode->pred){
	    if (PrevNode->right){
	      /* hier handelt es sich um Ausdruecke der Form
		 "1+sin2(...), also um eine implizite Multipikation,
		 es muss also ein "*" in den Baum eingefuegt werden*/
	      if (!(ActualNode = new MathExpression(varlist,functionlist))){
		delete TopNode;
		throw OutOfMemException();
	      }
	      ActualNode->setOprtr("*");
	    } else{      /* Prev hat kein ->right */
	      if ((offs=this->brackcpy(bracketstring,
				       &expr[e_indx],'(',')'))==-1){
		delete TopNode;
		throw SubException<SyntaxErr,MathExpression>("missing bracket!");
	      }	      
	      e_indx+=offs;

	      // if nothing's in the brackets, we connect an empty element
	      if (!(ActualNode=this->parse(bracketstring,locals)))
		ActualNode = new MathExpression(varlist,functionlist);
	      this->clearString(bracketstring);
	    }
	  } else{          /* PrevNode has no predecessor */

	    if (PrevNode->right){

	      if (!(ActualNode = new MathExpression(varlist,functionlist))){

		delete TopNode;
		throw OutOfMemException();

	      }

	      ActualNode->setOprtr("*");

	    } else{      /* Prev hat kein ->right */

	      if ((offs=this->brackcpy(bracketstring,
				       &expr[e_indx],'(',')'))==-1){

		delete TopNode;
		throw SubException<SyntaxErr,MathExpression>("missing bracket!");

	      }

	      e_indx+=offs;
	      if (!(ActualNode=this->parse(bracketstring,locals)))
		ActualNode = new MathExpression(varlist,functionlist);

	      this->clearString(bracketstring);

	    }
	  }
	} else{  /* Prev no operator -> digit or variable */
	  if (PrevNode->isVariable()){
	    if (!varlist->isMember(PrevNode->getVariable())
		&& !locals.isMember(PrevNode->getVariable())){

	      // a functioncall/-definition is following, thus variable
	      // must be operator
	      PrevNode->setOprtr(PrevNode->getVariable());
	      continue;

	    }
	  }

	  if (!(ActualNode = new MathExpression(varlist,functionlist))){

	    delete TopNode;
	    throw OutOfMemException();

	  }

	  ActualNode->setOprtr("*");

	}
      } else{ /* kein Prev vorhanden */
	if ((offs=this->brackcpy(bracketstring,&expr[e_indx],'(',')'))==-1){

	  delete TopNode;
	  throw SubException<SyntaxErr,MathExpression>("missing bracket!");

	}
	
	e_indx+=offs;

	if (!(ActualNode=this->parse(bracketstring,locals)))
	  ActualNode = new MathExpression(varlist,functionlist);

	this->clearString(bracketstring);

      }
    } else if(expr[e_indx]=='['){

      if (!PrevNode){

	delete TopNode;
	throw SubException<SyntaxErr,MathExpression>("invalid operator \"[\"!");

      }
      if (!PrevNode->isOprtr()){

	delete TopNode;
	throw SubException<SyntaxErr,MathExpression>("invalid operator \"[\"!");

      }

      if (!strcmp(PrevNode->getOprtr(),"log")
	  || !strcmp(PrevNode->getOprtr(),SUM)
	  || !strcmp(PrevNode->getOprtr(),PROD)){

	if (PrevNode->left){

	  delete TopNode;
	  throw SubException<SyntaxErr,MathExpression>("invalid expression!");

	}

	if ((offs=this->brackcpy(bracketstring,&expr[e_indx],'[',']'))==-1){

	  delete TopNode;
	  throw SubException<SyntaxErr,MathExpression>("missing bracket!");

	}

	e_indx+=offs;

	if (!(ActualNode=this->parse(bracketstring,locals)))
	  ActualNode = new MathExpression(varlist,functionlist);

	PrevNode->left=ActualNode;
	ActualNode->pred=PrevNode;
	this->clearString(bracketstring);
	continue;

      } else{

	delete TopNode;
	throw SubException<SyntaxErr,MathExpression>("invalid operator \"[\"!");

      }
    } else{        /* kein Bracket */
      if (!(ActualNode = new MathExpression(varlist,functionlist))){

	delete TopNode;
	throw OutOfMemException();

      }

      if (checkDigit(expr[e_indx])){
	if (PrevNode){
	  if (PrevNode->isOprtr()){
	    if (PrevNode->right){

	      ActualNode->setOprtr("*");

	    } else{ /* Prev hat kein right */

	      e_indx+=this->flcpy(exprstring,&expr[e_indx]);
	      ActualNode->setValue(atof(exprstring));
	      this->clearString(exprstring);

	    }
	  } else{  /* Prev kein operator
		      ( Actual=Zahl, Prev=Zahl => Prev->pred=ln o.a") */
	    ActualNode->setOprtr("*");

	  }

	} else{ /* kein Prev */

	  e_indx+=this->flcpy(exprstring,&expr[e_indx]);
	  ActualNode->setValue(atof(exprstring));
	  this->clearString(exprstring);

	}

      } else{               /* keine Zahl */
	if (PrevNode){
	  if (checkLetter(expr[e_indx])){
	    if (PrevNode->isOprtr()){
	      if (PrevNode->pred){
		if (PrevNode->right){

		  ActualNode->setOprtr("*");

		} else{      /* Prev hat kein ->right */

		  e_indx+=this->oprcpy(exprstring,&expr[e_indx]);

		  if (isBuiltinFunc(exprstring) || checkOprtr(exprstring[0]))
		    ActualNode->setOprtr(exprstring);
		  else if (functionlist){

		    if (functionlist->isMember(exprstring))
		      ActualNode->setOprtr(exprstring);
		    else
		      ActualNode->setVariable(exprstring);

		  } else
		    ActualNode->setVariable(exprstring);

		  this->clearString(exprstring);

		}
	      } else{          /* Prev hat kein ->pred */

		if (PrevNode->right){

		  ActualNode->setOprtr("*");

		} else{      /* Prev hat kein ->right */

		  e_indx+=this->oprcpy(exprstring,&expr[e_indx]);

		  if (isBuiltinFunc(exprstring) || checkOprtr(exprstring[0]))
		    ActualNode->setOprtr(exprstring);
		  else if (functionlist){

		    if (functionlist->isMember(exprstring))
		      ActualNode->setOprtr(exprstring);
		    else
		      ActualNode->setVariable(exprstring);

		  } else
		    ActualNode->setVariable(exprstring);

		  this->clearString(exprstring);
		}
	      }
	    } else{  /* Prev kein operator also Zahl */

	      ActualNode->setOprtr("*");

	    }
	  } else{ /* Arithmetischer Operator */

	    if (PrevNode->isOprtr()){
	      /* behandelt werden sollen Ausdruecke der Form: "sin^2(...)": */
	      /* ist das  aktuelle Zeichen ein "^" und der PrevNode ein
		 Operator, handelt es sich um die Form wie z.B. "sin^2",
		 allerdings nur, wenn der PrevNode noch keinen rechten
		 Unterast hat, denn dann muss es sich um eine Form wie
		 "(sin2)^3" handeln */
	      if (expr[e_indx]=='^'
		  && (pri(&expr[e_indx],PrevNode->getOprtr())==POT_PRI-SIN_PRI)
		  && !PrevNode->right){
		ActualNode->setOprtr("^");
		/* im Folgenden wird geprueft, ob eine fehlerhafte Eingabe
		   der Form "sin^^ ..." gemacht wurde. Dann ist der PrevNode
		   (immer noch der sin) schon der linke Unterast eines
		   "^"-Operators. Nun wird absichtlich ein fehlerhafter Baum
		   aufgebaut, indem der ActualNode einfach als linker Unterast
		   an den "sin"-Node gehaengt wird. Dies wird beim checken
		   automatisch zu einem Syntaxfehler fuehren. */
		if (PrevNode->pred){
		  if (PrevNode==PrevNode->pred->left){

		    prevn=PrevNode;

		    while (prevn->left)
		      prevn=prevn->left;

		    prevn->left=ActualNode;
		    ActualNode->pred=prevn;
		    e_indx++;
		    continue;

		  }
		}
		/* hier ist alles ok (es handelt sich also um das erste "^"),
		   der "sin"-Node muss also zum linken Unterast des ActualNode
		   werden */
		ActualNode->left=PrevNode;
		/* hat der "sin"-Node einen Vorgaenger, muss diese Verbindung
		   auch verbogen werden */
		if (PrevNode->pred){

		  ActualNode->pred=PrevNode->pred;
		  PrevNode->pred->right=ActualNode;

		} else
		  TopNode=ActualNode;

		PrevNode->pred=ActualNode;
		e_indx++;
		/* Auswertung der Potenz: handelt es sich um einen
		   Klammerausdruck? ... */
		if (!expr[e_indx])
		  continue;
		else if (expr[e_indx]=='('){

		  if ((offs=this->brackcpy(bracketstring,&expr[e_indx],
					   '(',')'))==-1){

		    delete TopNode;
		    throw SubException<SyntaxErr,MathExpression>("missing bracket!");

		  }
		  
		  e_indx+=offs;

		  if (!(actn=this->parse(bracketstring,locals)))
		    actn = new MathExpression(varlist,functionlist);

		  this->clearString(bracketstring);

		} else{ /* oder um eine simple Zahl? */

		  if (!(actn = new MathExpression(varlist,functionlist))){

		    delete TopNode;
		    throw OutOfMemException();

		  }

		  e_indx+=this->flcpy(exprstring,&expr[e_indx]);
		  actn->setValue(atof(exprstring));
		  this->clearString(exprstring);

		}

		actn->pred=ActualNode;
		ActualNode->right=actn;
		actn=PrevNode->pred;
		continue;

	      } else{

		if (expr[e_indx]=='!'){

		  if (!(actn = new MathExpression(varlist,functionlist))){

		    delete TopNode;
		    throw OutOfMemException();

		  }

		  actn->setOprtr("!");
		  e_indx++;

		  if (PrevNode->pred){

		    PrevNode->pred->right=actn;
		    actn->pred=PrevNode->pred;

		  }  else
		    TopNode=actn;

		  actn->right=PrevNode;
		  PrevNode->pred=actn;
		  continue;

		} else if (expr[e_indx]=='='){

		  // it's a functiondefinition
		  // to build a correct tree, the local vars must be remembered
		  if (PrevNode)
		    PrevNode->addVarsToList(&locals);

		}

		e_indx+=this->oprcpy(exprstring,&expr[e_indx]);

		if (isBuiltinFunc(exprstring) || checkOprtr(exprstring[0]))
		  ActualNode->setOprtr(exprstring);
		else if (functionlist){

		  if (functionlist->isMember(exprstring))
		    ActualNode->setOprtr(exprstring);
		  else
		    ActualNode->setVariable(exprstring);

		} else
		  ActualNode->setVariable(exprstring);

		this->clearString(exprstring);

	      }
	    } else{ /* der Vorgaenger ist kein Operator, also "normale"
		       Verhaeltnisse */
	      if (expr[e_indx]=='!'){

		if (!(actn = new MathExpression(varlist,functionlist))){

		  delete TopNode;
		  throw OutOfMemException();

		}

		actn->setOprtr("!");
		e_indx++;

		if (PrevNode->pred){

		  PrevNode->pred->right=actn;
		  actn->pred=PrevNode->pred;

		} else
		  TopNode=actn;

		actn->right=PrevNode;
		PrevNode->pred=actn;
		continue;

	      }

	      e_indx+=this->oprcpy(exprstring,&expr[e_indx]);

	      if (isBuiltinFunc(exprstring) || checkOprtr(exprstring[0]))
		ActualNode->setOprtr(exprstring);
	      else if (functionlist){

		if (functionlist->isMember(exprstring))
		  ActualNode->setOprtr(exprstring);
		else
		  ActualNode->setVariable(exprstring);

	      }
	      else
		ActualNode->setVariable(exprstring);

	      this->clearString(exprstring);

	    }
	  }
	} else{ /* kein PrevNode */

	  e_indx+=this->oprcpy(exprstring,&expr[e_indx]);

	  if (isBuiltinFunc(exprstring) || checkOprtr(exprstring[0]))
	    ActualNode->setOprtr(exprstring);
	  else if (functionlist){

	    if (functionlist->isMember(exprstring))
	      ActualNode->setOprtr(exprstring);
	    else
	      ActualNode->setVariable(exprstring);

	  } else
	    ActualNode->setVariable(exprstring);

	  this->clearString(exprstring);

	}
      }
    }
    if (PrevNode){
      if (ActualNode->isOprtr()){
	if (PrevNode->isOprtr()){

	  if ((ActualNode->oprtr[0]=='-' || ActualNode->oprtr[0]=='+')
	      && !ActualNode->left && !PrevNode->right){

	    /* falls unaerer minus- oder plusoperator */
	    /* wird eine Mult. mit - bzw. +1 in den Baum eingefuegt */
	    if (ActualNode->oprtr[0]=='-')         
	      ActualNode->setValue(-1);
	    else                                   
	      ActualNode->setValue(1);

	    prevn=ActualNode;

	    if (!(ActualNode = new MathExpression(varlist,functionlist))){

	      delete TopNode;
	      throw OutOfMemException();

	    }

	    ActualNode->setOprtr("*");
	    ActualNode->left=prevn;
	    prevn->pred=ActualNode;

	    PrevNode->right = ActualNode;
	    ActualNode->pred = PrevNode;

	  } else if (PrevNode->right){
	    if (PrevNode->pred){ /* Da Prev pred hat, muss prioritaet des
				    operators gecheckt werden */
	      actn=ActualNode, prevn=PrevNode->pred;

	      while ((priority=pri(actn->getOprtr(),prevn->getOprtr()))<=0
		     && prevn->pred)
		prevn=prevn->pred;

	      if (priority<=0){  /* hier muss prevn Top sein */

		actn->left=prevn;
		prevn->pred=actn;
		TopNode=actn;

	      }
	      else{               /* actn hat hoehere Prioritaet */

		actn->left=prevn->right;
		actn->pred=prevn;
		prevn->right=actn;

	      }
	    } else{

	      ActualNode->left=PrevNode;
	      PrevNode->pred=ActualNode;
	      TopNode=ActualNode;

	    }
	  } else{   /* Prev hat keinen rechten Unterbaum, also wird hier neuer
		       Ast angefuegt */
	    // z.B "3+sin2"
	    // 	    if (pri(ActualNode->oprtr,PrevNode->oprtr)){
	    PrevNode->right=ActualNode;
	    ActualNode->pred=PrevNode;
	    // 	    }
	  }
	} else{      /* Prev ist Zahl */

	  if (PrevNode->pred){

	    actn=ActualNode, prevn=PrevNode->pred;

	    while ((priority=pri(actn->getOprtr(),prevn->getOprtr()))<=0
		   && prevn->pred)
	      prevn=prevn->pred;

	    if (priority<=0){  /* hier muss prevn Top sein */

	      actn->left=prevn;
	      prevn->pred=actn;
	      TopNode=actn;

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

      if ((ActualNode->oprtr[0]=='-' || ActualNode->oprtr[0]=='+')
	  && !ActualNode->left){

	/* falls unaerer minus- oder plusoperator */
	/* wird eine Mult. mit - bzw. +1 in den Baum eingefuegt */
	if (ActualNode->oprtr[0]=='-')         
	  ActualNode->setValue(-1);
	else                                   
	  ActualNode->setValue(1);

	prevn=ActualNode;

	if (!(ActualNode = new MathExpression(varlist,functionlist))){

	  delete TopNode;
	  throw OutOfMemException();

	}

	ActualNode->setOprtr("*");
	ActualNode->left=prevn;
	prevn->pred=ActualNode;

      }

      PrevNode=ActualNode;
      TopNode=ActualNode;

    }
  }
  return(TopNode);
}

inline bool MathExpression::checkOprtr(char x){ 
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

bool MathExpression::isBuiltinFunc(const char *strname){

  char *flist[]={FLIST};

  for (unsigned int i=0;i<sizeof(flist)/sizeof(char *);i++)
    if (!strcmp(strname,flist[i]))
      return true;
  return false;
}

void MathExpression::setOprtr(const char *name){

  clearString(oprtr);
  strcpy(oprtr,name);
  value=0;
  clearString(variable);
  type=OP;
}

void MathExpression::setVariable(const char *name){

  clearString(variable);
  strcpy(variable,name);
  clearString(oprtr);
  value=0;
  type=VAR;
}

void MathExpression::setValue(double value){

  this->value=value;
  clearString(oprtr);
  clearString(variable);
  type=VAL;
}

void MathExpression::addVarsToList(VarList *varlist){

  if (isVariable())
    varlist->insert(getVariable(),0);

  if (right)
    right->addVarsToList(varlist);
  if (left)
    left->addVarsToList(varlist);
}
 
bool MathExpression::isBuiltinOp(char op){

  char oplist[]={OPLIST};

  for (unsigned int i=0;i<sizeof(oplist)/sizeof(char);i++)
    if (op==oplist[i])
      return true;
  return false;
}

int MathExpression::brackcpy(char *exprstring, const char *arg, char open,
			     char close){

  int br_cnt=0, indx=0, eindx=0;

  if (open && (arg[indx]!=open))
    return (-1);

  do{
    if (arg[indx]==open || (!open && !indx)){  // auch moeglich: alles bis ...
      if (br_cnt)
	exprstring[eindx]=arg[indx];
      else if (!open && !indx)
	exprstring[0]=arg[0];
      else
	eindx--;
      br_cnt++;
    } else if (arg[indx]==close){
      br_cnt--; 
      if (br_cnt)
	exprstring[eindx]=arg[indx];
    } else
      exprstring[eindx]=arg[indx];
    indx++;
    eindx++;
  } while (br_cnt&&arg[indx]);
  if (br_cnt)
    return (-1);
  return(indx);
}
  
int MathExpression::flcpy(char *exprstring, const char *arg){

  int indx=0;
  while (checkDigit(arg[indx])){ 
    exprstring[indx]=arg[indx];
    indx++;
  }
  return(indx);
}

int MathExpression::oprcpy(char *exprstring, const char *arg){

  int indx=0;

  switch (arg[indx]){
  case '(':
  case ')':
    break;
  default:
    if (checkOprtr(arg[indx])){
      exprstring[indx]=arg[indx];
      indx++;
    } else{
      while (arg[indx] && indx<OP_LEN){ 
	if (!(checkLetter(arg[indx])))
	  break;
	exprstring[indx]=arg[indx];
	indx++;
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

bool MathExpression::checkSyntaxAndOptimize(void) throw (SubException<SyntaxErr,MathExpression>){

  if (this->isOprtr()){
    if (checkOprtr(this->oprtr[0])){
      if (this->oprtr[0]=='='){
	if (this->left && this->right)
	  if (!this->left->empty() && !this->right->empty()){
	    if (this->left->isVariable()){
	      if (!this->left->left && !this->left->right)
		if (this->right->checkSyntaxAndOptimize())
		  return(true);
	    } else if (this->left->isOprtr()){
	      if (!this->left->left && this->left->right)
		if (this->right->checkSyntaxAndOptimize())
		  return true;
	    }
	    throw SubException<SyntaxErr,MathExpression>("invalid expression!");
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
		if ( abs(this->left->getValue()) == 1 && abs(this->right->getValue()) == 1 ){

		  this->setOprtr("");
		  this->setValue(sgn(this->left->getValue()) * this->right->getValue());

		  delete this->left;
		  delete this->right;
		  this->left = this->right = 0;

		}
	      
	    return(true);

	  }
      }

      throw SubException<SyntaxErr,MathExpression>("invalid expression!");

    } else{
      if (!strcmp(this->getOprtr(),"log")){
	if (this->left && this->right)
	  if (!this->left->empty() && !this->right->empty())
	    if (this->left->checkSyntaxAndOptimize() && this->right->checkSyntaxAndOptimize())
	      return(true);
	throw SubException<SyntaxErr,MathExpression>
	  ("invalid expression in function log!");
      } else if (!strcmp(this->getOprtr(),SUM)
		 || !strcmp(this->getOprtr(),PROD)){
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
		    } catch (SubException<SyntaxErr,MathExpression> mese){
		      throw SubException<SyntaxErr,MathExpression>
			("invalid expression in function Sum/Prod!");
		    }
		  }
	throw SubException<SyntaxErr,MathExpression>
	  ("invalid expression in function Sum/Prod!");
      } else if (isBuiltinFunc(this->getOprtr())){
	if (!this->left && this->right)
	  if (this->right->empty()==false)
	    if (this->right->checkSyntaxAndOptimize())
	      return(true);
	throw SubException<SyntaxErr,MathExpression>("invalid expression!");
      } else if (functionlist->isMember(this->getOprtr())){
	if (!this->left && this->right)
	  if (this->right->checkSyntaxAndOptimize())
	    if (this->right->countArgs() ==
		functionlist->get(this->getOprtr())->getParList()->countArgs())
	      return(true);
	throw SubException<SyntaxErr,MathExpression>("invalid expression!");
      } else
	return true;
    }
  }
  return(true);
}

void MathExpression::print() const{
  cout << toString();
}

string MathExpression::toString() const{

  ostringstream exp;

  exp << "(";
  if (!this->empty()){
    if (this->isOprtr()){
      if (this->oprtr[0]=='!'){
	if (this->right)
	  exp << this->right->toString();
	exp << this->getOprtr();
      } else if (!strcmp(this->getOprtr(),"log")
		 || !strcmp(this->getOprtr(),SUM)
		 || !strcmp(this->getOprtr(),PROD)){
	exp << this->getOprtr();
	exp << "[";
	exp << this->left->toString();
	exp << "]";
	exp << this->right->toString();
      } else{
	if (this->left)
	  exp << this->left->toString();
	exp << this->getOprtr();
	if (this->right)
	  exp << this->right->toString();
      }
    } else if (this->isVariable())
      exp << this->getVariable();
    else
      exp << this->getValue();
  }
  exp << ")";

  return exp.str();

}

double MathExpression::eval() throw (Exception_T){

  double result;
  if (isOprtr()){
    switch (oprtr[0]){
    case '+':
      return(left->eval()+right->eval());
      break;
    case '-':
      return(left->eval()-right->eval());
      break;
    case '*':
      return(left->eval()*right->eval());
      break;
    case '/':
      if ((result=right->eval()))
	return(left->eval()/result);
      else
	throw EvalException("division by zero!");
      break;
    case '\\':
      if ((result=right->eval()) != 0)
	return((double)((int)(left->eval()/result)));
      else
	throw EvalException("division by zero!");
      break;
    case '%':
      if ((result=right->eval()))
	return((int)left->eval()%(int)result);
      else
	throw EvalException("division by zero!");
      break;
    case '^':
      result=right->eval();/*
			     if (left->eval()<=0 && (result/(int)result)){
			     fprintf(stderr,"negative root!\n");
			     exit(1);
			     }
			     else*/
      return(pow(left->eval(),right->eval()));
      break;
    case '!':
      return faculty(right->eval());
      break;
    case '@':
      return (faculty(left->eval())/(faculty(right->eval())
				     *faculty(left->eval() - right->eval())));
      break;
    case '=':
      if (left->isVariable())
	return assignValue();
      else if (left->isOprtr()){
	defineFunction();
	return 0;
      }
      throw EvalException("invalid use of assignment!");
      break;
    default:
      if (!strcmp("sin",getOprtr()))
	return(sin(right->eval()));
      else if (!strcmp("cos",getOprtr()))
	return(cos(right->eval()));
      else if (!strcmp("tan",getOprtr()))
	return(tan(right->eval()));
      else if (!strcmp("asin",getOprtr()))
	return(asin(right->eval()));
      else if (!strcmp("acos",getOprtr()))
	return(acos(right->eval()));
      else if (!strcmp("atan",getOprtr()))
	return(atan(right->eval()));
      else if (!strcmp("sinh",getOprtr()))
	return(sinh(right->eval()));
      else if (!strcmp("cosh",getOprtr()))
	return(cosh(right->eval()));
      else if (!strcmp("tanh",getOprtr()))
	return(tanh(right->eval()));
      else if (!strcmp("asinh",getOprtr()))
	return(asinh(right->eval()));
      else if (!strcmp("acosh",getOprtr()))
	return(acosh(right->eval()));
      else if (!strcmp("atanh",getOprtr()))
	return(atanh(right->eval()));
      else if (!strcmp("ln",getOprtr())){
	if ((result=right->eval())<=0)
	  throw EvalException("argument of ln negative!");
	else
	  return(log(right->eval()));
      } else if (!strcmp("ld",getOprtr())){
	if ((result=right->eval())<=0)
	  throw EvalException("argument of ld negative!");
	else
	  return(log(right->eval())/log(2));
      } else if (!strcmp("log",getOprtr())){
	if ((result=right->eval())<=0)
	  throw EvalException("argument of log negative!");
	else
	  return(log(right->eval())/log(left->eval()));
      } else if (!strcmp("exp",getOprtr()))
	return(exp(right->eval()));
      else if (!strcmp(SUM,getOprtr()) || !strcmp(PROD,getOprtr())){

	return sumProd();

      } else if (!strcmp("sgn",getOprtr())){

	return sgn(right->eval());

      } else{  // user defined function

	if (functionlist){
	  if (functionlist->isMember(getOprtr()))
	    return evalFunction();
	}

	throw EvalException
	  ("unknown operator/function!",getOprtr());

      }
      break;
    }
  } else if (isVariable()){
    if (varlist)
      return (varlist->getValue(getVariable()));
    else
      throw EvalException("unauthorized use of variables!");
  }

  return(value);

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

double MathExpression::faculty(double fac) throw (Exception_T){
  
  if (fac!=(double)((int)(fac)) || fac<0)
    throw EvalException
      ("Argument of Faculty not a natural number or negative!");
  if (fac==0)
    return(1);
  for (int i=(int)fac-1;i>0;i--)
    fac*=i;
  return(fac);
}

double MathExpression::sumProd(void)
  throw (Exception_T){

  VarList vl = *this->varlist;  // lokaler Scope: Kopie der globalen Varlist
  MathExpression me(this,&vl,this->functionlist);
  VarElement *currve;
  double from, to, res=0;
  char p=0;
  
  if (!strcmp(PROD,this->getOprtr())){
    res=1;
    p=1;
  }
  
  // Indexvariable zum lokalen Scope hinzufuegen
  vl.insert(this->left->left->left->getVariable(),
	    this->left->left->right->eval());

  //Zaehlbereich
  from=vl.getValue(me.left->left->left->getVariable());
  to=me.left->right->eval();

  if (from!=(int)from || from<0 || to!=(int)to || to<0)
    throw EvalException
      ("indices in Sum/Prod not natural or negative!");

  for (int i=(int)from;i<=(int)to;){
    if (!p)
      res+=me.right->eval();  // Summe
    else
      res*=me.right->eval();  // Produkt
    i++;
    // neuen Wert der Zaehlvariablen eintragen
    vl.insert(this->left->left->left->getVariable(),(double)i);
  }

  // neu definierte Variablen in globale Varlist eintragen
  currve=vl.first;
  while (currve){
    if (strcmp(currve->getName(),this->left->left->left->getVariable())
	&& !currve->protect)
      this->varlist->insert(currve->getName(),currve->getValue());
    currve=currve->next;
  }

  return res;
}

double MathExpression::assignValue(void) throw (Exception_T){

  double result;

  result=right->eval();

  if (functionlist)
    if (functionlist->isMember(left->getOprtr()))
      throw EvalException ("variable already defined as function");

  if (varlist){

    try{

      varlist->insert(left->getVariable(),result);

    }catch (OutOfMemException &oome){

      delete varlist;
      throw OutOfMemException();

    }

    varlist->setModified(true);
    return result;

  }

  throw EvalException("invalid use of assignment!");

}

double MathExpression::evalFunction(void) throw (Exception_T){

  VarList vl = *this->varlist; // local scope
  MathExpression *args =  0,*params = 0;
  FunctionElement *f_template = functionlist->get(this->getOprtr());

  vl.unprotect();  // remove protection-status for all variables

  args = this->right;
  params = f_template->getParList();

  // eval args and insert them in varlist
  if (!args->empty()){

    while (args->oprtr[0] == ','){

      vl.insert(params->right->getVariable(),args->right->eval());
      args = args->left;
      params = params->left;

    }

    vl.insert(params->getVariable(),args->eval());

  }

  MathExpression function(f_template->getBody(),&vl,functionlist);

  return function.eval();

}

void MathExpression::defineFunction(void) throw (SubException<MathExpression::DefinitionError,MathExpression>){

  const char *functionname;
  MathExpression *paramlist=0, *body=0;
  FunctionElement *fe=0;
  VarList locals;

  functionname = this->left->getOprtr();
  
  // already declared as variable? (impossible to happen)
  if (varlist->isMember(functionname))
    throw SubException<MathExpression::DefinitionError,MathExpression>(string(functionname)
									  + " already declared as variable, definition "
									  + "not possible!");
  
  // built-in function?
  if (isBuiltinFunc(functionname))
    throw SubException<MathExpression::DefinitionError,MathExpression>(string(functionname)
								       + " is built-in function and can't be replaced!");
  
  // already declared as function?
  if (functionlist->isMember(functionname))
    throw SubException<MathExpression::DefinitionError,MathExpression>(string(functionname)
									   + " already declared as function, to redefine it"
									   + " first undefine it!");

  // build parameterlist
  paramlist = new MathExpression(this->left->right,varlist,functionlist);
  
  // must be a variabletree
  if (paramlist->checkForVartree()==false){

    delete paramlist;
    throw SubException<MathExpression::DefinitionError,MathExpression>(string("only letters and non-built-in functionnames allowed,")
								       + " variables must be seperated by commas!");

  }
  
  // build functionbody
  body = new MathExpression(this->right,varlist,functionlist);
  
  // body must only contain defined variables/functions/operators
  if (!checkBody(body,paramlist,&locals)){

    delete body;
    delete paramlist;
    throw SubException<MathExpression::DefinitionError,MathExpression>(string("undefined variables/functions/operators used, ")
								       + "no definition possible!");

  }
  
  // build functionelement
  fe = new FunctionElement(functionname,paramlist,body);
  
  this->functionlist->insert(fe); // insert in functionlist;

  functionlist->setModified(true);
  
}

bool MathExpression::checkBody(MathExpression *body, MathExpression *pl,
			       VarList *lvl){

  // pl: parameterTree
  // lvl: local VarList

  bool inparam=false;
  
  if (!body)
    return true;

  if (*(body->getOprtr())){
    if (!(isBuiltinOp(*(body->getOprtr()))))
      if (!(isBuiltinFunc(body->getOprtr())))
	if (!(functionlist->isMember(body->getOprtr())))
	  return false;
  } else if (*(body->getVariable())){
    if (pl)
      if (pl->varInTree(body->getVariable()))
	inparam=true;
    if (!inparam){
      if (!varlist->isMember(body->getVariable())){
	if (!lvl->isMember(body->getVariable())){
	  if (!body->getPred())
	    return false;
	  else{
	    if ((body->getPred()->getOprtr())[0]!='=')
	      return false;
	    else
	      lvl->insert(body->getVariable(),0);
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

bool MathExpression::checkForVartree(){

  if (this->left){
    if (this->left->checkForVartree()==false){
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
    if (this->right->checkForVartree()==false){
      return false;
    }
  }
  return true;
}

bool MathExpression::varInTree(const char *name){

  if (!this)
    return false;

  if (strcmp(name,this->getVariable())){
    if (this->left->varInTree(name))
      return true;
    else if (this->right->varInTree(name))
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
