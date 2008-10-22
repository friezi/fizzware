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
#include <graph.hpp>

namespace parse{

  class GrammarSymbol;
  class Terminal;
  class Nonterminal;
  class Production;
  class Rule;
  class LLParser;
  class Grammar;

  enum Nullability{ NL_NONE, NL_IS_NULLABLE, NL_IS_NOT_NULLABLE };

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
       Calculates the direct first set and checks for nullability of each nonterminal. Detects
       leftrecursion and throws an exception in this case
       @brief direct first, nullability, left recursion
       @throw Exception<Grammar> in case of detected left recursion
    */
    void calculateDFNL() throw (exc::Exception<Grammar>, exc::ExceptionBase);

  protected:

    Nonterminal * findNonterminal(std::string nonterminal);

    Terminal * findTerminal(std::string terminal);

    /**
       helper function for calculateDFNL()
    */
    void traverseDFNL(Rule *rule) throw (exc::Exception<Grammar>, exc::ExceptionBase);

  };

  class Production{

    friend class Rule;

  protected:

    std::list<GrammarSymbol *> symbols;

    std::set<Terminal *> director_set;

    Production * clone();

  public:

    Production(){}

    ~Production();

    std::list<GrammarSymbol *> & getSymbols(){ return symbols; }

    std::set<Terminal *> & getDirectorSet(){ return director_set; }

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

  protected:

    // name of nonterminal
    std::string name;

    // reference is stored in Grammar and will be deleted from there
    Rule * rule;

  public:

    Nonterminal(std::string name) : name(name), rule(0){}

    std::string getName(){ return name; }

    void setRule(Rule *rule){ this->rule = rule; }

    Rule * getRule(){ return rule; }

  };

  class Rule{

    friend class LLParser;
    friend class Grammar;

  private:

    bool visited;

    bool circlefree;
 
    Nullability nullable;
   
  protected:

    // one for each component only
    std::set<Terminal *> *first_set;

    // one for each component only
    std::set<Terminal *> *follow_set;

    Nonterminal * nonterminal;

    std::list<Production *> alternatives;

    // one for each rule
    std::set<Terminal *> direct_first_set;

  public:

    Rule() : visited(false), circlefree(false), nullable(NL_NONE), first_set(0), follow_set(0), nonterminal(0){}

    Rule(Nonterminal *nonterminal) : visited(false), circlefree(false), nullable(NL_NONE), first_set(0), follow_set(0), nonterminal(nonterminal){}

    ~Rule();

    void setNonterminal(Nonterminal *nonterminal){ this->nonterminal = nonterminal; }

    Nonterminal * getNonterminal(){ return nonterminal; }
   
    std::list<Production *> & getAlternatives(){ return alternatives; }

    std::string toString();

    std::set<Terminal *> & getDirectFirstSet(){ return direct_first_set; }

    std::set<Terminal *> * getFirstSet(){ return first_set; }

    std::set<Terminal *> * getFollowSet(){ return follow_set; }

    void setFirstSet(std::set<Terminal *> *first_set){ this->first_set = first_set; }

    void setFollowSet(std::set<Terminal *> *follow_set){ this->follow_set = follow_set; }

    void setNullable(Nullability val){ nullable = val; }

    bool isNullable(){ return nullable == NL_IS_NULLABLE; }

  protected:

    Rule * clone(Grammar::Token lookahead) throw (exc::Exception<LLParser>);

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

//   class FirstSetNodeIterator : public graph::abstract_node_iterator{
//   };

//   class FirstSetNeighbourIterator : public graph::abstract_node_iterator{
//   };

//   class FirstSetGraph : public graph::Graphable{
//   };

}

#endif
