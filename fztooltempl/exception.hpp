/**
   @mainpage
   This page documents the static-library 'Friedemann-Zintel`s-Tools`n-Templates'
   @author Friedemann Zintel
   
   Copyright (C) 1999-2004 Friedemann Zintel

   Compilation is successfully tested with g++ v.2.95, v.3.3 and v.4.0 .
   
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
   @file exception.hpp
   @author Friedemann Zintel
*/

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>
#include <string>
#include <typeinfo>

// Not for direct use!!!
/**
   @class ExceptionBase
   @internal Use Exception< T > instead
   @brief The baseclass for all exceptions
*/
class ExceptionBase{

private:

  ExceptionBase(){}
  
protected:
  
  std::string errormsg;
  std::string spc_id;
  std::string id_errormsg;
  
  ExceptionBase(const std::string &id);
  
  ExceptionBase(const std::string &id, const std::string &errormsg);
  
  // um fuehrende Zahlen bei durch typeid() generierten Klassennamen zu entfernen
  /**
     To skip leading digits produced by typeid()
     @param s the text
  */
  static std::string skipDigits(const std::string &s);

  // um fuehrende Buchstaben bei durch typeid() generierten Klassennamen zu entfernen
  /**
     To skip leading letters produced by typeid()
     @param s the text
  */
  static std::string skipLetters(const std::string &s);

public:
  
  virtual ~ExceptionBase(){}

  /** For using like: \n cout << e;
      @brief to pass it to an output-stream
      @param ostr the output-stream
      @param e the exception
  */
  friend std::ostream& operator<<(std::ostream& ostr, const ExceptionBase& e);

  /// prints the exception
  virtual void show() const;

  /** @brief returns only errormessage as string
      @return the constructed string
  */
  virtual const std::string &getMsg() const { return errormsg; }

  /** @brief returns id + errormessage
      @return the constructed string
  */
  virtual const std::string &getIdMsg() const { return id_errormsg; }

  /** @brief returns only errormessage as char *
      @return the ptr
  */
  virtual const char *getMsgCharPtr() const { return errormsg.c_str(); }

  /** @brief returns id + errormessage as char *
      @return the ptr
  */
  virtual const char *getIdMsgCharPtr() const { return id_errormsg.c_str(); }
};

/** 
    @class Exception
    @brief A general exception-classtemplate
    @see SubException
*/
template<typename T> class Exception : public ExceptionBase{

public:

  /**
     constructor whithout message
 */
  Exception() : ExceptionBase(classname()){}

  /**
     constructor whith errormessage
     @param errormsg pointer to the errormessage
  */
  Exception(const char * const  errormsg) : ExceptionBase(classname() + "Exception",std::string(errormsg)){}
  
  /**
     constructor whith errormessage
     @param errormsg the errormessage
  */
  Exception(const std::string &errormsg) : ExceptionBase(classname() + "Exception",errormsg){}

  /**
     extensibility for derived classes
     @param id the identifikation-string for the class (classname)
     @param errormsg the errormessage
  */
  Exception(const std::string &id, const std::string &errormsg) : ExceptionBase(id,errormsg){}

  virtual ~Exception(){}

protected:

  /// Extracts the class-name from a typeid()-information
  std::string extract(const std::string &s){ return ExceptionBase::skipDigits(ExceptionBase::skipLetters(s)); }

  /**
     Generates the corresponding classname
     @return the classname
  */
  virtual std::string classname(){ return extract(typeid(T).name()); }

};

/**
   Sometimes it's very usefull to have the possibility of defining a subclass of an exception-class \e Exception<Class>
   which should have the same properties. For example a function \e function in the class \e Class should send an exception and you want to
   specify the exception indicating the error more precisely. But you also want to have the possibility of just catching the exception \e Exception<Class>.
   So your wish is to create a subclass of \e Exception<Class>. It's possible with the SubException< ... >-class. The following
   coding-method works very well and fullfills this purpose:\n
   - define in the beginning of your class \e Class a new class:\n
   @code
   private:
   class FunctionErr{};
   @endcode
   - call somewhere in your code of \e function :\n
   @code throw SubException<FunctionErr,Class>( ... ) @endcode
   - the code which is calling the function \e function can now ...\n
   @code catch ( Exception<Class> &e ) { ... } @endcode
   @brief For building derived exception-classes from Exception<T>
*/
template<typename TSub, typename TBase> class SubException : public Exception<TBase>{
  
public:
  SubException() : Exception<TBase>(classname()){}

  SubException(const char * const errormsg) : Exception<TBase>(classname(),std::string(errormsg)){}
  
  SubException(const std::string &errormsg) : Exception<TBase>(classname(),errormsg){}

  SubException(const std::string &id, const std::string &errormsg) : Exception<TBase>(id,errormsg){}

  virtual ~SubException(){}

protected:

  /// Extracts the subclass-name from a typeid()-information
  std::string extract_sub(const std::string &s){
    return ExceptionBase::skipDigits(ExceptionBase::skipLetters(ExceptionBase::skipDigits(ExceptionBase::skipLetters(s)))); }

  virtual std::string classname(){ return ( this->extract(typeid(TBase).name()) + this->extract_sub(typeid(TSub).name()) + "Exception" ); }
};

/**
   @class OutOfMem
   @internal
   @brief to create a OutOfMemException
   @see OutOfMemException
*/
class OutOfMem{

protected:

  OutOfMem(){}
  OutOfMem(const OutOfMem&){}
};

/**
   @class NullPointer
   @internal
   @brief to create a NullPointerException
   @see NullPointerException
*/
class NullPointer{
  
protected:
  
  // Objekteerzeugung nicht erlaubt!!!
  NullPointer(){}
  NullPointer(const NullPointer&){}
  
};

/**
   the generated OutOfMemException
*/
typedef Exception<OutOfMem> OutOfMemException;
/**
   the generated NullPointerException
*/
typedef Exception<NullPointer> NullPointerException;

#endif
