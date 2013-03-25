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

#include <fztooltempl/lex.hpp>

using namespace std;
using namespace exc;
using namespace parse;
using namespace utils;

const unsigned char LexCharClasses::FL_EOL = 0;
const unsigned char LexCharClasses::FL_WHITESPACE = 1;
const unsigned char LexCharClasses::FL_INTRODUCING_WORD = 2;
const unsigned char LexCharClasses::FL_WORD_CONSTITUENT = 3;
const unsigned char LexCharClasses::FL_INTRODUCING_NUMBER = 4;
const unsigned char LexCharClasses::FL_NUMBER_CONSTITUENT = 5;
const unsigned char LexCharClasses::FL_SIGN = 6;

const string LexCharClasses::eol_chars = "\n\r";

const int LexToken::TT_EOL = 10;
const int LexToken::TT_NONE = -1;
const int LexToken::TT_EOF = -2;
const int LexToken::TT_NUMBER = -3;
const int LexToken::TT_WORD = -4;
const int LexToken::TT_WHITE = -5;
const int LexToken::TT_NUMBERWORD = -6;

LexCharClasses::LexCharClasses(){

  resetSyntax();
  setCommonSyntax();

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

void LexCharClasses::setCommonSyntax(){

  setBaseSyntax();

  setBlockCommentStart("/*");
  setBlockCommentStop("*/");
  setLineComment("//");
  setQuoteStart('"');
  setQuoteStop('"');
  setWordConstituents("1234567890");

}

void LexCharClasses::setWordConstituents(const string constituents){

  for ( string::const_iterator it = constituents.begin(); it != constituents.end(); it++ )
    setFlag(*it,FL_WORD_CONSTITUENT);

}

void LexCharClasses::setWordConstituents(const char from, const char to){

  for ( unsigned char i = (unsigned char)from; i <= (unsigned char)to ; i++ )
    setFlag(i,FL_WORD_CONSTITUENT);

}

void LexCharClasses::setIntroducingWord(const string intro){

  for ( string::const_iterator it = intro.begin(); it != intro.end(); it++ )
    setFlag(*it,FL_INTRODUCING_WORD);

}

void LexCharClasses::setIntroducingWord(const char from, const char to){

  for ( unsigned char i = (unsigned char)from; i <= (unsigned char)to ; i++ )
    setFlag(i,FL_INTRODUCING_WORD);

}

void LexCharClasses::setIntroducingNumber(const string intro){

  for ( string::const_iterator it = intro.begin(); it != intro.end(); it++ )
    setFlag(*it,FL_INTRODUCING_NUMBER);

}

void LexCharClasses::setNumberConstituents(const string constituents){

  for ( string::const_iterator it = constituents.begin(); it != constituents.end(); it++ )
    setFlag(*it,FL_NUMBER_CONSTITUENT);

}

void LexCharClasses::setWhitespaces(const string whitespaces){

  for ( string::const_iterator it = whitespaces.begin(); it != whitespaces.end(); it++ )
    setFlag(*it,FL_WHITESPACE);

}

void LexCharClasses::setSigns(const string signs){

  for ( string::const_iterator it = signs.begin(); it != signs.end(); it++ )
    setFlag(*it,FL_SIGN);

}

void LexCharClasses::setOrdinaries(const string ordinaries){

  for ( string::const_iterator it = ordinaries.begin(); it != ordinaries.end(); it++ )
    ascii_table[(unsigned char)(*it)] = 0;

}

void LexCharClasses::setOrdinaries(const char from, const char to){

  for ( unsigned char i = (unsigned char)from; i <= (unsigned char)to ; i++ )
    ascii_table[i] = 0;

}

void LexCharClasses::setEOLs(const string eols){

  for ( string::const_iterator it = eols.begin(); it != eols.end(); it++ )
    setFlag(*it,FL_EOL);

}
    
LexScreener::LexScreener(std::istream * input, LexCharClasses * char_classes) throw (Exception<LexScreener>) :
  input(input), char_classes(0), nested_comments(false),
  lower_case_mode(false), skipped_lines(0), store_whitespaces(false){

  string thisMethod = "LexScreener()";

  if ( char_classes == 0 )
    throw Exception<LexScreener>(thisMethod + ": char_classes is 0");

  this->char_classes = char_classes;

}

bool LexScreener::skipIfMatchingWord(char c, const string & word){

  string::const_iterator it = word.begin();

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

  if ( isStoreWhitespaces() )
    whitespaces.str("");

  while ( true ){

    input->get(c);

    if ( input->eof() == true )
      break;

    if ( lower_case_mode == true )
      c = String::latinToLower(c);

    if ( first_char == true and isWhitespace(c) ){

      skipped = true;
      first_char = false;
      whitespace_mode = true;

      if ( isStoreWhitespaces() )
	whitespaces << c;

      continue;

    }
    
    first_char = false;
    
    if ( block_comment_mode == false and line_comment_mode == false ){
      
      if ( isWhitespace(c) ){

	skipped = true;

	if ( isStoreWhitespaces() )
	  whitespaces << c;
	
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
	
	if ( block_comment_mode == false and whitespace_mode == false )
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

LexScanner::LexScanner(std::istream * input) throw (Exception<LexScreener>, ExceptionBase) :
  input(input), line_number(1), previous_token_type(LexToken::TT_NONE), lower_case_mode(false),
  report_eol(false), report_white(false), token_putback(false), report_numbers_as_real(false),
  signed_numbers(false), floating_points(false), raw_quoting(false), uncollapsed_white(false){

  screener = new LexScreener(input,&char_classes);
  
  char_classes.setCommonSyntax();

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

void LexScanner::reportWhiteUncollapsed(bool flag){
  
  if ( flag == true ){
    
    reportEOL(true);
    reportWhite(true);
    
  }  
  
  screener->setStoreWhitespaces(flag);
  uncollapsed_white = flag;
  
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

  string thisMethod = "nextToken()";

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

  token.line_number = line_number;

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

    if ( isEscape(c) and treat_normal == false ){

      if ( token.type == LexToken::TT_WORD and isUseRawQuoting() )
	token.sval += c;

      escape_next = true;
      continue;

    }
      
    if ( token.type == LexToken::TT_NONE ){
      
      if ( (isIntroducingBlockComment(c) or isIntroducingLineComment(c)) and treat_normal == false ){
	
	input->putback(c);
	escape_next = !screener->screen();
	line_number += screener->getSkippedLines();
	token.line_number = line_number;
	
      } else if ( screener->isWhitespace(c) ){
	
	input->putback(c);
	escape_next = !screener->screen();
	line_number += screener->getSkippedLines();
	
	if ( isReportWhite() ){

	  char next;

	  input->get(next);

	  // whitespaces should only be reported if not at end of line
	  if ( !input->eof() ){
	    
	    if ( !isEOL(next) ){
	      
	      input->putback(next);
	      token.type = LexToken::TT_WHITE;
	      previous_token_type = token.type;

	      if ( isWhiteUncollapsed() )
		token.sval = screener->getWhitespaces().str();

	      return token.type;
	      
	    } else
	      input->putback(next);
	    
	  }
	  
	}
	
	token.line_number = line_number;
	
      } else if ( isIntroducingWord(c) ){
	
	token.sval += c;
	token.type = LexToken::TT_WORD;
	
      } else if ( isIntroducingNumber(c) ){
	
	number += c;
	token.type = LexToken::TT_NUMBER;
	
      } else if ( isUseSignedNumbers() and isSign(c) and input->eof() == false ){
	// check for sign
	
	char next;
	input->get(next);
	input->putback(next);
	
	if ( isIntroducingNumber(next) ){
	  
	  number += c;
	  token.type = LexToken::TT_NUMBER;
	  
	} else{
	  
	  token.type = c;
	  previous_token_type = token.type;
	  return token.type;
	  
	}
	
      } else if ( isIntroducingQuote(c) ){
	
	quote_mode = true;
	token.type = LexToken::TT_WORD;  

	if ( isUseRawQuoting() )
	  token.sval += c;
	
      } else if ( isEOL(c) ){
	
	if ( treat_normal == false ){
	  // treat_normal: an escaped EOL must not be reported
	  
	  line_number += LexScreener::skipIfReturn(input,c);
	  
	  if ( isReportEOL() and previous_token_type != LexToken::TT_EOL ){
	    
	    token.type = LexToken::TT_EOL;
	    previous_token_type = token.type;
	    return token.type;
	    
	  }
	  
	  token.line_number = line_number;

	} else
	  line_number += LexScreener::skipIfReturn(input,c);
	
      } else if ( isEscape(c) and treat_normal == false ){
	
	escape_next = true;
	
      } else {
	
	token.type = c;
	token.line_number = line_number;
	previous_token_type = token.type;
	return token.type;
	
      }
      
    } else if ( token.type == LexToken::TT_WORD ){
      
      if ( quote_mode == true ){
	
	if ( isTerminatingQuote(c) and treat_normal == false ) {

	  quote_mode = false;
	  token.line_number = line_number;

	  if ( isUseRawQuoting() )
	    token.sval += c;
	
	} else if ( isEOL(c) and treat_normal == false ){
	  
	  input->putback(c);
	  break;
	  
	} else{
	  
	  if ( isEOL(c) )
	    line_number += LexScreener::skipIfReturn(input,c);

	  else
	      token.sval += c;

	}

      } else if ( !isIntroducingWord(c) and !isWordConstituent(c) ){

	if ( (isIntroducingBlockComment(c) or isIntroducingLineComment(c)) and treat_normal == false ){

	  input->putback(c);
	  escape_next = !screener->screen();
	  line_number += screener->getSkippedLines();
	  token.line_number = line_number;
	  
	} else {

	  if ( isEOL(c) and treat_normal == true ){
	    
	    line_number += LexScreener::skipIfReturn(input,c);
	    
	  } else {
	    
	    input->putback(c);
	    previous_token_type = token.type;
	    return token.type;
	    
	  }

	}
	  
      } else 
	// belonging to word
	token.sval += c;
      
    } else if ( token.type == LexToken::TT_NUMBER ){
      
      if ( isNumberConstituent(c) )
	number += c;
      
      else if ( (isIntroducingBlockComment(c) or isIntroducingLineComment(c)) and treat_normal == false ){
	
	input->putback(c);
	escape_next = !screener->screen();
	line_number += screener->getSkippedLines();
	token.line_number = line_number;
	
      } else {
	
	input->putback(c);
	
	if ( isReportNumbersAsReal() ){

	  token.nval = atof(number.c_str());
	
	} else {
	  
	  token.type = LexToken::TT_NUMBERWORD;
	  token.sval = number;
	  
	}
	
	previous_token_type = token.type;
	return token.type;
	
      }
    }
    
    treat_normal = false;
    
  }
  // EOF
  
  if ( quote_mode == true ){
    
    ostringstream liness;
    liness << line_number;
    throw Exception<LexScanner>(thisMethod + ": unterminated quote in line " + liness.str() + "!");

  }

  if ( token.type == LexToken::TT_NUMBER ){

    if ( isReportNumbersAsReal() ){

      token.nval = atof(number.c_str());

    } else {

      token.type = LexToken::TT_WORD;
      token.sval = number;
      
    }
    
  } else if ( token.type == LexToken::TT_NONE )
    token.type = LexToken::TT_EOF;
  
  previous_token_type = token.type;
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

  token.line_number = line_number;

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
  
    if ( token.type == LexToken::TT_NONE ){

      if ( (isIntroducingBlockComment(c) or isIntroducingLineComment(c)) and treat_normal == false ){

	input->putback(c);
	escape_next = !screener->screen();
	line_number += screener->getSkippedLines();
	
      } else if ( screener->isWhitespace(c) ){
    
	input->putback(c);
	escape_next = !screener->screen();
	line_number += screener->getSkippedLines();
    
      } else if ( isEOL(c) ){

	line_number += LexScreener::skipIfReturn(input,c);	
	token.line_number = line_number;

      } else {

	token.type = LexToken::TT_WORD;
	token.sval += c;
      
      }

    } else {
      // found an unscreenable character
      
      if ( isEscape(c) and treat_normal == false ){

	escape_next = true;
	
      } else if ( (isIntroducingBlockComment(c) or isIntroducingLineComment(c)) and treat_normal == false ){

	input->putback(c);
	escape_next = !screener->screen();
	line_number += screener->getSkippedLines();
	
      } else if ( isEOL(c) and treat_normal == false ){

	input->putback(c);
	break;
      
      } else {

	if ( isEOL(c) ){

	  line_number += LexScreener::skipIfReturn(input,c);

	} else {

	token.sval += c;
	treat_normal = false;

	}
	
      }
      
    }
  
    treat_normal = false;
    
  }
  
  if ( token.type == LexToken::TT_NONE )
    token.type = LexToken::TT_EOF;
  
  return token.type;
  
}

string LexToken::typeToString() const {
  
  switch ( type ){

  case TT_EOF:

    return string("EOF");
    break;

  case TT_EOL:

    return string("EOL");
    break;

  case TT_NUMBER:
    
    return string("NUMBER");
    break;

  case TT_WORD:
    
    return string("WORD");
    break;

  case TT_NUMBERWORD:
    
    return string("NUMBERWORD");
    break;

  case TT_NONE:

    return string("NONE");
    break;

  case TT_WHITE:

    return string("WHITE");
    break;

  default:

    return string("ORDINARY");
    break;

  }

}

string LexToken::toString() const {

  string thisMethod = "tokenToString()";

  ostringstream tsstream;

  tsstream << "[" << "'";

  switch ( type ){

  case TT_WORD:
  case TT_NUMBERWORD:
  case TT_WHITE:

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

    tsstream << (char)type;
    break;

  }

  tsstream << "'" << "," << typeToString() << "," << line_number << "]";

  return tsstream.str();

}
