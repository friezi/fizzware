#ifndef __FZTT_LLPARSER_HPP__
#define __FZTT_LLPARSER_HPP__

#include <iostream>
#include <utility>
#include <list>
#include <set>

namespace parse{

  class Rule;

  class ProductionElement{
  };

  class Terminal : public ProductionElement{

  public:

    std::string name;
  
  };

  class Nonterminal : public ProductionElement{

  public:

    // reference is stored in Grammar and will be deleted from there
    Rule * rule;

  };

  class Production{

  public:

    std::list<ProductionElement *> words;

    ~Production();
  
  };

  class Rule{

  protected:

    // combination of an alternative with the corresponding director set
    typedef std::pair< Production *, std::set<std::string> * > Alternative;

    // name of rule, i.e. left hand side
    std::string name;

    std::list<Alternative> alternatives;

  public:

    ~Rule();

  };

  class Grammar{

  public:

    Rule *startrule;

    std::set<Rule *> rules;

    ~Grammar();

  };

  class LLParser{

  protected:

    Grammar * grammar;

  };

}

#endif
