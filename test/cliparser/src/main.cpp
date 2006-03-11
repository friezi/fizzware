#include "main.hpp"

using namespace std;
using namespace cmdl;

string setupParser(CmdlParser& parser);

int main(int argc, char **argv){

  string usage;

  try{
    
    
    CmdlParser parser(argc,argv);
    
    usage = setupParser(parser);
    
    parser.parse();

    if ( parser.checkOption("help") == true ){

      cout << parser.usage() << endl;
      cout << parser.infoUsage();
      exit(0);
    }

    cerr << usage << endl;
    
    cout << parser.contents() << endl;
    
    cout << "a ist drin: " << parser.checkShortoption('a') << endl;
    cout << "q ist drin: " << parser.checkShortoption('q') << endl;
    
    pair<bool,string> res;
    
    if ( (res = parser.checkParameter("parameter1")).first == false )
      cout << "parameter1 nicht vorhanden" << endl;
    else
      cout << "parameter1 hat den Wert: " << res.second << endl;
    
    if ( (res = parser.checkParameter("parameter_xyz")).first == false )
      cout << "parameter_xyz nicht vorhanden" << endl;
    else
      cout << "parameter_xyz hat den Wert: " << res.second << endl;
    
    cout << "Die Argumente waren:" << endl;
    for ( CmdlParser::Arguments::const_iterator it = parser.beginArguments(); it != parser.endArguments(); it++ )
      cout << *it << endl;
    
    cout << "source=" << parser.getObligatoryArgument("source") << endl;

    cout << "Final-Argument ist: " << parser.getFinalArgumentId() << " = " << parser.getFinalArgumentValue() << endl;

    cout << endl << parser.infoUsage();

  } catch (Exception_T &e){
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

  parser.shortsynonym('h') << '?';

  parser.addOption("option1","description for option1");
  parser.addOption("option2","");
  parser.addOption("help","print this helpscreen");
  parser.synonym("help") << "h";

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

  parser.addObligatoryArgument("source","The Source");
  parser.addObligatoryArgument("target","The Target");

  parser.setInfiniteArguments("filename");

  parser.setFinalArgument("directory");

  return parser.usage();
}
