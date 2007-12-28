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

#define LEX_AT_SIZE 256

/**
   @brief necessary stuff for lexical analysis
   @since V2.1
*/
namespace lex{

  /**
     @brief A struct to store all relevant delimitor-charakters, comment-signs, whitespaces, etc. for lexical analysis
     @since V2.1
  */
  class LexCharClasses{

  protected:

    static const unsigned char FL_EOL;
    static const unsigned char FL_WHITESPACE;
    static const unsigned char FL_INTRODUCING_WORD;
    static const unsigned char FL_WORD_CONSTITUENT;
    static const unsigned char FL_INTRODUCING_NUMBER;
    static const unsigned char FL_NUMBER_CONSTITUENT;
    static const unsigned char FL_SIGN;

    static const utils::String eol_chars;
      
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
       @brief the escape character to remove any special meaning of the following character
    */
    char escape;

    unsigned char ascii_table[LEX_AT_SIZE];

  public:

    LexCharClasses();

    /**
       @brief resets the syntaxtable and clears all special meaning of any symbol
    */
    void resetSyntax();

    /**
       Sets values for the syntax-table which are at least necessary to provide the minimum
       basis of syntax-checking. Theses are:\n
       EOL-characters, whitespaces, signs and
       @brief sets initial values for the syntax-table
       @todo doku
    */

    void setBaseSyntax();
    
    /**
       @brief sets default-values
    */
    void setDefaultSyntax();

    /**
       @brief sets the introducing-word for a block comment
       @param word the word which defines the block comment introducing-word
    */
    void setBlockCommentStart(std::string word) { block_comment_start = word; }

    /**
       @brief sets the terminating-word for a block comment
       @param word the word which defines the block comment terminating-word
    */
    void setBlockCommentStop(std::string word) { block_comment_stop = word; }

    /**
       @brief sets the word for a line comment
       @param word the word which defines the block comment terminating-word
    */
    void setLineComment(std::string word) { line_comment = word; }

    /**
       @brief gets the introducing-word for a block comment
       @return the word which defines the block comment introducing-word
    */
    utils::String getBlockCommentStart() { return block_comment_start; }

    /**
       @brief gets the terminating-word for a block comment
       @return the word which defines the block comment terminating-word
    */
    utils::String getBlockCommentStop() { return block_comment_stop; }

    /**
       @brief gives the word for a line comment
       @return the word which defines the line-comment word
    */
    utils::String getLineComment() { return line_comment; }

    /**
       @brief checks whether a character is the quote_start character
       @param c the character to be checked
       @return true if c is quote_start character
    */
    bool isQuoteStart(const char & c) const { return ( c == quote_start ); }

    /**
       @brief checks whether a character is the quote_stop character
       @param c the character to be checked
       @return true if c is quote_stop character
    */
    bool isQuoteStop(const char & c) const { return ( c == quote_stop ); }

    /**
       Removes special meaning of quote, escape (and newline in method nextLine(); in nextToken() online newlines within words
       are escaped) for the next character.
       @brief sets the escape character
       @param c the escape character
       @todo implementation!!!
    */
    void setEscape(const char & c){ escape = c; }

    /**
       @brief checks whether a character is the escape character
       @param c the character to be checked
       @return true if c is the escape character
    */
    bool isEscape(const char & c){ return ( c == escape ); }

    /**
       @brief all characters in the word will be word-constituents
       @param constituents the word which defines characters being word-constituents
    */
    void setWordConstituents(const std::string constituents);

    /**
       @brief all characters in the range [from,to] will be word-constituents
       @param from the character which defines the lower bound for word-constituents
       @param to the character which defines the upper bound for word-constituents
    */
    void setWordConstituents(const char from, const char to);

    /**
       @brief checks if the character is constituent of a word
       @param c character to be checked
       @return true if c is constituent of a word
    */
    bool isWordConstituent(const char & c) const { return getFlag(c,FL_WORD_CONSTITUENT); }

    /**
       @brief Sets all characters occuring in given word as whitespaces
       @param whitespaces the word containing the whitespace-characters
    */
    void setWhitespaces(const std::string whitespaces);
    
    /**
       @brief checks if the given char is a whitespace
       @param c the character to be checked
       @return true, is c is a whitespace
       @todo check for uninitialised pointer!!!
    */
    bool isWhitespace(const char & c) const { return getFlag(c,FL_WHITESPACE); }

    /**
       @brief Sets all characters occuring in given word as end-of-line characters
       @param eols the word containing the eol-characters
    */
    void setEOLs(const std::string eols);

    /**
       @brief checks if the given char is a return character
       @param c the character to be checked
       @return true, is c is a return character
       @todo check for uninitialised pointer!!!
    */
    bool isEOL(const char & c) const { return getFlag(c,FL_EOL); }

    /**
       @brief all characters in the word will introduce a word
       @param intro the word which defines characters introducing a word
    */
    void setIntroducingWord(const std::string intro);

    /**
       @brief all characters in the range [from,to] will introduce a word
       @param from the character which defines the lower bound for introducing word
       @param to the character which defines the upper bound for introducing word
    */
    void setIntroducingWord(const char from, const char to);

    /**
       Only the following characters introduces a word:\n
       "A-Z", "a-z", "_" (underscore)
       @brief checks if the character introduces a word
       @param c character to be checked
       @return true if c introduces a word
       @todo doku
    */
    bool isIntroducingWord(const char & c) const { return getFlag(c,FL_INTRODUCING_WORD); }

    /**
       @brief all characters in the word will introduce a number
       @param intro the word which defines characters introducing a number
    */
    void setIntroducingNumber(const std::string intro);

    /**
       @brief checks if the character introduces a number
       @param c character to be checked
       @return true if c introduces a number
       @todo doku
    */
    bool isIntroducingNumber(const char & c) const { return getFlag(c,FL_INTRODUCING_NUMBER); }

    /**
       @brief all characters in the word will be number constituents
       @param constituents the word which defines characters being number constituents
    */
    void setNumberConstituents(const std::string constituents);

    /**
       @brief checks if the character is a number constituent
       @param c character to be checked
       @return true if c is a number constituent
       @todo doku
    */
    bool isNumberConstituent(const char & c) { return getFlag(c,FL_NUMBER_CONSTITUENT); }

    /**
       @brief all characters in the word willdefines number-signs
       @param signs the word which consists of all sign-characters
    */
    void setSigns(const std::string signs);

    /**
       @brief checks whether a character is a positive or negative sign
       @param c the character to be checked
       @return true if c is a sign
    */
    bool isSign(const char & c) const { return getFlag(c,FL_SIGN); }

    /**
       @brief checks if the character introduces a block comment
       @param c character to be checked
       @return true if c introduces block comment
    */
    bool isIntroducingBlockComment(const char & c) const { return ( block_comment_start.empty() ? false : c == block_comment_start[0] ); }

    /**
       @brief checks if the character introduces a line comment
       @param c character to be checked
       @return true if c introduces line comment
    */
    bool isIntroducingLineComment(const char & c) const { return ( line_comment.empty() ? false : c == line_comment[0] ); }

    /**
       @brief all characters in the word will lose any special meaning
       @param ordinary the word which defines characters having no special meaning
    */
    void setOrdinaries(const std::string ordinary);

    /**
       @brief all characters in the range [from,to] will lose any special meaning
       @param from the character which defines the lower bound for characters without special meaning
       @param to the character which defines the upper bound for characters without special meaning
    */
    void setOrdinaries(const char from, const char to);

    /**
       @brief checks if the character is ordinary
       @param c character to be checked
       @return true if c is ordinary
    */
    bool isOrdinary(const char & c) const { return ( ascii_table[(unsigned char)c] == 0 ); }

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

    /**
       @brief checks if the given char is the ASCII-0 character
       @param c the character to be checked
       @return true, is c ASCII-0 character
    */
    static bool isNull(const char & c){ return ( c == '\0' ); }

  protected:

    bool getFlag(const char & c, const unsigned char & flag) const { return ( ascii_table[(unsigned char)c] & 1L<<flag ); }

    void setFlag(const char & c, const unsigned char & flag){ ascii_table[(unsigned char)c] |= 1L<<flag; }

    void cleaFlag(const char & c, const unsigned char & flag){ ascii_table[(unsigned char)c] &= ~(1L<<flag); }
    
  };
  
  /**
     @brief A class for screening the input, i.e. removing comments and whitespaces
     @since V2.1
  */
  class LexScreener{
    
  protected:
    
    std::istream * input;
    
    LexCharClasses * char_classes;
    
    bool nested_comments;
    
    bool lower_case_mode;

    unsigned long skipped_lines; 
    
  public:
    
    LexScreener(std::istream * input) : input(input), char_classes(0), nested_comments(false), lower_case_mode(false), skipped_lines(0){}
    
    /**
       @brief sets a reference to the lexical character classes
       @param char_classes the reference
       @note on destruction of the LexScreener the lexical character classes won't be destroyed. You have to take care for the destruction.
    */
    void setLexCharClasses(LexCharClasses * char_classes){ this->char_classes = char_classes; }
    
    /**
       @brief reference to the lexical character classes
       @return lexical character classes
    */
    LexCharClasses * getLexCharClasses(){ return char_classes; }
    
    /**
       @brief determines whether nested comments should be supported
       @param flag true if nested commments should be supported
    */
    void setNestedComments(bool flag){ this->nested_comments = flag; }

    /**
       @brief returns the state of nested comments
       @return the state
    */
    bool isNestedComments(){ return nested_comments; }

    /**
       @brief read characters will be lower-cased if true
       @param flag true,false
    */
    void setLowerCaseMode(bool flag){ this->lower_case_mode = flag; }
    
    /**
       @brief returns the state of lower-case mode
       @return true if lower-case mode is switched on, false otherwise
    */
    bool isLowerCaseMode(){ return lower_case_mode; }

    /**
       It reads the next character from the input stream.\n
       If the first character is a whitespace the stream position will be set to the first non-whitespace charakter. Included comments
       will be overread.\n
       If the first character  is introducing a comment the stream position will be set to the position right behind the comment-stop.\n
       In all other cases the screen() will return immediately.
       @brief screens the input
       @return number of skipped lines
       @todo doku!!!
    */
    bool screen();

    /**
       @brief returns the number of skipped lines
       @return te skipped lines
    */
    unsigned long getSkippedLines(){ return skipped_lines; }

    /**
       @brief checks if the given char is a whitespace
       @param c the character to be checked
       @return true, is c is a whitespace
       @todo check for uninitialised pointer!!!
    */
    bool isWhitespace(const char & c) const { return char_classes->isWhitespace(c); }

    /**
       @brief checks if the given char is a return character
       @param c the character to be checked
       @return true, is c is a return character
       @todo check for uninitialised pointer!!!
    */
    bool isEOL(const char & c) const { return char_classes->isEOL(c); }

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
    bool skipIfIntroducingBlockComment(char c){ return skipIfMatchingWord(c,char_classes->getBlockCommentStart()); }
    bool skipIfTerminatingBlockComment(char c){ return skipIfMatchingWord(c,char_classes->getBlockCommentStop()); }
    bool skipIfIntroducingLineComment(char c){ return skipIfMatchingWord(c,char_classes->getLineComment()); }

  };

  /**
     @brief A class for storing all token-relevant attributes
     @see LexScanner
  */
  struct LexToken{

    /**
       @brief will store the characters of the string if type == TT_WORD
       @since V2.1
    */
    utils::String sval;

    /**
       @brief will strore the numeric value if type == TT_NUMBER
    */
    long double nval;

    /**
       @brief the token type
    */
    int type;
    
    /**
       @brief the linenumber the token occured in
    */
    unsigned long token_line_number;

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

    /** @brief Whitespace */
    static const int TT_WHITE;

    /** @brief None */
    static const int TT_NONE;
    //@}

    LexToken() : sval(""), nval(0), type(TT_NONE), token_line_number(0){}

    /**
       String representation is of the following form:\n
       [\<token\>,\<token type\>,\<line number\>]
       @brief returns a string representation of the current token
       @return the string representation
    */
    utils::String toString() const;

    /**
       @brief returns a string representation of the current token type
       @return the string representation
    */
    utils::String typeToString() const;

  };

  /**
     @brief A class for performing lexical analysis
     @since V2.1
  */
  class LexScanner{

  protected:

    std::istream * input;

    unsigned long line_number;

    bool lower_case_mode;

    bool report_eol;

    bool report_white;

    bool token_putback;

    bool parse_numbers;

    bool signed_numbers;

    bool floating_points;
    
  public:

    LexToken token;

  protected:

    LexToken saved_token;

    LexScreener * screener;

    LexCharClasses char_classes;

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
       Causes the scanner to put back this token which will be given in next turn of nextToken().
       The internal status will be set to the previous token.
       @brief causes the scanner to put back this token
    */
    void putback();

    /**
       @brief stores the complete the next resp. rest of line in the token
       @return the token type
       @note The line is untruncted. Use String::trunc() to trunc leading and trailing blanks.
    */
    int nextLine() throw (Exception<LexScanner>);

    /**
       @brief returns the current line number
       @return the line number
    */
    unsigned long getLineNumber() { return token.token_line_number; }

    /**
       @brief reference to the lexical character classes
       @return lexical character classes
    */
    LexCharClasses & getLexCharClasses(){ return char_classes; }

    /**
       @brief determines whether nested comments should be supported
       @param flag true if nested comments should be supported
    */
    void supportNestedComments(bool flag){ screener->setNestedComments(flag); }

    /**
       @brief returns state of using nested comments
       @return state of using nested comments
    */
    bool isNestedComments(){ return screener->isNestedComments(); }

    /**
       @brief specifies lower-case mode: all input-chars will be lower-cased before comparison
    */
    void lowerCaseMode();

    /**
       @brief returns state of lower-case mode
       @ true, if lower-case mode is switched on, false otherwise
    */
    bool isLowerCaseMode(){ return lower_case_mode; }

    /**
       The number will be stored in the nval-field of LexToken
       @brief specifies that numbers should not be treated as plain words but as real-numbers
    */
    void parseNumbers(){ parse_numbers = true; }

    /**
       @brief returns status of the parse-numbers-flag
       @return true if parse-numbers is set
    */
    bool isParseNumbers(){ return parse_numbers; }

    /**
       @brief determines whether or not EOL will be reported as a token
       @param flag true if EOL should be reported as a token
    */
    void reportEOL(bool flag){ report_eol = flag; }

    /**
       @brief returns the status of "report EOL"
       @return true if EOL will be reported as a token
    */
    bool isReportEOL() { return report_eol; }

    /**
       If set to true and if an arbitrary number of whitespaces occur between two tokens they will be reported
       as one single token of type TT_WHITE
       @brief determines whether or not whitespaces will be reported as a token
       @param flag true if whitespaces should be reported as a token
    */
    void reportWhite(bool flag){ report_white = flag; }

    /**
       @brief returns the status of "report white"
       @return true if whitespaces will be reported as a token
    */
    bool isReportWhite() { return report_white; }

    /**
       Only the following characters introduces a word:\n
       "A-Z", "a-z", "_" (underscore)
       @brief checks if the character introduces a word
       @param c character to be checked
       @return true if c introduces a word
       @todo doku
    */
    bool isIntroducingWord(const char & c) const { return char_classes.isIntroducingWord(c); }

    /**
       @brief checks if the character introduces a number
       @param c character to be checked
       @return true if c introduces a number
       @todo doku
    */
    bool isIntroducingNumber(const char & c) { return char_classes.isIntroducingNumber(c); }

    /**
       @brief checks if the character is a number constituent
       @param c character to be checked
       @return true if c is a number constituent
       @todo doku
    */
    bool isNumberConstituent(const char & c) { return char_classes.isNumberConstituent(c); }

    /**
       @brief checks whether a character is a positive or negative sign
       @param c the character to be checked
       @return true if c is a sign
    */
    bool isSign(const char & c){ return char_classes.isSign(c); }

    /**
       @brief a sign preceding a number will be interpreted as belonging to the number
    */
    void useSignedNumbers(){ signed_numbers = true; }

    /**
       @brief returns the status of "use signed numbers"
       @return true if signes will be treated as preceding numbers
    */
    bool isUseSignedNumbers() { return signed_numbers; }

    /**
       @brief a numbers seperated by a point will be interpreted as floatingpoints
    */
    void useFloatingpoints();

    /**
       @brief returns the status of "use floating points"
       @return true if floating points will be recognized
    */
    bool isUseFloatingpoints() { return floating_points; }

    /**
       @brief checks if the character introduces a word-quote
       @param c character to be checked
       @return true if c introduces a word-quote
    */
    bool isIntroducingQuote(const char & c) const { return char_classes.isQuoteStart(c); }

    /**
       @brief checks if the character terminates a word-quote
       @param c character to be checked
       @return true if c terminates a word-quote
    */
    bool isTerminatingQuote(const char & c) const { return char_classes.isQuoteStop(c); }

    /**
       @brief checks if the character introduces a block comment
       @param c character to be checked
       @return true if c introduces block comment
    */
    bool isIntroducingBlockComment(const char & c) const { return char_classes.isIntroducingBlockComment(c); }

    /**
       @brief checks if the character introduces a line comment
       @param c character to be checked
       @return true if c introduces line comment
    */
    bool isIntroducingLineComment(const char & c) const { return char_classes.isIntroducingLineComment(c); }

    /**
       @brief checks if the character is constituent of a word
       @param c character to be checked
       @return true if c is constituent of a word
    */
    bool isWordConstituent(const char & c) const { return char_classes.isWordConstituent(c); }

    /**
       @brief checks if the given char is a whitespace
       @param c the character to be checked
       @return true, is c is a whitespace
       @todo check for uninitialised pointer!!!
    */
    bool isWhitespace(const char & c) const { return char_classes.isWhitespace(c); }

    /**
       @brief checks whether a character is the escape character
       @param c the character to be checked
       @return true if c is the escape character
    */
    bool isEscape(const char & c){ return char_classes.isEscape(c); }
    
    /**
       @brief checks if the given char is a return character
       @param c the character to be checked
       @return true, is c is a return character
       @todo check for uninitialised pointer!!!
    */
    bool isEOL(const char & c) const { return char_classes.isEOL(c); }

    /**
       @brief checks if the given char is the ASCII-0 character
       @param c the character to be checked
       @return true, is c ASCII-0 character
    */
    bool isNull(const char & c) const { return char_classes.isNull(c); }

    /**
       String representation is of the following form:\n
       [<token>,<token type>,<line number>]
       @brief returns a string representation of the current token
       @return the string representation
    */
    utils::String tokenToString() const { return token.toString(); }

    /**
       @brief returns a string representation of the current token type
       @return the string representation
    */
    utils::String typeToString() const { return token.typeToString(); }

  };


}

#endif
