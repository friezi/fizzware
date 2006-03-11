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
   @file mathexpression.hpp
   @author Friedemann Zintel
*/

#ifndef MATHEXPRESSION_HPP
#define MATHEXPRESSION_HPP

#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string>
#include <exception.hpp>

/**
   @brief for evaluating easy-to-write mathematical expressions
*/
namespace mexp{
  
  class MathExpression;
  class EvalException;
  class VarList;
  class VarElement;
  class FunctionList;
  class FunctionElement;
  
  /**
     @brief for evaluating easy-to-write mathematical expressions
  */
  class MathExpression {
    
  public:
    
    // constants:
    static const int OP_LEN=24;
    static const int VARLEN=24;
    static const int BR_LEN=256;
    static const int KOMMA_PRI=0;
    static const int EQUAL_PRI=5;
    static const int ADDSUB_PRI=10;
    static const int MULTDIV_PRI=15;
    static const int SIN_PRI=20;
    static const int POT_PRI=30;
    static const int BINOM_PRI=40;
    static const int FAC_PRI=50;
    static const char EMPTY=0;
    static const char OP=1;
    static const char VAR=2;
    static const char VAL=3;

    // Exception-Classes
    /** @internal */
    class SyntaxErr{};
    
  private:
    
    VarList *varlist;
    FunctionList *functionlist;
    MathExpression *left;
    MathExpression *right;
    MathExpression *pred;
    char oprtr[OP_LEN];
    char variable[VARLEN];
    double value;
    char type;
    
    MathExpression *parse(char *expr, VarList& locals)
    throw (SubException<SyntaxErr,MathExpression>,Exception_T);
    
    // pri checks difference in priorities
    // return: <0 if c0<c1, =0 if c0=c1, >0 if c0>c1
    static int pri(char *c0, char *c1);
    bool checkSyntax(void) throw (SubException<SyntaxErr,MathExpression>);
    double faculty(double fac) throw (Exception_T);
    double sumProd(void) throw (Exception_T);
    double assignValue(void) throw (Exception_T);
    double evalFunction(void) throw (Exception_T);
    void defineFunction(void);
    bool checkBody(MathExpression *body, MathExpression *pl, VarList *lvl);
    bool empty(void) { return (getType()==EMPTY); }
    void setOprtr(char *name);
    void setVariable(char *name);
    void setValue(double value);
    void setType(char type) { this->type=type; }
    bool isVariable(void) const { return (getType()==VAR); }
    bool isOprtr(void) const { return (getType()==OP); }
    char *getOprtr(void) { return oprtr; }
    char *getVariable(void) { return variable; }
    double getValue(void) { return value; }
    char getType() const { return type; }
    MathExpression *getLeft(void) { return left; }
    MathExpression *getRight(void) { return right; }
    MathExpression *getPred(void) { return pred; }
    
    // addVarsToList adds all occuring variables in tree to varlist
    void addVarsToList(VarList *varlist);
    
    // checkForVars checks, if leaf-elements of expression are only variables
    // and operators are only ","'s. No variable must have childtrees.
    // return: true, false
    bool checkForVartree();
    
    // varInTree checks, if a given variable-name is defined as variable in tree
    bool varInTree(char *name);
    
    // countArgs functions only with a correct (syntax!) tree!
    unsigned int countArgs(void);
    
    // private constructor:
    MathExpression(VarList *vl = 0, FunctionList *fl = 0);
    // private copyconstructor: not for use
    MathExpression(const MathExpression&){}
    
  public:
    
    //  constructors:
    /**
       @param expression pointer to an expression which should be evaluated
       @param vl pointer to a list of variables
       @param fl pointer to a list of functions
       @exception SubException<SyntaxErr,MathExpression>
       @exception OutOfMemException
    */
    MathExpression(char *expression, VarList *vl = 0, FunctionList *fl = 0)
      throw (SubException<SyntaxErr,MathExpression>,Exception_T);
 
  protected:

    MathExpression(MathExpression *me, VarList *vl, FunctionList *fl)
      throw (SubException<SyntaxErr,MathExpression>,Exception_T);

  public:
    
    // destructor:
    ~MathExpression();
    
    // main functions:
    
    /// print prints the mathematic expression totally bracketed to stdout
    void print();
    
    // eval evaluates the expression
    // return: the result
    /**
       @brief evaluates the expression
       @return the result
       @exception EvalException
       @exception OutOfMemException
    */
    double eval() throw (Exception_T);
    
    // more usefull functions:
    
    /**
       brackcpy copies all from "arg" into "exprstring" between "open" and 
       "close"; is "open"=0 all to "close" will be copied
       @brief for copying arguments
       @param exprstring pointer to the memory-area for the copy-operation
       @param arg pointer to the data to be copied
       @param open the sign for the open-bracket
       @param close the sign for the close-bracket
       @return number of copied signs
    */
    static int brackcpy(char *exprstring, char *arg, char open, char close);
    
    /**
       flcpy copies the numeric part from "arg" into "exprstring"
       @brief for copying numeric contents
       @param exprstring pointer to the memory-area for the copy-operation
       @param arg pointer to the data to be copied
       @return number of copied signs
    */
    static int flcpy(char *exprstring, char *arg);
    
    /**
       oprcpy copies the operatorpart (also functionnames) from "arg" into "exprstring"
       @brief for copying operator contents
       @param exprstring pointer to the memory-area for the copy-operation
       @param arg pointer to the data to be copied
       @return number of copied signs
    */
    static int oprcpy(char *exprstring, char *arg);
    
    /**
       @brief clears a Null-terminated char-Array
       @param exprstring pointer to the memory-area
    */
    static void clearString(char *exprstring);

    /**
       @brief checks, whether "x" is operator
       @param x value to be checked
       @return true, if x is an operator, else false
    */
    static inline bool checkOprtr(char x);
    
    /**
       @brief checks, whether "x" is digit
       @param x value to be checked
       @return true, if x is an digit, else false
    */
    static inline bool checkDigit(char x);
    
    /**
       @brief checks, whether "x" is letter
       @param x value to be checked
       @return true, if x is an letter, else false
    */
    static inline bool checkLetter(char x);
    
    /**
       @brief checks, whether "strname" is member of built-in functions
       @param strname pointer to chararray to be checked
       @return true, if strname is a built-in-function, else false
    */
    static bool isBuiltinFunc(char *strname);
    
    /**
       @brief checks, whether "op" is member of built-in operators
       @param op value to be checked
       @return true, if op is a built-in-operator, else false
    */
    static bool isBuiltinOp(char op);
    
    // constants: see top of class
    
  };
  
  class EvalException : public Exception<MathExpression> {
    
  private:
    static const std::string id;
    std::string objname;
    
  public:
    EvalException() : Exception<MathExpression>(""){

      this->objname = "";
    }
    
    EvalException(char *errormsg, char *objname=0)
      : Exception<MathExpression>(std::string(errormsg)){

      if (objname)
	this->objname = objname;
      else
	this->objname = "";
    }
    
    EvalException(const std::string &id, const std::string &errormsg)
      : Exception<MathExpression>(id,errormsg){

      this->objname = "";
    }
    
    std::string getObjName() const{
      return this->objname;
    }
  };
  
  class VarElement {
    
    friend class VarList;
    friend class MathExpression;
    
  private:
    char *name;
    double value;
    char protect;
    VarElement *next;
    
    // copyconstructor: not for use
    VarElement(const VarElement&){}
    
  public:
    // constructor:
    /**
       @exception OutOfMemException
    */
    VarElement(const char *name, double value, char protect)
      throw (Exception_T);
    // destructor:
    ~VarElement();
    char *getName() const{ return name; }
    double getValue() const{ return this->value; }
    
  };
  
  class VarList {
    
    friend class MathExpression;
    
  private:
    VarElement *first;
    VarElement *last;
    
    
  public:
    
    // constructor:
    VarList() : first(0), last(0){}
    // copyconstructor:
    /**
       @exception EvalException
       @exception OutOfMemException
    */
    VarList(const VarList& vl)
      throw (Exception_T);
    // destructor:
    ~VarList();
 
    /**
       @exception EvalException
       @exception OutOfMemException
    */
    void insert(char *name, double value, char protect=0)
      throw (Exception_T);
    void remove(char *name) throw (Exception<VarList>);
    /**
       @exception EvalException
    */
    double getValue(char *name) const throw (Exception_T);
    VarElement *isMember(char *name) const;
    void unprotect(char *name=0);
    void print() const;
    
  };
  
  class FunctionElement{
    
    friend class FunctionList;
    
  private:
    char *name;
    MathExpression *paramlist;
    MathExpression *body;
    FunctionElement *next;
    
    // copyconstructor: not for use
    FunctionElement(const FunctionElement& fe){}
    
  public:
    // constructor:
    FunctionElement(const char *name, MathExpression *paramlist,
		    MathExpression *body);
    // destructor:
    ~FunctionElement();
    char *getName(void) const{ return name; }
    MathExpression *getParList(void){ return paramlist; }
    MathExpression *getBody(void){ return body; }
  };
  
  class FunctionList{
    
  private:
    FunctionElement *first;
    FunctionElement *last;
    
    // copyconstructor: not for use
    FunctionList(const FunctionList& fl){}
    
  public:
    
    // constructor:
    FunctionList(): first(0), last(0){}
    
    // destructor:
    ~FunctionList();
    
    void insert(FunctionElement *fe) throw(Exception<FunctionList>);
    void remove(char *name) throw(Exception<FunctionList>);
    void print(char *name=0);
    bool isMember(char *name) const;
    FunctionElement *get(char *name) const;
    
  };
  
}


#endif
