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
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string>
#include <list>
#include <set>
#include <functional>
#include <exception.hpp>
#include <datastructures.hpp>

/**
   @brief for evaluating easy-to-write mathematical expressions
*/
namespace mexp{
  
  class MathExpression;
  class VariableList;
  class Variable;
  class FunctionList;
  class Function;
  class Value;
  class Tuple;
  class Complex;

  class FunctionDefinition {

  private:

    std::string name;

  public:

    FunctionDefinition(std::string name) : name(name){}

    std::string getName(){ return name; }

  };
 
  class ParseException : public Exception<MathExpression> {
    
  private:
    static const std::string id;
    int pos;
    
  public:
    ParseException(int pos) : Exception<MathExpression>(""){
      this->pos = pos;
    }
    
    ParseException(int pos, char *errormsg) : Exception<MathExpression>(std::string(errormsg)){
      this->pos = pos;
    }
    
    ParseException(int pos, std::string errormsg) : Exception<MathExpression>(errormsg){
      this->pos = pos;
    }
    
    ParseException(int pos, const std::string &id, const std::string &errormsg)
      : Exception<MathExpression>(id,errormsg){
      this->pos = pos;
    }

    int getPos(){ return pos; }

  };
  
  class EvalException : public Exception<MathExpression> {
    
  private:
    static const std::string id;
    std::string objname;
    
  public:
    EvalException() : Exception<MathExpression>(""){
      this->objname = "";
    }
    
    EvalException(char *errormsg, char *objname=0) : Exception<MathExpression>(std::string(errormsg)){

      if (objname)
	this->objname = objname;
      else
	this->objname = "";

    }
    
    EvalException(std::string errormsg) : Exception<MathExpression>(errormsg){
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

  class Value{

  public:

    static const int TUPLE = 1;
    static const int COMPLEX = 2;

    static const std::streamsize DFLT_PRECISION = 6;

  protected:

    int type;

    Value(int type) : type(type){}
    
    Value *notSupported() const throw (ExceptionBase) { throw EvalException("not supported!"); }

  public:

    virtual ~Value(){}

    int getType() const { return type; }

    virtual std::string toString(std::streamsize precision) const = 0;
    std::string toString() const { return toString(DFLT_PRECISION); }

    virtual Value *clone() const { return notSupported(); }

    virtual Value *operator+(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *operator-(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *operator*(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *operator/(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *integerDivision(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *operator%(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *pow(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *faculty() throw (ExceptionBase){ return notSupported(); }
    virtual Value *choose(Value *right) throw (ExceptionBase){ return notSupported(); }
    virtual Value *sin() throw (ExceptionBase){ return notSupported(); }
    virtual Value *cos() throw (ExceptionBase){ return notSupported(); }
    virtual Value *tan() throw (ExceptionBase){ return notSupported(); }
    virtual Value *asin() throw (ExceptionBase){ return notSupported(); }
    virtual Value *acos() throw (ExceptionBase){ return notSupported(); }
    virtual Value *atan() throw (ExceptionBase){ return notSupported(); }
    virtual Value *sinh() throw (ExceptionBase){ return notSupported(); }
    virtual Value *cosh() throw (ExceptionBase){ return notSupported(); }
    virtual Value *tanh() throw (ExceptionBase){ return notSupported(); }
    virtual Value *asinh() throw (ExceptionBase){ return notSupported(); }
    virtual Value *acosh() throw (ExceptionBase){ return notSupported(); }
    virtual Value *atanh() throw (ExceptionBase){ return notSupported(); }
    virtual Value *ln() throw (ExceptionBase){ return notSupported(); }
    virtual Value *ld() throw (ExceptionBase){ return notSupported(); }
    virtual Value *log(Value *base) throw (ExceptionBase){ return notSupported(); }
    virtual Value *exp() throw (ExceptionBase){ return notSupported(); }
    virtual Value *sumProd() throw (ExceptionBase){ return notSupported(); }
    virtual Value *sgn() throw (ExceptionBase){ return notSupported(); }
    virtual Value *tst() throw (ExceptionBase){ return notSupported(); }

  };

  class Tuple : public Value{

  public:

    std::list<Value *> elements;

    Tuple() : Value(Value::TUPLE){}

    ~Tuple();

    static Tuple *assertTuple(const Value *value) throw(EvalException);
    static void assertKind(const Tuple *t1, const Tuple *t2) throw(EvalException);

    std::string toString(std::streamsize precision) const;

    void addElement(Value *value){ elements.push_back(value); }

    Value *clone() const;

    Value *operator+(Value *right) throw (ExceptionBase);

  };

  class Complex : public Value{

  private:

    double re;
    double im;

    static Complex *assertComplex(Value *value) throw(EvalException);
    static Complex *assertReal(Value *value) throw(EvalException);
    
  public:

    
    Complex(): Value(Value::COMPLEX), re(0), im(0){}
    Complex(double re) : Value(Value::COMPLEX), re(re), im(0){}
    Complex(double re, double im) : Value(Value::COMPLEX), re(re), im(im){}

    ~Complex(){}

    std::string toString(std::streamsize precision) const;

    Value *clone() const { return new Complex(getRe(), getIm()); }

    double getRe() const { return re; }
    double getIm() const { return im; }
    bool isReal() const { return ( getIm() == 0 ); }

    Value *operator+(Value *right) throw (ExceptionBase);
    Value *operator-(Value *right) throw (ExceptionBase);
    Value *operator*(Value *right) throw (ExceptionBase); 
    Value *operator/(Value *right) throw (ExceptionBase);
    Value *integerDivision(Value *right) throw (ExceptionBase);
    Value *operator%(Value *right) throw (ExceptionBase);
    Value *pow(Value *right) throw (ExceptionBase){ return notSupported(); }
    Value *faculty() throw (ExceptionBase);
    Value *choose(Value *right) throw (ExceptionBase){ return notSupported(); }
    Value *sin() throw (ExceptionBase){ return notSupported(); }
    Value *cos() throw (ExceptionBase){ return notSupported(); }
    Value *tan() throw (ExceptionBase){ return notSupported(); }
    Value *asin() throw (ExceptionBase){ return notSupported(); }
    Value *acos() throw (ExceptionBase){ return notSupported(); }
    Value *atan() throw (ExceptionBase){ return notSupported(); }
    Value *sinh() throw (ExceptionBase){ return notSupported(); }
    Value *cosh() throw (ExceptionBase){ return notSupported(); }
    Value *tanh() throw (ExceptionBase){ return notSupported(); }
    Value *asinh() throw (ExceptionBase){ return notSupported(); }
    Value *acosh() throw (ExceptionBase){ return notSupported(); }
    Value *atanh() throw (ExceptionBase){ return notSupported(); }
    Value *ln() throw (ExceptionBase){ return notSupported(); }
    Value *ld() throw (ExceptionBase){ return notSupported(); }
    Value *log(Value *base) throw (ExceptionBase){ return notSupported(); }
    Value *exp() throw (ExceptionBase){ return notSupported(); }
    Value *sumProd() throw (ExceptionBase){ return notSupported(); }
    Value *sgn() throw (ExceptionBase){ return notSupported(); }
    Value *tst() throw (ExceptionBase){ return notSupported(); }

  };

  /**
     @brief For defining the context a MathExpression occurs in
  */
  class Context {

  private:

    bool withinBrackets;

  public:

    Context() : withinBrackets(false){}

    bool getWithinBrackets() const { return withinBrackets; }
    void setWithinBrackets(bool value){ this->withinBrackets = value; }
    bool isWithinBrackets() const { return ( getWithinBrackets() == true ); }

  };
 
  /**
     @brief for evaluating easy-to-write mathematical expressions
  */
  class MathExpression {
    
  public:
    
    // constants:

    // minimal size of buffer for dynamical memory of unknown size
    static const int BUFSIZE = 256;

    // operator-priorities
    static const int EQUAL_PRI = 2;
    static const int KOMMA_PRI = 5;
    static const int ADDSUB_PRI = 10;
    static const int MULTDIV_PRI = 15;
    static const int SIN_PRI = 20;
    static const int POT_PRI = 30;
    static const int BINOM_PRI = 40;
    static const int FAC_PRI = 50;
 
    // expression-types
    static const char ET_EMPTY = 0;
    static const char ET_OP = 1;
    static const char ET_VAR = 2;
    static const char ET_VAL = 3;

    // operator-types
    static const unsigned char OT_EMPTY = 0;
    static const unsigned char OT_FUNCTION = 1;
    static const unsigned char OT_OPERATION = 2;
    static const unsigned char OT_PARAMETER = 3;
    static const unsigned char OT_TUPLE = 4;
    
  private:
    
    // ###################################################
    // # instantiated with defaults in constructor-calls #
    // ###################################################
    //                                                   #
    //                                                   #
    
    VariableList *varlist;
    FunctionList *functionlist;
    
    // for unary and binary operators
    // unary operators will store the argument in the 'right'-pointer
    MathExpression *left;
    MathExpression *right;
    MathExpression *pred;
    Value *value;
    
    /**
       type can be: operator, variable or value
       @brief the type of expression
    */
    char type;

    /**
       @brief type of operator (if expression is an operator)
    */
    unsigned char operator_type;

    // absolute position in expression-string
    int abs_pos;

    char imaginary_unit;

    // if set no pointer will be destroyed on destructor-call
    bool delete_flat;

    //                                                    #
    //                                                    #
    // ####################################################

    std::string oprtr;
    std::string variable;

    // for n-ary operators (like ',')
    std::list<MathExpression *> elements;
    
    MathExpression *parse(const char *expr, const Context & context, VariableList& locals) throw (ParseException,ExceptionBase);

    int parseCommaOperator(const char *expr, int e_indx, MathExpression * & ActualNode, MathExpression * & PrevNode, MathExpression * & TopNode,
			   const Context & context, const Context & new_context, VariableList & locals) throw (ParseException, ExceptionBase);
    void searchAndSetLowerPriNode(MathExpression * & ActualNode, MathExpression * & PrevNode, MathExpression * & TopNode);
    int determineAndSetOperatorOrVariable(const char *expr, int e_indx, MathExpression * & ActualNode, const FunctionList *functionlist);
    
    // erases the elements of a n-ary operator
    void eraseElements();

    // pri checks difference in priorities
    // return: <0 if c0<c1, =0 if c0=c1, >0 if c0>c1
    static int pri(const char *c0, const char *c1);
    bool checkSyntaxAndOptimize(void) throw (ParseException);
    double sumProd(void) throw (ExceptionBase);
    Value *assignValue(void) throw (ExceptionBase);
    Value *evalFunction(void) throw (ExceptionBase);
    void defineFunction(void) throw (EvalException,ParseException);
    void checkBody(MathExpression *body, MathExpression *pl, VariableList *lvl) const throw(EvalException);
    bool isEmpty(void) const { return ( getEType() == ET_EMPTY ); }
    void setETOperator(const char *name);
    void setETVariable(const char *name);
    void setETValue(Value *value);
    
    /**
       @brief sets the operator-type
       @param type the type to be set
    */
    void setOType(unsigned char type){ operator_type = type; }
    void setOTFunction(){ setOType(OT_FUNCTION); }
    void setOTOperation(){ setOType(OT_OPERATION); }
    void setOTParameter(){ setOType(OT_PARAMETER); }
    void setOTTuple(){ setOType(OT_TUPLE); }
    /**
       @brief returns the operator-type
       @return the operator-type
    */
    unsigned char getOType() const { return operator_type; }

    /**
       @brief sets the expression-type
    */
    void setEType(char type) { this->type=type; }

    // checks the expression-type
    bool isVariable(void) const { return ( getEType() == ET_VAR ); }
    bool isOperator(void) const { return ( getEType() == ET_OP ); }
    bool isValue(void) const { return ( getEType() == ET_VAL ); }

    // checks the operator-type
    bool isOTFunction(){ return ( getOType() == OT_FUNCTION ); }
    bool isOTOperation(){ return ( getOType() == OT_OPERATION ); }
    bool isOTParameter(){ return ( getOType() == OT_PARAMETER ); }
    bool isOTTuple(){ return ( getOType() == OT_TUPLE ); }

    void setValue(Value *value);
    const char *getOperator(void) const { return oprtr.c_str(); }
    const char *getVariable(void) const { return variable.c_str(); }

    /**
       @brief returns a pointer to the value
       @return pointer to value
    */
    Value *getValue(void) const { return value; }
    char getEType() const { return type; }

    //TODO
    MathExpression *getLeft(void) const { return left; }
    MathExpression *getRight(void) const { return right; }
    MathExpression *getPred(void) const { return pred; }

//     void initLeftRight();

    void setRight(MathExpression *right);
    void setLeft(MathExpression *left);
    void addElement(MathExpression *element);
    
    // addVariablesToList adds all occuring variables in tree to varlist
    void addVariablesToList(VariableList *varlist);
    
    // checkForVars checks, if leaf-elements of expression are only variables
    // and operators are only ","'s. No variable must have childtrees.
    // return: true, false
    bool checkForVariableTree() const;

    bool checkForVariableTree(std::set< std::string,std::less<std::string> > & variableset) const;
    
    // isVariableInTree checks, if a given variable-name is defined as variable in tree
    bool isVariableInTree(const char *name) const;
    
    // countArgs functions only with a correct (syntax!) tree!
    unsigned int countArgs(void);
    
    // private constructor:
    MathExpression(int abs_pos, VariableList *vl = 0, FunctionList *fl = 0);
    // private copyconstructor: not for use
    MathExpression(const MathExpression&){}
    
  public:
    
    //  constructors:
    /**
       @param expression pointer to an expression which should be evaluated
       @param vl pointer to a list of variables
       @param fl pointer to a list of functions
       @exception ParseException
       @exception OutOfMemException
    */
    MathExpression(const char *expression, VariableList *vl = 0, FunctionList *fl = 0)
      throw (ParseException,ExceptionBase);
 
  protected:

    MathExpression(MathExpression *me, VariableList *vl, FunctionList *fl, int abs_pos)
      throw (ParseException,ExceptionBase);

    Value * assignTupleExpression();

  public:
    
    // destructor:
    ~MathExpression();
    
    // main functions:

    /**
       @brief calculates the faculty
       @return the faculty
    */
    static double faculty(double fac) throw (ExceptionBase);
    
    /**
       @brief print prints the mathematic expression totally bracketed to stdout
    */
    void print(std::streamsize precision) const;

    /**
       @brief returns a string-representation of the expression
       @return the string-representation
    */
    std::string toString(std::streamsize precision) const throw (Exception<MathExpression>,ExceptionBase);

    /**
       @brief returns a string-representation of the builtin functions
       @return the string-repreentation
    */
    static std::string builtinsToString();
    
    // eval evaluates the expression
    // return: the result
    /**
       @brief evaluates the expression
       @return the result
       @exception EvalException
       @exception OutOfMemException
    */
    Value *eval() throw (ExceptionBase,FunctionDefinition);

    /**
       @brief returns the signum of the value
       @param value the value
       @return the signum
    */
    static double sgn(double value);

    /**
       @brief returns the absolute value
       @param value the value
       @return the absolute value
    */
    static double abs(double value);
    
    // more usefull functions:

    static bool isABlank(char c);

    static std::string skipTrailingZeros(std::string value);

    static int skipBlanks(const char *expr, int indx);

    static bool isOpenBrace(char c);
    static bool isCloseBrace(char c);
   
    /**
       copyBracketContent copies all from "arg" into "exprstring" between "open" and 
       "close"; is "open"=0 all upto "close" will be copied
       @brief for copying arguments
       @param exprstring pointer to the memory-area for the copy-operation
       @param arg pointer to the data to be copied
       @param open the sign for the open-bracket
       @param close the sign for the close-bracket
       @return number of copied signs
    */
    int copyBracketContent(char * & exprstring, const char *arg, char open, char close);
    
    /**
       copyFloatContent copies the numeric part from "arg" into "exprstring"
       @brief for copying numeric contents
       @param exprstring pointer to the memory-area for the copy-operation
       @param arg pointer to the data to be copied
       @return number of copied signs
    */
    int copyFloatContent(char * & exprstring, const char *arg);
    
    /**
       copyOperatorContent copies the operatorpart (also functionnames) from "arg" into "exprstring"
       @brief for copying operator contents
       @param exprstring pointer to the memory-area for the copy-operation
       @param arg pointer to the data to be copied
       @return number of copied signs
    */
    int copyOperatorContent(char * & exprstring, const char *arg);

    int copyCommaContent(char * & exprstring, const char *arg);
    
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
    static inline bool checkOperator(char x);
    
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
    static bool isBuiltinFunction(const char *strname);
    
    /**
       @brief checks, whether "op" is member of built-in operators
       @param op value to be checked
       @return true, if op is a built-in-operator, else false
    */
    static bool isBuiltinOperator(char op);
    
    // constants: see top of class

    /**
       @brief sets the character for the imaginary unit
       @param imaginary_unit charakter to set
    */
    void setImaginaryUnit(char c){ imaginary_unit = c; }

    /**
       @brief returns the charakter for imaginary unit
       @return imaginary unit
    */
    char getImaginaryUnit() const { return imaginary_unit; }
    
  };
  
  /**
     @brief A variable-element
     @see VariableList
     @internal
  */
  class Variable {

  protected:

    Value *value;
    char *name;
    char protect;
    Variable *next;
    
    // copyconstructor: not for use
    Variable(const Variable&){}
    
  public:
    // constructor:
    /**
       @exception OutOfMemException
    */
    Variable(const char *name, Value *value, char protect)
      throw (ExceptionBase);
    // destructor:
    ~Variable();

    void setValue(Value *value);
    void setNext(Variable *next){ this->next = next; }
    void setProtect(char protect){ this->protect = protect; }

    char *getName() const { return name; }
    
    /**
       @brief returns a pointer to the value
       @return pointer to value
    */
    Value *getValue() const { return this->value; }
    Variable *getNext() const { return next; }
    char getProtect() const { return protect; }
    
  };
  
  /**
     @brief A list of variables
     @see MathExpression
     @internal
  */
  class VariableList {
    
    friend class MathExpression;
    
  protected:
    Variable *first;
    Variable *last;

    bool modified;
    
    
  public:

    class iterator{

      friend class VariableList;

    private:

      Variable *current;

    public:

      iterator(Variable *velem) : current(velem){};

      iterator(const iterator & it) { this->current = it.current; }

      void operator++(){ 
	if ( current )
	  current = current->getNext();
      }
      void operator++(int){ this->operator++(); }
      Variable & operator*(){ return *current; }
      bool operator==(const iterator & it_rval) { return it_rval.current == this->current; }
      bool operator!=(const iterator & it_rval) { return it_rval.current != this->current; }
      
    };
    
    // constructor:
    VariableList() : first(0), last(0), modified(false){}

    // copyconstructor:
    /**
       @exception EvalException
       @exception OutOfMemException
    */
    VariableList(const VariableList& vl) throw (ExceptionBase);

    // destructor:
    ~VariableList();
 
    /**
       @exception EvalException
       @exception OutOfMemException
    */
    void insert(const char *name, Value *value, char protect=0)
      throw (ExceptionBase);
    void remove(const char *name) throw (Exception<VariableList>);

    /**
       @brief returns a new copy of the requested value
       @return copy of value
       @exception EvalException
    */
    Value *getValue(const char *name) const throw (ExceptionBase);
    Variable *isMember(const char *name) const;
    void unprotect(const char *name=0);
    void print(std::streamsize precision) const;    
    std::string toString(const bool include_protected, std::streamsize precision) const;
    VariableList::iterator begin(){ return iterator(first); }
    VariableList::iterator end(){ return iterator(0); }

    /**
       Automatically called by a MathExpression-object with value=true if a variable-definition occurs.
       @brief Sets the MathExpression-object to be modified.
    */
    void setModified(bool value){ modified = value; }

    /**
       @brief returns the modified-status of the object.
       @return modified-status
    */
    bool isModified(){ return modified; }
    
  };
  
  /**
     @brief A function-element
     @see FunctionList
     @internal
  */
  class Function{
    
    friend class FunctionList;
    
  private:
    char *name;
    MathExpression *paramlist;
    MathExpression *body;
    Function *next;
    
    // copyconstructor: not for use
    Function(const Function& fe){}
    
  public:
    // constructor:
    Function(const char *name, MathExpression *paramlist,
	     MathExpression *body);
    // destructor:
    ~Function();
    char *getName(void) const{ return name; }
    MathExpression *getParameterList(void){ return paramlist; }
    MathExpression *getBody(void){ return body; }
  };
  
  /**
     @brief A list of functions
     @see MathExpression
     @internal
  */
  class FunctionList{
    
  private:
    Function *first;
    Function *last;

    bool modified;
    
    // copyconstructor: not for use
    FunctionList(const FunctionList& fl){}
    
  public:
    
    // constructor:
    FunctionList(): first(0), last(0), modified(false){}
    
    // destructor:
    ~FunctionList();

    /**
       @brief inserts a functionelement to the list
       @param fe the functionelement
    */
    void insert(Function *fe) throw(Exception<FunctionList>);

    /**
       @brief removes a function from the list
    */
    void remove(const char *name) throw(Exception<FunctionList>);

    /**
       @brief prints the complete list (if name==0) or the function with name <name> to standard-output
       @param name a functionname
    */
    void print(std::streamsize precision,const char *name=0);

    /**
       @brief returns the complete list represented as a string
       @return the string representing the list
    */
    std::string toString(std::streamsize precision) const;

    /**
       @brief tests if function <name> is a member of list
       @return true, if member, false otherwise
    */
    bool isMember(const char *name) const;

    /**
       @brief returns the functionelement with name <name>
       @param name the functionname
       @return the functionelement
    */
    Function *get(const char *name) const;

    /**
       Automatically called with value=true by a MathExpression-object if a function-definition occurs.
       @brief Sets the MathExpression-object to be modified.
       @param value the value
    */
    void setModified(bool value){ modified = value; }

    /**
       @brief returns the modified-status of the object.
       @return modified-status
    */
    bool isModified(){ return modified; }
    
  };
  
}


#endif
