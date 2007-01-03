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
   @file propertyreader.cpp
   @author Friedemann Zintel
*/

#include <propertyreader.hpp>

using namespace std;
using namespace ds;
using namespace cmdl;
using namespace util;

string PropertyReader::NO_SECTION = "NO_SECTION";

PropertyReader::PropertyReader(const PropertyReader &propertyreader) : cmdlparser(0){

  filename = propertyreader.filename;
  Properties *properties;

  for ( Sections::const_iterator s_it = propertyreader.sections.begin(); s_it != propertyreader.sections.end(); s_it++ ){

    properties = new Properties();
    sections[s_it->first] = properties;

    for ( Properties::iterator p_it = s_it->second->begin(); p_it != s_it->second->end(); p_it++ )
      (*properties)[p_it->first] = p_it->second;

  }

  line = 0;

}

PropertyReader::~PropertyReader(){

  for (Sections::iterator it = sections.begin(); it != sections.end(); it++ )
    delete it->second;

}


void PropertyReader::read() throw (Exception<PropertyReader>,
				   SubException<OpenErr,PropertyReader>,
				   SubException<Parser::InputInvalidErr,Parser>,
				   SubException<Parser::IncompleteErr,Parser>,
				   SubException<Parser::SyntaxErr,Parser>,
				   SubException<Parser::NoValErr,Parser>,
				   SubException<Parser::NoIDErr,Parser>){

  ifstream file(filename.c_str());
  ostringstream lines;

  line = 0;

  if ( file == NULL )
    throw SubException<OpenErr,PropertyReader>("Error: could not open file " + filename + ": " + err());

  Parser parser(file,line);
  Property property;
  Properties *properties;
  string section;

  try {
    
    while ( true ){

      try {
	
	property = parser.nextKeyValuePair();

      } catch ( SubException<Parser::NoValErr,Parser> &se ){
	cout.flush();  // probably due to a bug in the compiler or libstdc++, whithout flushing the program will segfault
	continue;  // Leere Properties werden ignoriert
      }

      section = parser.getCurrentSection();

      Sections::iterator p_it = sections.find(section);

      if ( p_it == sections.end() ){

	properties = new Properties();
	sections[section] = properties;

      } else
	properties = p_it->second;
      
      (*properties)[property.first] = property.second;
    }

  } catch ( SubException<Parser::NoIDErr,Parser> &se ){
    lines << line;
    throw SubException<Parser::NoIDErr,Parser>("In file " + filename + ": No Id in line " + lines.str());
  } catch ( SubException<Parser::IncompleteErr,Parser> &se ){
    lines << line;
    throw SubException<Parser::IncompleteErr,Parser>("In file " + filename + ": Property incomplete in line " + lines.str());
  } catch ( SubException<Parser::SyntaxErr,Parser> &se ){
    lines << line;
    throw SubException<Parser::SyntaxErr,Parser>("In file " + filename + ": Syntax-Error in line " + lines.str());
  } catch ( SubException<Parser::EOFErr,Parser> &se ){

    if ( cmdlparser != 0 ){

      Sections::iterator p_it = sections.find(NO_SECTION);

      if ( p_it == sections.end() ){

	properties = new Properties();
	sections[NO_SECTION] = properties;

      } else
	properties = p_it->second;

      for (CmdlParser::Parameters::const_iterator it = cmdlparser->beginParameters(); it != cmdlparser->endParameters(); it++ )
	(*properties)[it->first] = it->second;

    }

    line = 0;

    return;
  }
}

void PropertyReader::modify() throw (Exception<PropertyReader>,
				     SubException<OpenErr,PropertyReader>,
				     SubException<Parser::InputInvalidErr,Parser>,
				     SubException<Parser::IncompleteErr,Parser>,
				     SubException<Parser::SyntaxErr,Parser>,
				     SubException<Parser::NoValErr,Parser>,
				     SubException<Parser::NoIDErr,Parser>){

  ifstream file(filename.c_str());
  ostringstream lines;
  ostringstream output;

  line = 0;

  if ( file == NULL )
    throw SubException<OpenErr,PropertyReader>("Error: could not open file " + filename + ": " + err());

  Parser parser(file,line);
  Property property;
  PropertyReader propertyreader(*this);
  string section, written_section="";
  ostream & store = output;

  try {

    // debugging
    // store << "# --- store start ---" << endl;

    while ( true ){

      try {

  	property = parser.nextKeyValuePairSaveComments(&store);

      } catch ( SubException<Parser::NoValErr,Parser> &se ){
  	cout.flush(); // probably due to a bug in the compiler or libstdc++, whithout flushing the program will segfault
  	continue;  // Leere Properties werden ignoriert
      }
 
      section = parser.getCurrentSection();

      Sections::iterator p_it = sections.find(section);

      // falls section nicht existiert, muss sie gelöscht werden
      if ( p_it == sections.end() )
	continue;

      if ( written_section != section ){

	// on new section: write remaining properties of old section
	if ( written_section != "" ){

	  PropertyReader::properties_iterator p_it = propertyreader.begin(written_section);
	  string temp_prop;
	  bool written_something = false;

	  while ( p_it != propertyreader.end(written_section) ){

	    store << p_it->first << '=' << p_it->second << endl;

	    temp_prop = p_it->first;
	    p_it++;
	    propertyreader.erase(written_section,temp_prop);
	    written_something = true;

	  }

	  if ( written_something == true )
	    store << endl;

	}

	// at starting of file, NO_SECTION should not be written
	if ( !(written_section == "" && section == NO_SECTION) )
	  store << '[' << section << ']' << endl;

	written_section = section;

      }
     
      // only if property is a member in the list write it out
      if ( propertyreader.isMember(section,property.first) == true ){

  	store << property.first << '=' << propertyreader.get(section,property.first) << endl;
	propertyreader.erase(section,property.first);

      }

    }

  } catch ( SubException<Parser::NoIDErr,Parser> &se ){
    lines << line;
    throw SubException<Parser::NoIDErr,Parser>("In file " + filename + ": No Id in line " + lines.str());
  } catch ( SubException<Parser::IncompleteErr,Parser> &se ){
    lines << line;
    throw SubException<Parser::IncompleteErr,Parser>("In file " + filename + ": Property incomplete in line " + lines.str());
  } catch ( SubException<Parser::SyntaxErr,Parser> &se ){
    lines << line;
    throw SubException<Parser::SyntaxErr,Parser>("In file " + filename + ": Syntax-Error in line " + lines.str());
  } catch ( SubException<Parser::EOFErr,Parser> &se ){

    file.close();

    // write all remaining sections and properties to file
    for ( Sections::iterator s_it = propertyreader.sections.begin(); s_it != propertyreader.sections.end(); s_it++ ){

      if ( s_it->second->empty() == true )
	continue;

      if ( (written_section != NO_SECTION && written_section != "") || (written_section != s_it->first && s_it->first != NO_SECTION) )
	store << '[' << s_it->first << ']' << endl;

      for ( PropertyReader::properties_iterator p_it = propertyreader.begin(s_it->first); p_it != propertyreader.end(s_it->first); p_it++ )
	store << p_it->first << '=' << p_it->second << endl;

      written_section = s_it->first;

    }

    // debugging
    // store << "# --- store end ---" << endl;

    ofstream modfile(filename.c_str());

    if ( modfile == NULL )
      throw SubException<OpenErr,PropertyReader>("Error: could not open file " + filename + " for writing: " + err());

    modfile << output.str();

    return;
  }
}

char PropertyReader::Parser::delimitors[] = {'\n','\r','\0'};
char PropertyReader::Parser::separators[] = {'=',':'};
char PropertyReader::Parser::blanks[] = {' ','\t'};
char PropertyReader::Parser::zero = '\0';
char PropertyReader::Parser::space = ' ';

bool PropertyReader::Parser::isDelimitor(char c){

  for ( unsigned int i = 0; i < sizeof(delimitors); i++ )
    if ( c == delimitors[i] )
      return true;

  return false;
}

bool PropertyReader::Parser::isSeparator(char c){

  for ( unsigned int i = 0; i < sizeof(separators); i++ )
    if ( c == separators[i] )
      return true;

  return false;
}

bool PropertyReader::Parser::isABlank(char c){

  for ( unsigned int i = 0; i < sizeof(blanks); i++ )
    if ( c == blanks[i] )
      return true;

  return false;
}

bool PropertyReader::Parser::skipSpaces(char &c){

  while ( true ){
    
    if ( isABlank(c) == false )
      return true;
    
    if ( input.eof() == true )
      return false;
    
    input.read(&c,1);

  }
}

void PropertyReader::Parser::skipRestOfLine(char &c) throw (SubException<EOFErr,Parser>){

  while ( true ){
	
    if ( input.eof() == true )
      throw SubException<EOFErr,Parser>();
	
    input.read(&c,1);

    if ( isDelimitor(c) ){  // Neue Zeile lesen bei Return
      return;
	  
    } else
      continue;

  }

}

void PropertyReader::Parser::overreadCommentline(char &c, ostream * comments) throw (SubException<EOFErr,Parser>){

  bool comment = true;

  // save comments if wanted
  if ( comments != 0 )
    *comments << c;
      
  while ( comment == true ){
	
    if ( input.eof() == true )
      throw SubException<EOFErr,Parser>();
	
    input.read(&c,1);

    // save comments if wanted
    if ( comments != 0 )
      *comments << c;
	
    if ( isDelimitor(c) ){  // Neue Zeile lesen bei Return
	  
      line++;
      comment = false;
      continue;
	  
    }
  }
}

void PropertyReader::Parser::readCurrentSection(char &c) throw (SubException<EOFErr,Parser>,SubException<SyntaxErr,Parser>){

  bool isSection = true;
  ostringstream section;

  while ( isSection == true ){
	
    if ( input.eof() == true )
      throw SubException<EOFErr,Parser>();
	
    input.read(&c,1);

    if ( c == ']' ){

      isSection = false;
      continue;

    } else if ( isDelimitor(c) )  // Neue Zeile lesen bei Return
      throw SubException<SyntaxErr,Parser>();

    section << c;

  }

  currentSection = ((String)section.str()).trunc();

  if ( currentSection == "" )
    currentSection = NO_SECTION;

  skipRestOfLine(c);

}

PropertyReader::Property PropertyReader::Parser::nextKeyValuePair() throw (SubException<InputInvalidErr,Parser>,
									   SubException<IncompleteErr,Parser>,
									   SubException<EOFErr,Parser>,
									   SubException<SyntaxErr,Parser>,
									   SubException<NoIDErr,Parser>,
									   SubException<NoValErr,Parser>){
  return nextKeyValuePairSaveComments(0);

}

PropertyReader::Property PropertyReader::Parser::nextKeyValuePairSaveComments(ostream *comments) throw (SubException<InputInvalidErr,Parser>,
													      SubException<IncompleteErr,Parser>,
													      SubException<EOFErr,Parser>,
													      SubException<SyntaxErr,Parser>,
													      SubException<NoIDErr,Parser>,
													      SubException<NoValErr,Parser>){

  Buffer<char> buffer(BLKSIZE);
  MemPointer<char> propptr,valueptr;
  bool eol = false; //End-Of-Line
  bool id,val;
  char c = 0;
  unsigned int spacecount = 0;  // zaehlt Leerzeichen

  id = val = false;

  line++;

  
  // overread comments, determine sections
  while ( true ){
    
    if ( input.eof() == true )
      throw SubException<EOFErr,Parser>();
    
    input.read(&c,1);

    // skip leading blanks
    if ( skipSpaces(c) == false )
      throw SubException<EOFErr,Parser>();

    // Kommentar
    if ( c == '#' ){

      overreadCommentline(c,comments);

    } else if ( c == '[' ){

      readCurrentSection(c);

    } else if ( isDelimitor(c) ){ // neue Zeile 

      if ( comments != 0 )
	*comments << endl;
      
      line++;
      continue;

    } else // kein Kommentar
      break;
  }
    
  while ( !eol ){
    
    // Property-ID einlesen
    while ( true ){
      
      if ( input.eof() == true )  // EOF zu frueh
	throw SubException<NoValErr,Parser>();
      
      if ( isSeparator(c) ){  // Separator ueberlesen -> Value einlesen
	
	if ( input.eof() )
	  throw SubException<NoValErr,Parser>();
	
	input.read(&c,1);
	
	break;

      } else if ( isDelimitor(c) ){  // darf noch nicht auftreten
	throw SubException<NoValErr,Parser>();

      } else if ( isABlank(c) == true ){  // nach Leerzeichen muss Separator kommen
	
	// Leerzeichen ueberlesen
	if ( skipSpaces(c) == false )
	  throw SubException<NoValErr,Parser>();
	
	if ( !isSeparator(c) )  // Separator muss folgen
	  throw SubException<SyntaxErr,Parser>();
	
	if ( input.eof() )
	  throw SubException<NoValErr,Parser>();
	
	input.read(&c,1);
      
	break;
	
      }
      else{ // normales Zeichen
	
	buffer.put(c);
	id = true;
	
      }
      
      input.read(&c,1);
    }
    
    if ( id == false )  // nichts gelesen
      throw SubException<NoIDErr,Parser>();

    buffer.put(zero); // fuer string '\0' anhaengen
    propptr = buffer.merge();
    buffer.clear();

    // fuehrende Leerzeichen ueberlesen
    if ( skipSpaces(c) == false )
      throw SubException<NoValErr,Parser>();
    
    // Value einlesen
    
    while ( true ){
      
      if ( isDelimitor(c) ){
	
	if ( val == true ){
	  
	  buffer.put(zero); // fuer string '\0' anhaengen
	  valueptr = buffer.merge();
	  break;

	} else  // nichts gelesen
	  throw SubException<NoValErr,Parser>();
	
	break;

      } else if ( isABlank(c) == true ){  // Leerzeichen
	spacecount++;  // abgehende Leerzeichen ueberlesen

      } else{  // normales Zeichen

	// Sind zwischendurch Leerzeichen aufgetreten, muessen diese eingefuegt werden
	if ( spacecount > 0 ){
	  for ( ; spacecount > 0; spacecount-- )
	    buffer.put(space);
	}

	buffer.put(c);
	val = true;

      }
      
      if ( input.eof() == true ){  // EOF -> return

	buffer.put(zero); // fuer string '\0' anhaengen
	valueptr = buffer.merge();

	break;

      }

      input.read(&c,1);
      
    }
    
    break;
    
  }

  return Property(string(propptr.get()),string(valueptr.get()));
}
 
void PropertyReader::set(const string & section, const string & property, const string & value){

  Properties *properties;
  Sections::iterator p_it = sections.find(section);

  if ( p_it == sections.end() ){

    properties = new Properties();
    sections[section] = properties;

  } else
    properties = p_it->second;

  (*properties)[property] = value;

}

std::string PropertyReader::get(const string section, const string property) throw (SubException<NoSectionErr,PropertyReader>){

  Sections::iterator properties;
  
  if ( (properties = sections.find(section)) == sections.end() )
    throw SubException<NoSectionErr,PropertyReader>(section);

  return (*properties->second)[property];

}

void PropertyReader::erase(const string section, const string property){ 

  Sections::iterator properties;
  
  if ( (properties = sections.find(section)) == sections.end() )
    return;

  properties->second->erase(property);

}

PropertyReader::properties_iterator PropertyReader::begin(const string section) throw (SubException<NoSectionErr,PropertyReader>){

  Sections::iterator properties;
  
  if ( (properties = sections.find(section)) == sections.end() )
    throw SubException<NoSectionErr,PropertyReader>(section);

  return properties_iterator(properties->second->begin());

}

PropertyReader::properties_iterator PropertyReader::end(const string section) throw (SubException<NoSectionErr,PropertyReader>){

  Sections::iterator properties;
  
  if ( (properties = sections.find(section)) == sections.end() )
    throw SubException<NoSectionErr,PropertyReader>(section);

  return properties_iterator(properties->second->end());

}

bool PropertyReader::isMember(const string section, string property){

  Sections::iterator properties;
  
  if ( (properties = sections.find(section)) == sections.end() )
    throw SubException<NoSectionErr,PropertyReader>(section);

  return ( properties->second->find(property) != properties->second->end() );
}

void PropertyReader::write() throw (Exception<PropertyReader>){

  ofstream file(filename.c_str());
  
  file << toString();

}

void PropertyReader::clear(){

  for ( Sections::iterator it = sections.begin() ; it != sections.end() ; it++ )
    delete it->second;

  sections.clear();

}

void PropertyReader::show(){

  cout << toString();

}

string PropertyReader::toString(){

  ostringstream outstring;

  PropertyReader::properties_iterator start = begin();
  PropertyReader::properties_iterator stop = end();

  for ( PropertyReader::properties_iterator it = start; it != stop; it++ )
    outstring << it->first << '=' << it->second << endl;

  for ( PropertyReader::sections_iterator s_it = beginSections(); s_it != endSections(); s_it++ ){

    if ( s_it->first == NO_SECTION )
      continue;

    outstring << endl << '[' << s_it->first << ']' << endl;

    for ( PropertyReader::properties_iterator p_it = begin(s_it); p_it != end(s_it); p_it++ )
      outstring << p_it->first << '=' << p_it->second << endl;

  }

  return outstring.str();

}
