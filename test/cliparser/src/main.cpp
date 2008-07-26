#include "main.hpp"

using namespace std;
using namespace exc;
using namespace cmdl;

string setupParser(CmdlParser& parser);

int main(int argc, char **argv){

  string usage;

  try{
    
    
    CmdlParser parser(argc,argv);
    
    usage = setupParser(parser);
    
    parser.parse();
    
    // we must only check for representatives. checking for non-representative keys won't be succesful
    if ( parser.checkOption("help") == true || parser.checkShortoption('h') == true ){

      cout << parser.usage() << endl;
      cout << parser.infoUsage();
      exit(0);

    }

    if ( parser.checkOption("version") == true || parser.checkShortoption('v') == true ){

      cout << "alphanull" << endl;
      exit(0);

    }

    cerr << usage << endl;
    
    cout << parser.contents() << endl;
    
    cout << "a is contained: " << parser.checkShortoption('a') << endl;
    cout << "q is contained: " << parser.checkShortoption('q') << endl;
    
    pair<bool,string> res;
    
    if ( (res = parser.checkParameter("parameter1")).first == false )
      cout << "parameter1 not contained" << endl;
    else
      cout << "parameter1 has value: " << res.second << endl;
    
    if ( (res = parser.checkParameter("parameter_xyz")).first == false )
      cout << "parameter_xyz not contained" << endl;
    else
      cout << "parameter_xyz has value: " << res.second << endl;
    
    cout << "provided arguments:" << endl;
    for ( CmdlParser::Arguments::const_iterator it = parser.beginArguments(); it != parser.endArguments(); it++ )
      cout << *it << endl;
    
    cout << "source=" << parser.getMandatoryArgument("source") << endl;

    cout << "final argument is: " << parser.getFinalArgumentId() << " = " << parser.getFinalArgumentValue() << endl;

    cout << endl << parser.infoUsage();

  } catch (ExceptionBase &e){

    e.show();
    cerr << usage << endl;

  }
  
  return 0;
}

string setupParser(CmdlParser& parser){

  parser.addShortoption('b',"byte order");
  parser.addShortoption('a',"ASCII");
  parser.addShortoption('g',"generalization");
  parser.addShortoption('h',"help");
  parser.addShortoption('v',"version");

  parser.shortsynonym('h') << '?';

  parser.addOption("option1","description for option1");
  parser.addOption("option2","");
  parser.addOption("help","print this helpscreen");
  parser.addOption("version","print version");

  parser.synonym("help") << "h";
  parser.synonym("version") << "v";
  parser.synonym("option1") << "o" << "opt";

  parser.addParameter("parameter1","value1","description for parameter1");
  parser.addParameter("printer","printer","description for printer",true);
  parser.addParameter("color","value","description for color",true);

  parser.synonym("printer") << "p";

  parser.addMultiParameter("lib","lib","a library");
  parser.addMultiParameter("include","includedir","Include-directory");

  parser.synonym("lib") << "library" << "l";

  parser.synonym("include") << "inc" << "i" << "incl" << "includedir";

  parser.alias("all") << "option1" << "option2";

  parser.shortalias('x') << 'b' << 'a' << 'g';

  parser.shortsupervisor() << 'h' << 'v';
  parser.supervisor() << "help" << "version";

  parser.addMandatoryArgument("source","The Source");
  parser.addMandatoryArgument("target","The Target");

  parser.setInfiniteArguments("filename");

  parser.setFinalArgument("directory");

  parser.allowRelaxedSyntax();

  return parser.usage();

}
