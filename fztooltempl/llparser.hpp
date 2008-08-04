#ifndef __FZTT_LLPARSER_HPP__
#define __FZTT_LLPARSER_HPP__

#include <iostream>
#include <sstream>
#include <utility>
#include <list>
#include <set>
#include <exception.hpp>
#include <utils.hpp>
#include <lex.hpp>

namespace parse{

  class ProductionElement;
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

    std::set<Rule *> rules;

    std::set<Terminal *> terminals;

    std::set<Nonterminal *> nonterminals;

  public:

    ~Grammar();

    void setStartRule(Rule *rule){ this->startrule = rule; }

    /**
       @brief returns the pseudostartrule S'->S
    */
    Rule * getStartRule(){ return startrule; }

    std::set<Rule *> & getRules(){ return rules; }

    std::set<Terminal *> & getTerminals(){ return terminals; }

    std::set<Nonterminal *> & getNonterminals(){ return nonterminals; }

    std::string toString();

  };

  class Production{

    friend class Rule;

  protected:

    std::list<ProductionElement *> words;

    Grammar::TaggedTerminals director_set;

    Production * clone();

  public:

    ~Production();

    std::list<ProductionElement *> & getWords(){ return words; }

    Grammar::TaggedTerminals & getDirectorSet(){ return director_set; }

    std::string toString();
  
  };
  
  class ProductionElement{
    
  public:
    
    virtual ~ProductionElement(){}

    virtual std::string getName() = 0;
    
  };

  class Terminal : public ProductionElement{

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
  
  };

  class Nonterminal : public ProductionElement{

  protected:

    // name of nonterminal
    std::string name;

    // reference is stored in Grammar and will be deleted from there
    Rule * rule;

  public:

    Nonterminal(std::string name) : name(name){}

    std::string getName(){ return name; }

    void setRule(Rule *rule){ this->rule = rule; }

    Rule * getRule(){ return rule; }

  };

  class Rule{

    friend class LLParser;
    
  protected:

    Nonterminal * nonterminal;

    std::list<Production *> alternatives;

  protected:

    Rule * clone(Grammar::Token lookahead) throw (exc::Exception<LLParser>);

  public:

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

    //     // coombination of production and terminal for backtracking
    //     typedef std::pair<Rule *, > StackEntry;

    // result of a parse-step
    // first: true, if parse was successfull
    // second: number of read terminals in this step
    typedef std::pair<bool, unsigned long> ParseResult;

    Grammar *grammar;

    std::list<Grammar::Token> terminalstack;

  private:

    std::list<Grammar::Token>::iterator tStackPointer;

    ParseResult parse(LexScanner *tokenizer, Rule *rule, bool backtrack) throw (exc::Exception<LLParser>);
    
    Grammar::Token nextToken(LexScanner *tokenizer) throw (exc::Exception<LexScanner>,exc::ExceptionBase);

    void putback(LexScanner *tokenizer);
    
    Grammar::Token lookAhead(LexScanner *tokenizer) throw (exc::Exception<LexScanner>, exc::ExceptionBase);
    
    /**
       @brief restores the terminals
    */
    void restoreTerminals(unsigned long number) throw (exc::Exception<LLParser>);

  public:

    LLParser(Grammar *grammar) : grammar(grammar){}

    bool parse(LexScanner *tokenizer) throw (exc::Exception<LLParser>);

  };

}

#endif
