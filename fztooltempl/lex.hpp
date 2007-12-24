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

/**
   @file lex.hpp
   @author Friedemann Zintel
*/
#ifndef LEX_HPP
#define LEX_HPP

#include <istream>
#include <sstream>
#include <stdlib.h>
#include <exception.hpp>
#include <utils.hpp>

/**
   @brief necessary stuff for lexical analysis
*/
namespace lex{

  /**
     @brief A struct to store all relevant delimitor-charakters, comment-signs, whitespaces, etc. for lexical analysis
  */
  struct LexCharClasses{

  public:
  
    /**
       @brief the start string of a block-comment
    */
    utils::String block_comment_start;

    /**
       @brief the stop string of a block-comment
    **/
    utils::String block_comment_stop;

    /**
       @brief the string of a line-comment
    **/
    utils::String line_comment;

    /**
       @brief the whitespace characters
    */
    utils::String white_spaces;

    /**
       @brief the start character for string-quoting
    */
    char quote_start;
    
    /**
       @brief the stop character for string-quoting
    */
    char quote_stop;

    /**
       @brief the characters in this string will be treated as word-constituents (not at beginning of word)
    */
    utils::String word_constituents;

    /**
       @brief the return characters
    */
    static const utils::String return_chars;
    
    /**
       @brief sets default-values
    */
    void setDefaults();

    /**
       @brief checks if character is a linefeed
       @param c character to be checked
       @return true if c is linefeed
    */
    static bool isLF(const char & c){ return ( c == '\n' ); }

    /**
       @brief checks if character is a carriage-return
       @param c character to be checked
       @return true if c is carriage-return
    */
    static bool isCR(const char & c){ return ( c == '\r' ); }
    
  };
  
  /**
     @brief A class for screening the input, i.e. removing comments and whitespaces
  */
  class LexScreener{
    
  protected:
    
    std::istream * input;
    
    LexCharClasses * char_classes;
    
    bool nested_comments;
    
    bool lower_case_mode;
    
  public:
    
    LexScreener(std::istream * input) : input(input), char_classes(0), nested_comments(false), lower_case_mode(false){}
    
    /**
       @brief sets a reference to the lexical character classes
       @params char_classes the reference
       @note on destruction of the LexScreener the lexical character classes won't be destroyed. You have to take care for the destruction.
    */
    void setLexCharClasses(LexCharClasses * char_classes){ this->char_classes = char_classes; }
    
    /**
       @brief reference to the lexical character classes
       @return lexical character classes
    */
    LexCharClasses * getLexCharClasses(){ return char_classes; }
    
    /**
       @brief enables of disables use of nested comments
       @params true for using nested comments, false otherwise
    */
    void setNestedComments(bool nested_comments){ this->nested_comments = nested_comments; }

    /**
       @brief returns the state of nested comments
       @return the state
    */
    bool isNestedComments(){ return nested_comments; }

    /**
       @brief read characters will be lower-cased if true
       @param lower_case_mode true,false
    */
    void setLowerCaseMode(bool lower_case_mode){ this->lower_case_mode = lower_case_mode; }
    
    /**
       @brief returns the state of lower-case mode
       @return true if lower-case mode is switched on, false otherwise
    */
    bool isLowerCaseMode(){ return lower_case_mode; }

    /**
       If the first character is a whitespace the stream position will be set to the first non-whitespace charakter. Included comments
       will be overread.\n
       If the first character  is introducing a comment the stream position will be set to the position right behind the comment-stop.\n
       In all other cases the screen() will return immediately.
       @brief screens the input
       @return number of skipped lines
    */
    unsigned long screen();

    /**
       @brief checks if the given char is a whitespace
       @param c the character to be checked
       @return true, is c is a whitespace
       @todo check for uninitialised pointer!!!
    */
    bool isWhitespace(const char & c) const { return char_classes->white_spaces.containsChar(c); }

    /**
       @brief checks if the given char is a return character
       @param c the character to be checked
       @return true, is c is a return character
       @todo check for uninitialised pointer!!!
    */
    bool isReturn(const char & c) const { return char_classes->return_chars.containsChar(c); }

    /**
       A return is a CR of LF. A CR followed directly by an LF counts as one return.
       @brief checks and skips returns
       @param input the input-stream to be read from
       @param c character to be checked
       @return number of returns (at most 1)
    */
    static unsigned long skipIfReturn(std::istream * input, char c);

  protected:

    bool skipIfMatchingWord(char c, const utils::String & word);
    bool skipIfIntroducingBlockComment(char c){ return skipIfMatchingWord(c,char_classes->block_comment_start); }
    bool skipIfTerminatingBlockComment(char c){ return skipIfMatchingWord(c,char_classes->block_comment_stop); }
    bool skipIfIntroducingLineComment(char c){ return skipIfMatchingWord(c,char_classes->line_comment); }

  };
  /**
     @brief A class for performing lexical analysis
  */
  class LexScanner{

  protected:

    std::istream * input;

    unsigned long line_number;
    
    unsigned long token_line_number;

    bool lower_case_mode;

    bool report_return;

  public:

    utils::String sval;

    long double nval;

    int ttype;

  protected:

    utils::String saved_sval;

    long double saved_nval;

    int saved_ttype;

    LexScreener * screener;

    LexCharClasses char_classes;

  public:

  /** @name Token types */
  //@{
    /** @brief End-Of-File */
    static const int TT_EOF;

    /** @brief End-Of-Line */
    static const int TT_EOL;

    /** @brief Number */
    static const int TT_NUMBER;

    /** @brief Word */
    static const int TT_WORD;

    /** @brief None */
    static const int TT_NONE;
    //@}

  private:

    // copy-constructor
    LexScanner(LexScanner &){}
    
  public:

    /**
       @param input the input-stream to be scanned
    */
    LexScanner(std::istream * input);

    ~LexScanner();

    /**
       @brief gives the next token
       @return the token-type resp. token-value
    */
    int nextToken() throw (Exception<LexScanner>);

    /**
       @brief returns the next resp. rest of line
       @return next resp. rest of line
    */
    utils::String nextLine();

    /**
       @brief returns the current line number
       @return the line number
    */
    unsigned long getLineNumber() { return token_line_number; }

    /**
       @brief reference to the lexical character classes
       @return lexical character classes
    */
    LexCharClasses & getLexCharClasses(){ return char_classes; }

    /**
       @brief enables use of nested comments
    */
    void enableNestedComments(){ screener->setNestedComments(true); }

    /**
       @brief disables use of nested comments
    */
    void disableNestedComments(){ screener->setNestedComments(false); }

    /**
       @brief returns state of using nested comments
       @return state of using nested comments
    */
    bool isNestedComments(){ return screener->isNestedComments(); }

    /**
       @brief sets lower-case mode: all input-chars will be lower-cased before comparison
    */
    void lowerCaseMode();

    /**
       @brief EOL will be reported as a token
    */
    void reportEOL(){ report_return = true; }

    /**
       @brief returns the status of "report EOL"
       @return true if EOL will be reported as a token
    */
    bool isReportEOL() { return report_return; }

    /**
       @brief returns state of lower-case mode
       @ true, if lower-case mode is switched on, false otherwise
    */
    bool isLowerCaseMode(){ return lower_case_mode; }

    /**
       Only the following characters introduces a word:\n
       "A-Z", "a-z", "_"
       @brief checks if the character introduces a word
       @param c character to be checked
       @return true if c introduces a word
    */
    static bool isIntroducingWord(const char & c) { return ( ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || (c == '_') ); }

    /**
       @brief checks if the character introduces a number
       @param c character to be checked
       @return true if c introduces a number
    */
    static bool isIntroducingNumber(const char & c) { return ( '0' <= c && c <= '9' ); }

    /**
       @brief checks if the character is a number constituent
       @param c character to be checked
       @return true if c is a number constituent
    */
    static bool isNumberConstituent(const char & c) { return ( c == '.' ); }

    /**
       @brief checks if the character introduces a word-quote
       @param c character to be checked
       @return true if c introduces a word-quote
    */
    bool isIntroducingQuote(const char & c) const { return ( c == char_classes.quote_start ); }

    /**
       @brief checks if the character terminates a word-quote
       @param c character to be checked
       @return true if c terminates a word-quote
    */
    bool isTerminatingQuote(const char & c) const { return ( c == char_classes.quote_stop ); }

    /**
       @brief checks if the character introduces a block comment
       @param c character to be checked
       @return true if c introduces block comment
    */
    bool isIntroducingBlockComment(const char & c) const {
      
      return ( char_classes.block_comment_start.empty() ? false : c == char_classes.block_comment_start[0] );

    }

    /**
       @brief checks if the character introduces a line comment
       @param c character to be checked
       @return true if c introduces line comment
    */
    bool isIntroducingLineComment(const char & c) const {

      return ( char_classes.line_comment.empty() ? false : c == char_classes.line_comment[0] );

    }

    /**
       @brief checks if the character is constituent of a word
       @param c character to be checked
       @return true if c is constituent of a word
    */
    bool isWordConstituent(const char & c) const { return char_classes.word_constituents.containsChar(c); }

    /**
       @brief checks if the given char is a whitespace
       @param c the character to be checked
       @return true, is c is a whitespace
       @todo check for uninitialised pointer!!!
    */
    bool isWhitespace(const char & c) const { return char_classes.white_spaces.containsChar(c); }

    /**
       @brief checks if the given char is a return character
       @param c the character to be checked
       @return true, is c is a return character
       @todo check for uninitialised pointer!!!
    */
    bool isReturn(const char & c) const { return char_classes.return_chars.containsChar(c); }

    /**
       String representation is of the following form:\n
       [<token>,<token type>,<line number>]
       @brief returns a string representation of the current token
       @return the string representation
    */
    utils::String tokenToString() const;

    /**
       @brief returns a string representation of the current token type
       @return the string representation
    */
    utils::String ttypeToString() const;

  };


}

#endif
