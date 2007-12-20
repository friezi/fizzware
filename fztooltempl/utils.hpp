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

#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <exception.hpp>

#define UTILS_LU_BIT 5

/**
   @brief Contains templates for several utilities
   @ingroup utilities
   @since V1.96
*/
namespace util{

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

  };
}

#endif
