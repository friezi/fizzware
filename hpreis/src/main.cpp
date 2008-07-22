#include "main.hpp"

using namespace std;
using namespace cmdl;

int main(int argc, char **argv){

  string usage;
  
  try{

    CmdlParser cmdlparser(argc,argv);

    usage = setupParser(cmdlparser);

    cmdlparser.parse();

    if ( cmdlparser.checkShortoption('h') == true ){

      cout << cmdlparser.usage() << endl;
      cout << cmdlparser.infoUsage() << endl;
      exit(0);

    }

    int it = ::atoi(cmdlparser.getParameter("iterations").c_str());

    if ( it < 0 )
      throw Exception<Error>("iterations are negative!");

    game(it,cmdlparser.checkShortoption('r'));


  } catch ( ExceptionBase &e ){

    e.show();
    cerr << usage << endl;

  }

  return 0;

}

string setupParser(CmdlParser &parser){

  parser.addShortoption('r',"random choice of door");
  parser.addParameter("iterations","value","iterations of calculation",true);
  parser.synonym("iterations") << "i";

  parser.addShortoption('h',"help");
  parser.shortsynonym('h') << '?';
  parser.shortsupervisor() << 'h';

  parser.allowRelaxedSyntax();

  return parser.usage();

}

void initDoors(set<long> &doors){

  doors.clear();
  doors.insert(0);
  doors.insert(1);
  doors.insert(2);

}

long getDoor(set<long> &doors, long nmb) throw (ExceptionBase){

  long cnt = 0;

  for ( set<long>::iterator it = doors.begin(); it != doors.end(); it++ ){
    
    if ( cnt == nmb )
      return *it;

    cnt++;

  }

  throw Exception<Error>("doors out of bounds!");

}

long percent(long value, long total){

  return (value * 100) / total;

}

void game(int iterations, bool rand) throw (ExceptionBase){

  long stay, change, clientdoor, pricedoor, gmdoor;
  stay = change = 0;
  set<long> doors;

  for ( int it = 0; it < iterations; it++ ){

    initDoors(doors);

    pricedoor = ::random()%3;
    clientdoor = ::random()%3;

    if ( clientdoor == pricedoor ){

      stay++;
      continue;

    }

    if ( rand != true ){

      change++;
      continue;

    } else {
      
      doors.erase(clientdoor);
      gmdoor = getDoor(doors,::random()%2);

      if ( gmdoor != pricedoor )
	change++;

    }

  }

  long total = stay + change;
  long staypercent, changepercent;

  staypercent = percent(stay,total);
  changepercent = percent(change,total);

  cout << "stay: " << staypercent << "%" << endl << "change: " << changepercent << "%" << endl;
  
}
