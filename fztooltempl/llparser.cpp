#include <fztooltempl/llparser.hpp>

using namespace std;
using namespace exc;
using namespace utils;
using namespace ds;
using namespace graph;
using namespace parse;

const char Terminal::TT_WORD = 1;
const char Terminal::TT_NUMBER = 2;
const char Terminal::TM_OVERREAD = 1;
const char Terminal::TM_STORE = 2;

Production::~Production(){
}

Production * Production::clone(){

  Production *production = new Production();
  production->symbols = symbols;

  return production;

}

std::string Production::toString(){

  ostringstream out;

  if ( symbols.empty() ){

    out << "[]";  // lambda

  } else {
    
    for ( list<GrammarSymbol *>::iterator it = symbols.begin(); it != symbols.end(); it++ )
      out << (*it)->toString() << " ";
    
  }

  return out.str();

}

Rule::~Rule(){

  for (list<Production *>::iterator it = alternatives.begin(); it != alternatives.end(); it++ )
    delete *it;
  
}

bool Rule::isDisjointDirectorSets(){

  set<Terminal *> terminals;

  for ( list<Production *>::iterator pit = alternatives.begin(); pit != alternatives.end(); pit++ ){
    for ( set<Terminal *>::iterator tit = (*pit)->getDirectorSet().begin(); tit != (*pit)->getDirectorSet().end(); tit++ ){
      if ( terminals.insert(*tit).second == false ){
	return false;
      }
    }
  }

  return true;
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

  for ( set< set<Terminal *> *>::iterator it = first_sets.begin(); it != first_sets.end(); it++ )
    delete *it;

  for ( set< set<Terminal *> *>::iterator it = follow_sets.begin(); it != follow_sets.end(); it++ )
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
  out << "Productions:" << endl;
  out << startrule->toString() << endl << endl;

  for ( set<Rule *>::iterator it = rules.begin(); it != rules.end(); it++ )
    out << (*it)->toString() << endl;

  return out.str();

}

Nonterminal * Grammar::findNonterminal(string nonterminal){

  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ )
    if ( (*it)->getName() == nonterminal )
      return *it;

  return 0;

}

Terminal * Grammar::findTerminal(string terminal){

  for ( set<Terminal *>::iterator it = terminals.begin(); it != terminals.end(); it++ )
    if ( (*it)->getName() == terminal )
      return *it;

  return 0;

}

void Grammar::setStart(string nonterminal) throw(Exception<Grammar>){

  if ( startrule != 0 )
    throw Exception<Grammar>("startrule double-defined!");

  Nonterminal *start = new Nonterminal(nonterminal);
  getNonterminals().insert(start);

  Nonterminal *metastart = new Nonterminal("");
  getNonterminals().insert(metastart);
  Rule *rule = new Rule(metastart);
  metastart->setRule(rule);
  Production *production = new Production();
  production->getSymbols().push_back(start);
  rule->getAlternatives().push_back(production);
    
  setStartRule(rule);

  defmode = 0;

}

Grammar & Grammar::rule(string nonterminal){

  Nonterminal *nt;

  if ( (nt = findNonterminal(nonterminal)) == 0 ){

    nt = new Nonterminal(nonterminal);

    Rule *rule = new Rule(nt);
    rules.insert(rule);

    nt->setRule(rule);
    nonterminals.insert(nt);
    
  } else {

    if ( nt->getRule() == 0 ){
 
      Rule *rule = new Rule(nt);
      rules.insert(rule);
      nt->setRule(rule);
      
    }   
  }
 
  lastAccessedRule = nt->getRule(); 
  lastAccessedProduction = 0;
  lastGrammarSymbol = "";
  lastTerminal = 0;
  lastNonterminal = 0;
  return *this;
  
}

Grammar & Grammar::operator<<(string gsym) throw(Exception<Grammar>){

  if ( lastAccessedRule == 0 )
    throw Exception<Grammar>("no rule defined!");

  if ( lastAccessedProduction == 0 ){

    lastAccessedProduction = new Production();
    lastAccessedRule->getAlternatives().push_back(lastAccessedProduction);

  }

  lastGrammarSymbol = gsym;
  defmode = 'P';
  return *this;
}

Grammar & Grammar::operator<<(Grammar & grammar) throw(Exception<Grammar>){
  return grammar;
}
  

Grammar & Grammar::operator,(char type) throw(Exception<Grammar>){

  if ( defmode == 'P' ){

    if ( type == 'T' ){

      // Terminal
      
      // DANGER!!!!! please check type as well ind findTerminal()!!!!!
      if ( (lastTerminal = findTerminal(lastGrammarSymbol)) == 0 ){
	
	lastTerminal = new Terminal(lastGrammarSymbol);
	terminals.insert(lastTerminal);
	
      }
      
      lastAccessedProduction->getSymbols().push_back(lastTerminal);

      defmode = 'T';

    } else if ( type == 'N' ){
    
      Nonterminal *nt;

      if ( (nt = findNonterminal(lastGrammarSymbol)) == 0 ){
      
	// Nonterminal
	lastNonterminal = new Nonterminal(lastGrammarSymbol);
	nonterminals.insert(lastNonterminal);
	lastAccessedProduction->getSymbols().push_back(lastNonterminal);
      
      } else {

	lastNonterminal = nt;
	lastAccessedProduction->getSymbols().push_back(lastNonterminal);

      }

      defmode = 0;
    
    } else{
      
      ostringstream out;
      (out << "wrong PE-type \"").put(type) << "\" for element \"" << lastGrammarSymbol << "\"";
      throw Exception<Grammar>(out.str());
      
    }
    
  } else if ( defmode == 'T' ){

    if ( lastTerminal == 0 )
      throw new Exception<Grammar>("wrong calling-syntax for opertor \";\"!");


    // DANGER!!!!! please check type as well ind findTerminal()!!!!!
    if ( type == 'N' ){

      // NUMBER
      lastTerminal->setType(Terminal::TT_NUMBER);

    } else if ( type == 'W' ){

      // WORD
      lastTerminal->setType(Terminal::TT_WORD);

    } else
      throw Exception<Grammar>("wrong type, must be \'N\' or \'W\'!");

    defmode = 0;

  } else
    throw Exception<Grammar>("wrong calling-syntax for GrammarSymbols-types!");
  
  return *this;

}

void Grammar::newProduction() throw(exc::Exception<Grammar>){

  if ( lastAccessedRule == 0 )
    throw Exception<Grammar>("no rule defined!");

  lastAccessedProduction = new Production();
  lastAccessedRule->getAlternatives().push_back(lastAccessedProduction);

}

Grammar & Grammar::operator|(string gsym) throw(Exception<Grammar>){
  
  newProduction();
  
  lastGrammarSymbol = gsym;

  defmode = 'P';

  return *this;
}

Grammar & Grammar::operator|(Grammar &grammar) throw(Exception<Grammar>){

  defmode = 'P';

  return grammar;
}

Grammar & Grammar::lambda() throw(Exception<Grammar>){

  newProduction();

  defmode = 0;

  return *this;
}

void Grammar::prepare() throw (Exception<Grammar>, ExceptionBase){

  calculateDFNL();
  calculateFirstSets();
  calculateFollowSets();
  calculateDirectorSets();
  calculateLL1Property();

}

void Grammar::calculateDFNL() throw (Exception<Grammar>, exc::ExceptionBase){

  Rule *rule;

  // init
  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ ){

    rule = (*it)->getRule();
    rule->visited = false;
    rule->circlefree = false;

  }
  
  // select each nonterminal
  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ ){

    rule = (*it)->getRule();

    if ( rule->visited == true )
      continue;

    traverseDFNL(rule);
    
  }
}

void Grammar::traverseDFNL(Rule *rule) throw (Exception<Grammar>, ExceptionBase){

  Terminal *terminal;
  Nonterminal *next_nonterminal;
  Rule *next_rule;
  list<Production *> & alternatives = rule->getAlternatives();

  if ( rule->visited == true ){

    if ( rule->circlefree == false ){

      leftrecursive = true;
      throw Exception<Grammar>("Grammar left recursive!");

    }
    else
      return;
    
  }

  rule->visited = true;

  for ( list<Production *>::iterator ait = alternatives.begin(); ait != alternatives.end(); ait++ ){

    list<GrammarSymbol *> & symbols = (*ait)->getSymbols();

    if ( symbols.empty() ){

      (*ait)->setNullable(NL_IS_NULLABLE);
      rule->setNullable(NL_IS_NULLABLE);
      continue;

    }
    
    list<GrammarSymbol *>::iterator sit;
    for ( sit = symbols.begin(); sit != symbols.end(); sit++ ){
      
      if ( (terminal = dynamic_cast<Terminal *>(*sit)) != 0 ){

	rule->getDirectFirstSet().insert(terminal);
	(*ait)->setNullable(NL_IS_NOT_NULLABLE);
	break;
	
      } else if ( (next_nonterminal = dynamic_cast<Nonterminal *>(*sit)) != 0 ){

	next_rule = next_nonterminal->getRule();

	traverseDFNL(next_rule);

	if ( next_rule->nullable == NL_IS_NOT_NULLABLE ){

	  (*ait)->setNullable(NL_IS_NOT_NULLABLE);
	  break;

	}
	
      } else
	throw Exception<LLParser>("internal Error: GrammarSymbol neither Terminal nor Nonterminal!");
    }

    if ( sit == symbols.end() ){

      (*ait)->setNullable(NL_IS_NULLABLE);
      rule->setNullable(NL_IS_NULLABLE);

    }
  
  }
  
  if ( rule->nullable == NL_NONE )   
    rule->setNullable(NL_IS_NOT_NULLABLE);
  
  rule->circlefree = true;  
  
}

void  Grammar::calculateFirstSets() throw (Exception<Grammar>, ExceptionBase){

  FirstSetGraph firstSetGraph(*this);
  FirstSetCollector firstSetCollector(firstSetGraph);

  firstSetCollector.find_scc();
 
}

void  Grammar::calculateFollowSets() throw (Exception<Grammar>, ExceptionBase){
  
  FollowSetGraph followSetGraph(*this);
  FollowSetCollector followSetCollector(followSetGraph);

  followSetCollector.find_scc();

}

void  Grammar::calculateDirectorSets() throw (Exception<Grammar>, ExceptionBase){

  Terminal *terminal;
  Nonterminal *nonterminal;
  set<Terminal *> *first_set;
  set<Terminal *> *follow_set;
  set<Terminal *> *director_set;

  for ( set<Rule *>::iterator rule_it = getRules().begin(); rule_it != getRules().end(); rule_it++ ){
    for ( list<Production *>::iterator alt_it = (*rule_it)->getAlternatives().begin(); alt_it !=  (*rule_it)->getAlternatives().end(); alt_it++ ){
      
      director_set = &(*alt_it)->getDirectorSet();
      for ( list<GrammarSymbol *>::iterator sym_it = (*alt_it)->getSymbols().begin(); sym_it != (*alt_it)->getSymbols().end(); sym_it++ ){
	
	if ( (terminal = dynamic_cast<Terminal *>(*sym_it)) != 0 ){
	  
	  director_set->insert(terminal);
	  break;

	} else {

	  nonterminal = static_cast<Nonterminal *>(*sym_it);
	  first_set = nonterminal->getRule()->getFirstSet();
	  for ( set<Terminal *>::iterator first_it = first_set->begin(); first_it != first_set->end(); first_it++ ){
	    director_set->insert(*first_it);
	  }

	  if ( nonterminal->getRule()->isNullable() == false)	    
	    break;

	}
      }
      
      if ( (*alt_it)->isNullable() ){
	
	follow_set = (*rule_it)->getFollowSet();
	for ( set<Terminal *>::iterator follow_it = follow_set->begin(); follow_it != follow_set->end(); follow_it++ ){
	  director_set->insert(*follow_it);
	}
	
      }
    }
  }
}

void Grammar::calculateLL1Property(){

  if ( isLeftrecursive() ){

    propertyLL1 = false;
    return;

  }
  
  for ( set<Rule *>::iterator it = rules.begin(); it != rules.end(); it++ ){
    if ( !(*it)->isDisjointDirectorSets() ){

      propertyLL1 = false;
      return;
    }
  }

  propertyLL1 = true;
  
}

void LLParser::restoreTerminals(unsigned long number, unsigned long level) throw (Exception<LLParser>){

  static const string thisMethod = "restoreTerminals()";

  if ( number == 0 )
    return;

  if ( debug )
    clog << level << "\trestoring: ";

  for ( unsigned long cnt = 0; cnt < number; cnt++ ){

    if ( tStackPointer == tokenstack.begin() )
      throw Exception<LLParser>(thisMethod + ": Stack exceeded!");

    tStackPointer--;

    if ( debug )
      clog << (*tStackPointer).first << " ";

  }

  if ( debug )
    clog << endl;

}

void LLParser::putback(Tokenizer *tokenizer){

  if ( tStackPointer == tokenstack.end() )
    tokenizer->putback();
  else
    tStackPointer--;

}

Grammar::Token LLParser::lookAhead(Tokenizer *tokenizer) throw (Exception<Tokenizer>, ExceptionBase){

  Grammar::Token token = nextToken(tokenizer);
  putback(tokenizer);

  return token;

}

void LLParser::pushToken(Grammar::Token token){

  if ( inputtype == STREAM )
    tokenstack.push(token);

}

Grammar::Token LLParser::nextToken(Tokenizer *tokenizer) throw (Exception<Tokenizer>, ExceptionBase){

  if ( tStackPointer == tokenstack.end() ) {

    int scantoken = tokenizer->nextToken();
    int type = tokenizer->getTokenType();
    inputtype = STREAM;
    
    if ( type == LexToken::TT_WORD || type == LexToken::TT_NUMBERWORD ){

      return Grammar::Token(tokenizer->getSVal(),type);

    } else if ( type == LexToken::TT_EOF || type == LexToken::TT_EOL ){

      return Grammar::Token("",type);
      
    } else {
      
      ostringstream out;
      out.put((char)scantoken);
      return Grammar::Token(out.str(),LexToken::TT_WORD);
      
    }
    
  } else {

    Grammar::Token token = *tStackPointer;
    tStackPointer++;
    inputtype = STACK;
    return token;

  }

}

Rule * LLParser::clone(Rule *rule, Grammar::Token lookahead) throw (Exception<LLParser>){

  static string thisMethod = "clone()";

  static const bool useLookahead = true;

  bool found = false;
  Rule *rule_clone = new Rule(rule->getNonterminal());

  for ( list<Production *>::iterator pit = rule->getAlternatives().begin(); pit != rule->getAlternatives().end(); pit++ ){

    if ( useLookahead == false ){

      rule_clone->getAlternatives().push_back((*pit)->clone());
      
    } else {

      if ( lookahead.first == "" ){
	// comes from the rule S'->S

	rule_clone->getAlternatives().push_back((*pit)->clone());

      } else {

	set<Terminal *> & director_set = (*pit)->getDirectorSet();
	set<Terminal *>::iterator dit;
	  
	if ( lookahead.second == LexToken::TT_WORD ){

	  for ( dit = director_set.begin(); dit != director_set.end(); dit++ ){
	    if ( (*dit)->getName() == lookahead.first && (*dit)->getType() == Terminal::TT_WORD ){

	      rule_clone->getAlternatives().push_back((*pit)->clone());
	      found = true;
	      break;
	      
	    }
	  }
	    
	} else if ( lookahead.second == LexToken::TT_NUMBERWORD ){
	    
	  for ( dit = director_set.begin(); dit != director_set.end(); dit++ ){
	    if ( (*dit)->getType() == Terminal::TT_NUMBER ){

	      rule_clone->getAlternatives().push_back((*pit)->clone());
	      found = true;
	      break;

	    }      
	  }
	    
	} else{
	  throw Exception<LLParser>(thisMethod + ": wrong token-type!");
	}
      }
    }

    if ( found == true and grammar->isLL1() ){
      break;
    }

  }
  
  return rule_clone;
  
}

bool LLParser::parse(Tokenizer *tokenizer) throw (Exception<LLParser>){

  if ( tokenizer == 0 )
    throw Exception<LLParser>("tokenizer == 0!");

  tStackPointer = tokenstack.end();

  Rule *rule = clone(grammar->getStartRule(),Grammar::Token("",LexToken::TT_WORD));

  ParseResult result = parse(tokenizer,rule,false,0);
  bool succeeded;

  delete rule;

  if ( lookAhead(tokenizer).second == LexToken::TT_EOF )
    succeeded = result.first;
  else
    succeeded = false;
  if ( debug ){

    if ( succeeded == true )
      clog << "\tsuccess" << endl;
    else
      clog << "\tfailure" << endl;

  }

  tokenstack.clear();
  return succeeded;

}

LLParser::ParseResult LLParser::parse(Tokenizer *tokenizer, Rule *rule, bool backtrack, unsigned long level) throw (Exception<LLParser>){

  if ( getTraverseMethod() == DFS ){

    return parseDFS(tokenizer,rule,backtrack,level);

  }else if ( getTraverseMethod() == BFS ){

    return parseBFS(tokenizer,rule);

  }

  throw Exception<LLParser>("unsupported traverse method!");

}

LLParser::ParseResult LLParser::parseDFS(Tokenizer *tokenizer, Rule *rule, bool backtrack, unsigned long level) throw (Exception<LLParser>){

  unsigned long shifted_terminals_cnt = 0;
  Terminal *terminal;
  Nonterminal *nonterminal;
  Grammar::Token token;
  Production *production;
  GrammarSymbol *symbol;
  Rule *unfolded_rule;
  ParseResult result;
  bool nextAlternative;

  level++;

  if ( debug )
    clog << level << "\trule: " << rule->toString() << endl;

  list<Production *> & alternatives = rule->getAlternatives();

  while ( !alternatives.empty() ){
 
    nextAlternative = false;

    production = alternatives.front();
    list<GrammarSymbol *> & symbols = production->getSymbols();

    if ( debug )
      clog << level << "\tchoosing: " << production->toString() << endl;

    while ( !symbols.empty() && nextAlternative == false ){
      
      symbol = symbols.front();
      symbols.pop_front();
      
      if ( (terminal = dynamic_cast<Terminal *>(symbol)) != 0 ){
	// Terminal
	
	token = nextToken(tokenizer);

	if ( token.second == LexToken::TT_EOF ){
	  
	  putback(tokenizer);
	  restoreTerminals(shifted_terminals_cnt,level);
	  shifted_terminals_cnt = 0;
	  nextAlternative = true;
	  
	} else if ( token.second == LexToken::TT_NUMBERWORD && terminal->getType() == Terminal::TT_NUMBER ){
	  
	  if ( debug )
	    clog << level << "\tshifting number: " << token.first << endl;

	  if ( backtrack ){

	    pushToken(token);
	    shifted_terminals_cnt++;

	  }
	  
	} else if ( token.second == LexToken::TT_WORD && terminal->getType() == Terminal::TT_WORD ){
	  
	  if ( token.first == terminal->getName() ){
	    
	    if ( debug )
	      clog << level << "\tshifting: " << token.first << endl;

	    if ( backtrack ){

	      pushToken(token);
	      shifted_terminals_cnt++;

	    }
	    
	  } else {
	    
	    if ( debug )
	      clog << level << "\tfailing: " << token.first << endl;
	
	    putback(tokenizer);
	    restoreTerminals(shifted_terminals_cnt,level);
	    shifted_terminals_cnt = 0;
	    nextAlternative = true;

	  }	    
	}

      } else if ( (nonterminal = dynamic_cast<Nonterminal *>(symbol)) != 0 ){
	// Nonterminal
	
	if ( debug )
	  clog << level << "\tunfolding: " << nonterminal->getName() << " with lookahead: " << lookAhead(tokenizer).first << endl;
	
	rule = nonterminal->getRule();

	if ( rule == 0 )
	  throw Exception<LLParser>(string("no rule for nonterminal ") + nonterminal->getName());

	unfolded_rule = clone(rule,lookAhead(tokenizer));

	// recursion: do the production
	result = parseDFS(tokenizer,unfolded_rule,(backtrack || !alternatives.empty()),level);
	delete unfolded_rule;

	shifted_terminals_cnt += result.second;
	
	if ( result.first == false ){

	  restoreTerminals(shifted_terminals_cnt,level);
	  shifted_terminals_cnt = 0;
	  nextAlternative = true;
	  break;
	  
	}
	
      } else
	throw Exception<LLParser>("internal Error: GrammarSymbol neither Terminal nor Nonterminal!");
      
    }

    if ( nextAlternative == false )
      return ParseResult(true,shifted_terminals_cnt);
    else
      alternatives.pop_front();
  }

  return ParseResult(false,shifted_terminals_cnt);
  
}

LLParser::ParseResult LLParser::parseBFS(Tokenizer *tokenizer, Rule *rule) throw (Exception<LLParser>){

  Production *production;
  Production *expanded_production;
  GrammarSymbol *symbol;
  Terminal *terminal;
  Nonterminal *nonterminal;
  bool token_match = false;
  bool result;
  Grammar::Token token;

  std::deque<Production *> queue;

  for ( list<Production *>::iterator pit = rule->getAlternatives().begin(); pit != rule->getAlternatives().end(); pit++ ){
    queue.push_back((*pit)->clone());
  }
  queue.push_back(0); // end-of-productions

  production = queue.front();
  while ( production != 0 ){

    queue.pop_front();
    token_match = false;
    token = nextToken(tokenizer);

    if ( debug )
      clog << "shifting: " << token.first << endl;

    while ( production != 0 ){

      if ( debug )
	clog << "taking: " << production->toString() << endl;

      list<GrammarSymbol *> & symbols = production->getSymbols();
      
      if ( token.second == LexToken::TT_EOF ){

	if ( symbols.empty() ){
	  
	  delete production;
	  clearQueue(queue);
	  return ParseResult(true,0);
	  
	} else {
	  
	  symbol = symbols.front();
	  symbols.pop_front();

	  if ( (terminal = dynamic_cast<Terminal *>(symbol)) != 0 ){
	    
	    if ( debug )
	      clog << "skipping: " << production->toString() << endl;
	    
	    delete production;

	  } else {
	    // Nonterminal
	    
	    nonterminal = dynamic_cast<Nonterminal *>(symbol);
	    if ( nonterminal->getRule()->isNullable() ){

	      token_match = true;	      
	      if ( debug )
		clog << "prepending: " << production->toString() << endl;

	      queue.push_front(production);

	    } else{

	      if ( debug )
		clog << "skipping: " << production->toString() << endl;

	      delete production;

	    }
	  }
	}
      } else if ( token.second == LexToken::TT_NUMBERWORD ){
	
	if ( symbols.empty() ){
	  
	  if ( debug )
	    clog << "skipping: " << production->toString() << endl;
	  
	  delete production;
	  
	} else {
	  
	  symbol = symbols.front();
	  symbols.pop_front();
	  
	  if ( (terminal = dynamic_cast<Terminal *>(symbol)) != 0 ){
	    
	    if ( terminal->getType() == Terminal::TT_NUMBER ){
	      
	      token_match = true;
	      queue.push_back(production);

	      if ( debug )
		clog << "appending: " << production->toString() << endl;
	      
	    } else {

	      if ( debug )
		clog << "skipping: " << production->toString() << endl;
	      
	      delete production;
	      
	    }
	    
	  } else {
	    
	    token_match = true;
	    nonterminal = dynamic_cast<Nonterminal *>(symbol);
	    rule = clone(nonterminal->getRule(),token);
	    
	    for ( list<Production *>::iterator pit = rule->getAlternatives().begin(); pit != rule->getAlternatives().end(); pit++ ){
	      
	      expanded_production = (*pit)->clone();
	      // all symbols of production must be added to unfolded nonterminal
	      for ( list<GrammarSymbol *>::iterator sit = symbols.begin(); sit != symbols.end(); sit++ ){
		expanded_production->getSymbols().push_back(*sit);
	      }
	      
	      if ( debug )
		clog << "prepending: " << expanded_production->toString() << endl;

	      queue.push_front(expanded_production);
	    }
	    
	    delete production;	  
	    
	  }
	}
      } else{
	// TT_WORD

 	if ( symbols.empty() ){

	  if ( debug )
	    clog << "skipping: " << production->toString() << endl;
	  
	  delete production;
	  
	} else {
	  
	  symbol = symbols.front();
	  symbols.pop_front();

	  if ( (terminal = dynamic_cast<Terminal *>(symbol)) != 0 ){

	    if ( terminal->getType() == Terminal::TT_WORD && token.first == terminal->getName() ){
	      
	      token_match = true;
	      queue.push_back(production);

	      if ( debug )
		clog << "appending: " << production->toString() << endl;

	    } else {

	      if ( debug )
		clog << "skipping: " << production->toString() << endl;

	      delete production;

	    }

	  } else {

	    token_match = true;
	    nonterminal = dynamic_cast<Nonterminal *>(symbol);
	    rule = clone(nonterminal->getRule(),token);

	    if ( debug )
	      clog << "prepending: ";


	    for ( list<Production *>::iterator pit = rule->getAlternatives().begin(); pit != rule->getAlternatives().end(); pit++ ){

	      expanded_production = (*pit)->clone();
	      // all symbols of production must be added to unfolded nonterminal
	      for ( list<GrammarSymbol *>::iterator sit = symbols.begin(); sit != symbols.end(); sit++ ){
		expanded_production->getSymbols().push_back(*sit);
	      }
	      
	      if ( debug )
		clog << expanded_production->toString() << ", ";
	      
	      queue.push_front(expanded_production);
	    }

	    if ( debug )
	      clog << endl;
	    
	    delete rule;
	    delete production;
	  }
	}
      }
    
      production = queue.front();
      queue.pop_front();
    }

    // EndContent
    if ( production != 0 ){
      
      clearQueue(queue);
      throw Exception<LLParser>("content not EndContent as expected!");

    }

    if ( token_match == false ){
      
      clearQueue(queue);
      return ParseResult(false,0);
      
    }
    
    queue.push_back(production);
    production = queue.front();
  }
  
  token = nextToken(tokenizer);

  if ( token.second == LexToken::TT_EOF )
    result = true;
  else
    result = false;  

  clearQueue(queue);
  return ParseResult(result,0);
  
}

void LLParser::clearQueue(deque<Production *> & queue){

  for ( deque<Production *>::iterator it = queue.begin(); it != queue.end(); it++ ){
    if (*it)
      delete *it;
  }
}

FirstSetGraph::FirstSetNodeIterator::FirstSetNodeIterator(set<Rule *>::iterator it) : it(it){
}

Rule * FirstSetGraph::FirstSetNodeIterator::operator*() throw(ExceptionBase){
  return *it;
}

void FirstSetGraph::FirstSetNodeIterator::operator++(int) throw(ExceptionBase){
  it++;
}

bool  FirstSetGraph::FirstSetNodeIterator::operator==(const abstract_node_iterator<Rule *> *it_rval) throw(ExceptionBase){
  return it == static_cast<const FirstSetNodeIterator *>(it_rval)->it;
}

FirstSetGraph::FirstSetNeighbourIterator::FirstSetNeighbourIterator(Rule *rule, bool at_end) : rule(rule), at_end(at_end){
  init();
}

void FirstSetGraph::FirstSetNeighbourIterator::init(){

  if ( at_end == false ) {

    alternatives_iterator = rule->getAlternatives().begin();
    selectRuleWithStartingNonterminal();

  }
}

void FirstSetGraph:: FirstSetNeighbourIterator::selectRuleWithStartingNonterminal(){

  while ( alternatives_iterator != rule->getAlternatives().end() ){

    symbols_iterator = (*alternatives_iterator)->getSymbols().begin();
    if ( symbols_iterator != (*alternatives_iterator)->getSymbols().end() and dynamic_cast<Nonterminal *>(*symbols_iterator) != 0 ){
      return;
    }

    alternatives_iterator++;

  }

  at_end = true;

}

void FirstSetGraph::FirstSetNeighbourIterator::setToNextNonterminal() throw (Exception<FirstSetNeighbourIterator>){

  static const string thisMethod = "setToNextNonterminal()";

  list<GrammarSymbol *>::iterator symbols_end;
  Nonterminal *nonterminal;

  
  if ( alternatives_iterator != rule->getAlternatives().end() ){

    if ( (nonterminal = dynamic_cast<Nonterminal *>(*symbols_iterator)) == 0 )
      throw Exception<FirstSetNeighbourIterator>(thisMethod + ": not pointing to nonterminal!");

    if ( nonterminal->getRule()->isNullable() ) {

      symbols_iterator++;
      if ( symbols_iterator != (*alternatives_iterator)->getSymbols().end() and dynamic_cast<Nonterminal *>(*symbols_iterator) != 0 )
	return;

    }

    alternatives_iterator++;
    selectRuleWithStartingNonterminal();

  }
}

Rule * FirstSetGraph::FirstSetNeighbourIterator::operator*() throw(ExceptionBase){

  static const string thisMethod = " FirstSetNeighbourIterator::operator*()";

  if ( at_end == true ){
    throw Exception< abstract_node_iterator<Rule *> >(thisMethod + "iterator is at end!");
  }

  if ( dynamic_cast<Nonterminal *>(*symbols_iterator) == 0 ){
    throw Exception< abstract_node_iterator<Rule *> >(thisMethod + "iterator does not point to a nonterminal!");
  }

  return static_cast<Nonterminal *>(*symbols_iterator)->getRule();

}

void FirstSetGraph:: FirstSetNeighbourIterator::operator++(int) throw(ExceptionBase){

  if ( at_end == true ){
    return;
  }

  setToNextNonterminal();

}

bool FirstSetGraph::FirstSetNeighbourIterator::operator==(const abstract_node_iterator<Rule *> *it_rval) throw(ExceptionBase){

  const FirstSetNeighbourIterator *ni = static_cast<const FirstSetNeighbourIterator *>(it_rval);

  if ( this->at_end == true and ni->at_end == true ){
    return true;
  } else if ( this->at_end == true or ni->at_end == true ){
    return false;
  } else {
    return ( *alternatives_iterator == *(ni->alternatives_iterator) and *symbols_iterator == *(ni->symbols_iterator) );
  }

}

FirstSetGraph::FirstSetGraph(Grammar & grammar) : grammar(grammar){
}

abstract_node_iterator<Rule *> * FirstSetGraph::beginNodesPtr(){
  return new FirstSetNodeIterator(grammar.getRules().begin());
}

abstract_node_iterator<Rule *> *  FirstSetGraph::endNodesPtr(){
  return new FirstSetNodeIterator(grammar.getRules().end());
}

abstract_node_iterator<Rule *> *  FirstSetGraph::beginNeighboursPtr(Rule *rule){
  return new FirstSetNeighbourIterator(rule,false);
}

abstract_node_iterator<Rule *> *  FirstSetGraph::endNeighboursPtr(Rule *rule){
  return new FirstSetNeighbourIterator(rule,true);
}

size_t  FirstSetGraph::maxNodes(){
  return grammar.getRules().size();
}

FirstSetCollector::FirstSetCollector(FirstSetGraph & graph) : SCCProcessor<Rule *>(&graph){
}

FirstSetCollector::~FirstSetCollector(){

  for ( IncludedFirstSets::iterator it = included_first_sets_map.begin(); it != included_first_sets_map.end(); it++ ){
    delete (*it).second;
  }

}
    
void FirstSetCollector::processComponent() throw (ExceptionBase){

  first_set = new FirstSet();

  // slightly opimized search by maintaining a first sets map
  included_first_sets_map[first_set] = new FirstSets();

  static_cast<FirstSetGraph *>(this->graph)->getGrammar().getFirstSets().insert(first_set);

}

void FirstSetCollector::processComponentNode(Rule *rule) throw (ExceptionBase){

  rule->first_set = first_set;
  FirstSet & direct_first_set = rule->getDirectFirstSet();

  // insert direct_first_sets
  for ( FirstSet::iterator tit = direct_first_set.begin(); tit != direct_first_set.end(); tit++ ){
    first_set->insert(*tit);
  }

  // insert including first_sets
  for ( graph::node_iterator<Rule *> nit = graph->beginNeighbours(rule); nit != graph->endNeighbours(rule); nit++ ){

    FirstSet *neighbour_first_set = (*nit)->getFirstSet();

    if ( neighbour_first_set != first_set ){

      FirstSets *included_first_sets = included_first_sets_map[first_set];
      if ( find(included_first_sets->begin(),included_first_sets->end(),neighbour_first_set) == included_first_sets->end() ){

	for ( FirstSet::iterator tit = neighbour_first_set->begin(); tit != neighbour_first_set->end(); tit++ ){
	  first_set->insert(*tit);
	}

	included_first_sets->insert(neighbour_first_set);
	FirstSets *included_neighbour_first_sets = included_first_sets_map[neighbour_first_set];
	for ( FirstSets::iterator iit = included_neighbour_first_sets->begin(); iit != included_neighbour_first_sets->end(); iit++ )
	  included_first_sets->insert(*iit);

      }
    }

  }  
}

FollowSetGraph::FollowSetGraph(Grammar & grammar) : grammar(grammar){

  constructGraph();
  follow_map.clear();
  first_map.clear();

}

FollowSetGraph::~FollowSetGraph(){

  for ( set<FollowGraphNode *>::iterator it = nodes.begin(); it != nodes.end(); it++ )
    delete *it;

}

abstract_node_iterator<FollowGraphNode *> * FollowSetGraph::beginNodesPtr(){
  return new FollowSetNodeIterator(nodes.begin());
}

abstract_node_iterator<FollowGraphNode *> * FollowSetGraph::endNodesPtr(){
  return new FollowSetNodeIterator(nodes.end());
}

abstract_node_iterator<FollowGraphNode *> * FollowSetGraph::beginNeighboursPtr(FollowGraphNode *node){
  return new FollowSetNodeIterator(node->neighbours.begin());
}

abstract_node_iterator<FollowGraphNode *> * FollowSetGraph::endNeighboursPtr(FollowGraphNode *node){
  return new FollowSetNodeIterator(node->neighbours.end());
}

size_t FollowSetGraph::maxNodes(){
  return nodes.size();
}

void FollowSetGraph::constructGraph(){

  FollowNode *follow_node;
  FollowNode *nb_follow_node;
  FirstNode *nb_first_node;
  Nonterminal *nonterminal;
  

  for ( set<Rule *>::iterator rule_it = grammar.getRules().begin(); rule_it != grammar.getRules().end(); rule_it++ ){

    // this asserts that a node for each rule will be inserted, the return-value is irrelevant
    getFollowNode((*rule_it)->getNonterminal());

    for ( list<Production *>::iterator alt_it = (*rule_it)->getAlternatives().begin(); alt_it != (*rule_it)->getAlternatives().end(); alt_it++ ){

      list<GrammarSymbol *>::iterator symbols_end = (*alt_it)->getSymbols().end();
      for ( list<GrammarSymbol *>::iterator sym_it = (*alt_it)->getSymbols().begin(); sym_it != symbols_end; sym_it++ ){

	if ( dynamic_cast<Nonterminal *>(*sym_it) == 0 )
	  continue; // shift terminals

	// look if FollowNode of Nonterminal already declared
	follow_node = getFollowNode(*sym_it);

	// check next symbol
	list<GrammarSymbol *>::iterator next_sym_it = sym_it;
	next_sym_it++;

	if ( next_sym_it == symbols_end ){

	  nb_follow_node = getFollowNode((*rule_it)->getNonterminal());
	  follow_node->neighbours.insert(nb_follow_node);

	} else {

	  // include first_set in any case
	  nb_first_node = getFirstNode(*next_sym_it);
	  follow_node->neighbours.insert(nb_first_node);

	  // if next is a nonterminal and nullable: include follow_set
	  if ( (nonterminal = dynamic_cast<Nonterminal *>(*next_sym_it)) != 0 ){
	    if ( nonterminal->getRule()->isNullable() ){

	      nb_follow_node = getFollowNode(nonterminal);
	      follow_node->neighbours.insert(nb_follow_node);

	    }
	  }
	}
      }
    }
  }
}

FollowNode * FollowSetGraph::getFollowNode(GrammarSymbol *symbol){

  FollowNode *follow_node;

  map<GrammarSymbol *, FollowNode *>::iterator fn_it = follow_map.find(symbol);
  if ( fn_it == follow_map.end() ){
	  
    follow_node = newFollowNode(symbol);
    follow_map[symbol] = follow_node;

  } else{

    follow_node = (*fn_it).second;

  }

  return follow_node;
}

FirstNode * FollowSetGraph::getFirstNode(GrammarSymbol *symbol){

  FirstNode *first_node;

  map<GrammarSymbol *, FirstNode *>::iterator fn_it = first_map.find(symbol);
  if ( fn_it == first_map.end() ){
	  
    first_node = newFirstNode(symbol);
    first_map[symbol] = first_node;

  } else{

    first_node = (*fn_it).second;

  }

  return first_node;
}

FollowNode * FollowSetGraph::newFollowNode(GrammarSymbol *symbol){

  FollowNode *node = new FollowNode(symbol);
  nodes.insert(node);
  return node;
  
}
    
FirstNode * FollowSetGraph::newFirstNode(GrammarSymbol *symbol){

  FirstNode *node = new FirstNode(symbol);
  nodes.insert(node);
  return node;
  
}

FollowSetGraph::FollowSetNodeIterator::FollowSetNodeIterator(set<FollowGraphNode *>::iterator it) : it(it){
}

FollowGraphNode * FollowSetGraph::FollowSetNodeIterator::operator*() throw(ExceptionBase){
  return *it;
}

void FollowSetGraph::FollowSetNodeIterator::operator++(int) throw(ExceptionBase){
  it++;
}

bool FollowSetGraph::FollowSetNodeIterator::operator==(const abstract_node_iterator<FollowGraphNode *> *it_rval) throw(ExceptionBase){
  return ( this->it == static_cast<const FollowSetNodeIterator *>(it_rval)->it );
}

FollowSetCollector::FollowSetCollector(FollowSetGraph & graph) : SCCProcessor<FollowGraphNode *>(&graph){
}

void FollowSetCollector::processComponent() throw (ExceptionBase){

  follow_set = new FollowSet();

  static_cast<FollowSetGraph *>(this->graph)->getGrammar().getFollowSets().insert(follow_set);

}

void FollowSetCollector::processComponentNode(FollowGraphNode *node) throw (ExceptionBase){

  Terminal *nb_terminal;
  Nonterminal *nb_nonterminal;
  set<Terminal *> *nb_first_set;
  set<Terminal *> *nb_follow_set;

  if ( dynamic_cast<FirstNode *>(node) != 0 )
    return;

  Nonterminal *nonterminal = static_cast<Nonterminal *>(node->getSymbol());

  nonterminal->getRule()->follow_set = follow_set;

  for ( set<FollowGraphNode *>::iterator fit = node->neighbours.begin(); fit != node->neighbours.end(); fit++ ){
    
    if ( dynamic_cast<FirstNode *>(*fit) != 0 ){

      if ( (nb_terminal = dynamic_cast<Terminal *>((*fit)->getSymbol())) != 0 ){

	follow_set->insert(nb_terminal);

      } else {
	// Nonterminal
	nb_nonterminal = static_cast<Nonterminal *>((*fit)->getSymbol());
	nb_first_set = nb_nonterminal->getRule()->getFirstSet();

	for ( set<Terminal *>::iterator tit = nb_first_set->begin(); tit != nb_first_set->end(); tit++ ){
	  follow_set->insert(*tit);
	}

      }

    } else {
      // FollowNode

      nb_nonterminal = static_cast<Nonterminal *>((*fit)->getSymbol());
      nb_follow_set = nb_nonterminal->getRule()->getFollowSet();
      
      if ( nb_follow_set != follow_set ){
	for ( set<Terminal *>::iterator tit = nb_follow_set->begin(); tit != nb_follow_set->end(); tit++ ){
	  follow_set->insert(*tit);
	}
      }     
    }    
  }  
}
