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

/**
   @file cmdlparser.hpp
   @author Friedemann Zintel
*/

#ifndef CMDLPARSER_HPP
#define CMDLPARSER_HPP

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <list>
#include <utility>
#include <libgen.h>
#include <exception.hpp>

/**
   @example cmdlparser_example
*/

/**
   @brief for classes concerning the commandline
*/
namespace cmdl{

  class CmdlParser;

  /**
     @brief For storing Synonyms
  */
  class Synonyms : private std::set< std::string,std::less<std::string> >{

    friend class CmdlParser;

  private:

    Synonyms(){}
    Synonyms(Synonyms &){}

  public:
    
    /**
       @brief adds the synonym to the list of synonyms
       @return reference to synonyms
       @see CmdlParser::synonym()

    */
    Synonyms & operator<<(std::string synonym);

  };

  /**
     @brief For storing short-Synonyms ( short-options )
  */
  class ShortSynonyms : private std::set< char,std::less<char> >{

    friend class CmdlParser;

  private:

    ShortSynonyms(){}
    ShortSynonyms(ShortSynonyms &){}

  public:
    
    /**
       @brief adds the synonym to the list of synonyms
       @return reference to synonyms
       @see CmdlParser::synonym()
    */
    ShortSynonyms & operator<<(char synonym);

  };

  /**
     @brief For storing Aliases
  */
  class Aliases : private std::set< std::string,std::less<std::string> >{

    friend class CmdlParser;

  private:

    Aliases(){}
    Aliases(Aliases &){}

  public:
    
    /**
       @brief adds the entry to the list of aliases
       @return reference to aliases
       @see CmdlParser::alias()
    */
     Aliases& operator<<(std::string entry);

  };

  /**
     @brief For storing short-Aliases
  */
  class ShortAliases : private std::set<char>{

    friend class CmdlParser;

  private:

    ShortAliases(){}
    ShortAliases(ShortAliases &){}

  public:
    
    /**
       @brief adds the entry to the list of shortaliases
       @return reference to shortaliases
       @see CmdlParser::shortalias()
    */
     ShortAliases& operator<<(char entry);

  };

  /** 
      The parser differentiates between:\n
      - shortoptions: e.g. "-lg" means the same like "-l -g"\n
      - normal options: e.g. "--quiet"\n
      - parameters: e.g. "--editor=emacs"\n
      - multiparameters: e.g. "--Incude:mathlib --Include:clib"
      - arguments: e.g. "filename"\n

      The end of options and regular arguments on the commandline has to be marked by a "-- " (realise the single space-sign).
      Each string after the "--" is taken as an argument.
      @see Example @ref cmdlparser_example
      @brief For parsing the commandline and extracting the options and arguments
  */
  class CmdlParser{


  private:

    class Info{

    public:

      std::string valueid;
      std::string description;

      Info(): valueid(""), description(""){}
      Info(std::string v, std::string d) : valueid(v), description(d){}
    }; 
  
    class APInfo : public Info{
    
    public:
    
      bool obligatory;
      bool found;
    
      APInfo() : Info(),obligatory(false), found(false){}
      APInfo(std::string v, std::string d, bool o, bool f) : Info(v,d),obligatory(o), found(f){}
    };
  
  protected:
  
    typedef std::list<std::string> Arguments;   // for scanned arguments
    typedef std::map< std::string,std::string,std::less<std::string> > ObligatoryArguments; // for obligatory arguments
    typedef std::list<Info> ExpOblArguments; // for expected obligatory arguments
    typedef std::map< std::string,std::string,std::less<std::string> > Parameters;  // for scanned parameters
    typedef std::map< std::string,APInfo,std::less<std::string> > AParameters;  // allowed parameters
    typedef std::map< std::string,Info,std::less<std::string> > AMParameters; // allowed multi-parameters
    typedef std::set< std::string,std::less<std::string> > Options;
    typedef std::map< std::string,Info,std::less<std::string> > AOptions; // allowed options
    typedef std::map< char,Info,std::less<char> > AShortoptions;
    typedef std::set< char,std::less<char> > Shortoptions;
 
  protected:

    class MultiParameters : public std::map< std::string,std::set< std::string,std::less<std::string> > *,std::less<std::string> >{

    public:
      ~MultiParameters();
    };

    class SynonymDict : public std::map< std::string,Synonyms *,std::less<std::string> >{

    public:
      ~SynonymDict();
    };

    class ShortSynonymDict : public std::map< char,ShortSynonyms *,std::less<char> >{

    public:
      ~ShortSynonymDict();
    };

    class AliasDict : public std::map< std::string,Aliases *,std::less<std::string> >{

    public:
      ~AliasDict();
    };

    class ShortAliasDict : public std::map< char,ShortAliases *,std::less<char> >{

    public:
      ~ShortAliasDict();
    };

  private:

    int argc;
    char **argv;

    /// parseerror-flag
    bool parseerror;

    /// is an infinite number of args allowed?
    bool infinite_args;

    /// the identifier for the purpose of the arguments
    std::string infinite_args_id;

    /// tuple for the final-argument: identifier and value
    std::pair<std::string,std::string> finalargument;

    /// for parse-errors
    std::ostringstream errors;
  
    /// the programname
    std::string programname;

    /// stores the normal arguments
    Arguments arguments;
    /// stores obligatory arguments
    ObligatoryArguments obligatoryarguments;
    /// for allowed obligatory arguments
    ExpOblArguments exp_obl_arguments;

    /// stores parameters, like: --color=blue
    Parameters parameters;
    /// for allowed parameters
    AParameters allowedparameters;

    /// stores multi-parameters, like --L:clib
    MultiParameters multiparameters;
    /// for allowed multi-parameters
    AMParameters allowedmultiparameters;

    /// stores options like: --help
    Options options;
    /// allowed options
    AOptions allowedoptions;

    /// stores short options like: -lsa, where l, s, a are single combinable options
    Shortoptions shortoptions;
    /// allowed shortoptions
    AShortoptions allowedshortoptions;

    // stores all synonyms
    SynonymDict synonymdict;

    // stores all shortsynonyms
    ShortSynonymDict shortsynonymdict;

    // stores all aliases
    AliasDict aliasdict;

    // stores all shortaliases
    ShortAliasDict shortaliasdict;

  protected:

    /**
       @internal
    */
    CmdlParser(const CmdlParser&){}

  public:

    /**
       @param argc the argc from main()
       @param argv the argv from main()
       @exception Exception<CmdlParser>
       @remark The constructor does no parsing yet, see parse()
    */
    CmdlParser(int argc, char **argv) throw(Exception<CmdlParser>);

    // the default-destructor
    ~CmdlParser(){}

    /**
       Shortoptions are single characters which can be compined with one optioncall, on the
       commandline started by a "-", e.g. "-czf",\n
       but can also be given individually, like "-c -z -f"\n
       general syntax: -\<shortoptions\>\n
       @brief to add a shortoption to the set of allowed options
       @param option the option
       @param description a descriptive string for the option, printed by infoUsage()
    */
    void addShortoption(char option, std::string description);

    /**
       Normal non-single-character options, on the commandline started by a "--", e.g. "--verbose"\n
       general syntax: --\<option\>
       @brief to add an allowed option
       @param option is an allowed option
       @param description a descriptive string for the option, printed by infoUsage()
    */
    void addOption(std::string option, std::string description);

  
    /**
       A Parameter is like a normal option which is assigned a value, e.g. "--color=blue"\n
       general syntax: --\<parameter\>=\<value\>
       @brief to add an allowed parameter
       @param parameter the parameter, which can be assigned a value
       @param valueid id-string for the value how it should be printed by usage()
       @param description a descriptive string for the parameter, printed by infoUsage()
       @param obligatory if occurence of parameter is must
    */
    void addParameter(std::string parameter, std::string valueid, std::string description, bool obligatory = false);

    /**
       A multi-parameter is an optional parameter which can occur more than once, e.g. useful for a
       compiler, which can link more than one library: "--l:lib1 --l:lib2 ..."\n
       general syntax: --\<parameter\>:\<value\>
       @brief to add a multi-parameter
       @param multiparameter the parameter, which can contain several value-entries
       @param valueid id-string for the value how it should be printed by usage()
       @param description a descriptive string for the multiparameter, printed by infoUsage()
    **/
    void addMultiParameter(std::string multiparameter, std::string valueid, std::string description);

    /**
       An obligatory Argument is an argument which must be present (e.g. \<filename\>)\n
       The order of definition is relevant for the order of connecting the value.
       @brief to add an allowed obligatory argument
       @param argument a name to which \<argumentname\> from the commandline will be connected to
       @param description a descriptive string for the argument, printed by infoUsage()
    */
    void addObligatoryArgument(std::string argument, std::string description);

    /**
       Just to inform the user with the usage()-function that an infinite number of
       arguments are allowed (e.g. filenames).\n
       It will be shown as "[\<id\>] [...] at the end of the usage-string".
       @brief for infinite arguments
       @param id the identification-string for which more arguments for the same purpose can be given
    */
    void setInfiniteArguments(std::string id);

    /**
       @brief For an obligatory final argument
       @param id the identifier
       @note Useful if you have an infinite number of arguments and need one last, e.g. directoryname for copying
       @remark This last argument won't be a member of the normal Arguments
    */
    void setFinalArgument(std::string id){
      finalargument.first = id;
    }

    /**
       Actuall, this method returns all synonyms belonging to representative.
       This method's purpose is to define synonyms for the representative. Example:\n
       synonym("representative") << "s1" << "s2";
       @brief to define synonyms
       @param representative the representative
       @exception Exception<CmdlParser>
       @return the synonyms
       @see very useful in combination with Synonyms::operator<<()
       @note the representative exists if you defined it as a option/parameter
    */
    Synonyms & synonym(std::string representative) throw(Exception<CmdlParser>);

    /**
       Method for short-options\n
       Actuall, this method returns all synonyms belonging to representative.
       This method's purpose is to define synonyms for the representative. Example:\n
       synonym("representative") << "s1" << "s2";
       @brief to define shortsynonyms
       @param representative the representative
       @exception Exception<CmdlParser>
       @return the shortsynonyms
       @see very useful in combination with ShortSynonyms::operator<<()
       @note the representative exists if you defined it as a option/parameter
    */
    ShortSynonyms & shortsynonym(char representative) throw(Exception<CmdlParser>);

    /**
       Actually, this method returns all aliases belonging to aliasname.
       This method's purpose is to define an alias for many options. Example:\n
       alias("aliasname") << "a1" << "a2";
       @brief to define an alias for many options
       @param aliasname the aliasname
       @return the aliases
       @see very useful in combination with Aliases::operator<<()
    */
    Aliases & alias(std::string aliasname);

    /**
       Actually, this method returns all shortaliases belonging to aliasname.
       This method's purpose is to define a shortalias for many shortoptions. Example:\n
       alias("aliasname") << "a1" << "a2";
       @brief to define an alias for many options
       @param aliasname the aliasname
       @return the shortaliases
       @see very useful in combination with ShortAliases::operator<<()
       @note shortaliases can't be mixed with normal aliases
    */
    ShortAliases & shortalias(char aliasname);

    /**
       @brief get the representative for a synonym
       @param synonym the synonym to find representative for
       @return the representative
    */
    std::string getRepresentative(std::string synonym);

    /**
       @brief get the representative for a synonym for a short-option
       @param synonym the synonym to find representative for
       @return the representative
    */
    char getShortRepresentative(char synonym);

    /**
       It parses the commandline and extracts the options and arguments according to the definition with the add-methods
       @brief for parsing the commandline
       @exception Exception<CmdlParser>
       @remark This method has to be called manually, it's not called by the constructor
    */
    void parse() throw(Exception<CmdlParser>);


    /**
       @brief check if a single option is set in shortoptions
       @param c the character to be checked
       @return true if c is contained, otherwise false
    */
    bool checkShortoption(char c){
      return ( shortoptions.find(c) != shortoptions.end() );
    }


    /**
       @brief check if a normal option is set in options
       @param option to be checked
       @return true if option is contained, otherwise false
    */
    bool checkOption(std::string option){
      return ( options.find(option) != options.end() );
    }


    /**
       @brief check if a parameter is set in options
       @param parameter to be checked
       @return a pair, first element is a bool value if or if not
       the parameter is contained, second element is the value of the parameter
    */
    std::pair<bool,std::string> checkParameter(std::string parameter);


    /**
       @brief check if a multiparameter is set in options
       @param multiparameter to be checked
       @return a pair, first element is a bool value if or if not
       the parameter is contained, second element is a set of all value belonging to the multiparameter
    */
    std::pair< bool,std::set<std::string> * > checkMultiParameter(std::string multiparameter);


    /**
       @brief get value the obligatory argument \<argument\>
       @return the value for an obligatory-argument (only after parsing usefull). If there's no entry for argument
       (should not happen if you use everything correctly) the empty-string is returned.
    */
    std::string getObligatoryArgument(std::string argument);

 
    /**
       @brief check if a parse-error has occured
       @return true if error has occured
    */
    bool checkParseerror(){
      return parseerror;
    }

 
    /**
       @brief get the program-name
       @return the program-name
    */
    std::string getProgramname(){
      return programname;
    }

    /**
       @brief get the identification-string of the final argument
       @return id of the final argument
       @exception Exception<CmdlParser>
    */
    std::string getFinalArgumentId() throw (Exception<CmdlParser>);

    /**
       @brief get the value of the final argument
       @return value of the final argument
       @exception Exception<CmdlParser>
    */
    std::string getFinalArgumentValue() throw (Exception<CmdlParser>);
  
    /**
       Returns an iterator pointing to the beginning of the non-obligatory-argument-list.
       @brief beginning of arguments
       @return the const_iterator
    */
    Arguments::const_iterator beginArguments(){ return arguments.begin(); }
  
    /**
       Returns an iterator pointing to the end of the non-obligatory-argument-list.
       @brief end of arguments
       @return the const_iterator
    */
    Arguments::const_iterator endArguments(){ return arguments.end(); }
  
    /**
       @brief returns a string containing the syntax of how the programm should be called
       @return the usage-string
       @note \\n is not added
    */
    std::string usage();
  
    /**
       @brief returns a string containing the syntax of how the programm should be called in a more
       informative and easy to read way
       @return the usage-string
    */
    std::string infoUsage();

    /** 
	@brief returns all contents of the CmdlParser
	@return a string containing all the contents of the object
    */
    std::string contents();

  protected:

    std::string synonymUsage(std::string representative);

  };

}

#endif
