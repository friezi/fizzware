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
    //     addTest(&LLTest::test3Grammar,"test3Grammar");
    //     addTest(&LLTest::test4Grammar,"test4Grammar");
    //     addTest(&LLTest::nullabilityGrammar,"nullabilityGrammar");
    addTest(&LLTest::leftrecursionGrammar,"leftrecursionGrammar");
    
  }

  void setUp(){

    grammar.setStart("S");

    (((((((((((grammar.rule("S") << "a",'T','W') << "b",'T','W') << "A",'N') << "A",'N') | "C",'N') << "A",'N')
	 << "g",'T','W') << "v",'T','W') | "c",'T','W') << "g",'T','W') << "1",'T','W') << "a",'T','W';
    (((grammar.rule("A") << "end",'T','W') | "end",'T','W') << "A",'N');
    ((grammar.rule("S") << "C",'N') << "B",'N') << "hue",'T','W'; 
    //grammar.lambda(); // muss eine Zeile tiefer stehen, da sonst zu frueh ausgewertet (warum ist noch nicht klar)
    grammar.rule("B") << "123",'T','W';
    grammar.rule("B").lambda();
    (grammar.rule("C") << "x",'T','W') | "y",'T','W';
    grammar.lambda();

    //   ((grammar.rule("S") << "a",'T','W') << "b",'T','W');
    cout << "Grammar:" << endl << grammar.toString() << endl;

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

    production->getSymbols().push_back(terminal);
    production->getSymbols().push_back(nonterminal);

    rule->getAlternatives().push_back(production);
    production = new Production();

    terminal = new Terminal("b",Terminal::TT_WORD,Terminal::TM_OVERREAD);
    grammar.getTerminals().insert(terminal);

    production->getSymbols().push_back(terminal);

    nonterminal->setRule(rule);
    rule->getAlternatives().push_back(production);

    grammar.getRules().insert(rule);

    nonterminal = new Nonterminal("");
    grammar.getNonterminals().insert(nonterminal);
    rule = new Rule(nonterminal);
    nonterminal->setRule(rule);
    production = new Production();
    production->getSymbols().push_back(start);
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
    text << 'c' << 'g' << 'v';
    cout << "word: " << text.str() << endl;

    LexScanner tokenizer(&text);
    tokenizer.getLexCharClasses().resetSyntax();
    
    LLParser llparser(&grammar,true);

    assertTrue(llparser.parse(&tokenizer));
    cout << endl << endl;

  }

  void test3Grammar() throw (ExceptionBase){

    stringstream text;
    text << 'c' << 'g' << '1' << 'a';
    cout << "word: " << text.str() << endl;

    LexScanner tokenizer(&text);
    tokenizer.getLexCharClasses().resetSyntax();
    
    LLParser llparser(&grammar,true);

    assertTrue(llparser.parse(&tokenizer));

  }

  void test4Grammar() throw (ExceptionBase){

    stringstream text;
    text << 'c' << 'g' << 'b';
    cout << "word: " << text.str() << endl;

    LexScanner tokenizer(&text);
    tokenizer.getLexCharClasses().resetSyntax();
    
    LLParser llparser(&grammar,true);

    assertFalse(llparser.parse(&tokenizer));

  }

  //   void nullabilityGrammar() throw (ExceptionBase){

  //     grammar.calculateNDF();
    
  //   }

  void leftrecursionGrammar() throw (ExceptionBase){

    //     grammar.calculateNDF();
    grammar.calculateDFNL();

    cout << endl << "Nullability and direct first:" << endl;

    for ( set<Rule *>::iterator it = grammar.getRules().begin(); it != grammar.getRules().end(); it++ ){

      cout << (*it)->getNonterminal()->getName() << ": " << (*it)->isNullable() << "\t";
      for ( set<Terminal *>::iterator fit = (*it)->getDirectFirstSet().begin(); fit != (*it)->getDirectFirstSet().end(); fit++ )
	cout << (*fit)->getName() << " ";
      cout << endl;
    }

    grammar.calculateFirstSets();

    cerr << endl << "firstsets:" << endl;
    for ( set<Rule *>::iterator rit = grammar.getRules().begin(); rit != grammar.getRules().end(); rit++ ){
     
      cerr << (*rit)->getNonterminal()->getName() << ": ";

      for ( set<Terminal *>::iterator tit = (*rit)->getFirstSet()->begin(); tit != (*rit)->getFirstSet()->end(); tit++ ){
	cerr << (*tit)->getName() << "  ";
      }
      cerr << endl;
    }

    grammar.calculateFollowSets();

    cerr << endl << "followsets:" << endl;
    for ( set<Rule *>::iterator rit = grammar.getRules().begin(); rit != grammar.getRules().end(); rit++ ){
     
      cerr << (*rit)->getNonterminal()->getName() << ": ";

      for ( set<Terminal *>::iterator tit = (*rit)->getFollowSet()->begin(); tit != (*rit)->getFollowSet()->end(); tit++ ){
	cerr << (*tit)->getName() << "  ";
      }
      cerr << endl;
    }

    grammar.calculateDirectorSets();

    cerr << endl << "directorsets:" << endl;
    for ( set<Rule *>::iterator rit = grammar.getRules().begin(); rit != grammar.getRules().end(); rit++ ){
      for ( list<Production *>::iterator pit = (*rit)->getAlternatives().begin(); pit != (*rit)->getAlternatives().end(); pit++ ){
     
	cerr << (*rit)->getNonterminal()->getName() << " -> " << (*pit)->toString()<< ": ";

	for ( set<Terminal *>::iterator tit = (*pit)->getDirectorSet().begin(); tit != (*pit)->getDirectorSet().end(); tit++ ){
	  cerr << (*tit)->getName() << "  ";
	}
	cerr << endl;
      }
    }
    
  }
  
};

#endif
