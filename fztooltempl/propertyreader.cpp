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

PropertyReader::PropertyReader(const PropertyReader &propertyreader) : cmdlparser(0){

  filename = propertyreader.filename;
  properties = propertyreader.properties;
  line = 0;

}

void PropertyReader::read() throw (Exception<PropertyReader>,
				   SubException<OpenErr,PropertyReader>,
				   SubException<Scanner::InputInvalidErr,Scanner>,
				   SubException<Scanner::IncompleteErr,Scanner>,
				   SubException<Scanner::SyntaxErr,Scanner>,
				   SubException<Scanner::NoValErr,Scanner>,
				   SubException<Scanner::NoIDErr,Scanner>){

  ifstream file(filename.c_str());
  ostringstream lines;

  line = 0;

  if ( file == NULL )
    throw SubException<OpenErr,PropertyReader>("Error: could not open file " + filename + ": " + err());

  Scanner scanner(file,line);
  Property property;

  try {
    
    while ( true ){

      try {
	
	property = scanner.nextTokenpair();

      } catch ( SubException<Scanner::NoValErr,Scanner> &se ){
	cout.flush();  // probably due to a bug in the compiler or libstdc++, whithout flushing the program will segfault
	continue;  // Leere Properties werden ignoriert
      }
      
      properties[property.first] = property.second;
    }

  } catch ( SubException<Scanner::NoIDErr,Scanner> &se ){
    lines << line;
    throw SubException<Scanner::NoIDErr,Scanner>("In file " + filename + ": No Id in line " + lines.str());
  } catch ( SubException<Scanner::IncompleteErr,Scanner> &se ){
    lines << line;
    throw SubException<Scanner::IncompleteErr,Scanner>("In file " + filename + ": Property incomplete in line " + lines.str());
  } catch ( SubException<Scanner::SyntaxErr,Scanner> &se ){
    lines << line;
    throw SubException<Scanner::SyntaxErr,Scanner>("In file " + filename + ": Syntax-Error in line " + lines.str());
  } catch ( SubException<Scanner::EOFErr,Scanner> &se ){

    if ( cmdlparser != 0 ){

      for (CmdlParser::Parameters::const_iterator it = cmdlparser->beginParameters(); it != cmdlparser->endParameters(); it++ )
	properties[(*it).first] = (*it).second;

    }

    return;
  }
}

void PropertyReader::modify() throw (Exception<PropertyReader>,
				     SubException<OpenErr,PropertyReader>,
				     SubException<Scanner::InputInvalidErr,Scanner>,
				     SubException<Scanner::IncompleteErr,Scanner>,
				     SubException<Scanner::SyntaxErr,Scanner>,
				     SubException<Scanner::NoValErr,Scanner>,
				     SubException<Scanner::NoIDErr,Scanner>){

  ifstream file(filename.c_str());
  ostringstream lines;
  ostringstream output;

  line = 0;

  if ( file == NULL )
    throw SubException<OpenErr,PropertyReader>("Error: could not open file " + filename + ": " + err());

  Scanner scanner(file,line);
  Property property;
  PropertyReader propertyreader(*this);

  try {
    
    while ( true ){

      try {
	
	property = scanner.nextTokenpairSaveComments(&output);

      } catch ( SubException<Scanner::NoValErr,Scanner> &se ){
	cout.flush(); // probably due to a bug in the compiler or libstdc++, whithout flushing the program will segfault
	continue;  // Leere Properties werden ignoriert
      }
      
      // only if property is a member in the list write it out
      if ( propertyreader.isMember(property.first) == true ){

	output << property.first << " = " << propertyreader.get(property.first) << endl;
	propertyreader.erase(property.first);

      }

    }

  } catch ( SubException<Scanner::NoIDErr,Scanner> &se ){
    lines << line;
    throw SubException<Scanner::NoIDErr,Scanner>("In file " + filename + ": No Id in line " + lines.str());
  } catch ( SubException<Scanner::IncompleteErr,Scanner> &se ){
    lines << line;
    throw SubException<Scanner::IncompleteErr,Scanner>("In file " + filename + ": Property incomplete in line " + lines.str());
  } catch ( SubException<Scanner::SyntaxErr,Scanner> &se ){
    lines << line;
    throw SubException<Scanner::SyntaxErr,Scanner>("In file " + filename + ": Syntax-Error in line " + lines.str());
  } catch ( SubException<Scanner::EOFErr,Scanner> &se ){

    file.close();
    ofstream modfile(filename.c_str());

    if ( modfile == NULL )
      throw SubException<OpenErr,PropertyReader>("Error: could not open file " + filename + " for writing: " + err());

    modfile << output.str();

    // write all remaining properties to file
    for ( PropertyReader::iterator it = propertyreader.begin(); it != propertyreader.end(); it++ )
      modfile << (*it).first << " = " << (*it).second << endl;

    return;
  }
}

char PropertyReader::Scanner::delimitors[] = {'\n','\0'};
char PropertyReader::Scanner::separators[] = {'=',':'};
char PropertyReader::Scanner::zero = '\0';
char PropertyReader::Scanner::space = ' ';

bool PropertyReader::Scanner::isDelimitor(char c){

  for ( unsigned int i = 0; i < sizeof(delimitors); i++ )
    if ( c == delimitors[i] )
      return true;

  return false;
}

bool PropertyReader::Scanner::isSeparator(char c){

  for ( unsigned int i = 0; i < sizeof(separators); i++ )
    if ( c == separators[i] )
      return true;

  return false;
}

bool PropertyReader::Scanner::skipSpaces(char &c){

  while ( true ){
    
    if ( c != space )
      return true;
    
    if ( input.eof() == true )
      return false;
    
    input.read(&c,1);

  }
}

PropertyReader::Property PropertyReader::Scanner::nextTokenpair() throw (SubException<InputInvalidErr,Scanner>,
									 SubException<IncompleteErr,Scanner>,
									 SubException<EOFErr,Scanner>,
									 SubException<SyntaxErr,Scanner>,
									 SubException<NoIDErr,Scanner>,
									 SubException<NoValErr,Scanner>){
  return nextTokenpairSaveComments(0);
}

PropertyReader::Property PropertyReader::Scanner::nextTokenpairSaveComments(ostringstream *comments) throw (SubException<InputInvalidErr,Scanner>,
													    SubException<IncompleteErr,Scanner>,
													    SubException<EOFErr,Scanner>,
													    SubException<SyntaxErr,Scanner>,
													    SubException<NoIDErr,Scanner>,
													    SubException<NoValErr,Scanner>){

  Buffer<char> buffer(BLKSIZE);
  MemPointer<char> propptr,valueptr;
  bool eol = false; //End-Of-Line
  bool comment = false;
  bool id,val;
  char c = 0;
  unsigned int spacecount = 0;  // zaehlt Leerzeichen

  id = val = false;

  line++;

  
  // Kommentare ueberlesen
  while ( true ){
    
    if ( input.eof() == true )
      throw SubException<EOFErr,Scanner>();
    
    input.read(&c,1);

    // fuehrende Leerzeichen ueberlesen
    if ( skipSpaces(c) == false )
      throw SubException<EOFErr,Scanner>();

    // Kommentar
    if ( c == '#' ){
      comment = true;

      // save comments if wanted
      if ( comments != 0 )
	*comments << c;
      
      while ( comment == true ){
	
	if ( input.eof() == true )
	  throw SubException<EOFErr,Scanner>();
	
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
	throw SubException<NoValErr,Scanner>();
      
      if ( isSeparator(c) ){  // Separator ueberlesen -> Value einlesen
	
	if ( input.eof() )
	  throw SubException<NoValErr,Scanner>();
	
	input.read(&c,1);
	
	break;
      } else if ( isDelimitor(c) ){  // darf noch nicht auftreten
	throw SubException<NoValErr,Scanner>();

      } else if ( c == space ){  // nach Leerzeichen muss Separator kommen
	
	// Leerzeichen ueberlesen
	if ( skipSpaces(c) == false )
	  throw SubException<NoValErr,Scanner>();
	
	if ( !isSeparator(c) )  // Separator muss folgen
	  throw SubException<SyntaxErr,Scanner>();
	
	if ( input.eof() )
	  throw SubException<NoValErr,Scanner>();
	
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
      throw SubException<NoIDErr,Scanner>();

    buffer.put(zero); // fuer string '\0' anhaengen
    propptr = buffer.merge();
    buffer.clear();

    // fuehrende Leerzeichen ueberlesen
    if ( skipSpaces(c) == false )
      throw SubException<NoValErr,Scanner>();
    
    // Value einlesen
    
    while ( true ){
      
      if ( isDelimitor(c) ){
	
	if ( val == true ){

	  buffer.put(zero); // fuer string '\0' anhaengen
	  valueptr = buffer.merge();
	  break;

	}
	
	else{  // nichts gelesen
// 	  free(propptr);
	  throw SubException<NoValErr,Scanner>();
	}

	break;

      } else if ( c == space ){  // Leerzeichen
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
 
bool PropertyReader::isMember(std::string property){

  return ( properties.find(property) != properties.end() );
}

void PropertyReader::write() throw (Exception<PropertyReader>){

  ofstream file(filename.c_str());

  for ( PropertyReader::iterator it = begin(); it != end(); it++ )
    file << (*it).first << " = " << (*it).second << endl;
}

void PropertyReader::clear(){

  properties.clear();
  line = 0;
}

void PropertyReader::show(){

  for ( PropertyReader::iterator it = begin(); it != end(); it++ )
    cout << (*it).first << " = " << (*it).second << endl;
}

