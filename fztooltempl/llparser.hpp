#ifndef __FZTT_LLPARSER_HPP__
#define __FZTT_LLPARSER_HPP__

#include <iostream>
#include <sstream>
#include <utility>
#include <list>
#include <set>
#include <map>
#include <algorithm>
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
  class FirstSetCollector;
  class FollowSetCollector;
  class FollowSetGraph;

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

    std::set< std::set<Terminal *> *> first_sets;

    std::set< std::set<Terminal *> *> follow_sets;

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

    std::set< std::set<Terminal *> *> & getFirstSets(){ return first_sets; }

    std::set< std::set<Terminal *> *> & getFollowSets(){ return follow_sets; }

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
       leftrecursion and throws an exception in this case. Will be called by prepare().
       @brief direct first, nullability, left recursion
       @throw Exception<Grammar> in case of detected left recursion
    */
    void calculateDFNL() throw (exc::Exception<Grammar>, exc::ExceptionBase);

    /**
       Calculates the first-sets for all rules. Will be called by prepare().
       @brief first-set calculation
       @pre calculateDFNL() must have been called already
       @throw Exception<Grammar>
    */
    void calculateFirstSets() throw (exc::Exception<Grammar>, exc::ExceptionBase);

    /**
       Calculates the follow-sets for all rules. Will be called by prepare().
       @brief follow-set calculation
       @pre calculateFirstSets() must have been called already
       @throw Exception<Grammar>
    */
    void calculateFollowSets() throw (exc::Exception<Grammar>, exc::ExceptionBase);

    /**
       Calculates the director-sets for all productions. Will be called by prepare().
       @brief director-set calculation
       @pre calculateFollowSets() must have been called already
       @throw Exception<Grammar>
    */
    void calculateDirectorSets() throw (exc::Exception<Grammar>, exc::ExceptionBase);

    /**
       Prepares the grammar for parsing, i. e. calculates all relevant sets.
    */
    void prepare() throw (exc::Exception<Grammar>, exc::ExceptionBase);

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

    Nullability nullable;

    std::list<GrammarSymbol *> symbols;

    std::set<Terminal *> director_set;

    Production * clone();

  public:

    Production() : nullable(NL_NONE){}

    ~Production();

    void setNullable(Nullability val){ nullable = val; }

    bool isNullable(){ return nullable == NL_IS_NULLABLE; }
    
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
    friend class FirstSetCollector;
    friend class FollowSetCollector;

  private:

    // ------------- for temprary management only
    bool visited;
    // -------------

    bool circlefree;
 
    Nullability nullable;
   
  protected:

    // one for each component only
    std::set<Terminal *> *first_set;

    // one for each component only
    std::set<Terminal *> *follow_set;

    Nonterminal * nonterminal;

    std::list<Production *> alternatives;

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

    bool debug;

    Inputtype inputtype;

    ds::Stack<Grammar::Token> tokenstack;

  private:

    ds::Stack<Grammar::Token>::iterator tStackPointer;

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

  class FirstSetGraph : public graph::Graphable<Rule *>{

  private:

    class FirstSetNodeIterator : public graph::abstract_node_iterator<Rule *>{

    private:
    
      std::set<Rule *>::iterator it;

    public:

      FirstSetNodeIterator(std::set<Rule *>::iterator it);

      Rule * operator*() throw(exc::ExceptionBase);

      void operator++(int) throw(exc::ExceptionBase);

      bool operator==(const graph::abstract_node_iterator<Rule *> *it_rval) throw(exc::ExceptionBase);   

    };

    class FirstSetNeighbourIterator : public graph::abstract_node_iterator<Rule *>{

    private:

      Rule *rule;

      bool at_end;

      std::list<Production *>::iterator alternatives_iterator;

      std::list<GrammarSymbol *>::iterator symbols_iterator;

      void init();

      void setToNextNonterminal() throw (exc::Exception<FirstSetNeighbourIterator>);

      void selectRuleWithStartingNonterminal();

    public:

      FirstSetNeighbourIterator(Rule *rule, bool at_end);

      Rule * operator*() throw(exc::ExceptionBase);

      void operator++(int) throw(exc::ExceptionBase);

      bool operator==(const graph::abstract_node_iterator<Rule *> *it_rval) throw(exc::ExceptionBase);   
   
    };

    Grammar & grammar;

  public:

    FirstSetGraph(Grammar & grammar);

    graph::abstract_node_iterator<Rule *> * beginNodesPtr();

    graph::abstract_node_iterator<Rule *> * endNodesPtr();

    graph::abstract_node_iterator<Rule *> * beginNeighboursPtr(Rule *rule);

    graph::abstract_node_iterator<Rule *> * endNeighboursPtr(Rule *rule);

    size_t maxNodes();

    Grammar & getGrammar(){ return grammar; }

  };

  class FirstSetCollector : public graph::SCCProcessor<Rule *>{

  private:

    typedef std::set<Terminal *> FirstSet;
    typedef std::set<FirstSet *> FirstSets;
    typedef std::map< FirstSet *, FirstSets * > IncludedFirstSets;

    // for the current first_set
    FirstSet *first_set;

    // stores the included first_sets for each first_set
    IncludedFirstSets included_first_sets_map;
    
  public:

    FirstSetCollector(FirstSetGraph & graph);

    ~FirstSetCollector();

    void prepareFind() throw (exc::ExceptionBase){}
    
    void processComponent() throw (exc::ExceptionBase);
    
    void processComponentNode(Rule *rule) throw (exc::ExceptionBase);
    
  };

  class FollowGraphNode{

    friend class FollowSetGraph;
    friend class FollowSetCollector;

  protected:

    GrammarSymbol *symbol;

    std::set<FollowGraphNode *> neighbours;

    FollowGraphNode(GrammarSymbol *symbol) : symbol(symbol){}

    virtual ~FollowGraphNode(){}

  public:

    GrammarSymbol * getSymbol(){ return symbol; }

  };

  class FollowNode : public FollowGraphNode{

  public:

    FollowNode(GrammarSymbol *symbol) : FollowGraphNode(symbol){}

  };

  class FirstNode : public FollowGraphNode{

  public:

    FirstNode(GrammarSymbol *symbol) : FollowGraphNode(symbol){}

  };

  // for both of node_iterator and neighbour_iterator
  class FollowSetGraph : public graph::Graphable<FollowGraphNode *>{

  private:

    class FollowSetNodeIterator : public graph::abstract_node_iterator<FollowGraphNode *>{

    private:
    
      std::set<FollowGraphNode *>::iterator it;

    public:

      FollowSetNodeIterator(std::set<FollowGraphNode *>::iterator it);

      FollowGraphNode * operator*() throw(exc::ExceptionBase);

      void operator++(int) throw(exc::ExceptionBase);

      bool operator==(const graph::abstract_node_iterator<FollowGraphNode *> *it_rval) throw(exc::ExceptionBase);   

    };
   
    Grammar & grammar;
    
    std::set<FollowGraphNode *> nodes;

    // ------------------ for temporary management
    std::map<GrammarSymbol *, FirstNode *> first_map;
    
    std::map<GrammarSymbol *, FollowNode *> follow_map;
    // ------------------------------------------

    FollowNode * getFollowNode(GrammarSymbol *symbol);

    FirstNode * getFirstNode(GrammarSymbol *symbol);

  public:

    FollowSetGraph(Grammar & grammar);
    
    ~FollowSetGraph();

    graph::abstract_node_iterator<FollowGraphNode *> * beginNodesPtr();

    graph::abstract_node_iterator<FollowGraphNode *> * endNodesPtr();

    graph::abstract_node_iterator<FollowGraphNode *> * beginNeighboursPtr(FollowGraphNode *node);

    graph::abstract_node_iterator<FollowGraphNode *> * endNeighboursPtr(FollowGraphNode *node);

    size_t maxNodes();

    Grammar & getGrammar(){ return grammar; }

    void constructGraph();

    FollowNode * newFollowNode(GrammarSymbol *symbol);
    
    FirstNode * newFirstNode(GrammarSymbol *symbol);

  };

  class FollowSetCollector : public graph::SCCProcessor<FollowGraphNode *>{

  private:

    typedef std::set<Terminal *> FollowSet;

    // current follow_set
    FollowSet *follow_set;
    
  public:

    FollowSetCollector(FollowSetGraph & graph);

    ~FollowSetCollector(){}

    void prepareFind() throw (exc::ExceptionBase){}
    
    void processComponent() throw (exc::ExceptionBase);
    
    void processComponentNode(FollowGraphNode *node) throw (exc::ExceptionBase);

  };

}

#endif
