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
   @file functions.hpp
   @author Friedemann Zintel
*/

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <vector>

/**
   @brief for useful functions and algorithms, defined in functions.hpp
   @ingroup functions
*/
namespace fun{

  /** @name map-functions */
  //@{
  // wendet f auf jedes Element von l an und gibt neues Objekt zurueck
  /**
     f is applied on each element of l, l itself will be left unmodified,
     a new object will be created and returned.
     @brief a map-function
     @param f the function to apply
     @param l the object f should be applied on
  */
  template<typename T1, typename T2, template<typename> class T3>
  T3<T2> *map_new(T1 (*f)(T2),T3<T2> &l){
    
    T3<T2> *obj = new T3<T2>;

    typename T3<T2>::iterator iter = l.begin();
    typename T3<T2>::iterator end = obj->end();
    
    while (iter != l.end()){
      obj->insert(end,(*f)(*iter++));
      end = obj->end();
    }
    return obj;
  }
  
  // wendet f auf jedes Element von l an wodurch l modifiziert wird
  /**
     f is applied on each element of l, l will be modified.
     @brief a map-function
     @param f the function to apply
     @param l the object f should be applied on
  */
  template<typename T1, typename T2, template<typename> class T3>
  void map_same(T1 (*f)(T2),T3<T2> &l){
    
    typename T3<T2>::iterator iter = l.begin();
    
    while (iter != l.end()){
      *iter = (*f)(*iter++);
    }
  }

  
  // wendet f auf jedes Element von l an wodurch die Elemente aus l NICHT modifiziert werden
  /**
     f is applied on each element of l, each element of l will be left unmodified.
     @brief a map-function
     @param f the function to apply
     @param l the object f should be applied on
  */
  template<typename T2, template<typename> class T3>
  void map_unchg(void (*f)(T2),T3<T2> &l){
    
    typename T3<T2>::iterator iter = l.begin();
    
    while (iter != l.end()){
      (*f)(*iter++);
    }
  }
  //@}

  /** @name filter-functions */
  //@{
  // Erstellt neue Liste mit den Elementen, fuer die f true ergibt
  /**
     a new list will be created including each element e where (*f)(e) == true 
     @brief a filter-function
     @param f the function to apply
     @param l the object f should be applied on
  */
  template<typename T1, template<typename> class T2>
  T2<T1> *filter_new(bool (*f)(T1),T2<T1> &l){

    T2<T1> *obj = new T2<T1>;

    typename T2<T1>::iterator iter = l.begin();
    typename T2<T1>::iterator end = obj->end();

    while (iter != l.end()){
      if ((*f)(*iter) == true){
	obj->insert(end,*iter);
	end = obj->end();
      }
      iter++;
    }
    return obj;
  }

  // Elemente aus l werden entfernt, fuer die f false ergibt
  /**
     each element e will be excluded from l, where (*f)(e) == false 
     @brief a filter-function
     @param f the function to apply
     @param l the object f should be applied on
  */
  template<typename T1, template<typename> class T2>
  void filter_same(bool (*f)(T1),T2<T1> &l){
    
    typename T2<T1>::iterator iter = l.begin();
    
    while (iter != l.end()){
      if ((*f)(*iter) == false)
	l.erase(iter++);
      else
	iter++;
    }
  }

 // Elemente aus vector l werden entfernt, fuer die f false ergibt
  /**
     Because vector has a slightly different behaviour than other containerclasses,
     it needs its own function-definition.
     @brief a filter-function
     @param f the function to apply
     @param l the vector f should be applied on
     @overload
  */
  template<typename T1>
  void filter_same(bool (*f)(T1),std::vector<T1> &l){
    
    typename std::vector<T1>::iterator iter = l.begin();
    
    while (iter != l.end()){
      if ((*f)(*iter) == false)
	l.erase(iter); // !!!!!!
      else
	iter++;
    }
  }
  //@}
}
#endif
