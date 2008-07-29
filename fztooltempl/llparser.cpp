#include <llparser.hpp>

using namespace std;
using namespace exc;
using namespace parse;

Production::~Production(){
}

Production * Production::clone(){

  Production * production = new Production();
  production->words = words;

  return production;

}

Rule::~Rule(){

  for (list<Production *>::iterator it = alternatives.begin(); it != alternatives.end(); it++ )
    delete *it;
  
}

Rule * Rule::clone(string lookahead){

  Rule *rule = new Rule(getNonterminal());

  for ( list<Production *>::iterator pit = alternatives.begin(); pit != alternatives.end(); pit++ ){

    if ( lookahead == "" ){

      rule->getAlternatives().push_back((*pit)->clone());

    } else {

      set<string> & director_set = (*pit)->getDirectorSet();
      set<string>::iterator dit;
      
      if ( (dit = director_set.find(lookahead)) != director_set.end() )
	rule->getAlternatives().push_back((*pit)->clone());
      
    }
    
  }

  return rule;

}

Grammar::~Grammar(){
  
  for ( set<Rule *>::iterator it = rules.begin(); it != rules.end(); it++ )
    delete *it;

  for ( set<Terminal *>::iterator it = terminals.begin(); it != terminals.end(); it++ )
    delete *it;

  for ( set<Nonterminal *>::iterator it = nonterminals.begin(); it != nonterminals.end(); it++ )
    delete *it;
  
}

void LLParser::reset(){

  stack.clear();
  backtrack_count = 0;
  pushed_back_terminals.clear();

}

bool LLParser::parse(Tokenizer *tokenizer) throw (Exception<LLParser>){

  if ( tokenizer == 0 )
    throw Exception<LLParser>("tokenizer == 0!");

  reset();

  stack.push_back(StackEntry(grammar->getStartRule()->clone(""),""));

  return false;

}
