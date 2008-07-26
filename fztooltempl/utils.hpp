/*
  Copyright (C) 1999-2006 Friedemann Zintel

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
   @file utils.hpp
   @since V1.96
   @author Friedemann Zintel
*/

#ifndef _FZTT_UTILS_HPP_
#define _FZTT_UTILS_HPP_
#include <string>
#include <set>
#include <sstream>
#include <exception.hpp>

#define UTILS_LU_BIT 5

/**
   @brief Contains templates for several utilities
   @ingroup utilities
   @since V1.96
*/
namespace utils{

  /**
     This class is directly derived from the STL string class.\n
     Unfortunately the string class of the STL doesn't include some more usefull string-operations (like trunc() for cutting off
     leading and trailing blanks). The String-class enhances the STL-string-class with usefull functions.
     @brief An extended string-class
     @since v1.96
  */
  class String : public std::string{

  protected:

    static char blanks[2];

  public:

    /**
       @brief the standard-constructor
    */
    String(){}

    /**
       @brief The cast-constructor
    */
    String(const std::string & s){ (*(std::string *)this) = s; }

    /**
       @brief The cast-constructor
    */
    String(const char * s){ (*(std::string *)this) = s; }
    
    /**
       @brief cuts off leading and trailing blanks;
       @return an truncated string
    */
    String trunc() const ;

    /**
       @brief converts all latin-characters to lower-case
       @return the converted string
       @since V2.1
   */
    String toLower() const ;

    /**
       @brief converts all latin-characters to upper-case
       @return the converted string
       @since V2.1
   */
    String toUpper() const ;

    /**
       @brief checks if a char is a lower latin-character
       @return true if char is a lower latin-character
       @since V2.1
    */
    static bool isLower(char c){ return ( c >= 'a' && c<= 'z' ); }

    /**
       @brief checks if a char is a upper latin-character
       @return true if char is a upper latin-character
       @since V2.1
    */
    static bool isUpper(char c){ return ( c >= 'A' && c <= 'Z' ); }

    /**
       @brief converts a latin-char to lower case
       @return char converted to lower case
       @since V2.1
    */
    static char latinToLower(char c){ return ( isUpper(c) ? c | 1L<<UTILS_LU_BIT : c ); }

    /**
       @brief converts a latin-char to upper case
       @return char converted to upper case
       @since V2.1
    */
    static char latinToUpper(char c){ return ( isLower(c) ? c & ~(1L<<UTILS_LU_BIT) : c ); }

    /**
       @brief tests if a char is a blank-character
       @return true if char is a blank-character
       @since V2.1
    */
    static bool isABlank(char c);

    /**
       @brief checks, if a character is contained in a String
       @param c the character ro be checked
       @return true if c is conatined in String
    */
    bool containsChar(const char & c) const;

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const bool & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const unsigned char & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const char & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const unsigned int & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const int & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const unsigned long & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const long & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const double & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const std::string & value);

    /**
       @brief concatenates value with the string
       @param value the value to concatenate
       @return a reference to the string
       @since V2.1
    */
    String & operator+(const char *value);

  };

  template <typename T> class Observable;

  /**
     An observer can be used to observe the state of an object. If an observer has registered at an observable-object its
     update()-method will be called whenever a state-change of the observed object has occured. The decision if a state has changed
     lies with the observable-object.
     Its method update() will be called
     @brief Represents the design-pattern Observer.
     @see Observable
     @since V2.1
  **/
  template <typename T> class Observer{

  public:
    
    /**
       On state-change of the observed object the method update() will be called.
       @brief For updating on state-change.
       @param observable the observable-object which called update()
       @param object the observed object resp. its value
       @see Observable
       @since V2.1
    */
    virtual void update(Observable<T> *observable, T object) = 0;

    /**
       @brief for taking means in case of observable destruction
    */
    virtual void observableDeleted(Observable<T> * const observable){}

    virtual ~Observer(){}

  };

  /**
     An observable-object can be used to notify observers on a state-change of the observed object. Observers have to register
     at the observable-object to be notified. If state-change occurs the observable-object calls the observers Observer::update()-method.
     @brief Represents the design-pattern Observable.
     @see Observer
     @since V2.1
  **/
  template <typename T> class Observable{
    
  private:
    
    bool changed;

    std::set<Observer<T> *> observers;

  public:

    Observable() : changed(false){}

    virtual ~Observable(){
      
      for ( typename std::set<Observer <T> *>::const_iterator it = observers.begin(); it != observers.end(); it++ )
	(*it)->observableDeleted(this);
      
    }
    
    /**
       @brief adds an observer to be notified on state-change.
       @param observer the observer to be added
       @throws Exception< Observable<T> >
    */
    virtual void addObserver(Observer<T> *observer) throw (exc::Exception< Observable<T> >){ 
      
      if ( observer == 0 )
	throw exc::Exception< Observable<T> >("observer is a nullpointer!");
      
      observers.insert(observer);
      
    }
    
    /**
       @brief removes an observer and prevents its notification.
       @param observer the observer to be removed.
    */
    void removeObserver(Observer<T> *observer){ observers.erase(observer); }
    
    /**
       @brief removes all observer and prevents their notification.
    */
    void removeObservers(){ observers.clear(); }

    /**
       @brief number of observers
       @return number of observers
    **/
    size_t countObservers(){ return observers.size(); }

    /**
       On state-change (hasChanged() == true) of the observed object all observers will be notified by calling their
       Observer::update()-method.
       @brief notifies observers
       @param object the object which's state has changed
    **/
    virtual void notifyObservers(T object){

      if ( changed == true ){

	for ( typename std::set<Observer<T> *>::iterator oit = observers.begin(); oit != observers.end(); oit++)
	  (*oit)->update(this,object);

	changed = false;

      }

    }

    /**
       @brief marks the observed object to have changed.
    **/
    void setChanged(){ changed = true; }

    /**
       @brief marks the observed object to have not changed.
    **/
    void clearChanged(){ changed = false; }

    /**
       @brief returns the changed-state of the notified object.
       @return change-state
    **/
    bool hasChanged(){ return changed; }
      
  };
  
  /**
     This class is a special Derivate of an Observable-class. It stores a value. If the value changes on
     call of setValue() all observers will be notified, and they will be passed a copy of the stored value.
     @brief Notifier which notifies on change of a value
     @since V2.1
  **/
  template <typename T> class ChangeNotifier : public utils::Observable<T>{
    
  private:
    
    T value;

    bool initialized;
    
  public:

    ChangeNotifier() : initialized(false){}

    /**
       @brief Initialization on construction
       @param value the value
    **/
    ChangeNotifier(T value){ initValue(value); }

    /**
       Its unavoidable if T is a pointer or a class-type.
       @brief Initialises the observed value whithout notifying the observers.
       @param value the value
    **/
    void initValue(T value){
      
      initialized = true;
      this->value = value;

    }

    /**
       After adding, the observer will automatically be notified about the current value
       @brief adds an observer to be notified on state-change
       @param observer the observer to be added
       @throws Exception< Observable<T> >
    */
    virtual void addObserver(Observer<T> *observer) throw (exc::Exception< Observable<T> >){ 

      Observable<T>::addObserver(observer);
      
      if ( initialized == true )
	observer->update(this,value);

    }

    /**
       If the value is different from the previous value all observers will be notified.
       @brief sets a new value
       @param value the new value
    **/
    void setValue(T value){
      
      if ( initialized == false || this->value != value ){
	
	initialized = true;
	this->value = value;
	
	Observable<T>::setChanged();
	
	notifyObservers(value);
	
      }    
      
    }
    
    /**
       @brief returns the stored value.
       @return the value
    **/
    T getValue(){ return value; }

    /**
       @brief same like setValue()
       @param value the new value
       @see setValue()
    */
    void operator=(T value){ setValue(value); }

    /**
       @brief sets the value to the value stored in notifier
       @param notifier the notifier string the new value
       @see setValue()
    */
    void operator=(ChangeNotifier<T> & notifier){ setValue(notifier.getValue()); }
    
  };

  /**
     @brief A SmartObserver automatically removes itself on destruction from all notifiers i.e. Observables it has registered to.
     @note Remember to overwrite Observer::update()
     @since V2.1
  */
  template <typename T> class SmartObserver : public Observer<T>{

    std::set<Observable<T> *> notifiers;
    
  public:

    /**
       @brief removes itself from all notifiers
    **/
    ~SmartObserver(){

      for ( typename std::set<Observable<T> *>::iterator oit = notifiers.begin(); oit != notifiers.end(); oit++ )
	(*oit)->removeObserver(this);

    }

    /**
       @brief adds itself to a notifier
       @param notifier the notifier to be added to
       @exception Exception< Observable<T> >
    **/
    void addToNotifier(Observable<T> *notifier) throw (exc::Exception< Observable<T> >,exc::ExceptionBase){

      notifier->addObserver(this);
      notifiers.insert(notifier);

    }


    /**
       @brief removes itself from a notifier
       @param notifier the notifier to be removed from
    **/
    void removeFromNotifier(Observable<T> *notifier){

      notifiers.erase(notifier);
      notifier->removeObserver(this);

    }
 
    void observableDeleted(Observable<T> * const notifier){
      notifiers.erase(notifier);
    }
   
  };
  
}

#endif
