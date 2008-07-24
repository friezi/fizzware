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

#ifndef _FZTT_PROPERTYREADER_HPP_
#define _FZTT_PROPERTYREADER_HPP_

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

namespace utils{

  /**
     It reads key/value-pairs from files. Sections are optional and not required.
     If the parser finds a section-definition all subsequent parameters will be stored in this section until a different
     section-definition is found. If properties are not bound to a section, they're internally stored in the section "NO_SECTION".
     Section-names are surround by "[" and "]". Values must be separated from keys with either "=" or ":". The syntax of property-files is as follows:
     \verbatim
     [<Section1>]
     <key1>=<value1>
     <key2>=<value2>
     ...
     \endverbatim
     @brief To extract properties from a property-file
  */
  class PropertyReader{
 
  private:

    typedef std::pair< std::string, std::string > Property;
    typedef std::map< std::string, std::string, std::less<std::string> > Properties;
    typedef std::pair< std::string, Properties * > Section;
    typedef std::map< std::string, Properties *, std::less<std::string> > Sections;

    static const std::string NO_SECTION;
  
    static const unsigned long DFLT_BLKSIZE;
  
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
      void skipRestOfLine(char &c) throw (excpt::SubException<EOFErr,Parser>);
      std::string truncateString(std::string str);

      /**
	 @pre line is a comment line and has already started with a '#'
      */
      void overreadCommentline(char &c, std::ostream * comments) throw (excpt::SubException<EOFErr,Parser>);
      /**
	 @pre line is a section-definition and has already started with a '['
      */
      void readCurrentSection(char &c) throw (excpt::SubException<EOFErr,Parser>,excpt::SubException<SyntaxErr,Parser>);
    
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
      Property nextKeyValuePair() throw (excpt::SubException<InputInvalidErr,Parser>,
					 excpt::SubException<IncompleteErr,Parser>,
					 excpt::SubException<EOFErr,Parser>,
					 excpt::SubException<SyntaxErr,Parser>,
					 excpt::SubException<NoIDErr,Parser>,
					 excpt::SubException<NoValErr,Parser>);
    
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
      Property nextKeyValuePairSaveComments(std::ostream *comments) throw (excpt::SubException<InputInvalidErr,Parser>,
									   excpt::SubException<IncompleteErr,Parser>,
									   excpt::SubException<EOFErr,Parser>,
									   excpt::SubException<SyntaxErr,Parser>,
									   excpt::SubException<NoIDErr,Parser>,
									   excpt::SubException<NoValErr,Parser>);
      std::string getCurrentSection(){ return currentSection; }
    
    };

    /**
       @brief iterator to properties
    */
    class properties_iterator{

      friend class PropertyReader;

    private :
      
      Properties::const_iterator p_it;

    public:

      properties_iterator(const PropertyReader::properties_iterator & it) { *this = it; }
      properties_iterator(const Properties::const_iterator & it) : p_it(it){}
      
      void operator++(){ ++p_it; }
      void operator--(){ --p_it; }
      void operator++(int){ p_it++; }
      void operator--(int){ p_it--; }
      Property operator*() const { return *p_it; }
      Properties::const_iterator operator->() const { return p_it; }
      bool operator==(const PropertyReader::properties_iterator & it) const { return ( &*p_it == &*(it.p_it) ); }
      bool operator!=(const PropertyReader::properties_iterator & it) const { return ( &*p_it != &*(it.p_it) ); }
      void operator=(const PropertyReader::properties_iterator & it){ p_it = it.p_it; }
      
    };

    /**
       @brief iterator to sections
    */
    class sections_iterator{

      friend class PropertyReader;

    private :
      
      Sections::const_iterator s_it;
    
    public:

      sections_iterator(const PropertyReader::sections_iterator &it){ *this = it; }
      sections_iterator(const Sections::const_iterator & it) : s_it(it){}
      
      void operator++(){ ++s_it; }
      void operator--(){ --s_it; }
      void operator++(int){ s_it++; }
      void operator--(int){ s_it--; }
      Section operator*() const { return *s_it; }
      Sections::const_iterator operator->() const { return s_it; }
      bool operator==(const PropertyReader::sections_iterator & it) const { return ( &*s_it == &*(it.s_it) ); }
      bool operator!=(const PropertyReader::sections_iterator & it) const { return ( &*s_it != &*(it.s_it) ); }
      void operator=(const PropertyReader::sections_iterator & it){ s_it = it.s_it; }
      
    };

    Sections sections;
    const cmdl::CmdlParser *cmdlparser;
    std::string filename;
  
  protected:

    /// The system-error
    std::string err() const { return std::string(strerror(errno)); }

  public:

    /**
       @param filename the file conatining the properties
    */
    PropertyReader(const std::string &filename) : cmdlparser(0), filename(filename){};

    /**
       @param filename the file containing the properties
       @param cmdlparser from this cmdlparser all parameters (key/value-pairs) will inserted in the property-list
    */
    PropertyReader(const std::string &filename, const cmdl::CmdlParser &cmdlparser) : cmdlparser(&cmdlparser), filename(filename){};

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
       @exception SubException<Parser::SyntaxErr,Parser>
       @exception SubException<Parser::InputInvalidErr,Parser>
    */
    void read() throw (excpt::Exception<PropertyReader>,
		       excpt::SubException<OpenErr,PropertyReader>,
		       excpt::SubException<Parser::InputInvalidErr,Parser>,
		       excpt::SubException<Parser::IncompleteErr,Parser>,
		       excpt::SubException<Parser::SyntaxErr,Parser>,
		       excpt::SubException<Parser::NoValErr,Parser>,
		       excpt::SubException<Parser::NoIDErr,Parser>);

    /**
       All the properties in the file \<filename\> will be updated, non-existing properties will
       be erased, comments will stay
       @brief Updates all the properties in file \<filename\>
       @deprecated Use update() instead
       @exception Exception<PropertyReader>
       @exception SubException<OpenErr,PropertyReader>
       @exception SubException<Parser::IncompleteErr,Parser>
       @exception SubException<Parser::NoValErr,Parser>
       @exception SubException<Parser::SyntaxErr,Parser>
       @exception SubException<Parser::InputInvalidErr,Parser>
    */
    void modify(bool keep_empty_sections = false) const throw (excpt::Exception<PropertyReader>,
							       excpt::SubException<OpenErr,PropertyReader>,
							       excpt::SubException<Parser::InputInvalidErr,Parser>,
							       excpt::SubException<Parser::IncompleteErr,Parser>,
							       excpt::SubException<Parser::SyntaxErr,Parser>,
							       excpt::SubException<Parser::NoValErr,Parser>,
							       excpt::SubException<Parser::NoIDErr,Parser>){ update(keep_empty_sections); }

    /**
       All the properties in the file \<filename\> will be updated, non-existing properties will
       be erased, comments will stay
       @brief Updates all the properties in file \<filename\>
       @since V1.96
       @param keep_empty_sections will also save declarations of empty sections; DOES NOT WORK YET!!!
       @exception Exception<PropertyReader>
       @exception SubException<OpenErr,PropertyReader>
       @exception SubException<Parser::IncompleteErr,Parser>
       @exception SubException<Parser::NoValErr,Parser>
       @exception SubException<Parser::SyntaxErr,Parser>
       @exception SubException<Parser::InputInvalidErr,Parser>
    */
    void update(bool keep_empty_sections = false) const throw (excpt::Exception<PropertyReader>,
							       excpt::SubException<OpenErr,PropertyReader>,
							       excpt::SubException<Parser::InputInvalidErr,Parser>,
							       excpt::SubException<Parser::IncompleteErr,Parser>,
							       excpt::SubException<Parser::SyntaxErr,Parser>,
							       excpt::SubException<Parser::NoValErr,Parser>,
							       excpt::SubException<Parser::NoIDErr,Parser>);
  
    /**
       @brief Writes all the properties to file \<filename\>
       @exception Exception<PropertyReader>
    */
    void write() const throw (excpt::Exception<PropertyReader>);

    /**
       @brief Get the value of the property of a specific section 
       @param section name of section
       @param property name of property
       @return value of property
    */
    std::string get(const std::string section, const std::string property) const throw (excpt::SubException<NoSectionErr,PropertyReader>);

    /**
       @brief Get the value of the property of section "NO_SECTION"
       @param property name of property
       @return value of property
    */
    std::string get(const std::string property) const { return get(NO_SECTION,property); }

    /**
       @brief Set the value of the property of a specific section
       @param section name of section
       @param property name of property
       @param value value of property
    */
    void set(const std::string & section, const std::string & property, const std::string & value);

    /**
       @brief Set the value of the a property of section "NO_SECTION"
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
    bool isMember(const std::string section, std::string property) const;

    /**
       @brief checks if property is a member of section "NO_SECTION"
       @param property the property
       @return true or false
    */
    bool isMember(std::string property) const throw (excpt::SubException<NoSectionErr,PropertyReader>){ return isMember(NO_SECTION,property); }

    /**
       @brief erases one entry of the properties of the a specific section
       @param section name of section
       @param property name of property to be erased
    */
    void erase(const std::string section, const std::string property);

    /**
       @brief erases one entry of the properties of section "NO_SECTION"
       @param property name of property to be erased
    */
    void erase(const std::string property){ erase(NO_SECTION,property); }

    /**
       @brief Get the filename
       @return filename
    */
    std::string getFilename() const { return filename; }

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
    void show() const;

    /**
       @brief iterator pointing to beginning of properties of a specific section
       @param s_it iterator of section
       @return the iterator pointing to the first element
    */
    properties_iterator begin(const sections_iterator & s_it) const throw (excpt::SubException<NoSectionErr,PropertyReader>){
      return properties_iterator(s_it->second->begin());
    }
  
    /**
       For better performance the return value should be assigned to a local varibale which should be used for comparison instead.
       @brief iterator pointing to beginning of properties of a specific section
       @param section name of section
       @return the iterator pointing to the first element
    */
    properties_iterator begin(const std::string section) const throw (excpt::SubException<NoSectionErr,PropertyReader>);

    /**
       For better performance the return value should be assigned to a local varibale which should be used for comparison instead.
       @brief iterator pointing to beginning of properties of section "NO_SECTION"
       @return the iterator pointing to the first element
    */
    properties_iterator begin() const throw (excpt::SubException<NoSectionErr,PropertyReader>) { return begin(NO_SECTION); }

    /**
       @brief iterator pointing to end of properties
       @param s_it section-iterator of section
       @return the iterator pointing beyond last element
    */
    properties_iterator end(const sections_iterator & s_it) const throw (excpt::SubException<NoSectionErr,PropertyReader>){
      return properties_iterator(s_it->second->end());
    }
  
    /**
       For better performance the return value should be assigned to a local varibale which should be used for comparison instead.
       @brief iterator pointing to end of properties
       @param section name of section
       @return the iterator pointing beyond last element
    */
    properties_iterator end(const std::string section) const throw (excpt::SubException<NoSectionErr,PropertyReader>);

    /**
       For better performance the return value should be assigned to a local varibale which should be used for comparison instead.
       @brief iterator pointing to end of properties of section "NO_SECTION"
       @return the iterator pointing beyond end
    */
    properties_iterator end() const throw (excpt::SubException<NoSectionErr,PropertyReader>) { return end(NO_SECTION); }

    /**
       @brief iterator pointing to beginning of sections
       @return the iterator pointing to the first element
    */
    sections_iterator beginSections() const { return sections_iterator(sections.begin()); }

    /**
       @brief iterator pointing to end of sections
       @return the iterator pointing beyond last element
    */
    sections_iterator endSections() const { return sections_iterator(sections.end()); }

    /**
       @brief syntactic sugar for section "NO_SECTION"
    */
    std::string & operator[](const std::string & property) throw (excpt::SubException<NoSectionErr,PropertyReader>) { return (*sections[NO_SECTION])[property]; }

    /**
       @brief return a string-representation of all properties
       @return the string
    */
    std::string toString() const;

  };

}

#endif
