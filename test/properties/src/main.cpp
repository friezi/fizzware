#include "main.hpp"

using namespace std;
using namespace cmdl;

void setupParser(CmdlParser &parser);

int main(int argc, char **argv){
  
  CmdlParser parser(argc,argv);
  string usage;

  setupParser(parser);
  usage = parser.usage();

  try{

    parser.parse();

    PropertyReader prop((parser.checkParameter("filename")).second, parser);
    cout << "filename_0:::::::::::::::::::::: " << (parser.checkParameter("filename")).second << endl;
    
    prop.read();
  
    prop.show();

    cout << endl;

//     cout << "first: " << (parser.checkParameter("filename")).first;
// //     prop.setFilename((parser.checkParameter("filename")).second + string("2"));
// //     prop.setFilename(string("juche"));
//     cout << "filename_1:::::::::::::::::::::: " << (parser.checkParameter("filename")).second << endl;
//     prop.set("NEUES_PROPERTY","THE_WERT");
//     cout << "filename_2:::::::::::::::::::::: " << (parser.checkParameter("filename")).second << endl;
//     prop.write();
//     prop.show();
    cout << endl;
//     PropertyReader prop2(prop);
//     prop2.erase("NEUES_PROPERTY");
//     prop2.show();

    PropertyReader prop2(prop);
    prop2.setFilename((parser.checkParameter("filename")).second + "__second");
    prop2.write();

    PropertyReader prop3(prop);

    prop3.setFilename((parser.checkParameter("filename")).second + "__new");
    cout << "filename_3:::::::::::::::::::::: " << (parser.checkParameter("filename")).second << endl;
    prop3.erase("PRINTER");
    prop3["NEUES_PROPERTY"] = "Neuer_Wert";
    prop3.set("Neue Section","NEUES_PROPERTY","Neuer_Wert");
    prop3.set("JUPPY","JIPJAP");
    prop3.set("POL","true","false");
    prop3.erase("POL","a");
    
    prop3.update();

    cout << endl;

    cout << endl << "--------- show ---------" << endl << endl;

    prop.show();

  } catch (Exception<CmdlParser> &pe){

    pe.show();
    cerr << usage << endl;
  } catch (Exception_T &e){

    e.show();
  }

  return 0;
}

void setupParser(CmdlParser &parser){

  parser.addParameter("filename","filename","property-file",true);
  parser.addParameter("Host","host","the hostname");

}
