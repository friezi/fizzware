#ifndef __FZTT_LLPARSER_HPP__
#define __FZTT_LLPARSER_HPP__

#include <iostream>
#include <utility>
#include <list>
#include <set>
#include <exception.hpp>
#include <lex.hpp>

namespace parse{

  class Rule;
  class LLParser;

  class ProductionElement{
    
  public:
    
    virtual ~ProductionElement() = 0;
    
  };

  class Terminal : public ProductionElement{

  protected:

    std::string name;

  public:

    Terminal(std::string name) : name(name){}

    std::string getName(){ return name; }
  
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

  class Production{

    friend class Rule;

  protected:

    std::list<ProductionElement *> words;

    std::set<std::string> director_set;

    Production * clone();

  public:

    ~Production();

    std::list<ProductionElement *> & getWords(){ return words; }

    std::set<std::string> & getDirectorSet(){ return director_set; }
  
  };

  class Rule{

    friend class LLParser;
    
  protected:

    Nonterminal * nonterminal;

    std::list<Production *> alternatives;

  protected:

    Rule * clone(std::string lookahead);

  public:

    Rule(Nonterminal *nonterminal) : nonterminal(nonterminal){}

    ~Rule();

    Nonterminal * getNonterminal(){ return nonterminal; }
   
    std::list<Production *> & getAlternatives(){ return alternatives; }

    

  };

  class Grammar{

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

    /**
       @brief returns the pseudostartrule S'->S
    */
    Rule * getStartRule(){ return startrule; }

    std::set<Rule *> & getRules(){ return rules; }

  };

  class LLParser{

  protected:

    // coombination of production and terminal for backtracking
    typedef std::pair<Rule *, std::string> StackEntry;

    Grammar *grammar;

    std::list<StackEntry> stack;

    unsigned long backtrack_count;

    std::list<std::string> pushed_back_terminals;

    void reset();

  public:

    LLParser(Grammar *grammar) : grammar(grammar){}

    bool parse(Tokenizer *tokenizer) throw (exc::Exception<LLParser>);

  };

}

#endif
