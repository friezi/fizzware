#include <llparser.hpp>

using namespace std;
using namespace parse;

Production::~Production(){
  for ( list<ProductionElement *>::iterator it = words.begin(); it != words.end(); it++ )
    delete *it;
}

Rule::~Rule(){

  for (list<Alternative>::iterator it = alternatives.begin(); it != alternatives.end(); it++ ){
    delete (*it).first;
    delete (*it).second;
  }

}

Grammar::~Grammar(){
  
  for ( std::set<Rule *>::iterator it = rules.begin(); it != rules.end(); it++ )
    delete *it;
  
}
