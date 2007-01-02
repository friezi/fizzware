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
#include <cmdlparser.hpp>
#include <utils.hpp>

#define BLKSIZE 20

/**
   It reads key/value-pairs from files. Sections are supported but not required.
   @brief To extract properties from a property-file
*/

class PropertyReader{
 
private:

  typedef std::pair< std::string, std::string > Property;
  typedef std::map< std::string, std::string, std::less<std::string> > Properties;
  typedef std::map< std::string, Properties *, std::less<std::string> > Sections;

  static std::string NO_SECTION;
  
  // Exception-Classes
  /** @internal */
  class OpenErr{};
  
  /** @internal */
  class ReadErr{};

  /** @internal */
  class NoSectionErr{};
  
  /**
     @class Parser
     @internal
  */
  class Parser{
    
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

    std::string currentSection;

    static char delimitors[];
    static char separators[];
    static char blanks[];
    static char zero;
    static char space;

    static bool isDelimitor(char c);
    static bool isSeparator(char c);
    static bool isABlank(char c);

    bool skipSpaces(char &c);
    void skipRestOfLine(char &c) throw (SubException<EOFErr,Parser>);
    std::string truncateString(std::string str);

    /**
       @pre line is a comment line and has already started with a '#'
    */
    void overreadCommentline(char &c, std::ostream * comments) throw (SubException<EOFErr,Parser>);
    /**
       @pre line is a section-definition and has already started with a '['
    */
    void readCurrentSection(char &c) throw (SubException<EOFErr,Parser>,SubException<SyntaxErr,Parser>);
    
  public:
    
    Parser(std::ifstream &input, unsigned int &line) : input(input), line(line), currentSection(NO_SECTION){}
    
    ~Parser(){}
    
    
    /**
       @brief Get next pair of property and value
       @return the property-value-pair
       @exception SubException<InputInvalidErr,Parser>
       @exception SubException<IncompleteErr,Parser>
       @exception SubException<EOFErr,Parser>
       @exception SubException<SyntaxErr,Parser>
       @exception SubException<NoIDErr,Parser>
       @exception SubException<NoValErr,Parser>
    */
    Property nextKeyValuePair() throw (SubException<InputInvalidErr,Parser>,
				       SubException<IncompleteErr,Parser>,
				       SubException<EOFErr,Parser>,
				       SubException<SyntaxErr,Parser>,
				       SubException<NoIDErr,Parser>,
				       SubException<NoValErr,Parser>);
    
    /**
       @brief Get next pair of property and value and save all comments
       @param comments comments will be saved to here
       @return the property-value-pair
       @exception SubException<InputInvalidErr,Parser>
       @exception SubException<IncompleteErr,Parser>
       @exception SubException<EOFErr,Parser>
       @exception SubException<SyntaxErr,Parser>
       @exception SubException<NoIDErr,Parser>
       @exception SubException<NoValErr,Parser>
    */
    Property nextKeyValuePairSaveComments(std::ostream *comments) throw (SubException<InputInvalidErr,Parser>,
									       SubException<IncompleteErr,Parser>,
									       SubException<EOFErr,Parser>,
									       SubException<SyntaxErr,Parser>,
									       SubException<NoIDErr,Parser>,
									       SubException<NoValErr,Parser>);
    std::string getCurrentSection(){ return currentSection; }
    
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
    Properties::iterator operator->(){ return p_it; }
    bool operator==(PropertyReader::iterator it){ return ( &*p_it == &*(it.p_it) ); }
    bool operator!=(PropertyReader::iterator it){ return ( &*p_it != &*(it.p_it) ); }
    void operator=(PropertyReader::iterator it){ p_it = it.p_it; }
      
  };

  class sections_iterator{

    friend class PropertyReader;

  private :
      
    Sections::iterator s_it;

  public:

    //     iterator(const PropertyReader::sections_iterator &it){ *this = it; }
    sections_iterator(Sections::iterator it) : s_it(it){}
      
    void operator++(){ ++s_it; }
    void operator--(){ --s_it; }
    void operator++(int){ s_it++; }
    void operator--(int){ s_it--; }
    std::string operator*(){ return (*s_it).first; }
    bool operator==(PropertyReader::sections_iterator it){ return ( &*s_it == &*(it.s_it) ); }
    bool operator!=(PropertyReader::sections_iterator it){ return ( &*s_it != &*(it.s_it) ); }
    void operator=(PropertyReader::sections_iterator it){ s_it = it.s_it; }
      
  };

  Sections sections;
  const cmdl::CmdlParser *cmdlparser;
  std::string filename;
  unsigned int line;
  
protected:

  /// The system-error
  std::string err(){ return std::string(strerror(errno)); }

public:

  /**
     @param filename the file conatining the properties
  */
  PropertyReader(const std::string &filename) : cmdlparser(0), filename(filename), line(0){};

  /**
     @param filename the file containing the properties
     @param cmdlparser from this cmdlparser all parameters (key/value-pairs) will inserted in the property-list
  */
  PropertyReader(const std::string &filename, const cmdl::CmdlParser &cmdlparser) : cmdlparser(&cmdlparser), filename(filename), line(0){};

  /**
     @param propertyreader another PropertyReader, its entries wil be copied to this new one.
  */
  PropertyReader(const PropertyReader &propertyreader);

  ~PropertyReader();

  /**
     @brief Reads all the properties from file \<filename\>
     @exception Exception<PropertyReader>
     @exception SubException<OpenErr,PropertyReader>
     @exception SubException<Parser::IncompleteErr,Parser>
     @exception SubException<Parser::NoValErr,Parser>
     @exception  SubException<Parser::SyntaxErr,Parser>
     @exception SubException<Parser::InputInvalidErr,Parser>
  */
  void read() throw (Exception<PropertyReader>,
		     SubException<OpenErr,PropertyReader>,
		     SubException<Parser::InputInvalidErr,Parser>,
		     SubException<Parser::IncompleteErr,Parser>,
		     SubException<Parser::SyntaxErr,Parser>,
		     SubException<Parser::NoValErr,Parser>,
		     SubException<Parser::NoIDErr,Parser>);

  /**
     All the properties in the file \<filename\> will be modified, non-existing properties will
     be erased, comments will stay
     @brief Modifies all the properties in file \<filename\>
     @exception Exception<PropertyReader>
     @exception SubException<OpenErr,PropertyReader>
     @exception SubException<Parser::IncompleteErr,Parser>
     @exception SubException<Parser::NoValErr,Parser>
     @exception  SubException<Parser::SyntaxErr,Parser>
     @exception SubException<Parser::InputInvalidErr,Parser>
  */
  void modify() throw (Exception<PropertyReader>,
		       SubException<OpenErr,PropertyReader>,
		       SubException<Parser::InputInvalidErr,Parser>,
		       SubException<Parser::IncompleteErr,Parser>,
		       SubException<Parser::SyntaxErr,Parser>,
		       SubException<Parser::NoValErr,Parser>,
		       SubException<Parser::NoIDErr,Parser>);

  /**
     @brief Writes all the properties to file \<filename\>
     @exception Exception<PropertyReader>
  */
  void write() throw (Exception<PropertyReader>);

  /**
     @brief Get the value of the property of a specific section 
     @param section name of section
     @param property name of property
     @return value of property
  */
  std::string get(const std::string section, const std::string property) throw (SubException<NoSectionErr,PropertyReader>);

  /**
     @brief Get the value of the property of section "No_Section"
     @param property name of property
     @return value of property
  */
  std::string get(const std::string property){ return get(NO_SECTION,property); }

  /**
     @brief Set the value of the property of a specific section
     @param section name of section
     @param property name of property
     @param value value of property
  */
  void set(const std::string & section, const std::string & property, const std::string & value);

  /**
     @brief Set the value of the a property of section "No_Section"
     @param property name of property
     @param value value of property
  */
  void set(const std::string & property, const std::string & value){ set(NO_SECTION,property,value); }

  /**
     @brief checks if property is a member of a specific section
     @param section name of section
     @param property the property
     @return true or false
  */
  bool isMember(const std::string section, std::string property);

  /**
     @brief checks if property is a member of section NO_SECTION
     @param property the property
     @return true or false
  */
  bool isMember(std::string property) throw (SubException<NoSectionErr,PropertyReader>) { return isMember(NO_SECTION,property); }

  /**
     @brief erases one entry of the properties of a specific section
     @param section name of section
     @param property name of property to be erased
  */
  void erase(const std::string section, const std::string property);

  /**
     @brief erases one entry of the properties
     @param property name of property to be erased
  */
  void erase(const std::string property){ erase(NO_SECTION,property); }

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
     @brief Sets a new CmdlParser
     @param cmdlparser new CmdlParser
  */
  void setCmdlParser(const cmdl::CmdlParser &cmdlparser){ this->cmdlparser = &cmdlparser; }

  /**
     @brief Erases all entries, except filename
  */
  void clear();

  /**
     @brief Show all properties
  */
  void show();

  /**
     @brief iterator pointing to beginning of properties of a specific section
     @param section name of section
     @return the iterator pointing to the first element
  */
  iterator begin(const std::string section) throw (SubException<NoSectionErr,PropertyReader>);

  /**
     @brief iterator pointing to beginning of properties of section "No_Section"
     @return the iterator pointing to the first element
  */
  iterator begin() throw (SubException<NoSectionErr,PropertyReader>) { return begin(NO_SECTION); }

  /**
     @brief iterator pointing to end of properties
     @param section name of section
     @return the iterator pointing beyond last element
  */
  iterator end(const std::string section) throw (SubException<NoSectionErr,PropertyReader>);

  /**
     @brief iterator pointing to end of properties
     @return the iterator pointing beyond end
  */
  iterator end() throw (SubException<NoSectionErr,PropertyReader>) { return end(NO_SECTION); }

  /**
     @brief iterator pointing to beginning of sections
     @return the iterator pointing to the first element
  */
  sections_iterator beginSections() { return sections_iterator(sections.begin()); }

  /**
     @brief iterator pointing to end of sections
     @return the iterator pointing beyond last element
  */
  sections_iterator endSections() { return sections_iterator(sections.end()); }

  /**
     @brief syntactic sugar
  */
  std::string & operator[](const std::string & property) throw (SubException<NoSectionErr,PropertyReader>) { return (*sections[NO_SECTION])[property]; }

  /**
     @brief return a string-representation of all properties
     @return the string
  */
  std::string toString();

};

#endif
