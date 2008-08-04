#include <llparser.hpp>

using namespace std;
using namespace exc;
using namespace utils;
using namespace parse;

const char Terminal::TT_WORD = 1;
const char Terminal::TT_NUMBER = 2;
const char Terminal::TM_OVERREAD = 1;
const char Terminal::TM_STORE = 2;

Production::~Production(){
}

Production * Production::clone(){

  Production *production = new Production();
  production->words = words;

  return production;

}

std::string Production::toString(){

  ostringstream out;

  for ( list<ProductionElement *>::iterator it = words.begin(); it != words.end(); it++ )
    out << (*it)->getName() << " ";

  return out.str();

}

Rule::~Rule(){

  for (list<Production *>::iterator it = alternatives.begin(); it != alternatives.end(); it++ )
    delete *it;
  
}

Rule * Rule::clone(Grammar::Token lookahead) throw (Exception<LLParser>){

  static string thisMethod = "clone()";

  Rule *rule = new Rule(getNonterminal());

  for ( list<Production *>::iterator pit = alternatives.begin(); pit != alternatives.end(); pit++ ){

    if ( lookahead.first == "" ){
      // comes from the rule S'->S

      rule->getAlternatives().push_back((*pit)->clone());

    } else {

      Grammar::TaggedTerminals & director_set = (*pit)->getDirectorSet();
      Grammar::TaggedTerminals::iterator dit;

      if ( lookahead.second == LexToken::TT_WORD ){
      
	if ( (dit = director_set.find(lookahead)) != director_set.end() )
	  rule->getAlternatives().push_back((*pit)->clone());
	
      } else if ( lookahead.second == LexToken::TT_NUMBERWORD ){

	for ( dit = director_set.begin(); dit != director_set.end(); dit++ ){

	  if ( (*dit).second == Terminal::TT_NUMBER )
	    rule->getAlternatives().push_back((*pit)->clone());

	}

      } else
	throw Exception<LLParser>(thisMethod + ": wrong token-type!");
    }
    
  }

  return rule;

}

std::string Rule::toString(){

  ostringstream out;

  out << nonterminal->getName() << " -> ";

  for ( list<Production *>::iterator it = alternatives.begin(); it != alternatives.end(); it++ ){

    if ( it != alternatives.begin() )
      out << "| ";

    out << (*it)->toString();

  }

  return out.str();

}

Grammar::~Grammar(){
  
  for ( set<Rule *>::iterator it = rules.begin(); it != rules.end(); it++ )
    delete *it;

  for ( set<Terminal *>::iterator it = terminals.begin(); it != terminals.end(); it++ )
    delete *it;

  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ )
    delete *it;
  
}

std::string Grammar::toString(){

  ostringstream out;

  out << "Terminals:" << endl;
  for ( set<Terminal *>::iterator it = terminals.begin(); it != terminals.end(); it++ )
    out << (*it)->getName() << " ";

  out << endl << endl;
  out << "Nonterminals:" << endl;
  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ )
    out << (*it)->getName() << " ";

  out << endl << endl;
  out << startrule->toString() << endl << endl;

  for ( set<Rule *>::iterator it = rules.begin(); it != rules.end(); it++ )
    out << (*it)->toString() << endl;

  return out.str();

}

Grammar::Token LLParser::nextToken(LexScanner *tokenizer) throw (Exception<LexScanner>, ExceptionBase){

  if ( tStackPointer == terminalstack.end() ) {

    int scantoken = tokenizer->nextToken();
    int type = tokenizer->token.type;
    
    if ( type == LexToken::TT_WORD || type == LexToken::TT_NUMBERWORD ){

      return Grammar::Token(tokenizer->token.sval,type);

    } else if ( type == LexToken::TT_EOF || type == LexToken::TT_EOL ){

      return Grammar::Token("",type);

    } else {

      return Grammar::Token(String((char)scantoken),LexToken::TT_WORD);

    }

  } else {

    Grammar::Token token = *tStackPointer;
    tStackPointer++;
    return token;

  }

}

void LLParser::putback(LexScanner *tokenizer){

  if ( tStackPointer == terminalstack.end() )
    tokenizer->putback();
  else
    tStackPointer--;

}

Grammar::Token LLParser::lookAhead(LexScanner *tokenizer) throw (Exception<LexScanner>, ExceptionBase){

  Grammar::Token token = nextToken(tokenizer);

  putback(tokenizer);

  return token;

}

void LLParser::restoreTerminals(unsigned long number) throw (Exception<LLParser>){

  static const string thisMethod = "restoreTerminals()";

  if ( number == 0 )
    return;

  for ( unsigned long cnt = 0; cnt < number; cnt++ ){

    if ( tStackPointer == terminalstack.begin() )
      throw Exception<LLParser>(thisMethod + ": Stack exceeded!");

    tStackPointer--;

  }
}

bool LLParser::parse(LexScanner *tokenizer) throw (Exception<LLParser>){

  if ( tokenizer == 0 )
    throw Exception<LLParser>("tokenizer == 0!");

  tStackPointer = terminalstack.end();

  Rule *rule = grammar->getStartRule()->clone(Grammar::Token("",LexToken::TT_WORD));

  ParseResult result =  parse(tokenizer,rule,false);

  delete rule;
  terminalstack.clear();

  return result.first;

}

LLParser::ParseResult LLParser::parse(LexScanner *tokenizer, Rule *rule, bool backtrack) throw (Exception<LLParser>){

  unsigned long skipped_terminals_cnt = 0;
  Terminal *terminal;
  Nonterminal *nonterminal;
  Grammar::Token token;
  ParseResult result;
  bool nextAlternative;

  list<Production *> & alternatives = rule->getAlternatives();

  while ( !alternatives.empty() ){

    nextAlternative = false;

    Production *production = alternatives.front();
    alternatives.pop_front();
    list<ProductionElement *> words = production->getWords();

    while ( !words.empty() ){
      
      ProductionElement *word = words.front();
      words.pop_front();
      
      if ( (terminal = dynamic_cast<Terminal *>(word)) != 0 ){
	
	token = nextToken(tokenizer);
	
	if ( token.second == LexToken::TT_EOF ){
	  
	  putback(tokenizer);
	  return ParseResult(false,skipped_terminals_cnt);
	  
	} else if ( token.second == LexToken::TT_NUMBERWORD && terminal->getType() == Terminal::TT_NUMBER ){
	  
	  if ( backtrack ){

	    terminalstack.push_back(token);
	    skipped_terminals_cnt++;

	  }
	  
	} else if ( token.second == LexToken::TT_WORD && terminal->getType() == Terminal::TT_WORD ){
	  
	  if ( token.first == terminal->getName() ){
	    
	    if ( backtrack ){

	      terminalstack.push_back(token);
	      skipped_terminals_cnt++;

	    }
	    
	  } else {
	
	    putback(tokenizer);
	    return ParseResult(false,skipped_terminals_cnt);

	  }	    
	}

      } else if ( (nonterminal = dynamic_cast<Nonterminal *>(word)) != 0 ){

	Rule *rule = nonterminal->getRule()->clone(lookAhead(tokenizer));

	// the recursion
	result = parse(tokenizer,rule,(backtrack || !alternatives.empty()));
	delete rule;

	skipped_terminals_cnt += result.second;
	
	if ( result.first == false ){

	  restoreTerminals(skipped_terminals_cnt);
	  skipped_terminals_cnt = 0;
	  nextAlternative = true;
	  break;

	}

      }      
    }

    if ( nextAlternative == false )
      return ParseResult(true,skipped_terminals_cnt);
    
  }

  return ParseResult(false,skipped_terminals_cnt);
  
}
