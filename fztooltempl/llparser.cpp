#include <llparser.hpp>

using namespace std;
using namespace exc;
using namespace utils;
using namespace ds;
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

Nonterminal * Grammar::findNonterminal(string nonterminal){

  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ )
    if ( (*it)->getName() == nonterminal )
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
     lastAccessedRule = rule;

    nt->setRule(rule);
    nonterminals.insert(nt);
    
  } else {

    if ( nt->getRule() == 0 ){
 
      Rule *rule = new Rule(nt);
      rules.insert(rule);
      lastAccessedRule = rule;
      
      nt->setRule(rule);
      
    }

    lastAccessedRule = nt->getRule();
    
  }
  
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
      lastTerminal = new Terminal(lastGrammarSymbol);
      terminals.insert(lastTerminal);
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
    throw Exception<Grammar>("wrong calling-syntax for PE-types!");
  
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

  if ( lastAccessedRule == 0 )
    throw Exception<Grammar>("no rule defined!");

  lastAccessedProduction = new Production();
  lastAccessedRule->getAlternatives().push_back(lastAccessedProduction);

  defmode = 0;

  return *this;
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

  if ( debug )
    cout << "restoring: ";

  for ( unsigned long cnt = 0; cnt < number; cnt++ ){

    if ( tStackPointer == terminalstack.begin() )
      throw Exception<LLParser>(thisMethod + ": Stack exceeded!");

    if ( debug )
      cout << (*tStackPointer).first << " ";

    tStackPointer--;

  }

  if ( debug )
    cout << endl;

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

bool LLParser::parse(LexScanner *tokenizer) throw (Exception<LLParser>){

  if ( tokenizer == 0 )
    throw Exception<LLParser>("tokenizer == 0!");

  tStackPointer = terminalstack.end();

  Rule *rule = grammar->getStartRule()->clone(Grammar::Token("",LexToken::TT_WORD));

  ParseResult result =  parse(tokenizer,rule,false,rbstack.topIter(),0);

  delete rule;
  terminalstack.clear();

  return result.first;

}

LLParser::ParseResult LLParser::parse(LexScanner *tokenizer, Rule *rule, bool backtrack, Stack<Rule *>::const_iterator bottom,
				      unsigned long level) throw (Exception<LLParser>){

  unsigned long shifted_terminals_cnt = 0;
  Terminal *terminal;
  Nonterminal *nonterminal;
  Grammar::Token token;
  ParseResult result;
  bool nextAlternative;

  // local bottom for the word(back)trackstack
  Stack<GrammarSymbol *>::const_iterator symLocalBottom;

  Stack<Stack<Rule *>::const_iterator> rbs_bottomstack;

  Stack<Stack<GrammarSymbol *>::const_iterator> sym_bottomstack;

  level++;

  if ( debug )
    cout << level << "\trule: " << rule->toString() << endl;

  list<Production *> & alternatives = rule->getAlternatives();

  while ( !alternatives.empty() ){
 
    nextAlternative = false;

    Production *production = alternatives.front();
    list<GrammarSymbol *> & symbols = production->getSymbols();

    if ( debug )
      cout << level << "\tchoosing: " << production->toString() << endl;

    while ( !symbols.empty() && nextAlternative == false ){
      
      GrammarSymbol *word = symbols.front();
      
      if ( (terminal = dynamic_cast<Terminal *>(word)) != 0 ){
	// Terminal
	
	token = nextToken(tokenizer);

	if ( token.second == LexToken::TT_EOF ){
	  
	  putback(tokenizer);
	  restoreTerminals(shifted_terminals_cnt);
	  shifted_terminals_cnt = 0;
	  nextAlternative = true;
	  
	} else if ( token.second == LexToken::TT_NUMBERWORD && terminal->getType() == Terminal::TT_NUMBER ){
	  
	  if ( debug )
	    cout << level << "\tshifting number: " << token.first << endl;

	  if ( backtrack ){

	    terminalstack.push(token);
	    shifted_terminals_cnt++;

	  }
	  
	} else if ( token.second == LexToken::TT_WORD && terminal->getType() == Terminal::TT_WORD ){
	  
	  if ( token.first == terminal->getName() ){
	    
	    if ( debug )
	      cout << level << "\tshifting: " << token.first << endl;

	    if ( backtrack ){

	      terminalstack.push(token);
	      shifted_terminals_cnt++;

	    }
	    
	  } else {
	
	    putback(tokenizer);
	    restoreTerminals(shifted_terminals_cnt);
	    shifted_terminals_cnt = 0;
	    nextAlternative = true;

	  }	    
	}

      } else if ( (nonterminal = dynamic_cast<Nonterminal *>(word)) != 0 ){
	// Nonterminal
	
	if ( debug )
	  cout << level << "\texpanding: " << nonterminal->getName() << endl;
	
	Rule *rule = nonterminal->getRule();

	if ( rule == 0 )
	  throw Exception<LLParser>(string("no rule for nonterminal ") + nonterminal->getName());

	Rule *expanded_rule = rule->clone(lookAhead(tokenizer));

	// local bottom for the rbstack
	Stack<Rule *>::const_iterator rbsLocalBottom = rbstack.topIter();
	// recursion: do the production
	result = parse(tokenizer,expanded_rule,(backtrack || !alternatives.empty()),rbsLocalBottom,level);

	// done
	if ( !rbs_bottomstack.localEmpty(rbsLocalBottom) || !rbs_bottomstack.empty() )
	  rbs_bottomstack.push(rbsLocalBottom);

	// me must only delete the rule if it is not on the stack, i. e. stack is empty
	if ( rbstack.localEmpty(rbsLocalBottom) )
	  delete expanded_rule;
	
	shifted_terminals_cnt += result.second;
	
	if ( result.first == false ){
	  
	  restoreTerminals(shifted_terminals_cnt);
	  shifted_terminals_cnt = 0;
	  nextAlternative = true;
	  break;
	  
	}
	
      } else
	throw Exception<LLParser>("internal Error: GrammarSymbol neither Terminal nor Nonterminal!");
      
      //       if ( nextAlternative == false ){
      // 	symbols.pop_front();
      //       } else {
      
      // 	  restoreTerminals(shifted_terminals_cnt);
      // 	  shifted_terminals_cnt = 0;
      
      
      //       }
      
    }
    
    if ( nextAlternative == false ){
      // alternative was successfull
      // backtracking might be provided
      
      if ( alternatives.size() > 1 ){
	// still alternatives: we have to put the rule on the stack
	// for possible backtracking (we used only "first fit", not "best fit")
	
	// we can forget the first successfull alternative if we don't have to
	// consider backtracking upto here
	if ( rbstack.localEmpty(rbsLocalBottom) )
	  alternatives.pop_front();
	
	rbstack.push(rule);
	
      }
      
      return ParseResult(true,shifted_terminals_cnt);
      
    }
    else{
      // alternative was unsuccessfull

      // if we don't have to consider backtracking we can skip the first alternative
      if ( rbstack.localEmpty(rbsLocalBottom) )
	alternatives.pop_front();
      else {
	// consider backtrackiing
	
	

      }
    }
  }
  
  
  //   if ( rbstack->empty() ){
  //     return ParseResult(false,shifted_terminals_cnt);
  //   } else {
  
}

