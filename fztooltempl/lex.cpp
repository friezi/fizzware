/*
  Copyright (C) 1999-2007 Friedemann Zintel

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

#include <lex.hpp>

using namespace std;
using namespace lex;
using namespace utils;

const String LexCharClasses::return_chars = "\n\r";

const int LexScanner::TT_EOF = -1;
const int LexScanner::TT_EOL = 10;
const int LexScanner::TT_NUMBER = -2;
const int LexScanner::TT_WORD = -3;
const int LexScanner::TT_NONE = -4;

void LexCharClasses::setDefaults(){

  block_comment_start = "/*";
  block_comment_stop = "*/";
  line_comment = "//";
  white_spaces = " \t";
  quote_start = '"';
  quote_stop = '"';
  word_constituents = "";

}

bool LexScreener::skipIfMatchingWord(char c, const String & word){

  String::const_iterator it = word.begin();

  if ( it == word.end() )
    return false;

  if ( *it != c )
    return false;
  
  ++it;
  
  if ( it == word.end() )
    return true;
  
  while ( input->eof() == false ){

    input->get(c);

    if ( lower_case_mode == true )
      c = String::latinToLower(c);
    
    if ( *it != c ){

      input->putback(c);
      break;

    }
    
    if ( ++it == word.end() )
      return true;
    
  }
  
  --it;
  
  while ( it != word.begin() )
    input->putback(*it--);
  
  return false;

}

unsigned long LexScreener::skipIfReturn(istream * input, char c){

  unsigned long lines = 0;
	
  if ( LexCharClasses::isLF(c) == true )
    lines++;
  else if ( LexCharClasses::isCR(c) == true ){

    lines++;

    if ( input->eof() == false ){

      input->get(c);
	    
      if ( LexCharClasses::isLF(c) == false )
	input->putback(c);

    }
	  
  }

  return lines;

}

unsigned long LexScreener::screen(){

  bool whitespace_mode = false;
  bool block_comment_mode = false;
  bool line_comment_mode = false;
  bool first_char = true;
  unsigned long nesting_counter = 0;
  unsigned long lines = 0;

  char c;

  while ( input->eof() == false ){

    input->get(c);

    if ( lower_case_mode == true )
      c = String::latinToLower(c);

    if ( first_char == true && isWhitespace(c) ){

      first_char = false;
      whitespace_mode = true;
      continue;

    }
    
    first_char = false;
    
    if ( block_comment_mode == false && line_comment_mode == false ){
      
      if ( isWhitespace(c) )
	continue;
      
      else if ( skipIfIntroducingLineComment(c) ){
	
	line_comment_mode = true;
	continue;
	
      } else if ( skipIfIntroducingBlockComment(c) ){
	
	block_comment_mode = true;
	
	if ( isNestedComments() == true )
	  nesting_counter++;
	
	continue;
      
      } else {
      
	input->putback(c);
	return lines;

      }
      
    } else if ( block_comment_mode == true ){
      
      if ( skipIfTerminatingBlockComment(c) ){

	if ( isNestedComments() )
	  nesting_counter--;
	
	if ( nesting_counter == 0 )
	  block_comment_mode = false;
	
	if ( block_comment_mode == false && whitespace_mode == false )
	  return lines;
	
      } else if ( isNestedComments() ){
	
	if ( skipIfIntroducingBlockComment(c) )
	  nesting_counter++;
	else
	  lines += skipIfReturn(input,c);
	
      } else
	lines += skipIfReturn(input,c);
      
    } else if ( line_comment_mode == true ){
      
      if ( isReturn(c) == true ){

	input->putback(c);
	return lines;

      }
      
    }
    
  }
  
  return lines;
  
}

LexScanner::LexScanner(std::istream * input) : input(input), line_number(1), token_line_number(1), lower_case_mode(false), report_return(false),
					       sval(""), nval(0), ttype(TT_NONE), saved_sval(""), saved_nval(0), saved_ttype(TT_NONE){

  screener = new LexScreener(input);

  screener->setLexCharClasses(&char_classes);
  
  char_classes.setDefaults();

}

LexScanner::~LexScanner(){

  delete screener;

}

void LexScanner::lowerCaseMode(){

  lower_case_mode = true;
  screener->setLowerCaseMode(true);

}

int LexScanner::nextToken() throw (Exception<LexScanner>){

  String thisMethod = "nextToken()";

  char c;
  string number = "";
  bool quote_mode = false;

  saved_sval = sval;
  saved_nval = nval;
  saved_ttype = ttype;

  sval = "";
  nval = 0;
  ttype = TT_NONE;

  token_line_number = line_number;

  while ( input->eof() == false ){

    input->get(c);

    if ( isLowerCaseMode() )
      c = String::latinToLower(c);

    if ( ttype == TT_WORD ){

      if ( quote_mode == true ){

	if ( isTerminatingQuote(c) )
	  return ttype;
	else
	  sval += c;

      } else if ( !isIntroducingWord(c) && !isWordConstituent(c) ){

	if ( isIntroducingBlockComment(c) || isIntroducingLineComment(c) ){

	  input->putback(c);
	  line_number += screener->screen();
	  
	  
	} else {
	  
	  input->putback(c);
	  return ttype;

	}
	
      } else 
	// belonging to word
	sval += c;
      
    } else if ( ttype == TT_NUMBER ){

      if ( isIntroducingNumber(c) || isNumberConstituent(c) )
	number += c;

      else if ( isIntroducingBlockComment(c) || isIntroducingLineComment(c) ){
	
	input->putback(c);
	line_number += screener->screen();
	token_line_number = line_number;
	
      } else {
	
	input->putback(c);
	nval = atof(number.c_str());
	return ttype;
	
      }

    } else if ( ttype == TT_NONE ){

      if ( isIntroducingBlockComment(c) || isIntroducingLineComment(c) ){

	input->putback(c);
	line_number += screener->screen();
	
	if ( input->eof() == true ){
	  
	  ttype = TT_EOF;
	  return ttype;
	  
	}

	input->get(c);

	if ( isLowerCaseMode() )
	  c = String::latinToLower(c);
	
      }

      if ( screener->isWhitespace(c) ){
    
	line_number += screener->screen();
    
	if ( input->eof() == true ){
      
	  ttype = TT_EOF;
	  return ttype;
      
	}
    
	input->get(c);

	if ( isLowerCaseMode() )
	  c = String::latinToLower(c);
    
      }
  
      if ( isIntroducingWord(c) ){

	sval += c;
	ttype = TT_WORD;

      } else if ( isIntroducingNumber(c) ){

	number += c;
	ttype = TT_NUMBER;

      } else if ( isIntroducingQuote(c) ){
    
	if ( input->eof() == true ){
      
	  ttype = c;
	  return ttype;
      
	} else {
      
	  quote_mode = true;
	  ttype = TT_WORD;
      
	}

      } else if ( isReturn(c) ){

	line_number += LexScreener::skipIfReturn(input,c);

	if ( isReportEOL() == true ){

	  ttype = TT_EOL;
	  return ttype;

	}

	token_line_number = line_number;
    
      } else {
    
	ttype = c;
	return ttype;
    
      }

    }

  }

  if ( quote_mode == true ){

    ostringstream liness;
    liness << line_number;
    throw Exception<LexScanner>(thisMethod + ": unterminated quote in line " + liness.str() + "!");

  }

  if ( ttype == TT_NUMBER )
    nval = atof(number.c_str());
  else if ( ttype == TT_NONE )
    ttype = TT_EOF;
    
  return ttype;

}

String LexScanner::ttypeToString() const {

  switch ( ttype ){

  case TT_EOF:

    return String("EOF");
    break;

  case TT_EOL:

    return String("EOL");
    break;

  case TT_NUMBER:
    
    return String("number");
    break;

  case TT_WORD:
    
    return String("word");
    break;

  case TT_NONE:

    return String("none");
    break;

  default:

    return String("operator");
    break;

  }

}

String LexScanner::tokenToString() const {

  String thisMethod = "tokenToString()";

  ostringstream tsstream;

  tsstream << "[" << "'";

  switch ( ttype ){

  case TT_WORD:

    tsstream << sval;
    break;

  case TT_NUMBER:

    tsstream << nval;
    break;

  case TT_NONE:
  case TT_EOF:
  case TT_EOL:

    break;

  default:

    tsstream << (char)ttype;
    break;

  }

  tsstream << "'" << "," << ttypeToString() << "," << token_line_number << "]";

  return tsstream.str();

}
