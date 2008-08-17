#ifndef _MAIN_TEST_HPP_
#define _MAIN_TEST_HPP_

#include <iostream>
#include <list>
#include <exception.hpp>
#include <llparser.hpp>
#include <lex.hpp>
#include <test.hpp>

using namespace test;
using namespace exc;
using namespace std;
using namespace parse;

class LLTest : public TestCase<LLTest>{

public:

  Grammar grammar;

  LLTest() : TestCase<LLTest>(){
    
//     addTest(&LLTest::testGrammar1,"testGrammar1");
//     addTest(&LLTest::test1Grammar,"test1Grammar");
//     addTest(&LLTest::test2Grammar,"test2Grammar");
    addTest(&LLTest::test3Grammar,"test3Grammar");
    
  }

  void setUp(){

    grammar.setStart("S");

    ((((((((((grammar.rule("S") << "a",'T','W') << "b",'T','W') << "A",'N') << "A",'N') | "c",'T','W') << "A",'N')
	 << "g",'T','W') | "c",'T','W') << "g",'T','W') << "1",'T','W') << "a",'T','W';
    ((((grammar.rule("A") << "end",'T','W') | "end",'T','W') << "A",'N') | "B",'N') | grammar.lambda();
    grammar.rule("B") << grammar.lambda();

    //   ((grammar.rule("S") << "a",'T','W') << "b",'T','W');
    cout << grammar.toString() << endl;

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

  void test1Grammar() throw (ExceptionBase){

    stringstream text;
    text << 'a' << 'b';
    cout << "word: " << text.str() << endl;

    LexScanner tokenizer(&text);
    tokenizer.getLexCharClasses().resetSyntax();
    
    LLParser llparser(&grammar,true);

    assertTrue(llparser.parse(&tokenizer));
    cout << endl << endl;

  }

  void test2Grammar() throw (ExceptionBase){

    stringstream text;
    text << 'c' << 'g';
    cout << "word: " << text.str() << endl;

    LexScanner tokenizer(&text);
    tokenizer.getLexCharClasses().resetSyntax();
    
    LLParser llparser(&grammar,true);

    assertTrue(llparser.parse(&tokenizer));
    cout << endl << endl;

  }

  void test3Grammar() throw (ExceptionBase){

//     stringstream text;
//     text << 'c' << 'g' << '1' << 'a';
//     cout << "word: " << text.str() << endl;

//     LexScanner tokenizer(&text);
//     tokenizer.getLexCharClasses().resetSyntax();
    
//     LLParser llparser(&grammar,true);

//     assertTrue(llparser.parse(&tokenizer));
//     cout << endl << endl;

    list<int> l;

    list<int>::iterator it = l.begin();

    it--;

    assertTrue(it == l.end());
    cout << "hier1\n";
    l.push_back(3);
    it = l.begin();
    it--;
    cout << "hier2\n";    
    assertTrue(it == l.end());

    cout << *it << endl;

  }
  
};

#endif
