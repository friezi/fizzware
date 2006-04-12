/*
    Copyright (C) 1999-2005 Friedemann Zintel

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
   @file propertyreader.hpp
   @author Friedemann Zintel
*/

#ifndef PROPERTYREADER_HPP
#define PROPERTYREADER_HPP

#include <errno.h>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <functional>
#include <datastructures.hpp>
#include <exception.hpp>

#define BLKSIZE 20

/**
   @class PropertyReader
   @brief To extract properties from a property-file
*/

class PropertyReader{
 
private:

  typedef std::pair<std::string,std::string> Property;
  typedef std::map< std::string,std::string,std::less<std::string> > Properties;
  
  // Exception-Classes
  /** @internal */
  class OpenErr{};
  
  /** @internal */
  class ReadErr{};
  
  /**
     @class Scanner
     @internal
  */
  class Scanner{
    
  public:
  
    // muessen public sein, da sie sonst von PropertyReader::read() und PropertyReader::write() nicht
    // abgefangen werden koennen
    
    // Exception-Classes
    /** @internal */
    class InputInvalidErr{};
    
    /** @internal */
    class IncompleteErr{};
    
    /** @internal */
    class EOFErr{};
    
    /** @internal */
    class NoIDErr{};
    
    /** @internal */
    class NoValErr{};
    
    /** @internal */
    class SyntaxErr{};

  private:
   
    std::ifstream &input;
    unsigned int &line;

    static char delimitors[];
    static char separators[];
    static char zero;
    static char space;

    static bool isDelimitor(char c);
    static bool isSeparator(char c);

    bool skipSpaces(char &c);
    
  public:
    
    Scanner(std::ifstream &input, unsigned int &line) : input(input), line(line){}
    
    ~Scanner(){}
    
    
    /**
       @brief Get next pair of property and value
       @return the property-value-pair
       @exception SubException<InputInvalidErr,Scanner>
       @exception SubException<IncompleteErr,Scanner>
       @exception SubException<EOFErr,Scanner>
       @exception SubException<SyntaxErr,Scanner>
       @exception SubException<NoIDErr,Scanner>
       @exception SubException<NoValErr,Scanner>
    */
    Property nextTokenpair() throw (SubException<InputInvalidErr,Scanner>,
				    SubException<IncompleteErr,Scanner>,
				    SubException<EOFErr,Scanner>,
				    SubException<SyntaxErr,Scanner>,
				    SubException<NoIDErr,Scanner>,
				    SubException<NoValErr,Scanner>);
    
    /**
       @brief Get next pair of property and value and save all comments
       @param comments comments will be saved to here
       @return the property-value-pair
       @exception SubException<InputInvalidErr,Scanner>
       @exception SubException<IncompleteErr,Scanner>
       @exception SubException<EOFErr,Scanner>
       @exception SubException<SyntaxErr,Scanner>
       @exception SubException<NoIDErr,Scanner>
       @exception SubException<NoValErr,Scanner>
    */
    Property nextTokenpairSaveComments(std::ostringstream *comments) throw (SubException<InputInvalidErr,Scanner>,
								       SubException<IncompleteErr,Scanner>,
								       SubException<EOFErr,Scanner>,
								       SubException<SyntaxErr,Scanner>,
								       SubException<NoIDErr,Scanner>,
								       SubException<NoValErr,Scanner>);
    
  };

  class iterator{

    friend class PropertyReader;

  private :
      
    Properties::iterator p_it;

  public:

    iterator(const PropertyReader::iterator &it){ *this = it; }
    iterator(Properties::iterator it) : p_it(it){}
      
    void operator++(){ ++p_it; }
    void operator--(){ --p_it; }
    void operator++(int){ p_it++; }
    void operator--(int){ p_it--; }
    Property operator*(){ return *p_it; }
    bool operator==(PropertyReader::iterator it){ return ( &*p_it == &*(it.p_it) ); }
    bool operator!=(PropertyReader::iterator it){ return ( &*p_it != &*(it.p_it) ); }
    void operator=(PropertyReader::iterator it){ p_it = it.p_it; }
      
  };

  Properties properties;
  std::string filename;
  unsigned int line;
  
protected:

  /// The system-error
  std::string err(){ return std::string(strerror(errno)); }

public:

  /**
     @param filename the file conatining the properties
  */
  PropertyReader(const std::string &filename) : filename(filename), line(0){};

  PropertyReader(const PropertyReader &propertyreader);

  ~PropertyReader(){}

  /**
     @brief Reads all the properties from file \<filename\>
     @exception Exception<PropertyReader>
     @exception SubException<OpenErr,PropertyReader>
     @exception SubException<Scanner::IncompleteErr,Scanner>
     @exception SubException<Scanner::NoValErr,Scanner>
     @exception  SubException<Scanner::SyntaxErr,Scanner>
     @exception SubException<Scanner::InputInvalidErr,Scanner>
  */
  void read() throw (Exception<PropertyReader>,
		     SubException<OpenErr,PropertyReader>,
		     SubException<Scanner::InputInvalidErr,Scanner>,
		     SubException<Scanner::IncompleteErr,Scanner>,
		     SubException<Scanner::SyntaxErr,Scanner>,
		     SubException<Scanner::NoValErr,Scanner>,
		     SubException<Scanner::NoIDErr,Scanner>);

  /**
     All the properties in the file \<filename\> will be modified, non-existing properties will
     be erased, comments will stay
     @brief Modifies all the properties in file \<filename\>
     @exception Exception<PropertyReader>
     @exception SubException<OpenErr,PropertyReader>
     @exception SubException<Scanner::IncompleteErr,Scanner>
     @exception SubException<Scanner::NoValErr,Scanner>
     @exception  SubException<Scanner::SyntaxErr,Scanner>
     @exception SubException<Scanner::InputInvalidErr,Scanner>
  */
  void modify() throw (Exception<PropertyReader>,
		       SubException<OpenErr,PropertyReader>,
		       SubException<Scanner::InputInvalidErr,Scanner>,
		       SubException<Scanner::IncompleteErr,Scanner>,
		       SubException<Scanner::SyntaxErr,Scanner>,
		       SubException<Scanner::NoValErr,Scanner>,
		       SubException<Scanner::NoIDErr,Scanner>);

  /**
     @brief Writes all the properties to file \<filename\>
     @exception Exception<PropertyReader>
  */
  void write() throw (Exception<PropertyReader>);

  /**
     @brief Get the value of the a property
     @param property name of property
     @return value of property
  */
  std::string get(std::string property){ return properties[property]; }

  /**
     @brief Set the value of the a property
     @param property name of property
     @param value value of property
  */
  void set(const std::string & property, const std::string & value){ properties[property] = value; }

  /**
     @brief checks if property is a member
     @param property the property
     @return true or false
  */
  bool isMember(std::string property);

  /**
     @brief erases one entry of the properties
     @param property name of property to be erased
  */
  void erase(std::string property){ properties.erase(property); }

  /**
     @brief Get the filename
     @return filename
  */
  std::string getFilename(){ return filename; }

  /**
     @brief Sets a new filename
     @param filename new filename
  */
  void setFilename(std::string filename){ this->filename = filename; }

  /**
     @brief Erases all entries, except filename
  */
  void clear();

  /**
     @brief Show all properties
  */
  void show();

  /**
     @brief iterator pointing to beginning of properties
  */
  iterator begin(){ return iterator(properties.begin()); }

  /**
     @brief iterator pointing to end of properties
  */
  iterator end(){ return iterator(properties.end()); }

  /**
     @brief syntactic sugar
  */
  std::string & operator[](const std::string & property){ return properties[property]; }

};

#endif
