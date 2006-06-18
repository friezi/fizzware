/*
    Copyright (C) 1999-2005 Friedemann Zintel

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    For any questions, contact me at
    friezi@cs.tu-berlin.de
*/

#include <cmdlparser.hpp>

using namespace std;
using namespace cmdl;

Synonyms & Synonyms::operator<<(std::string synonym){
  
  this->insert(synonym);
  return *this;

}

ShortSynonyms & ShortSynonyms::operator<<(char synonym){
  
  this->insert(synonym);
  return *this;

}

Aliases & Aliases::operator<<(std::string entry){
  
  this->insert(entry);
  return *this;

}

ShortAliases & ShortAliases::operator<<(char entry){
  
  this->insert(entry);
  return *this;

}

CmdlParser::MultiParameters::~MultiParameters(){

  for ( MultiParameters::iterator it = this->begin(); it != this->end(); it++ )
    delete (*it).second;
}

CmdlParser::SynonymDict::~SynonymDict(){

  for ( SynonymDict::iterator it = this->begin(); it != this->end(); it++ )
    delete (*it).second;
}

CmdlParser::ShortSynonymDict::~ShortSynonymDict(){

  for ( ShortSynonymDict::iterator it = this->begin(); it != this->end(); it++ )
    delete (*it).second;
}

CmdlParser::AliasDict::~AliasDict(){

  for ( AliasDict::iterator it = this->begin(); it != this->end(); it++ )
    delete (*it).second;
}

CmdlParser::ShortAliasDict::~ShortAliasDict(){

  for ( ShortAliasDict::iterator it = this->begin(); it != this->end(); it++ )
    delete (*it).second;
}

void CmdlParser::addShortoption(char option, string description){

  allowedshortoptions[option] = Info("",description);

  if ( shortsynonymdict.find(option) == shortsynonymdict.end() )
    shortsynonymdict[option] = new ShortSynonyms();
}

void CmdlParser::addOption(string option, string description){

  allowedoptions[option] = Info("",description);

  if ( synonymdict.find(option) == synonymdict.end() )
    synonymdict[option] = new Synonyms(); 
}

void CmdlParser::addParameter(string parameter, string valueid, string description, bool mandatory){

  allowedparameters[parameter] = APInfo(valueid,description,mandatory,false);

  if ( synonymdict.find(parameter) == synonymdict.end() )
    synonymdict[parameter] = new Synonyms(); 
}

void CmdlParser::addMultiParameter(string multiparameter, string valueid, string description){

  allowedmultiparameters[multiparameter] = Info(valueid,description);

  if ( synonymdict.find(multiparameter) == synonymdict.end() )
    synonymdict[multiparameter] = new Synonyms(); 
}

void CmdlParser::addMandatoryArgument(string argument, string description){

  exp_mand_arguments.insert(exp_mand_arguments.end(),Info(argument,description));
} 

Synonyms & CmdlParser::synonym(string representative) throw(Exception<CmdlParser>){

  if ( synonymdict.find(representative) == synonymdict.end() )
    throw Exception<CmdlParser>("Representative " + representative + " not in list of synonyms!");

  return *synonymdict[representative];
}

ShortSynonyms & CmdlParser::shortsynonym(char representative) throw(Exception<CmdlParser>){

  if ( shortsynonymdict.find(representative) == shortsynonymdict.end() )
    throw Exception<CmdlParser>("Representative " + (string("") += representative) + " not in list of shortsynonyms!");

  return *shortsynonymdict[representative];
}

Aliases & CmdlParser::alias(string aliasname){

  if ( aliasdict.find(aliasname) == aliasdict.end() )
    aliasdict[aliasname] = new Aliases();

  return *aliasdict[aliasname];
}

ShortAliases & CmdlParser::shortalias(char aliasname){

  if ( shortaliasdict.find(aliasname) == shortaliasdict.end() )
    shortaliasdict[aliasname] = new ShortAliases();

  return *shortaliasdict[aliasname];
}

string CmdlParser::getRepresentative(string synonym){

  string representative;

  for ( SynonymDict::iterator it = synonymdict.begin(); it != synonymdict.end(); it++ ){

    representative = (*it).first;

    if ( (*it).second->find(synonym) != (*it).second->end() )
      return representative;
  }

  return synonym;
}

char CmdlParser::getShortRepresentative(char synonym){

  char representative;

  for ( ShortSynonymDict::iterator it = shortsynonymdict.begin(); it != shortsynonymdict.end(); it++ ){

    representative = (*it).first;

    if ( (*it).second->find(synonym) != (*it).second->end() )
      return representative;
  }

  return synonym;
}

CmdlParser::CmdlParser(int argc, char **argv) throw(Exception<CmdlParser>) :
  argc(argc), argv(argv), parseerror(false), infinite_args(false), infinite_args_id(""),finalargument("",""){

  if (argc < 1)
    throw Exception<CmdlParser>("fatal error: argc < 1!");

  programname = argv[0];
}

void CmdlParser::setInfiniteArguments(string id){

  infinite_args = true;
  infinite_args_id = id;
}

string CmdlParser::getFinalArgumentId() throw (Exception<CmdlParser>){

  if ( finalargument.first == "" )
    throw Exception<CmdlParser>("No final argument defined!");
  else
    return finalargument.first;
}

string CmdlParser::getFinalArgumentValue() throw (Exception<CmdlParser>){

  if ( finalargument.first == "" )
    throw Exception<CmdlParser>("No final argument defined!");
  else
    return finalargument.second;
}

void CmdlParser::parse() throw(Exception<CmdlParser>){

  int length;
  bool end_of_options = false;
  unsigned int argumentcounter = 0;

  errors << endl;

  // scan all words from argv
  for ( int word = 1; word<argc; word++){

    if ( end_of_options == false && argv[word][0] == '-'){  // did "--" occur?
      
	length = strlen(argv[word]);
	if ( length < 2 ){ // only "-" -> normal argument
	  if ( argumentcounter < exp_mand_arguments.size() ){
	    // it's an mandatory argument

	    string mandarg = exp_mand_arguments.front().valueid;
	    mandatoryarguments[mandarg] = argv[word];  // insertion

	    // membercycling
	    exp_mand_arguments.insert(exp_mand_arguments.end(),exp_mand_arguments.front());
	    exp_mand_arguments.pop_front();

	  } else  // random argument
	    arguments.insert(arguments.end(),argv[word]);

	  argumentcounter++;
	  continue;
	}
	
	if ( argv[word][1] != '-' ){ // shortoption

	  char shortoption;
	  
	  // insert chars in shortoptions
	  for ( int i = 1; i<length; i++){

	    shortoption = argv[word][i];

	    // check for synonyms
	    shortoption = getShortRepresentative(shortoption);
	    
	    // check if alias
	    ShortAliasDict::iterator sad_it;

	    if ( (sad_it = shortaliasdict.find(shortoption)) != shortaliasdict.end() ){

	      // insert all contained options
	      for ( ShortAliases::iterator sa_it = (*sad_it).second->begin(); sa_it != (*sad_it).second->end(); sa_it++ )
		shortoptions.insert(*sa_it);
		
	    } else{  // no alias
	    
	      // invalid option:
	      if ( allowedshortoptions.find(shortoption) == allowedshortoptions.end() ){

		errors << "invalid option: " << string(1,shortoption) << endl;
		parseerror = true;

	      } else
		shortoptions.insert(shortoption); // insert
	    }
	  }
	} else{  // maybe normal option
	  
	  if ( length == 2 ) // the end of all options: "--"
	    end_of_options = true;
	  else{  // definitely normal option or parameter
	    
	    int eqs;
	    bool param = false;
	    bool multiparam = false;
	    
	    // check if it is a parameter
	    for ( eqs = 2; eqs<length; eqs++ ){

	      if ( argv[word][eqs] == '=' ){

		param = true;
		break;

	      } else if ( argv[word][eqs] == ':' ){ // multi-parameter

		multiparam = true;
		break;

	      }
	    }
	    
	    if ( param == false && multiparam == false ){  // now it is really a normal option

	      string option(&argv[word][2]);

	      // check synonyms
	      option = getRepresentative(option);

	      // check if alias
	      AliasDict::iterator adit;

	      if ( (adit = aliasdict.find(option)) != aliasdict.end() ){

		// insert all contained options
		for ( Aliases::iterator a_it = (*adit).second->begin(); a_it != (*adit).second->end(); a_it++ )
		  options.insert(*a_it);
		
	      } else{  // no alias

		// invalid option
		if ( allowedoptions.find(option) == allowedoptions.end() ){

		  errors << "invalid option: " << option << endl;
		  parseerror = true;

		} else
		  options.insert(option);
	      
	      }
	    } else if ( multiparam == true ){  // a multi-parameter with value

	      string parameter(&argv[word][2],eqs-2);

	      // check synonyms
	      parameter = getRepresentative(parameter);
	      
	      // invalid parameter
	      if ( allowedmultiparameters.find(parameter) == allowedmultiparameters.end() ){

		errors << "invalid multi-parameter: " << parameter << endl;
		parseerror = true;

	      } else{

		// check if a value is given
		if ( argv[word][eqs+1] == '\0' ){

		  errors << "no value given for multi-parameter: " << parameter << endl;
		  parseerror = true;

		} else{

		  string value(&argv[word][eqs+1]);
		  
		  // first occurence
		  if ( multiparameters.find(parameter) == multiparameters.end()){

		    set<string> *valueset = new set<string>();
		    valueset->insert(value);
		    multiparameters[parameter] = valueset;

		  } else{  // there's an entry already
		    
		    set<string> *valueset = multiparameters[parameter];
		    valueset->insert(value);

		  }
		}
	      }
	    } else{   // a parameter with value
	      
	      string parameter(&argv[word][2],eqs-2);

	      // check synonyms
	      parameter = getRepresentative(parameter);
	      
	      // invalid parameter
	      if ( allowedparameters.find(parameter) == allowedparameters.end() ){

		errors << "invalid parameter: " << parameter << endl;
		parseerror = true;

	      } else{

		// check if a value is given
		if ( argv[word][eqs+1] == '\0' ){

		  errors << "no value given for parameter: " << parameter << endl;
		  parseerror = true;

		} else{  // ok, there's a value

		  string value(&argv[word][eqs+1]);

		  parameters[parameter] = value;
		  
		  // set the "found"-state
		  allowedparameters[parameter].found = true;

		}
	      }
	    }
	  }
	}
    } else{  // end_of_options == true -> only arguments follow

      string argument(argv[word]);

      if ( argumentcounter < exp_mand_arguments.size() ){
	// it's an mandatory argument;

	string mandarg = exp_mand_arguments.front().valueid;
	mandatoryarguments[mandarg] = argument;  // insertion

	// membercycling
	exp_mand_arguments.insert(exp_mand_arguments.end(),exp_mand_arguments.front());
	exp_mand_arguments.pop_front();
	
      } else  // random argument
	arguments.insert(arguments.end(),argument);

      argumentcounter++;
    }
  }

  // if a final-argument is desired, get it and remove it from the argument-list
  if ( finalargument.first != "" ){

    if ( arguments.empty() == true ){

      errors << "Too few arguments: final argument missing: " << finalargument.first << endl;
      parseerror = true;

    } else{

      finalargument.second = arguments.back();
      arguments.pop_back();

    }
  }
  
  // to few of the mandatory parameters
  for ( AParameters::iterator it = allowedparameters.begin(); it != allowedparameters.end(); it++ )
    if ( (*it).second.mandatory == true && (*it).second.found == false ){

      errors << "Too few parameters: mandatory parameter missing: " << "--" << (*it).first << endl;
      parseerror = true;

    }
  
  // to few of the mandatory arguments are given
  if ( argumentcounter < exp_mand_arguments.size() ){

    errors << "Too few mandatory arguments!\n";
    parseerror = true;

  }

  // got arguments, but none is allowed
  if ( (arguments.empty() == false) && (exp_mand_arguments.empty() == true)
       && (infinite_args == false) && (finalargument.first == "") ){

    errors << "No arguments allowed!\n";
    parseerror = true;

  }
  
 
  // Parseerror occured
  if ( parseerror == true ){
    throw Exception<CmdlParser>(errors.str());
  }
}

pair<bool,string> CmdlParser::checkParameter(string parameter){
  
  Parameters::iterator it = parameters.find(parameter);

  if ( it != parameters.end() )
    return pair<bool,string>(true,(*it).second);
  else
    return pair<bool,string>(false,"");
}

pair< bool,set<string> * > CmdlParser::checkMultiParameter(string multiparameter){
  
  MultiParameters::iterator it = multiparameters.find(multiparameter);

  if ( it != multiparameters.end() )
    return pair< bool,set<string> * >(true,(*it).second);
  else
    return pair< bool,set<string> * >(false,(set<string> *)0);
}

string CmdlParser::getMandatoryArgument(string argument){

  MandatoryArguments::iterator it = mandatoryarguments.find(argument);

  if ( it != mandatoryarguments.end() )
    return (*it).second;
  else
    return "";
}

string CmdlParser::synonymUsage(string representative){
  
  ostringstream usg;

  SynonymDict::iterator rit = synonymdict.find(representative);

  // it's simply to much and actually unnecessary to print all the synonyms
//   if ( rit != synonymdict.end() ){
//     if ( (*rit).second->empty() == false ){
//       usg << "{" << representative;
//       for ( set<string>::iterator s_it = (*rit).second->begin(); s_it != (*rit).second->end(); s_it++ )
// 	usg << "," << *s_it;
//       usg << "}";
//     } else
//       usg << representative;
//   } else
//     usg << representative;

// only show representative
  usg << representative;

  return usg.str();
}

string CmdlParser::usage(){
  
  ostringstream usg;
  
  usg << "Usage: " << ::basename((char *)programname.c_str()) << " ";
  
  if ( !allowedshortoptions.empty() ){
    usg << "[-";
    for ( AShortoptions::iterator it = allowedshortoptions.begin(); it != allowedshortoptions.end(); it++ )
      usg << (*it).first;
    // it's simply to much and actually unnecessary to print all the synonyms
//     for ( ShortSynonymDict::iterator syn_it = shortsynonymdict.begin(); syn_it != shortsynonymdict.end(); syn_it++ )
//       for ( set<char>::iterator s_it = (*syn_it).second->begin(); s_it != (*syn_it).second->end(); s_it++ )
// 	usg << *s_it;
    for ( ShortAliasDict::iterator a_it = shortaliasdict.begin(); a_it != shortaliasdict.end(); a_it++ )
      usg << (*a_it).first;
    usg << "] ";
  }

  if ( !allowedoptions.empty() ){
    for ( AOptions::iterator it = allowedoptions.begin(); it != allowedoptions.end(); it++ )
      usg << "[--" << synonymUsage((*it).first) << "] ";
  }

  if ( !aliasdict.empty() ){
    for ( AliasDict::iterator it = aliasdict.begin(); it != aliasdict.end(); it++ )
      usg << "[--" <<(*it).first << "] ";
  }
  
  if ( !allowedparameters.empty() ){
    for ( AParameters::iterator it = allowedparameters.begin(); it != allowedparameters.end(); it++ ){
      if ( (*it).second.mandatory == false )
	usg << "[";
      usg << "--" << synonymUsage((*it).first) << "=<" << (*it).second.valueid << ">";
      if ( (*it).second.mandatory == false )     
	usg << "]";
      usg << " ";
    }
  }

  if ( !allowedmultiparameters.empty() ){
    for ( AMParameters::iterator it = allowedmultiparameters.begin(); it != allowedmultiparameters.end(); it++ )
      usg << "[--" << synonymUsage((*it).first) << ":<" << (*it).second.valueid << "> [...]] ";
  }
  
  // falls Argumente erwartet werden, Argumentseparator ausgeben
  if ( (exp_mand_arguments.empty() == false) || (infinite_args == true) || (finalargument.first != "") )
    usg << "[--] ";
  
  if ( !exp_mand_arguments.empty() ){
    for ( ExpMandArguments::iterator it = exp_mand_arguments.begin(); it != exp_mand_arguments.end(); it++ )
      usg << "<" << (*it).valueid << "> ";
  }

  if ( infinite_args == true )
    usg << "[<" << infinite_args_id << "> [...]] ";

  if ( finalargument.first != "" )
    usg << "<" << finalargument.first << "> ";

  return usg.str();
}

std::string CmdlParser::infoUsage(){
  
  ostringstream usg;
  
  usg << "Usage: " << ::basename((char *)programname.c_str()) << " ";

  if ( !allowedshortoptions.empty() || !allowedoptions.empty()
       || !allowedparameters.empty() || !allowedmultiparameters.empty() )
    usg << "[Options1] ";

  if ( !exp_mand_arguments.empty() )
    usg << "Options2 ";
  
  usg << endl;
  
  if ( !allowedshortoptions.empty() || !allowedoptions.empty()
       || !allowedparameters.empty() || !allowedmultiparameters.empty() ){

    usg << "Options1:" << endl;

    for ( AShortoptions::iterator it = allowedshortoptions.begin(); it != allowedshortoptions.end(); ++it ){
      usg << "-" << (*it).first;
      ShortSynonymDict::iterator syn_it;
      if ( (syn_it = shortsynonymdict.find((*it).first)) != shortsynonymdict.end() )
	for ( set<char>::iterator s_it = (*syn_it).second->begin(); s_it != (*syn_it).second->end(); ++s_it )
	  usg << ", -" << *s_it;
      usg << "\t" << (*it).second.description << endl;
    }
    
    for ( ShortAliasDict::iterator it = shortaliasdict.begin(); it != shortaliasdict.end(); ++it ){
      usg << "-" << (*it).first << "\t";
      usg << "an alias for the combination of the following shortoptions:" << endl << "  ";
      for ( set<char>::iterator a_it = (*it).second->begin(); a_it != (*it).second->end(); ++a_it )
	  usg << " -" << *a_it;
      usg << endl << endl;
    }
    
    for ( AOptions::iterator it = allowedoptions.begin(); it != allowedoptions.end(); ++it ){
      usg << "--" << (*it).first;
      SynonymDict::iterator syn_it;
      if ( (syn_it = synonymdict.find((*it).first)) != synonymdict.end() )
	for ( set<std::string>::iterator s_it = (*syn_it).second->begin(); s_it != (*syn_it).second->end(); ++s_it )
	  usg << ", --" << *s_it;
      usg << "\n  " << (*it).second.description << endl << endl;
    }
    
    for ( AliasDict::iterator it = aliasdict.begin(); it != aliasdict.end(); ++it ){
      usg << "--" << (*it).first << endl;
      usg << "  an alias for the combination of the following options:" << endl << "  ";
      for ( set<std::string>::iterator a_it = (*it).second->begin(); a_it != (*it).second->end(); ++a_it )
	  usg << " --" << *a_it;
      usg << endl << endl;
    }
    
    for ( AParameters::iterator it = allowedparameters.begin(); it != allowedparameters.end(); ++it ){
      if ( (*it).second.mandatory == false ){
	usg << "--" << (*it).first << "=<" << (*it).second.valueid << ">";
	SynonymDict::iterator syn_it;
	if ( (syn_it = synonymdict.find((*it).first)) != synonymdict.end() )
	  for ( set<std::string>::iterator s_it = (*syn_it).second->begin(); s_it != (*syn_it).second->end(); ++s_it )
	    usg << ", --" << *s_it << "=<" << (*it).second.valueid << ">";
	usg << "\n  " << (*it).second.description << endl << endl;
      }
    }
    
    for ( AMParameters::iterator it = allowedmultiparameters.begin(); it != allowedmultiparameters.end(); ++it ){
      usg << "--" << (*it).first << ":<" << (*it).second.valueid << ">";
      SynonymDict::iterator syn_it;
      if ( (syn_it = synonymdict.find((*it).first)) != synonymdict.end() )
	for ( set<std::string>::iterator s_it = (*syn_it).second->begin(); s_it != (*syn_it).second->end(); ++s_it )
	  usg << ", --" << *s_it << ":<" << (*it).second.valueid << ">";
      usg << "\n  " << (*it).second.description << endl << endl;
    }

  }

  if ( !exp_mand_arguments.empty() ){

    usg << "Options2:" << endl;
 
    for ( AParameters::iterator it = allowedparameters.begin(); it != allowedparameters.end(); ++it ){
      if ( (*it).second.mandatory == true ){
	usg << "--" << (*it).first << "=<" << (*it).second.valueid << ">";
	SynonymDict::iterator syn_it;
	if ( (syn_it = synonymdict.find((*it).first)) != synonymdict.end() )
	  for ( set<std::string>::iterator s_it = (*syn_it).second->begin(); s_it != (*syn_it).second->end(); ++s_it )
	    usg << ", --" << *s_it << "=<" << (*it).second.valueid << ">";
	usg << "\n  " << (*it).second.description << endl << endl;
      }
    }

    for ( ExpMandArguments::iterator it = exp_mand_arguments.begin(); it != exp_mand_arguments.end(); it++ )
      usg << "<" << (*it).valueid << "> " << "\t" << (*it).description << endl;
    usg << endl;
  } 

  return usg.str();

}

string CmdlParser::contents(){

  ostringstream collect;

  // programname
  collect << "programname: " << programname << endl << endl;

  // argc:
  collect << "argc = " << argc << endl;

  // argv:
  collect << "argv:\n";
  for (int i = 0; i<argc; i++)
    collect << argv[i] << " ";
  collect << endl << endl;
  
  // shortsynonyms
  collect << "Short-Synonyms:\n";
  for ( ShortSynonymDict::iterator it = shortsynonymdict.begin(); it != shortsynonymdict.end(); it++ ){
    collect << (*it).first << " : ";
    for ( set<char>::iterator s_it = (*it).second->begin(); s_it != (*it).second->end(); s_it++ )
      collect << (*s_it) << " ";
    collect << endl;
  }
  collect << endl;
  
  // synonyms
  collect << "Synonyms:\n";
  for ( SynonymDict::iterator it = synonymdict.begin(); it != synonymdict.end(); it++ ){
    collect << (*it).first << " : ";
    for ( set<string>::iterator s_it = (*it).second->begin(); s_it != (*it).second->end(); s_it++ )
      collect << (*s_it) << " ";
    collect << endl;
  }
  collect << endl;
  
  // shortaliases
  collect << "Short-Aliases:\n";
  for ( ShortAliasDict::iterator it = shortaliasdict.begin(); it != shortaliasdict.end(); it++ ){
    collect << (*it).first << " : ";
    for ( set<char>::iterator a_it = (*it).second->begin(); a_it != (*it).second->end(); a_it++ )
      collect << (*a_it) << " ";
    collect << endl;
  }
  collect << endl;
  
  // aliases
  collect << "Aliases:\n";
  for ( AliasDict::iterator it = aliasdict.begin(); it != aliasdict.end(); it++ ){
    collect << (*it).first << " : ";
    for ( set<string>::iterator a_it = (*it).second->begin(); a_it != (*it).second->end(); a_it++ )
      collect << (*a_it) << " ";
    collect << endl;
  }
  collect << endl;

  // allowedshortoptions:
  collect << "Allowed Shortoptions:" << endl;
  for ( AShortoptions::iterator it = allowedshortoptions.begin(); it != allowedshortoptions.end(); it++ )
    collect << (*it).first << "\t" << (*it).second.description << endl;
  collect << endl;

  // shortoptions:
  collect << "Shortoptions:" << endl;
  for ( Shortoptions::iterator it = shortoptions.begin(); it != shortoptions.end(); it++ )
    collect << *it << " ";
  collect << endl << endl;

  // allowedoptions:
  collect << "Allowed Options:" << endl;
  for ( AOptions::iterator it = allowedoptions.begin(); it != allowedoptions.end(); it++ )
    collect << (*it).first << "\t" << (*it).second.description << endl;
  collect << endl;

  // options:
  collect << "Options:\n";
  for ( Options::iterator it = options.begin(); it != options.end(); it++ )
    collect << *it << " ";
  collect << endl << endl;

  // allowedparameters
  collect << "Allowed Parameters:\n";
  for ( AParameters::iterator it = allowedparameters.begin(); it != allowedparameters.end(); it++ )
    collect << (*it).first << "\t" << (*it).second.description << endl;
  collect << endl;
  
  // parameters
  collect << "Parameters:\n";
  for ( Parameters::iterator it = parameters.begin(); it != parameters.end(); it++ )
    collect << (*it).first << " = " << (*it).second << endl;
  collect << endl;

  // allowedmultiparameters
  collect << "Allowed Multiparameters:\n";
  for ( AMParameters::iterator it = allowedmultiparameters.begin(); it != allowedmultiparameters.end(); it++ )
    collect << (*it).first << " " << (*it).second.description << endl;
  collect << endl;
  
  // multiparameters
  collect << "Multiparameters:\n";
  for ( MultiParameters::iterator it = multiparameters.begin(); it != multiparameters.end(); it++ ){
    collect << (*it).first << " : ";
    for ( set<string>::iterator vit = (*it).second->begin(); vit != (*it).second->end(); vit++ )
      collect << (*vit) << " ";
    collect << endl;
  }
  collect << endl;

  // allowed mandatory arguments:
  collect << "Allowed mandatory arguments:\n";
  for ( ExpMandArguments::iterator it = exp_mand_arguments.begin(); it != exp_mand_arguments.end(); it++ )
    collect << (*it).valueid << "\t" << (*it).description << endl;
  collect << endl;

  // mandatory arguments:
  collect << "Mandatory arguments:\n";
  for ( MandatoryArguments::iterator it = mandatoryarguments.begin(); it != mandatoryarguments.end(); it++ )
    collect << (*it).first << " = " << (*it).second << endl;
  collect << endl;
  
  // arguments:
  collect << "Arguments:\n";
  for ( Arguments::iterator it = arguments.begin(); it != arguments.end(); it++ )
    collect << *it << endl;
  collect << endl;

  // final argument:
  collect << "Final Argument:\n";
  collect << finalargument.first << " = " << finalargument.second << endl;

  return collect.str();
}
