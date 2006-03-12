/**
   @mainpage
   This page documents the static-library 'Friedemann-Zintel`s-Tools`n-Templates'
   @author Friedemann Zintel
   
   Copyright (C) 1999-2004 Friedemann Zintel

   Compilation is only successfully tested with gcc v.2.95 and v.3.3 .
   
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
   @class Exception_T
   @internal Use Exception< T > instead
   @brief The baseclass for all exceptions
*/
class Exception_T{

private:

  Exception_T(){}
  
protected:
  
  std::string errormsg;
  std::string spc_id;
  std::string id_errormsg;
  
  Exception_T(const std::string &id);
  
  Exception_T(const std::string &id, const std::string &errormsg);
  
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
  
  virtual ~Exception_T(){}

  /** For using like: \n cout << e;
      @brief to pass it to an output-stream
      @param ostr the output-stream
      @param e the exception
  */
  friend std::ostream& operator<<(std::ostream& ostr, const Exception_T& e);

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
template<typename T> class Exception : public Exception_T{

public:

  /**
     constructor whithout message
 */
  Exception() : Exception_T(classname()){}

  /**
     constructor whith errormessage
     @param errormsg pointer to the errormessage
  */
  Exception(const char * const  errormsg) : Exception_T(classname() + "Exception",std::string(errormsg)){}
  
  /**
     constructor whith errormessage
     @param errormsg the errormessage
  */
  Exception(const std::string &errormsg) : Exception_T(classname() + "Exception",errormsg){}

  /**
     extensibility for derived classes
     @param id the identifikation-string for the class (classname)
     @param errormsg the errormessage
  */
  Exception(const std::string &id, const std::string &errormsg) : Exception_T(id,errormsg){}

  virtual ~Exception(){}

protected:

  /// Extracts the class-name from a typeid()-information
  std::string extract(const std::string &s){ return Exception_T::skipDigits(Exception_T::skipLetters(s)); }

  /**
     Generates the corresponding classname
     @return the classname
  */
  virtual std::string classname(){ return extract(typeid(T).name()); }

};

/**
   Sometimes it's very usefull to be able to define a subclass of an exception-class \e Exception<Class>
   which should have the same properties. For example a function \e Function in the class \e Class would send an exception and you want to
   specify the exception indicating the error. But also you want to be able just to catch the exception \e Exception<Class>.
   So you would like to create a subclass of \e Exception<Class>. It's possible with the SubException< ... >-class. The following
   coding-method works very well and fullfills this aim:\n
   - define in the beginning of your class \e Class a new class:\n
   @code
   private:
   class FunctionErr{};
   @endcode
   - call somewhere in your code:\n
   @code throw SubException<FunctionErr,Class>( ... ) @endcode
   - the code which is calling the function \e Function can now ...\n
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
    return Exception_T::skipDigits(Exception_T::skipLetters(Exception_T::skipDigits(Exception_T::skipLetters(s)))); }

  virtual std::string classname(){ return ( extract(typeid(TBase).name()) + extract_sub(typeid(TSub).name()) + "Exception" ); }
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
