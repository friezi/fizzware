#include <llparser.hpp>

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
      out << (*it)->getName() << " ";
    
  }

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

    //     if ( lookahead.first == "" ){
    //       // comes from the rule S'->S

    rule->getAlternatives().push_back((*pit)->clone());

    //     } else {

    //       Grammar::TaggedTerminals & director_set = (*pit)->getDirectorSet();
    //       Grammar::TaggedTerminals::iterator dit;

    //       if ( lookahead.second == LexToken::TT_WORD ){
      
    // 	if ( (dit = director_set.find(lookahead)) != director_set.end() )
    // 	  rule->getAlternatives().push_back((*pit)->clone());
	
    //       } else if ( lookahead.second == LexToken::TT_NUMBERWORD ){

    // 	for ( dit = director_set.begin(); dit != director_set.end(); dit++ ){

    // 	  if ( (*dit).second == Terminal::TT_NUMBER )
    // 	    rule->getAlternatives().push_back((*pit)->clone());

    // 	}

    //       } else
    // 	throw Exception<LLParser>(thisMethod + ": wrong token-type!");
    //     }
    
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
    
    } else
      throw Exception<Grammar>(String("wrong PE-type \"") + type + "\" for element \"" + lastGrammarSymbol + "\""); 

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

    if ( rule->circlefree == false )
      throw Exception<Grammar>("Grammar left recursive!");
    else
      return;
    
  }

  rule->visited = true;

  for ( list<Production *>::iterator ait = alternatives.begin(); ait != alternatives.end(); ait++ ){

    list<GrammarSymbol *> & symbols = (*ait)->getSymbols();

    if ( symbols.empty() ){

      rule->setNullable(NL_IS_NULLABLE);
      continue;

    }
    
    list<GrammarSymbol *>::iterator sit;
    for ( sit = symbols.begin(); sit != symbols.end(); sit++ ){
      
      if ( (terminal = dynamic_cast<Terminal *>(*sit)) != 0 ){

	rule->getDirectFirstSet().insert(terminal);
	break;
	
      } else if ( (next_nonterminal = dynamic_cast<Nonterminal *>(*sit)) != 0 ){

	next_rule = next_nonterminal->getRule();

	traverseDFNL(next_rule);

	if ( next_rule->nullable == NL_IS_NOT_NULLABLE )
	  break;	  
	
      } else
	throw Exception<LLParser>("internal Error: GrammarSymbol neither Terminal nor Nonterminal!");
    }

    if ( sit == symbols.end() )
      rule->setNullable(NL_IS_NULLABLE);
  
  }
  
  if ( rule->nullable == NL_NONE )
    rule->setNullable(NL_IS_NOT_NULLABLE);
  
  rule->circlefree = true;  
  
}

void LLParser::putback(LexScanner *tokenizer){

  if ( tStackPointer == tokenstack.end() )
    tokenizer->putback();
  else
    tStackPointer--;

}

void  Grammar::calculateFirstSets() throw (Exception<Grammar>, ExceptionBase){

  FirstSetGraph firstSetGraph(*this);
  FirstSetCollector firstSetCollector(firstSetGraph);

  firstSetCollector.find_scc();
 
}

void  Grammar::calculateFollowSets() throw (Exception<Grammar>, ExceptionBase){
}

void  Grammar::calculateDirectorSets() throw (Exception<Grammar>, ExceptionBase){
}

Grammar::Token LLParser::lookAhead(LexScanner *tokenizer) throw (Exception<LexScanner>, ExceptionBase){

  Grammar::Token token = nextToken(tokenizer);

  putback(tokenizer);

  return token;

}

void LLParser::pushToken(Grammar::Token token){

  if ( inputtype == STREAM )
    tokenstack.push(token);

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

Grammar::Token LLParser::nextToken(LexScanner *tokenizer) throw (Exception<LexScanner>, ExceptionBase){

  if ( tStackPointer == tokenstack.end() ) {

    int scantoken = tokenizer->nextToken();
    int type = tokenizer->token.type;
    inputtype = STREAM;
    
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
    inputtype = STACK;
    return token;

  }

}

bool LLParser::parse(LexScanner *tokenizer) throw (Exception<LLParser>){

  if ( tokenizer == 0 )
    throw Exception<LLParser>("tokenizer == 0!");

  tStackPointer = tokenstack.end();

  Rule *rule = grammar->getStartRule()->clone(Grammar::Token("",LexToken::TT_WORD));

  ParseResult result =  parse(tokenizer,rule,false,0);
  bool succeeded;

  delete rule;
  tokenstack.clear();

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

  return succeeded;

}

LLParser::ParseResult LLParser::parse(LexScanner *tokenizer, Rule *rule, bool backtrack, unsigned long level) throw (Exception<LLParser>){

  unsigned long shifted_terminals_cnt = 0;
  Terminal *terminal;
  Nonterminal *nonterminal;
  Grammar::Token token;
  ParseResult result;
  bool nextAlternative;

  level++;

  if ( debug )
    clog << level << "\trule: " << rule->toString() << endl;

  list<Production *> & alternatives = rule->getAlternatives();

  while ( !alternatives.empty() ){
 
    nextAlternative = false;

    Production *production = alternatives.front();
    list<GrammarSymbol *> & symbols = production->getSymbols();

    if ( debug )
      clog << level << "\tchoosing: " << production->toString() << endl;

    while ( !symbols.empty() && nextAlternative == false ){
      
      GrammarSymbol *symbol = symbols.front();
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
	  clog << level << "\texpanding: " << nonterminal->getName() << endl;
	
	Rule *rule = nonterminal->getRule();

	if ( rule == 0 )
	  throw Exception<LLParser>(string("no rule for nonterminal ") + nonterminal->getName());

	Rule *expanded_rule = rule->clone(lookAhead(tokenizer));

	// recursion: do the production
	result = parse(tokenizer,expanded_rule,(backtrack || !alternatives.empty()),level);
	delete expanded_rule;
	
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

FirstSetNodeIterator::FirstSetNodeIterator(set<Rule *>::iterator it) : it(it){
}

Rule * FirstSetNodeIterator::operator*() throw(ExceptionBase){
  return *it;
}

void FirstSetNodeIterator::operator++(int) throw(ExceptionBase){
  it++;
}

bool  FirstSetNodeIterator::operator==(const abstract_node_iterator<Rule *> *it_rval) throw(ExceptionBase){
  return it == static_cast<const FirstSetNodeIterator *>(it_rval)->it;
}

FirstSetNeighbourIterator::FirstSetNeighbourIterator(Rule *rule, bool at_end) : rule(rule), at_end(at_end){

  init();

}

void FirstSetNeighbourIterator::init(){

  if ( at_end == false ) {

    alternatives_iterator = rule->getAlternatives().begin();
    selectRuleWithStartingNonterminal();

  }
}

void FirstSetNeighbourIterator::selectRuleWithStartingNonterminal(){

  while ( alternatives_iterator != rule->getAlternatives().end() ){

    symbols_iterator = (*alternatives_iterator)->getSymbols().begin();
    if ( symbols_iterator != (*alternatives_iterator)->getSymbols().end() and dynamic_cast<Nonterminal *>(*symbols_iterator) != 0 ){
      return;
    }

    alternatives_iterator++;

  }

  at_end = true;

}

void FirstSetNeighbourIterator::setToNextNonterminal() throw (Exception<FirstSetNeighbourIterator>){

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

Rule * FirstSetNeighbourIterator::operator*() throw(ExceptionBase){

  static const string thisMethod = " FirstSetNeighbourIterator::operator*()";

  if ( at_end == true ){
    throw Exception< abstract_node_iterator<Rule *> >(thisMethod + "iterator is at end!");
  }

  if ( dynamic_cast<Nonterminal *>(*symbols_iterator) == 0 ){
    throw Exception< abstract_node_iterator<Rule *> >(thisMethod + "iterator does not point to a nonterminal!");
  }

  return static_cast<Nonterminal *>(*symbols_iterator)->getRule();

}

void FirstSetNeighbourIterator::operator++(int) throw(ExceptionBase){

  if ( at_end == true ){
    return;
  }

  setToNextNonterminal();

}

bool  FirstSetNeighbourIterator::operator==(const abstract_node_iterator<Rule *> *it_rval) throw(ExceptionBase){

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

FirstSetCollector::FirstSetCollector(FirstSetGraph & graph) : SCCCollector<Rule *>(&graph){
}

FirstSetCollector::~FirstSetCollector(){

  for ( IncludedFirstSets::iterator it = included_first_sets_map.begin(); it != included_first_sets_map.end(); it++ ){
    delete (*it).second;
  }

}
    
void FirstSetCollector::processComponent() throw (ExceptionBase){

  SCCCollector<Rule *>::processComponent();

  first_set = new FirstSet();

  // slightly opimized search by maintaining a first sets map
  included_first_sets_map[first_set] = new FirstSets();

  static_cast<FirstSetGraph *>(this->graph)->getGrammar().getFirstSets().insert(first_set);

}

void FirstSetCollector::processComponentNode(Rule *rule) throw (ExceptionBase){

  SCCCollector<Rule *>::processComponentNode(rule);

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
