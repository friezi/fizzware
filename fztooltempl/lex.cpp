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

const unsigned char LexCharClasses::FL_EOL = 0;
const unsigned char LexCharClasses::FL_WHITESPACE = 1;
const unsigned char LexCharClasses::FL_INTRODUCING_WORD = 2;
const unsigned char LexCharClasses::FL_WORD_CONSTITUENT = 3;
const unsigned char LexCharClasses::FL_INTRODUCING_NUMBER = 4;
const unsigned char LexCharClasses::FL_NUMBER_CONSTITUENT = 5;
const unsigned char LexCharClasses::FL_SIGN = 6;

const String LexCharClasses::eol_chars = "\n\r";

const int LexToken::TT_EOL = 10;
const int LexToken::TT_NONE = -1;
const int LexToken::TT_EOF = -2;
const int LexToken::TT_NUMBER = -3;
const int LexToken::TT_WORD = -4;
const int LexToken::TT_WHITE = -5;

LexCharClasses::LexCharClasses(){

  resetSyntax();
  setDefaultSyntax();

}

void LexCharClasses::resetSyntax(){

  for ( int i = 0; i < LEX_AT_SIZE; i++ )
    ascii_table[i] = 0;

}

void LexCharClasses::setBaseSyntax(){

   setWhitespaces(" \t");
   setEOLs(eol_chars);
   setSigns("+-");
   setWordConstituents('a','z');
   setWordConstituents('A','Z');
   setWordConstituents('_','_');
   setIntroducingWord('a','z');
   setIntroducingWord('A','Z');
   setIntroducingWord('_','_');
   setNumberConstituents("1234567890");
   setIntroducingNumber("1234567890");

}

void LexCharClasses::setDefaultSyntax(){

  setBaseSyntax();

  block_comment_start = "/*";
  block_comment_stop = "*/";
  line_comment = "//";
  quote_start = '"';
  quote_stop = '"';
  setWordConstituents("1234567890");

}

void LexCharClasses::setWordConstituents(const string constituents){

  for ( String::const_iterator it = constituents.begin(); it != constituents.end(); it++ )
    setFlag(*it,FL_WORD_CONSTITUENT);

}

void LexCharClasses::setWordConstituents(const char from, const char to){

  for ( unsigned char i = (unsigned char)from; i <= (unsigned char)to ; i++ )
    setFlag(i,FL_WORD_CONSTITUENT);

}

void LexCharClasses::setIntroducingWord(const string intro){

  for ( String::const_iterator it = intro.begin(); it != intro.end(); it++ )
    setFlag(*it,FL_INTRODUCING_WORD);

}

void LexCharClasses::setIntroducingWord(const char from, const char to){

  for ( unsigned char i = (unsigned char)from; i <= (unsigned char)to ; i++ )
    setFlag(i,FL_INTRODUCING_WORD);

}

void LexCharClasses::setIntroducingNumber(const string intro){

  for ( String::const_iterator it = intro.begin(); it != intro.end(); it++ )
    setFlag(*it,FL_INTRODUCING_NUMBER);

}

void LexCharClasses::setNumberConstituents(const string constituents){

  for ( String::const_iterator it = constituents.begin(); it != constituents.end(); it++ )
    setFlag(*it,FL_NUMBER_CONSTITUENT);

}

void LexCharClasses::setWhitespaces(const string whitespaces){

  for ( String::const_iterator it = whitespaces.begin(); it != whitespaces.end(); it++ )
    setFlag(*it,FL_WHITESPACE);

}

void LexCharClasses::setSigns(const string signs){

  for ( String::const_iterator it = signs.begin(); it != signs.end(); it++ )
    setFlag(*it,FL_SIGN);

}

void LexCharClasses::setOrdinaries(const string ordinaries){

  for ( String::const_iterator it = ordinaries.begin(); it != ordinaries.end(); it++ )
    ascii_table[(unsigned char)(*it)] = 0;

}

void LexCharClasses::setOrdinaries(const char from, const char to){

  for ( unsigned char i = (unsigned char)from; i <= (unsigned char)to ; i++ )
    ascii_table[i] = 0;

}

void LexCharClasses::setEOLs(const string eols){

  for ( String::const_iterator it = eols.begin(); it != eols.end(); it++ )
    setFlag(*it,FL_EOL);

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
  
  while ( true ){

    input->get(c);

    if ( input->eof() == true )
      break;

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
	
  if ( LexCharClasses::isLF(c) )
    lines++;
  else if ( LexCharClasses::isCR(c) ){

    lines++;

    input->get(c);

    if ( input->eof() == false )
      if ( LexCharClasses::isLF(c) == false )
	input->putback(c);
	  
  }

  return lines;

}

bool LexScreener::screen(){

  bool whitespace_mode = false;
  bool block_comment_mode = false;
  bool line_comment_mode = false;
  bool first_char = true;
  unsigned long nesting_counter = 0;

  char c;
  bool skipped = false;

  skipped_lines = 0;

  while ( true ){

    input->get(c);

    if ( input->eof() == true )
      break;

    if ( lower_case_mode == true )
      c = String::latinToLower(c);

    if ( first_char == true && isWhitespace(c) ){

      skipped = true;
      first_char = false;
      whitespace_mode = true;
      continue;

    }
    
    first_char = false;
    
    if ( block_comment_mode == false && line_comment_mode == false ){
      
      if ( isWhitespace(c) ){

	skipped = true;
	continue;

      }
      
      else if ( skipIfIntroducingLineComment(c) ){
	
	skipped = true;
	line_comment_mode = true;
	continue;
	
      } else if ( skipIfIntroducingBlockComment(c) ){
	
	skipped = true;
	block_comment_mode = true;
	
	if ( isNestedComments() == true )
	  nesting_counter++;
	
	continue;
      
      } else {
      
	input->putback(c);
	break;

      }
      
    } else if ( block_comment_mode == true ){
      
      if ( skipIfTerminatingBlockComment(c) ){

	if ( isNestedComments() )
	  nesting_counter--;
	
	if ( nesting_counter == 0 )
	  block_comment_mode = false;
	
	if ( block_comment_mode == false && whitespace_mode == false )
	  break;
	
      } else if ( isNestedComments() ){
	
	if ( skipIfIntroducingBlockComment(c) )
	  nesting_counter++;
	else
	  skipped_lines += skipIfReturn(input,c);
	
      } else
	skipped_lines += skipIfReturn(input,c);
      
    } else if ( line_comment_mode == true ){
      
      if ( isEOL(c) == true ){

	input->putback(c);
	break;

      }
      
    }
    
  }

  return skipped;
  
}

LexScanner::LexScanner(std::istream * input) : input(input), line_number(1), lower_case_mode(false), report_eol(false),
					       report_white(false), token_putback(false), parse_numbers(false),
					       signed_numbers(false), floating_points(false){

  screener = new LexScreener(input);

  screener->setLexCharClasses(&char_classes);
  
  char_classes.setDefaultSyntax();

}

LexScanner::~LexScanner(){

  delete screener;

}

void LexScanner::lowerCaseMode(){

  lower_case_mode = true;
  screener->setLowerCaseMode(true);

}
void LexScanner::useFloatingpoints(){
  
  floating_points = true;
  char_classes.setNumberConstituents(".");
  
}

void LexScanner::putback(){

  if ( token_putback == false ){

    LexToken tmp_token;
    
    token_putback = true;
    tmp_token = token;
    token = saved_token;
    saved_token = tmp_token;
    
  }
  
}

int LexScanner::nextToken() throw (Exception<LexScanner>){

  String thisMethod = "nextToken()";

  // specifies if a character that matches treated
  // as a normal character although it matches the beginning of a comment
  bool treat_normal = false;
  bool escape_next = false;

  char c = 0;
  string number = "";
  bool quote_mode = false;

  if ( token_putback == true ){
 
    LexToken tmp_token;
    
    token_putback = false;
    tmp_token = token;
    token = saved_token;
    saved_token = tmp_token;

    return token.type;
    
  }
  
  saved_token = token;    

  token.sval = "";
  token.nval = 0;
  token.type = LexToken::TT_NONE;

  token.token_line_number = line_number;

  while ( true ){
    
    input->get(c);
    
    if ( input->eof() == true )
      break;

    if ( escape_next == true ){

      escape_next = false;
      treat_normal = true;

    }

    if ( isLowerCaseMode() )
      c = String::latinToLower(c);

    if ( isEscape(c) && treat_normal == false ){

      escape_next = true;
      continue;

    }

    if ( token.type == LexToken::TT_WORD ){

      if ( quote_mode == true ){

	if ( isTerminatingQuote(c) && treat_normal == false )
	  return token.type;

	else if ( isEOL(c) and treat_normal == false ){

	  input->putback(c);
	  break;

	} else{

	  if ( isEOL(c) ){

	    treat_normal = false;
	    line_number += LexScreener::skipIfReturn(input,c);
	    continue;

	  }
	  
	  treat_normal = false;
	  token.sval += c;

	}

      } else if ( !isIntroducingWord(c) && !isWordConstituent(c) ){

	if ( (isIntroducingBlockComment(c) || isIntroducingLineComment(c)) && treat_normal == false ){

	  input->putback(c);
	  treat_normal = !screener->screen();
	  line_number += screener->getSkippedLines();
	  
	  
	} else {
	  
	  treat_normal = false;
	  input->putback(c);
	  return token.type;

	}
	
      } else 
	// belonging to word
	token.sval += c;
      
    } else if ( token.type == LexToken::TT_NUMBER ){

      if ( isNumberConstituent(c) )
	number += c;
      
      else if ( (isIntroducingBlockComment(c) || isIntroducingLineComment(c)) && treat_normal == false ){
	
	input->putback(c);
	treat_normal = !screener->screen();
	line_number += screener->getSkippedLines();
	token.token_line_number = line_number;
	
      } else {
	
	treat_normal = false;
	input->putback(c);
	
	if ( isParseNumbers() )
	  token.nval = atof(number.c_str());
	
	else {
	  
	  token.type = LexToken::TT_WORD;
	  token.sval = number;

	}

	return token.type;
	
      }

    } else if ( token.type == LexToken::TT_NONE ){

      if ( (isIntroducingBlockComment(c) || isIntroducingLineComment(c)) && treat_normal == false ){

	input->putback(c);
	treat_normal = !screener->screen();
	line_number += screener->getSkippedLines();
	token.token_line_number = line_number;

	input->get(c);
	
	if ( input->eof() == true ){
	  
	  token.type = LexToken::TT_EOF;
	  return token.type;
	  
	}

	if ( isLowerCaseMode() )
	  c = String::latinToLower(c);
	
      } else
	treat_normal = false;

      if ( screener->isWhitespace(c) ){
    
	treat_normal = !screener->screen();
	line_number += screener->getSkippedLines();

	if ( isReportWhite() ){

	  token.type = LexToken::TT_WHITE;
	  return token.type;

	}

	token.token_line_number = line_number;
	
	if ( input->eof() == true )
	  break;
    
      } else if ( isIntroducingWord(c) ){

	token.sval += c;
	token.type = LexToken::TT_WORD;

      } else if ( isIntroducingNumber(c) ){

	number += c;
	token.type = LexToken::TT_NUMBER;

      } else if ( isUseSignedNumbers() && isSign(c) &&input->eof() == false ){
	// check for sign

	char next;
	input->get(next);
	input->putback(next);

	if ( isIntroducingNumber(next) ){

	  number += c;
	  token.type = LexToken::TT_NUMBER;

	} else{

	  token.type = c;
	  return token.type;

	}

      } else if ( isIntroducingQuote(c) ){
	
	quote_mode = true;
	token.type = LexToken::TT_WORD;      
	
      } else if ( isEOL(c) ){
	  
	line_number += LexScreener::skipIfReturn(input,c);
	
	if ( isReportEOL() ){
	  
	  token.type = LexToken::TT_EOL;
	  return token.type;
	  
	}
	
	token.token_line_number = line_number;
	
      } else {
	
	treat_normal = false;
	token.type = c;
	return token.type;
    
      }

    }

  }
  // EOF

  if ( quote_mode == true ){

    ostringstream liness;
    liness << line_number;
    throw Exception<LexScanner>(thisMethod + ": unterminated quote in line " + liness.str() + "!");

  }

  if ( token.type == LexToken::TT_NUMBER ){

    if ( isParseNumbers() )
      token.nval = atof(number.c_str());

    else {

      token.type = LexToken::TT_WORD;
      token.sval = number;
      
    }
    
  } else if ( token.type == LexToken::TT_NONE )
    token.type = LexToken::TT_EOF;
  
  return token.type;
  
}

int LexScanner::nextLine() throw (Exception<LexScanner>){

  char c = 0;
  bool escape_next = false;

  // specifies if a character that matches treated
  // as a normal character although it matches the beginning of a comment
  bool treat_normal = false;

  if ( token_putback == true ){
 
    LexToken tmp_token;
    
    token_putback = false;
    tmp_token = token;
    token = saved_token;
    saved_token = tmp_token;

    return token.type;
    
  }
  
  saved_token = token;    

  token.sval = "";
  token.nval = 0;
  token.type = LexToken::TT_NONE;

  token.token_line_number = line_number;

  while ( true ){

    input->get(c);

    if ( input->eof() == true )
      break;

    if ( escape_next == true ){

      escape_next = false;
      treat_normal = true;

    }
  
    if ( token.type == LexToken::TT_NONE ){

      if ( (isIntroducingBlockComment(c) || isIntroducingLineComment(c)) && treat_normal == false ){

	input->putback(c);
	treat_normal = !screener->screen();
	line_number += screener->getSkippedLines();

	input->get(c);
	
	if ( input->eof() == true )
	  break;
	  
	if ( isLowerCaseMode() )
	  c = String::latinToLower(c);
	
      }

      if ( screener->isWhitespace(c) ){
    
	treat_normal = !screener->screen();
	line_number += screener->getSkippedLines();
    
	if ( input->eof() == true )
	  break;

    
      } else if ( isEOL(c) ){

	line_number += LexScreener::skipIfReturn(input,c);	
	token.token_line_number = line_number;

      } else {

	token.type = LexToken::TT_WORD;

	if ( isLowerCaseMode() )
	  c = String::latinToLower(c);

	token.sval += c;
	treat_normal = false;
      
      }

    } else {
      // found an unscreenable character
      
      if ( isEscape(c) && treat_normal == false ){

	escape_next = true;
	continue;
	
      }
      
      if ( (isIntroducingBlockComment(c) || isIntroducingLineComment(c)) && treat_normal == false ){

	input->putback(c);
	treat_normal = !screener->screen();
	line_number += screener->getSkippedLines();
	
      } else if ( isEOL(c) && treat_normal == false ){

	input->putback(c);
	break;
      
      } else {

	if ( isEOL(c) ){

	  treat_normal = false;
	  line_number += LexScreener::skipIfReturn(input,c);
	  continue;

	}

	if ( isLowerCaseMode() )
	  c = String::latinToLower(c);

	token.sval += c;
	treat_normal = false;
	
      }

    }
      
  }
  
  if ( token.type == LexToken::TT_NONE )
    token.type = LexToken::TT_EOF;
  
  return token.type;
  
}

String LexToken::typeToString() const {
  
  switch ( type ){

  case TT_EOF:

    return String("EOF");
    break;

  case TT_EOL:

    return String("EOL");
    break;

  case TT_NUMBER:
    
    return String("NUMBER");
    break;

  case TT_WORD:
    
    return String("WORD");
    break;

  case TT_NONE:

    return String("NONE");
    break;

  case TT_WHITE:

    return String("WHITE");
    break;

  default:

    return String("ORDINARY");
    break;

  }

}

String LexToken::toString() const {

  String thisMethod = "tokenToString()";

  ostringstream tsstream;

  tsstream << "[" << "'";

  switch ( type ){

  case TT_WORD:

    tsstream << sval;
    break;

  case TT_NUMBER:

    tsstream << nval;
    break;

  case TT_NONE:
  case TT_EOF:
  case TT_EOL:
  case TT_WHITE:

    break;

  default:

    tsstream << (char)type;
    break;

  }

  tsstream << "'" << "," << typeToString() << "," << token_line_number << "]";

  return tsstream.str();

}
