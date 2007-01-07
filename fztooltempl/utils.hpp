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

/**
   @brief Contains templates for several utilities
   @ingroup utilities
   @since V1.96
*/
namespace util{

  /**
     @brief An extended string-class
     @since V1.96
  */
  class String : public std::string{

  protected:

    static char blanks[2];

    bool isABlank(char c);

  public:

    /**
       @brief The cast-constructor
    */
    String(const std::string & s){ (*(std::string *)this) = s; }
    
    /**
       @brief truncates leading and trailing blanks;
       @return an truncated string
    */
    std::string trunc();

  };
}

#endif
