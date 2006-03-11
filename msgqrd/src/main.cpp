#include "main.hpp"

using namespace std;
using namespace cmdl;

int main(int argc, char **argv){
 
  int loglvl = LOG_NOTICE;
  string progname(basename(argv[0]));
  string usage;
  string stream_path = STREAM_PATH;
  string pidfile = string("/var/run/") + string(::basename(argv[0])) + string(".pid");
  int status = 0;
  bool daemon = true;
//   struct servent *msgqrd_entry;
 
  try{

    // Kommandozeile parsen
    CmdlParser cmdlparser(argc,argv);

    usage = setCmdlOptions(cmdlparser);
    cmdlparser.parse();

    // Optionen und Parameter abfragen
    if ( cmdlparser.checkShortoption('?') || cmdlparser.checkOption("help") )
      help_exit(cmdlparser);
    
    if ( cmdlparser.checkOption("version") )
      version_exit();

    if ( cmdlparser.checkOption("nodaemon") )
      daemon = false;

    // remove existing pidfile on option "force"
    if ( cmdlparser.checkOption("force") )
      ::unlink(pidfile.c_str());

    // modify socket-path
    pair<bool,string> par_socket;
    if ( (par_socket = cmdlparser.checkParameter("socket")).first == true )
      stream_path = par_socket.second;

    // modify loglevel
    pair<bool,string> par_loglvl;
    if ( (par_loglvl = cmdlparser.checkParameter("l")).first == true )
      loglvl = atoi(par_loglvl.second.c_str());
   

    // inet socket   
    //     // Portnummer abfragen
    //     msgqrd_entry = getservbyname("msgqrd","tcp");
    //     if (!msgqrd_entry){
    //       cerr << "in /etc/services no entry for msgqrd protocol:tcp!" << endl;
    //       return -1;
    //     }
    
    //     MsgQueueRegDaemon msgqrd(daemon,ntohs(msgqrd_entry->s_port),progname,loglvl);

    // local socket
    MsgQueueRegDaemon msgqrd(daemon,stream_path,progname,loglvl);
    
    openlog(progname.c_str(),LOG_PID,LOG_USER);
    
    msgqrd.start();

  } catch (Exception<CmdlParser>& e){
    e.show();
    cerr << usage << endl;
    
  } catch (Exception_T& e){
    e.show();
    status = -1;
  }
  
  closelog();

  return status;

}

void help_exit(CmdlParser& parser){

  cerr << parser.usage() << endl;
  cerr << parser.getProgramname() << ": messagequeue-registration daemon\n"; 
  cerr << "version: " << VERSION << endl;
  cerr << parser.infoUsage();
  exit(0);
}

void version_exit(){

  cerr << VERSION << endl;
  exit(0);
}

string setCmdlOptions(CmdlParser& parser){

  parser.addShortoption('?',"help");
  parser.shortsynonym('?') << 'h';
  parser.addOption("help","shows this helpscreen");
  parser.synonym("help") << "h" << "?";
  parser.addOption("version","version-information");
  parser.synonym("version") << "v";
  parser.addOption("nodaemon","starts program not in background");
  parser.addParameter("socket","socket-path","uses <socket-path> as the socket-file instead of default-value");
  parser.synonym("socket") << "s";
  parser.addOption("force","an already existing pidfile will be erased so that the program can run; attention: an existing socket will be erased as well");
  parser.synonym("force") << "f";
  parser.addParameter("l","loglevel","the level for logging");
  return parser.usage();
}
