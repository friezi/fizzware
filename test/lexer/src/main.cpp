#include "main.hpp"

using namespace exc;
using namespace parse;
using namespace cmdl;
using namespace std;
using namespace utils;

int main(int argc, char ** argv){

  CmdlParser cmdl(argc,argv);

  string usage;

  try{

    usage = setupCmdlParser(cmdl);

    cmdl.parse();

    if ( cmdl.checkShortoption('h') == true ){

      cout << cmdl.infoUsage() << endl;
      exit(0);

    }

    ifstream input(cmdl.getParameter("file").c_str());

    if ( input == false ){

      cerr << "could not open " << cmdl.getParameter("file") << endl;
      exit(1);

    }

//     char c;
//     while ( input.eof() == false ){
//       input.get(c);
//       if ( c == '\n' )
// 	cout << "\\n";
//       else if ( c == '\r' )
// 	cout << "\\r" ;
      
//       cout << c;

//     }

//     exit(0);

    LexScanner scanner(&input);

    scanner.getLexCharClasses().setEscape('\\');

    if ( cmdl.checkShortoption('e') == true )
      scanner.reportEOL(true);

    if ( cmdl.checkShortoption('w') == true )
      scanner.reportWhite(true);

    if ( cmdl.checkShortoption('c') == true )
      scanner.supportNestedComments(true);

    if ( cmdl.checkShortoption('n') == true )
      scanner.reportNumbersAsReal();

    if ( cmdl.checkShortoption('l') == true )
      scanner.lowerCaseMode();

    if ( cmdl.checkShortoption('f') == true )
      scanner.useFloatingpoints();

    if ( cmdl.checkShortoption('s') == true )
      scanner.useSignedNumbers();

    if ( cmdl.checkShortoption('r') == true )
      scanner.useRawQuoting();

    if ( cmdl.checkShortoption('u') == true )
      scanner.reportWhiteUncollapsed(true);

    if ( cmdl.checkParameter("line-comment").first == true )
      scanner.getLexCharClasses().setLineComment(cmdl.getParameter("line-comment"));

    if ( cmdl.checkParameter("wc").first == true )
      scanner.getLexCharClasses().setWordConstituents(cmdl.getParameter("wc"));

    if ( cmdl.checkParameter("escape").first == true )
      scanner.getLexCharClasses().setEscape(cmdl.getParameter("escape")[0]);

    //    scanner.getLexCharClasses().word_constituents += ".";

    ostringstream output;

    if ( cmdl.checkOption("lines") ){

      do {

	scanner.nextLine();
	cout << scanner.tokenToString() << endl;
	cout.flush();
	
      } while ( scanner.token.type != LexToken::TT_EOF );
      
    } else {
    
      do {

	scanner.nextToken();
	cout << scanner.tokenToString() << " ";
	cout.flush();

	if ( cmdl.checkShortoption('o') == true ){

	  if ( scanner.token.type == LexToken::TT_WORD )
	    output << scanner.token.sval << " ";
	  else if ( scanner.token.type == LexToken::TT_NUMBER )
	    output << scanner.token.nval << " ";
	  else if ( scanner.token.type == LexToken::TT_EOL )
	    output << endl;
	  else if ( scanner.token.type == LexToken::TT_WHITE )
	    output << " ";
	  else if ( scanner.token.type == LexToken::TT_EOF )
	    ; // do nothing
	  else
	    output << (char)scanner.token.type;


	}	

      } while ( scanner.token.type != LexToken::TT_EOF );

    }
    
    cout << endl;

    if ( cmdl.checkShortoption('o') == true )
      cout << output.str();

    return 0;
    
  } catch (ExceptionBase &e){
    
    e.show();

    cout << usage << endl;
    
  }
  
}

string setupCmdlParser(CmdlParser & cmdl){

  cmdl.addParameter("file","filename","input-file",true);
  cmdl.addParameter("line-comment","character","line-comment character");
  cmdl.addParameter("wc","string","word-constituents");
  cmdl.addParameter("escape","escape-char","escape-character");
  cmdl.synonym("line-comment") << "lc";
  cmdl.synonym("escape") << "esc";
  cmdl.addShortoption('c',"nested comments");
  cmdl.addShortoption('n',"report numbers as real");
  cmdl.addShortoption('e',"report EOL");
  cmdl.addShortoption('l',"lowercase-mode");
  cmdl.addShortoption('w',"report Whitespace");
  cmdl.addShortoption('o',"output");
  cmdl.addShortoption('h',"help");
  cmdl.addShortoption('f',"floating-points");
  cmdl.addShortoption('s',"use signs");
  cmdl.addShortoption('r',"use raw quoting");
  cmdl.addShortoption('u',"whitespaces should not be collapsed");
  cmdl.addOption("lines","report whole lines");
  cmdl.shortsupervisor() << 'h';

  return cmdl.usage();

}
