/*
  this sourcefile belongs to the programm focal,
  a mathematical formula-calculator
  Copyright (C) 1999-2006 Friedemann Zintel

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  For any questions, contact me at
  friezi@cs.tu-berlin.de
*/

#include "main.hpp"

using namespace std;
using namespace cmdl;
using namespace mexp;
using namespace ds;

const char *version="1.01";

const static string formula = "formula";
const static string commands = "commands";
const static string prompt = "> ";
const static string exit_modified_text = "Variables or functions have been modified! Do you really want to quit without saving? (y,n) ";

int main(int argc, char **argv, char **envp){

  CmdlParser cmdlparser(argc,argv);
  string usage;
  const char *programname = 0;
  string fullprompt = "";
  VariableList *varlist = 0;
  FunctionList *functionlist = 0;
  bool bflag = false;
  bool interactive = true;
  MemPointer<char> input;
  MemPointer<char> pathname;
  streamsize precision = 6;


  try{

    usage = setupParser(cmdlparser);

    cmdlparser.parse();

    // on some systems basename expects a char *, not a const char *, so we have to provide it with a char * !
    pathname = (char *)calloc(cmdlparser.getProgramname().size()+1,sizeof(char));
    strcpy(pathname.get(),cmdlparser.getProgramname().c_str());

    programname = ::basename(pathname.get());

    // help-output
    if ( cmdlparser.checkShortoption('h') == true || cmdlparser.checkOption("help") == true ){

      cout << endl << programname << " is a calculator/evaluator of mathematical formulas." << endl << endl;
      cout << "version " << version << ", Copyright (C) 1999-2006  Friedemann Zintel" << endl << endl;
      cout << cmdlparser.infoUsage();
      cout << "With no options given, focal will work in interactive mode" << endl << endl;
      exit(0);

    }

    // version-output
    if ( cmdlparser.checkShortoption('v') == true || cmdlparser.checkOption("version") == true ){

      cout << version << endl;
      exit(0);

    }

    // set precision
    if ( cmdlparser.checkParameter("precision").first == true )
      precision = (streamsize)atoi(cmdlparser.checkParameter("precision").second.c_str());
    
    // interactive-mode?
    if ( cmdlparser.checkParameter(formula).first == true )
      interactive = false;
    
    // info-output in interactive-mode
    if ( interactive == true )
      info(programname);
    
    // setup predefined variables
    varlist = new VariableList();
    varlist->insert("pi",new Complex(M_PI),true);
    varlist->insert("e",new Complex(M_E),true);
    varlist->insert("i",new Complex(0,1),true);

    // setup a functionlist for self-defined functions
    functionlist = new FunctionList();
    
    // load functions from file if given
    if ( cmdlparser.checkParameter(commands).first == true )
      load(varlist,functionlist,cmdlparser.checkParameter(commands).second,interactive);
    
    // run-once-mode
    if ( interactive == false ){

      MathExpression mathexpression(cmdlparser.checkParameter(formula).second.c_str(),varlist,functionlist);
      
      Value *value = mathexpression.eval();

      cout <<  value->toString(precision) << endl;
      
      delete functionlist;
      delete varlist;
      return(0);

    }

    fullprompt = string(programname) + prompt;

    for (;;){

      try{

	input = (char *)readline(fullprompt.c_str());

	if ( input.get() ){
	  if ( *input.get() ){
	    if ( current_history() ){
	      // only add new entries to history

	      if ( strcmp(input.get(),current_history()->line) )
		add_history(input.get());

	    }else
	      add_history(input.get());

	  } else
	    continue;
	} else {

	  if ( varlist->isModified() == true || functionlist->isModified() == true )
	    if ( checkAnswer(exit_modified_text) == false )
	      continue;

	  cout << endl;
	  
	  break;
	  
	}

	LineScanner lscanner = LineScanner(input.get()); 
	string firstword = lscanner.nextToken();
	
	if (!strcmp(input.get(),QUIT)){

	  if ( varlist->isModified() == true || functionlist->isModified() == true )
	    if ( checkAnswer(exit_modified_text) == false )
	      continue;

	  break;

	} else if (!strcmp(input.get(),HELP)){

	  show(programname,printHelp);
	  continue;

	} else if (!strcmp(input.get(),INFO)){

	  show(programname,info);
	  continue;

	} else if (!strcmp(input.get(),GPL)){

	  show(programname,gpl);
	  continue;

	} else if (!strcmp(input.get(),FON)){

	  bflag=true;
	  continue;

	} else if (!strcmp(input.get(),FOFF)){

	  bflag=false;
	  continue;

	} else if (!strcmp(input.get(),VARS)){

	  varlist->print(precision);
	  continue;

	} else if ( firstword == REMVAR ){

	  removeVariables(varlist,lscanner);
	  continue;

	} else if ( firstword == UNDEF ){

	  undefineFunctions(functionlist,lscanner);
	  continue;

	} else if ( firstword == SAVE ){

	  save(varlist,functionlist,precision,lscanner.nextToken(),lscanner);
	  continue;

	} else if ( firstword == LOAD ){

	  load(varlist,functionlist,lscanner.nextToken(),interactive);
	  continue;

	} else if ( firstword == SETPRECISION ){

	  string value = lscanner.nextToken();

	  if ( value != "" )
	    precision = (streamsize)atoi(value.c_str());

	  continue;

	} else if ( firstword == SHOWPRECISION ){

	  cout << precision << endl;
	  continue;

	} else if (!strcmp(input.get(),FUNCS)){

	  cout << functionlist->toString(precision);
	  continue;

	} else if (!strcmp(input.get(),BUILTINS)){

	  cout << MathExpression::builtinsToString();
	  continue;

	}

	MathExpression mathexpression(input.get(),varlist,functionlist);

	if ( bflag == true ){

	  mathexpression.print(precision);
	  cout << endl;

	}

	Value *value = mathexpression.eval();
	cout << value->toString(precision) << endl;

      } catch (FunctionDefinition &fd){

	if ( interactive == true )
	  cout << "function defined: '" << fd.getName() << "'" << endl;

      } catch ( ParseException &pe ){

	if ( interactive == true )
	  printErrorArrow(fullprompt.size() + pe.getPos());
	cerr << "parse-error: '" << pe.getMsg() << "'" << endl;

      } catch ( EvalException &ee ){

	// why this doesn't work sometimes I really don't know
	cerr << "evaluation-error: '" << ee.getMsg() << "'";

	if ( ee.getObjName() != "")
	  cerr << ": '" <<ee.getObjName() << "'";

	cerr << endl;

      }

      input.clear(true);

    }
    
    delete functionlist;
    delete varlist;
    return 0;

  } catch ( ParseException &pe ){

    // komischerweise werden ohne diesen catch-block die exceptions im inneren
    // nicht abgefangen !?!? (Ist eigentlich unnoetig)
    if ( interactive == true )
      printErrorArrow(pe.getPos());
    cerr << "parse-error: '" << pe.getMsg() << "'" << endl;
    exit(1);

  } catch ( ExceptionBase &e ){

    e.show();
    cerr << usage << endl;
    exit(1);

  }
  
}

string setupParser(CmdlParser& parser){

  parser.addShortoption('h',"print help");
  parser.shortsynonym('h') << 'H' << '?';

  parser.addShortoption('v',"print version");
  parser.shortsynonym('v') << 'V';

  parser.addOption("help","print help");
  parser.synonym("help") << "h" << "Help" << "H";

  parser.addOption("version","print version");
  parser.synonym("version") << "v" << "Version" << "V";

  parser.addParameter(formula,formula,"a formula to be evaluated");
  parser.synonym(formula) << "f" << "Formula" << "F";

  parser.addParameter("precision","value","display-precision of post decimal positions of floating-points");
  parser.synonym("precision") << "p" << "Precision" << "P";

  parser.addParameter(commands,commands,"commands that should be loaded before execution");
  parser.synonym(commands) << "c" << "Commands" << "C";

  return parser.usage();

}

void undefineFunctions(FunctionList *fl, LineScanner & lscanner){
  
  string fun;
  bool removed = false;

  while ( (fun = lscanner.nextToken()) != "" ){

    try{

      fl->remove(fun.c_str());
      cout << fun << " undefined" << endl;
      removed = true;

    } catch (Exception<FunctionList> &fle){
      cout << fle.getMsg() << ": " << fun << endl;
    }
  }
  
  if ( removed == false )
    cout << "nothing undefined" << endl;

}

void removeVariables(VariableList *vl, LineScanner & lscanner){

  string var;
  bool removed = false;

  while ( (var = lscanner.nextToken()) != "" ){

    try{

      vl->remove(var.c_str());
      cout << var << " removed" << endl;
      removed = true;

    } catch (Exception<VariableList> &vle){
      cout << vle.getMsg() << ": " << var << endl;
    }
  }
  
  if ( removed == false )
    cout << "nothing removed" << endl;

}
void save(VariableList *vl, FunctionList *fl, streamsize precision, string filename, LineScanner & lscanner){

  bool write = true;

  if ( filename == "" ){

    cout << "expecting filename!" << endl;
    return;

  }

  // check if file exists
  ifstream oldfile(filename.c_str());
  
  if ( oldfile != NULL ){

    ostringstream prompt;
    prompt << "file " << filename << " exists! Really overwrite it? (y,n) ";

    if ( checkAnswer(prompt.str()) == false )
      write = false;

    oldfile.close();

  }
  
  if ( write == false )
    return;

  ofstream file(filename.c_str());

  if ( file == NULL ){

    cout << "can't save to file " << filename << endl;
    return;

  }

  file << vl->toString(false,precision);
  file << fl->toString(precision);

  cout << "commands saved to file \"" << filename << "\"" << endl;

  vl->setModified(false);
  fl->setModified(false);
  
}

void load(VariableList *vl, FunctionList *fl,  string filename, bool interactive){

  // save initial state
  bool vl_modified = vl->isModified();
  bool fl_modified = fl->isModified();

  if ( filename == "" ){

    cerr << "expecting filename!" << endl;
    return;

  }

  ifstream file(filename.c_str());

  if ( file == NULL ){

    cerr << "file " << filename << " doesn't exist!" << endl;
    return;

  }

  FileScanner fscanner(file);
  string line;

  while ( (line = fscanner.nextLine()) != "" ){

    try{

      if ( interactive == true )
	cout << "loading " << line.c_str() << endl;

      MathExpression mathexpression(line.c_str(),vl,fl);
      mathexpression.eval();
      
    } catch (FunctionDefinition &fd){ 

      // nop

    } catch (ParseException &pe){

      if ( interactive == true )
	printErrorArrow(pe.getPos());
      cerr << "parse-error: " << pe.getMsg() << endl;

    } catch (EvalException &ee){

      cout << ee.getMsg();

      if ( ee.getObjName() != "")
	cout << " : " << ee.getObjName();
      cout << endl;

    }

  }

  // reset initial state
  vl->setModified(vl_modified);
  fl->setModified(fl_modified);

}

bool checkAnswer(const string & text){

  char *answer;
  bool result = true;

  answer = (char *)readline(text.c_str());

  if ( answer ){

    if ( strcmp(answer,"y") )
      result = false;

    free(answer);
    
  } else{

    cout << endl;
    result = false;

  }

  return result;

}

void printErrorArrow(int pos){

  if ( pos <=0 )
    return;

  for ( int i = 0; i < pos-1; i++ )
    cout << " ";

  cout << "^" << endl;

}

void info(const char *appname){

  cout << "\n" << appname << " version " << version
       << ", Copyright (C) 1999-2007  Friedemann Zintel\n";
  cout << "email: friezi@cs.tu-berlin.de\n";
  cout << "If you have any questions, comments or suggestions about ";
  cout << "this programm,\n";
  cout << "please feel free to email them to me.\n";
  cout << "To obtain the full source-code, write me an email.\n\n";
  cout << appname << " comes with ABSOLUTELY NO WARRANTY, for details ";
  cout << "type \"" << GPL << "\".\n";
  cout << "This is free software, and you are welcome to redistribute it\n";
  cout << "under certain conditions; type \"" << GPL;
  cout << "\" for to read the GPL.\n\n";
  cout << appname << " is a calculator/evaluator of mathematical formulas/expressions.\n"
       << "Type \"" << HELP << "\" for help, \"" << QUIT << "\" or CTRL-D to quit\n\n";
}

void show(const char *pname, void (*what)(const char *)){

  char *proc[]={SHOWHELP,0};
  char *proc2[]={SHOWHELP2,0};
  int filedes[2];
  int status;

  if ( !fork() ){

    pipe(filedes);
    close(0);
    dup(filedes[0]);
    close(filedes[0]);

    if ( !fork() ){

      close(1);
      dup(filedes[1]);
      close(filedes[1]);
      (*what)(pname);
      exit(0);

    }

    close(filedes[1]);
    execvp(proc[0],proc);
    (*what)(pname);
    execvp(proc2[0],proc);
    cout << "neither \"" << SHOWHELP << "\" nor \"" << SHOWHELP2
	 << "\" installed!\n";
    exit(1);

  } else
    wait(&status);

}

void printHelp(const char *pname){
  cout << "help:\n\n"
       << "the following commands are accessible:" << endl << endl
       << HELP << "\t\tdisplay this helpfile" << endl
       << INFO << "\t\tdisplay application-information" << endl
       << GPL << "\t\tdisplays the GPL" << endl
       << QUIT << "\t\tends the program (same as Ctrl-D)" << endl
       << VARS << "\t\tdisplays all variables with values" << endl
       << REMVAR << " [var1 [...]]" << "\t\tundefines the variables" << endl
       << UNDEF << " [fun1 [...]]" << "\t\tundefines the functions" << endl
       << FUNCS << "\t\tdisplays all user-defined functions\n"
       << BUILTINS << "\tdisplays all builtin-functions and -operators" << endl
       << SAVE << " <filename>" << "\t\tsaves all user-defined variables and commands to file <filename>" << endl
       << LOAD << " <filename>" << "\t\tloads all user-defined variables and commands from file <filename>" << endl
       << SETPRECISION << " <value>" << "\tsets the display-precision for the post decimal position for floating-points" << endl
       << SHOWPRECISION << "\t\tdisplays the display-precision for the post decimal position for floatong-points" << endl
       << FON << "\t\tdisplays the formula" << endl
       << FOFF << "\t\thides the formula" << endl
       << "\n\n"
       << "In " << pname << " you can type a mathematical formula which will"
       << " be evaluated. The\nspeciality of " << pname << " is that you can "
       << "omit many round brackets according\nto the mathematical "
       << "conventions. E.g. you can type \"sin2cos2\" instead "
       << "of\n\"sin(2)*cos(2)\", or \"sin^2(4)\" instead of \"(sin(4))^2\". "
       << "Of course you can\nalso use the bracketing. But note, that without"
       << " bracketing only the first\nexpression without operator is taken "
       << "as argument for a function. If you are not\nshure, how the "
       << "expression will be interpreted just "
       << "switch on the display of\nthe formula with \"" << FON << "\".\n"
       << "Note that an unary minus res. plus is only allowed "
       << "at the beginning\n"
       << "of the expression (but everything within braces is a "
       << "local expression).\n"
       << "You may also define variables, e.g. \"a=ln2\", \"b=(-a)\", it's "
       << "possible even within\nexpressions like \"4*(r=3^4)+7\"."
       << "The variables \"e\" and \"pi\" are predefined and\ncan't be "
       << "redefined.\n\n"
       << "It's even possible to define your own fuctions.\n"
       << "You can define a function using the following syntax:\n"
       << "<fun>([<lvar>[,<lvar>[,...]]])=<expr>\n"
       << "where <fun> means the functionname, <lvar> a local parameter and\n"
       << "<expr> "
       << "a math.-expression. Everything within the brackets [] "
       << "is optional.\n"
       << "Example: pow(x,y)=x^y\n"
       << "Within the function you can also use "
       << "global variables.\n\n"
       << "The following operators are predefined:\n\n"
       << "+\taddition\n-\tsubtraction\n*\tmultiplication\n/\t"
       << "division\n\\\tnon-broken division\n%\tmodulo\n!\tfaculty\n"
       << "@\tthe operator for binomial coefficient (e.g. a@b means a choose "
       << "b, i.e.\n\ta!/(b!(a-b)!) )\n=\tassignment to variables\n\n"
       << "The following functions are predefined (\"var\" means local variable, "
       << "\"exp\" means\nmath.-expression):\n\n"
       << "Sum[<var>=<exp1>;<exp2>](<exp3>)\tsum from <var>=<exp1> to "
       << "<exp2>\n\t\t\t\t\tover <exp3>\n"
       << "Prod[<var>=<exp1>;<exp2>](<exp3>)\tsame with Produkt\n"
       << "log[<exp1>](<exp2>)\t\t\tthe logarithm of <exp2> to the "
       << "base\n\t\t\t\t\tof <exp1>\n"
       << "\nThe following functions need all a mathematical "
       << "expression as argument:\n\n"
       << "ln\t\t\t\tthe logarithm to the base of e\n"
       << "ld\t\t\t\tthe logarithm to the base of 2\n"
       << "exp\t\t\t\tthe power to the base of e\n"
       << "sin\t\t\t\tthe sine\n"
       << "cos\t\t\t\tthe cosine\n"
       << "tan\t\t\t\tthe tangent\n"
       << "asin\t\t\t\tthe arc sine\n"
       << "acos\t\t\t\tthe arc cosine\n"
       << "atan\t\t\t\tthe arc tangent\n"
       << "sinh\t\t\t\tthe hyperbolic sine\n"
       << "cosh\t\t\t\tthe hyperbolic cosine\n"
       << "tanh\t\t\t\tthe hyperbolic tangent\n"
       << "asinh\t\t\t\tthe inverse hyperbolic sine\n"
       << "acosh\t\t\t\tthe inverse hyperbolic cosine\n"
       << "atanh\t\t\t\tthe inverse hyperbolic tangent\n"
       << "sgn\t\t\t\tthe signum\n"
       << "tst\t\t\t\ttests if value is unequal to zero\n"
       << "\n"
       << "the priorities of the operator's are as follows:\n\n"
       << "high pri\n\n"
       << "   .\n"
       << "  / \\\n"
       << "   |\t!\n"
       << "   |\t@\n"
       << "   |\tfunctions\n"
       << "   |\t^\n"
       << "   |\t* / \\ %\n"
       << "   |\t+ -\n"
       << "   |\t=\n"
       << "   |\t, ;\n"
       << "low pri\n";
  cout << "\n\n";
}

void gpl(const char *nix){

  cout << "		    GNU GENERAL PUBLIC LICENSE\n"
       << "		       Version 2, June 1991\n"
       << "\n"
       << " Copyright (C) 1989, 1991 Free Software Foundation, Inc.\n"
       << "     59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
       << " Everyone is permitted to copy and distribute verbatim copies\n"
       << " of this license document, but changing it is not allowed.\n"
       << "\n"
       << "			    Preamble\n"
       << "\n"
       << "  The licenses for most software are designed to take away your\n"
       << "freedom to share and change it.  By contrast, the GNU General Public\n"
       << "License is intended to guarantee your freedom to share and change free\n"
       << "software--to make sure the software is free for all its users.  This\n"
       << "General Public License applies to most of the Free Software\n"
       << "Foundation's software and to any other program whose authors commit to\n"
       << "using it.  (Some other Free Software Foundation software is covered by\n"
       << "the GNU Library General Public License instead.)  You can apply it to\n"
       << "your programs, too.\n"
       << "\n"
       << "  When we speak of free software, we are referring to freedom, not\n"
       << "price.  Our General Public Licenses are designed to make sure that you\n"
       << "have the freedom to distribute copies of free software (and charge for\n"
       << "this service if you wish), that you receive source code or can get it\n"
       << "if you want it, that you can change the software or use pieces of it\n"
       << "in new free programs; and that you know you can do these things.\n"
       << "\n"
       << "  To protect your rights, we need to make restrictions that forbid\n"
       << "anyone to deny you these rights or to ask you to surrender the rights.\n"
       << "These restrictions translate to certain responsibilities for you if you\n"
       << "distribute copies of the software, or if you modify it.\n"
       << "\n"
       << "  For example, if you distribute copies of such a program, whether\n"
       << "gratis or for a fee, you must give the recipients all the rights that\n"
       << "you have.  You must make sure that they, too, receive or can get the\n"
       << "source code.  And you must show them these terms so they know their\n"
       << "rights.\n"
       << "\n"
       << "  We protect your rights with two steps: (1) copyright the software, and\n"
       << "(2) offer you this license which gives you legal permission to copy,\n"
       << "distribute and/or modify the software.\n"
       << "\n"
       << "  Also, for each author's protection and ours, we want to make certain\n"
       << "that everyone understands that there is no warranty for this free\n"
       << "software.  If the software is modified by someone else and passed on, we\n"
       << "want its recipients to know that what they have is not the original, so\n"
       << "that any problems introduced by others will not reflect on the original\n"
       << "authors' reputations.\n"
       << "\n"
       << "  Finally, any free program is threatened constantly by software\n"
       << "patents.  We wish to avoid the danger that redistributors of a free\n"
       << "program will individually obtain patent licenses, in effect making the\n"
       << "program proprietary.  To prevent this, we have made it clear that any\n"
       << "patent must be licensed for everyone's free use or not licensed at all.\n"
       << "\n"
       << "  The precise terms and conditions for copying, distribution and\n"
       << "modification follow.\n"
       << "\n"
       << "		    GNU GENERAL PUBLIC LICENSE\n"
       << "   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n"
       << "\n"
       << "  0. This License applies to any program or other work which contains\n"
       << "a notice placed by the copyright holder saying it may be distributed\n"
       << "under the terms of this General Public License.  The \"Program\", below,\n"
       << "refers to any such program or work, and a \"work based on the Program\"\n"
       << "means either the Program or any derivative work under copyright law:\n"
       << "that is to say, a work containing the Program or a portion of it,\n"
       << "either verbatim or with modifications and/or translated into another\n"
       << "language.  (Hereinafter, translation is included without limitation in\n"
       << "the term \"modification\".)  Each licensee is addressed as \"you\".\n"
       << "\n"
       << "Activities other than copying, distribution and modification are not\n"
       << "covered by this License; they are outside its scope.  The act of\n"
       << "running the Program is not restricted, and the output from the Program\n"
       << "is covered only if its contents constitute a work based on the\n"
       << "Program (independent of having been made by running the Program).\n"
       << "Whether that is true depends on what the Program does.\n"
       << "\n"
       << "  1. You may copy and distribute verbatim copies of the Program's\n"
       << "source code as you receive it, in any medium, provided that you\n"
       << "conspicuously and appropriately publish on each copy an appropriate\n"
       << "copyright notice and disclaimer of warranty; keep intact all the\n"
       << "notices that refer to this License and to the absence of any warranty;\n"
       << "and give any other recipients of the Program a copy of this License\n"
       << "along with the Program.\n"
       << "\n"
       << "You may charge a fee for the physical act of transferring a copy, and\n"
       << "you may at your option offer warranty protection in exchange for a fee.\n"
       << "\n"
       << "  2. You may modify your copy or copies of the Program or any portion\n"
       << "of it, thus forming a work based on the Program, and copy and\n"
       << "distribute such modifications or work under the terms of Section 1\n"
       << "above, provided that you also meet all of these conditions:\n"
       << "\n"
       << "    a) You must cause the modified files to carry prominent notices\n"
       << "    stating that you changed the files and the date of any change.\n"
       << "\n"
       << "    b) You must cause any work that you distribute or publish, that in\n"
       << "    whole or in part contains or is derived from the Program or any\n"
       << "    part thereof, to be licensed as a whole at no charge to all third\n"
       << "    parties under the terms of this License.\n"
       << "\n"
       << "    c) If the modified program normally reads commands interactively\n"
       << "    when run, you must cause it, when started running for such\n"
       << "    interactive use in the most ordinary way, to print or display an\n"
       << "    announcement including an appropriate copyright notice and a\n"
       << "    notice that there is no warranty (or else, saying that you provide\n"
       << "    a warranty) and that users may redistribute the program under\n"
       << "    these conditions, and telling the user how to view a copy of this\n"
       << "    License.  (Exception: if the Program itself is interactive but\n"
       << "    does not normally print such an announcement, your work based on\n"
       << "    the Program is not required to print an announcement.)\n"
       << "\n"
       << "These requirements apply to the modified work as a whole.  If\n"
       << "identifiable sections of that work are not derived from the Program,\n"
       << "and can be reasonably considered independent and separate works in\n"
       << "themselves, then this License, and its terms, do not apply to those\n"
       << "sections when you distribute them as separate works.  But when you\n"
       << "distribute the same sections as part of a whole which is a work based\n"
       << "on the Program, the distribution of the whole must be on the terms of\n"
       << "this License, whose permissions for other licensees extend to the\n"
       << "entire whole, and thus to each and every part regardless of who wrote it.\n"
       << "\n"
       << "Thus, it is not the intent of this section to claim rights or contest\n"
       << "your rights to work written entirely by you; rather, the intent is to\n"
       << "exercise the right to control the distribution of derivative or\n"
       << "collective works based on the Program.\n"
       << "\n"
       << "In addition, mere aggregation of another work not based on the Program\n"
       << "with the Program (or with a work based on the Program) on a volume of\n"
       << "a storage or distribution medium does not bring the other work under\n"
       << "the scope of this License.\n"
       << "\n"
       << "  3. You may copy and distribute the Program (or a work based on it,\n"
       << "under Section 2) in object code or executable form under the terms of\n"
       << "Sections 1 and 2 above provided that you also do one of the following:\n"
       << "\n"
       << "    a) Accompany it with the complete corresponding machine-readable\n"
       << "    source code, which must be distributed under the terms of Sections\n"
       << "    1 and 2 above on a medium customarily used for software interchange; or,\n"
       << "\n"
       << "    b) Accompany it with a written offer, valid for at least three\n"
       << "    years, to give any third party, for a charge no more than your\n"
       << "    cost of physically performing source distribution, a complete\n"
       << "    machine-readable copy of the corresponding source code, to be\n"
       << "    distributed under the terms of Sections 1 and 2 above on a medium\n"
       << "    customarily used for software interchange; or,\n"
       << "\n"
       << "    c) Accompany it with the information you received as to the offer\n"
       << "    to distribute corresponding source code.  (This alternative is\n"
       << "    allowed only for noncommercial distribution and only if you\n"
       << "    received the program in object code or executable form with such\n"
       << "    an offer, in accord with Subsection b above.)\n"
       << "\n"
       << "The source code for a work means the preferred form of the work for\n"
       << "making modifications to it.  For an executable work, complete source\n"
       << "code means all the source code for all modules it contains, plus any\n"
       << "associated interface definition files, plus the scripts used to\n"
       << "control compilation and installation of the executable.  However, as a\n"
       << "special exception, the source code distributed need not include\n"
       << "anything that is normally distributed (in either source or binary\n"
       << "form) with the major components (compiler, kernel, and so on) of the\n"
       << "operating system on which the executable runs, unless that component\n"
       << "itself accompanies the executable.\n"
       << "\n"
       << "If distribution of executable or object code is made by offering\n"
       << "access to copy from a designated place, then offering equivalent\n"
       << "access to copy the source code from the same place counts as\n"
       << "distribution of the source code, even though third parties are not\n"
       << "compelled to copy the source along with the object code.\n"
       << "\n"
       << "  4. You may not copy, modify, sublicense, or distribute the Program\n"
       << "except as expressly provided under this License.  Any attempt\n"
       << "otherwise to copy, modify, sublicense or distribute the Program is\n"
       << "void, and will automatically terminate your rights under this License.\n"
       << "However, parties who have received copies, or rights, from you under\n"
       << "this License will not have their licenses terminated so long as such\n"
       << "parties remain in full compliance.\n"
       << "\n"
       << "  5. You are not required to accept this License, since you have not\n"
       << "signed it.  However, nothing else grants you permission to modify or\n"
       << "distribute the Program or its derivative works.  These actions are\n"
       << "prohibited by law if you do not accept this License.  Therefore, by\n"
       << "modifying or distributing the Program (or any work based on the\n"
       << "Program), you indicate your acceptance of this License to do so, and\n"
       << "all its terms and conditions for copying, distributing or modifying\n"
       << "the Program or works based on it.\n"
       << "\n"
       << "  6. Each time you redistribute the Program (or any work based on the\n"
       << "Program), the recipient automatically receives a license from the\n"
       << "original licensor to copy, distribute or modify the Program subject to\n"
       << "these terms and conditions.  You may not impose any further\n"
       << "restrictions on the recipients' exercise of the rights granted herein.\n"
       << "You are not responsible for enforcing compliance by third parties to\n"
       << "this License.\n"
       << "\n"
       << "  7. If, as a consequence of a court judgment or allegation of patent\n"
       << "infringement or for any other reason (not limited to patent issues),\n"
       << "conditions are imposed on you (whether by court order, agreement or\n"
       << "otherwise) that contradict the conditions of this License, they do not\n"
       << "excuse you from the conditions of this License.  If you cannot\n"
       << "distribute so as to satisfy simultaneously your obligations under this\n"
       << "License and any other pertinent obligations, then as a consequence you\n"
       << "may not distribute the Program at all.  For example, if a patent\n"
       << "license would not permit royalty-free redistribution of the Program by\n"
       << "all those who receive copies directly or indirectly through you, then\n"
       << "the only way you could satisfy both it and this License would be to\n"
       << "refrain entirely from distribution of the Program.\n"
       << "\n"
       << "If any portion of this section is held invalid or unenforceable under\n"
       << "any particular circumstance, the balance of the section is intended to\n"
       << "apply and the section as a whole is intended to apply in other\n"
       << "circumstances.\n"
       << "\n"
       << "It is not the purpose of this section to induce you to infringe any\n"
       << "patents or other property right claims or to contest validity of any\n"
       << "such claims; this section has the sole purpose of protecting the\n"
       << "integrity of the free software distribution system, which is\n"
       << "implemented by public license practices.  Many people have made\n"
       << "generous contributions to the wide range of software distributed\n"
       << "through that system in reliance on consistent application of that\n"
       << "system; it is up to the author/donor to decide if he or she is willing\n"
       << "to distribute software through any other system and a licensee cannot\n"
       << "impose that choice.\n"
       << "\n"
       << "This section is intended to make thoroughly clear what is believed to\n"
       << "be a consequence of the rest of this License.\n"
       << "\n"
       << "  8. If the distribution and/or use of the Program is restricted in\n"
       << "certain countries either by patents or by copyrighted interfaces, the\n"
       << "original copyright holder who places the Program under this License\n"
       << "may add an explicit geographical distribution limitation excluding\n"
       << "those countries, so that distribution is permitted only in or among\n"
       << "countries not thus excluded.  In such case, this License incorporates\n"
       << "the limitation as if written in the body of this License.\n"
       << "\n"
       << "  9. The Free Software Foundation may publish revised and/or new versions\n"
       << "of the General Public License from time to time.  Such new versions will\n"
       << "be similar in spirit to the present version, but may differ in detail to\n"
       << "address new problems or concerns.\n"
       << "\n"
       << "Each version is given a distinguishing version number.  If the Program\n"
       << "specifies a version number of this License which applies to it and \"any\n"
       << "later version\", you have the option of following the terms and conditions\n"
       << "either of that version or of any later version published by the Free\n"
       << "Software Foundation.  If the Program does not specify a version number of\n"
       << "this License, you may choose any version ever published by the Free Software\n"
       << "Foundation.\n"
       << "\n"
       << "  10. If you wish to incorporate parts of the Program into other free\n"
       << "programs whose distribution conditions are different, write to the author\n"
       << "to ask for permission.  For software which is copyrighted by the Free\n"
       << "Software Foundation, write to the Free Software Foundation; we sometimes\n"
       << "make exceptions for this.  Our decision will be guided by the two goals\n"
       << "of preserving the free status of all derivatives of our free software and\n"
       << "of promoting the sharing and reuse of software generally.\n"
       << "\n"
       << "			    NO WARRANTY\n"
       << "\n"
       << "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n"
       << "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n"
       << "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n"
       << "PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n"
       << "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
       << "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n"
       << "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n"
       << "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n"
       << "REPAIR OR CORRECTION.\n"
       << "\n"
       << "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"
       << "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n"
       << "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n"
       << "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n"
       << "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n"
       << "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n"
       << "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n"
       << "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n"
       << "POSSIBILITY OF SUCH DAMAGES.\n"
       << "\n"
       << "		     END OF TERMS AND CONDITIONS\n"
       << "\n"
       << "	    How to Apply These Terms to Your New Programs\n"
       << "\n"
       << "  If you develop a new program, and you want it to be of the greatest\n"
       << "possible use to the public, the best way to achieve this is to make it\n"
       << "free software which everyone can redistribute and change under these terms.\n"
       << "\n"
       << "  To do so, attach the following notices to the program.  It is safest\n"
       << "to attach them to the start of each source file to most effectively\n"
       << "convey the exclusion of warranty; and each file should have at least\n"
       << "the \"copyright\" line and a pointer to where the full notice is found.\n"
       << "\n"
       << "    <one line to give the program's name and a brief idea of what it does.>\n"
       << "    Copyright (C) <year>  <name of author>\n"
       << "\n"
       << "    This program is free software; you can redistribute it and/or modify\n"
       << "    it under the terms of the GNU General Public License as published by\n"
       << "    the Free Software Foundation; either version 2 of the License, or\n"
       << "    (at your option) any later version.\n"
       << "\n"
       << "    This program is distributed in the hope that it will be useful,\n"
       << "    but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
       << "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
       << "    GNU General Public License for more details.\n"
       << "\n"
       << "    You should have received a copy of the GNU General Public License\n"
       << "    along with this program; if not, write to the Free Software\n"
       << "    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
       << "\n"
       << "\n"
       << "Also add information on how to contact you by electronic and paper mail.\n"
       << "\n"
       << "If the program is interactive, make it output a short notice like this\n"
       << "when it starts in an interactive mode:\n"
       << "\n"
       << "    Gnomovision version 69, Copyright (C) year  name of author\n"
       << "    Gnomovision comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\n"
       << "    This is free software, and you are welcome to redistribute it\n"
       << "    under certain conditions; type `show c' for details.\n"
       << "\n"
       << "The hypothetical commands `show w' and `show c' should show the appropriate\n"
       << "parts of the General Public License.  Of course, the commands you use may\n"
       << "be called something other than `show w' and `show c'; they could even be\n"
       << "mouse-clicks or menu items--whatever suits your program.\n"
       << "\n"
       << "You should also get your employer (if you work as a programmer) or your\n"
       << "school, if any, to sign a \"copyright disclaimer\" for the program, if\n"
       << "necessary.  Here is a sample; alter the names:\n"
       << "\n"
       << "  Yoyodyne, Inc., hereby disclaims all copyright interest in the program\n"
       << "  `Gnomovision' (which makes passes at compilers) written by James Hacker.\n"
       << "\n"
       << "  <signature of Ty Coon>, 1 April 1989\n"
       << "  Ty Coon, President of Vice\n"
       << "\n"
       << "This General Public License does not permit incorporating your program into\n"
       << "proprietary programs.  If your program is a subroutine library, you may\n"
       << "consider it more useful to permit linking proprietary applications with the\n"
       << "library.  If this is what you want to do, use the GNU Library General\n"
       << "Public License instead of this License.\n";
}
