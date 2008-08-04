#ifndef _MAIN_TEST_HPP_
#define _MAIN_TEST_HPP_

#include <iostream>
#include <exception.hpp>
#include <llparser.hpp>
#include <test.hpp>

using namespace test;
using namespace exc;
using namespace std;
using namespace parse;

class LLTest : public TestCase<LLTest>{

public:

  LLTest() : TestCase<LLTest>(){
    
    addTest(&LLTest::testGrammar1,"testGrammar1");
    
  }
  
  void testGrammar1() throw (ExceptionBase){
    
    Grammar grammar;

    Terminal *terminal;
    Nonterminal *nonterminal;
    Nonterminal *start;
    Production *production;
    Rule *rule;

    production = new Production();

    terminal = new Terminal("a",Terminal::TT_WORD,Terminal::TM_OVERREAD);
    grammar.getTerminals().insert(terminal);

    nonterminal = new Nonterminal("S");
    start = nonterminal;
    grammar.getNonterminals().insert(nonterminal);

    rule = new Rule(nonterminal);

    production->getWords().push_back(terminal);
    production->getWords().push_back(nonterminal);

    rule->getAlternatives().push_back(production);
    production = new Production();

    terminal = new Terminal("b",Terminal::TT_WORD,Terminal::TM_OVERREAD);
    grammar.getTerminals().insert(terminal);

    production->getWords().push_back(terminal);

    nonterminal->setRule(rule);
    rule->getAlternatives().push_back(production);

    grammar.getRules().insert(rule);

    nonterminal = new Nonterminal("");
    grammar.getNonterminals().insert(nonterminal);
    rule = new Rule(nonterminal);
    nonterminal->setRule(rule);
    production = new Production();
    production->getWords().push_back(start);
    rule->getAlternatives().push_back(production);
    
    grammar.setStartRule(rule);

    cout << grammar.toString() << endl;

  }
  
};

#endif
