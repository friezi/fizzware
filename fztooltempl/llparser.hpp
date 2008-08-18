#ifndef __FZTT_LLPARSER_HPP__
#define __FZTT_LLPARSER_HPP__

#include <iostream>
#include <sstream>
#include <utility>
#include <list>
#include <set>
#include <exception.hpp>
#include <utils.hpp>
#include <datastructures.hpp>
#include <lex.hpp>

namespace parse{

  class GrammarSymbol;
  class Terminal;
  class Nonterminal;
  class Production;
  class Rule;
  class LLParser;
  class Grammar;

  class Grammar{

  public:

    // name and type of token
    typedef std::pair<std::string, int> Token;

    typedef Token TaggedTerminal;

    struct TaggedTerminalComparator{
    
      bool operator()(TaggedTerminal t1,TaggedTerminal t2){
	return t1.first < t2.first;
      }

    };

    typedef std::set<Grammar::TaggedTerminal,Grammar::TaggedTerminalComparator> TaggedTerminals;

  protected:

    /**
       @brief pseudostartrule S'->S
    */
    Rule *startrule;

  private:

    Rule *lastAccessedRule;

    Production *lastAccessedProduction;

    std::string lastGrammarSymbol;

    Terminal *lastTerminal;

    Nonterminal *lastNonterminal;

    char defmode;

  protected:

    std::set<Rule *> rules;

    std::set<Terminal *> terminals;

    std::set<Nonterminal *> nonterminals;

  public:

    Grammar() : startrule(0), lastAccessedRule(0), lastAccessedProduction(0), lastGrammarSymbol(""),
		lastTerminal(0), lastNonterminal(0), defmode(0){}

    ~Grammar();

    void setStartRule(Rule *rule) { this->startrule = rule; }

    /**
       @brief returns the pseudostartrule S'->S
    */
    Rule * getStartRule(){ return startrule; }

    std::set<Rule *> & getRules(){ return rules; }

    std::set<Terminal *> & getTerminals(){ return terminals; }

    std::set<Nonterminal *> & getNonterminals(){ return nonterminals; }

    std::string toString();

    void newProduction() throw(exc::Exception<Grammar>);

    void setStart(std::string nonterminal) throw(exc::Exception<Grammar>);

    Grammar & rule(std::string nonterminal);

    Grammar & lambda() throw(exc::Exception<Grammar>);

    Grammar & operator<<(std::string gsym) throw(exc::Exception<Grammar>);

    Grammar & operator<<(Grammar & grammar) throw(exc::Exception<Grammar>);

    Grammar & operator,(char type) throw(exc::Exception<Grammar>);

    Grammar & operator|(std::string gsym) throw(exc::Exception<Grammar>);

    Grammar & operator|(Grammar & grammar) throw(exc::Exception<Grammar>);

    /**
       calculates nullability, direct first. It will detect left-recursion and throw an
       exception in this case.
       @brief nullability, direct first, left recursion
       @throw Exception<Grammar> in case of detected left recursion
    */
    void calculateNDF() throw (exc::Exception<Grammar>, exc::ExceptionBase);

  protected:

    Nonterminal * findNonterminal(std::string nonterminal);

    /**
       helper function for calculateNDF()
    */
    void traverseNDF(Nonterminal *nonterminal) throw (exc::Exception<Grammar>, exc::ExceptionBase);

  };

  class Production{

    friend class Rule;

  protected:

    std::list<GrammarSymbol *> symbols;

    Grammar::TaggedTerminals director_set;

    Production * clone();

  public:

    Production(){}

    ~Production();

    std::list<GrammarSymbol *> & getSymbols(){ return symbols; }

    Grammar::TaggedTerminals & getDirectorSet(){ return director_set; }

    std::string toString();
  
  };
  
  class GrammarSymbol{
    
  public:
    
    virtual ~GrammarSymbol(){}

    virtual std::string getName() = 0;
    
  };

  class Terminal : public GrammarSymbol{

  public:

    static const char TT_WORD;

    static const char TT_NUMBER;

    static const char TM_OVERREAD;

    static const char TM_STORE;

  protected:

    std::string name;
    
    // type of terminal: word or number (-word)
    char type;

    // mode: overread or store (in AST)
    char mode;

  public:

    Terminal(std::string name, char type, char mode) : name(name), type(type), mode(mode){}

    Terminal(std::string name) : name(name){}

    std::string getName(){ return name; }

    char getType(){ return type; }
    
    char getMode(){ return mode; }

    void setType(char type){ this->type = type; }

    void setMode(char mode){ this->mode = mode; }
  
  };

  class Nonterminal : public GrammarSymbol{

    friend class Grammar;

  private:

    bool checked;

  protected:

    // name of nonterminal
    std::string name;

    // reference is stored in Grammar and will be deleted from there
    Rule * rule;

    char nullable;

    std::set<Terminal *> firstset;

  public:

    Nonterminal(std::string name) : checked(false), name(name), rule(0), nullable(-1){}

    std::string getName(){ return name; }

    void setRule(Rule *rule){ this->rule = rule; }

    Rule * getRule(){ return rule; }

    std::set<Terminal *> & getFirstset(){ return firstset; }

    void setNullable(char val){ nullable = val; }

    bool isNullable(){ return nullable == 1; }

  };

  class Rule{

    friend class LLParser;
    
  protected:

    Nonterminal * nonterminal;

    std::list<Production *> alternatives;

  protected:

    Rule * clone(Grammar::Token lookahead) throw (exc::Exception<LLParser>);

  public:

    Rule(){}

    Rule(Nonterminal *nonterminal) : nonterminal(nonterminal){}

    ~Rule();

    void setNonterminal(Nonterminal *nonterminal){ this->nonterminal = nonterminal; }

    Nonterminal * getNonterminal(){ return nonterminal; }
   
    std::list<Production *> & getAlternatives(){ return alternatives; }

    std::string toString();

  };

  class LLParser{

    friend class Rule;
    friend class Production;

  protected:

    enum Inputtype { NONE, STREAM, STACK };

    //     // coombination of production and terminal for backtracking
    //     typedef std::pair<Rule *, > StackEntry;

    // result of a parse-step
    // first: true, if parse was successfull
    // second: number of read terminals in this step
    typedef std::pair<bool, unsigned long> ParseResult;

    Grammar *grammar;

    ds::Stack<Grammar::Token> tokenstack;

    bool debug;

    Inputtype inputtype;

  private:

    std::list<Grammar::Token>::iterator tStackPointer;

    ParseResult parse(LexScanner *tokenizer, Rule *rule, bool backtrack, unsigned long level) throw (exc::Exception<LLParser>);
    
    Grammar::Token nextToken(LexScanner *tokenizer) throw (exc::Exception<LexScanner>,exc::ExceptionBase);

    void putback(LexScanner *tokenizer);
    
    Grammar::Token lookAhead(LexScanner *tokenizer) throw (exc::Exception<LexScanner>, exc::ExceptionBase);

    void pushToken(Grammar::Token token);
    
    /**
       @brief restores the terminals
    */
    void restoreTerminals(unsigned long number, unsigned long level) throw (exc::Exception<LLParser>);

  public:

    LLParser(Grammar *grammar, bool debug = false) : grammar(grammar), debug(debug), inputtype(NONE){}

    bool parse(LexScanner *tokenizer) throw (exc::Exception<LLParser>);

  };

}

#endif
