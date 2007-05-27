#include "main.hpp"

using namespace std;
using namespace graph;

// a class which derives from Graphable
class Tstgraph : public Graphable<int>{

public:

  typedef map< int, list<int> *, less<int> > Nodemap;

private:
  
  // our definition of nodes
  Nodemap * nodes;

public:

  // Here we derive from the nested classes Graphable::node_iterator and Graphable::neighbour_iterator. We have to implement the pure virtual (abstract)
  // methods which are declared in Graphable::iterator.

  class tg_node_iterator : public node_iterator{

    friend class Tstgraph;

  private:

    int count;

    void setCount(int rval_count){ count = rval_count; }

  public:

    ~tg_node_iterator(){}

    // Implement the virtual methods.
    void operator++(){ ++count; }
    void operator++(int){ count++; }
    const int & operator*(){ return count; }
    bool operator!=(const abstract_iterator & it_rval){ return ( this->count != ((tg_node_iterator &)it_rval).count ); }
    bool operator==(const abstract_iterator & it_rval){ return ( this->count == ((tg_node_iterator &)it_rval).count ); }
    

  };

  class tg_neighbour_iterator : public neighbour_iterator{

    friend class Tstgraph;

  private:

    list<int>::iterator it;

    void setIt(list<int>::iterator it_rval){ it = it_rval; }

  public:

    ~tg_neighbour_iterator(){}

    // Implement the virtual methods.
    void operator++(){ ++it; }
    void operator++(int){ it++; }
    const int & operator*(){ return *it; }
    bool operator!=(const abstract_iterator & it_rval){ return ( this->it != ((tg_neighbour_iterator &)it_rval).it ); }
    bool operator==(const abstract_iterator & it_rval){ return ( this->it == ((tg_neighbour_iterator &)it_rval).it ); }
    

  };

  Tstgraph(){ nodes = new Nodemap(); }

  ~Tstgraph(){

    for ( Nodemap::iterator it = nodes->begin(); it != nodes->end(); it++ )
      delete (*it).second;
    
    delete nodes;
    
  }

  Nodemap * getNodes(){ return nodes; }

  // The following methods are only necessary for our local Graphable-class Tstgraph. 

  void insertNode(int node){ 
    
    (*nodes)[node] = new list<int>();
  
  }

  void insertEdge(int node, int neighbour){

    (*nodes)[node]->push_back(neighbour);

  }

  void removeEdge(int node, int neighbour){

    list<int> * neighbourlist = (*nodes)[node];

    list<int>::iterator it = find(neighbourlist->begin(),neighbourlist->end(),neighbour);

    if ( it != neighbourlist->end() )
      neighbourlist->erase(it);

  }

  // We have to provide the Graphable-class Tstgraph with the implementation of the following pure virtual methods.

  node_iterator * beginNodesPtr(){

    tg_node_iterator * it = new tg_node_iterator();
    
    it->setCount(0);

    return it;

  }
  node_iterator * endNodesPtr(){

    tg_node_iterator * it = new tg_node_iterator();
   
    it->setCount(maxNodes());

    return it;

  }

  neighbour_iterator * beginNeighboursPtr(const int & node){

    tg_neighbour_iterator * it = new tg_neighbour_iterator();
    
    it->setIt((*nodes->find(node)).second->begin());

    return it;

  }
  neighbour_iterator * endNeighboursPtr(const int & node){

    tg_neighbour_iterator * it = new tg_neighbour_iterator();
   
    it->setIt((*nodes->find(node)).second->end());

    return it;

  }

  unsigned int maxNodes(){ return nodes->size(); }

  int startNode(){ return (*nodes->begin()).first; }

};
  

int main(int argc, char **argv){

  Tstgraph tg;

  // just build up a graph-structure

  tg.insertNode(1);
  tg.insertNode(0);
  tg.insertNode(2);
  tg.insertNode(3);
  tg.insertNode(4);
  tg.insertNode(5);
  tg.insertNode(6);
  tg.insertNode(7);
  tg.insertNode(8);
  tg.insertNode(9);
  tg.insertNode(10);
  tg.insertNode(11);
  

  tg.insertEdge(0,1);
  tg.insertEdge(0,2);

  tg.insertEdge(2,0);
  tg.insertEdge(1,3);
  tg.insertEdge(3,4);
  tg.insertEdge(3,1);
  tg.insertEdge(4,1);
  tg.insertEdge(5,3);
  tg.insertEdge(2,6);
  tg.insertEdge(6,7);
  tg.insertEdge(7,8);
  tg.insertEdge(8,5);
  tg.insertEdge(8,9);
  tg.insertEdge(10,9);
  tg.insertEdge(9,11);
  tg.insertEdge(11,10);
  tg.insertEdge(3,7);

  
  // print the graph-structure for demonstration
  for ( Tstgraph::Nodemap::iterator nm_it = tg.getNodes()->begin(); nm_it != tg.getNodes()->end(); nm_it++ ){

    cout << "neighbours from node " << (*nm_it).first << ": ";

    for ( Tstgraph::iterator ni = tg.beginNeighbours((*nm_it).first); ni != tg.endNeighbours((*nm_it).first); ni++ )
      cout << *ni << " ";

    cout << endl;

  }

  SCCGraph<int> *scc;

  // find all strongly connected components
  scc = tg.find_scc(true);

  for ( SCCGraph<int>::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){
      
    cout << "component " << (*scc_it)->getId() << ": {";
    for ( SCCGraphComponent<int>::nodeiterator c_it = (*scc_it)->beginNodes(); c_it != (*scc_it)->endNodes(); c_it++ ){

      if ( c_it != (*scc_it)->beginNodes() )
	cout << ",";
      cout << *c_it;

    }
    cout << "}" << endl;
  }

  cout << "componentgraph:" << endl;

  for ( SCCGraph<int>::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){

    cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
    for ( SCCGraph<int>::iterator nb_it = scc->beginNeighbours(*scc_it); nb_it != scc->endNeighbours(*scc_it); nb_it++ )
      cout << (*nb_it)->getId() << " ";
    cout << endl;

  }

  delete scc;

  Tstgraph cplg;
  int max = 1000;

  cout << "building complete graph ... " << flush;
  
  for ( int i = 0; i < max; i++ )
    cplg.insertNode(i);

  for ( int i = 0; i < max; i++ )
    for ( int j = 0; j < max; j++ )
      if ( j != i )
	cplg.insertEdge(i,j);

  for ( int i = max/2-1; i < max ; i++ )
    if ( i != max/2 )
      cplg.removeEdge(max/2-1,i);

  for ( int i = 0 ; i < max/2-1; i++ )
    for ( int j = max/2; j < max; j++ )
      cplg.removeEdge(i,j);

  for ( int i = max/2; i < max; i++ )
    for ( int j=0; j < max/2; j++ )
      cplg.removeEdge(i,j);

  cout << "finished" << endl << flush;

  cout << "constructing component-graph ... " << flush;

  scc = cplg.find_scc(true);

  cout << "finished" << endl << flush;

  for ( SCCGraph<int>::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){
      
    cout << "component " << (*scc_it)->getId() << ": {";
    for ( SCCGraphComponent<int>::nodeiterator c_it = (*scc_it)->beginNodes(); c_it != (*scc_it)->endNodes(); c_it++ ){

      if ( c_it != (*scc_it)->beginNodes() )
	cout << ",";
      cout << *c_it;

    }
    cout << "}" << endl;
  }

  cout << "componentgraph:" << endl;

  for ( SCCGraph<int>::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){

    cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
    for ( SCCGraph<int>::iterator nb_it = scc->beginNeighbours(*scc_it); nb_it != scc->endNeighbours(*scc_it); nb_it++ )
      cout << (*nb_it)->getId() << " ";
    cout << endl;
  }

  // scc is a graph itself so we can call find_scc() as well on this graph (won't make any difference because a dag just stays a dag; only for
  // demonstrative purpose).
  SCCGraph< SCCGraphComponent<int> *> * sccscc = scc->find_scc(true);

  cout << endl << "componentgraph's componentgraph:" << endl;

  for ( SCCGraph< SCCGraphComponent<int> *>::iterator scc_it = sccscc->beginNodes(); scc_it != sccscc->endNodes(); scc_it++ ){

    cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
    for ( SCCGraph< SCCGraphComponent<int> *>::iterator nb_it = sccscc->beginNeighbours(*scc_it); nb_it != sccscc->endNeighbours(*scc_it); nb_it++ )
      cout << (*nb_it)->getId() << " ";
    cout << endl;
  }

  delete sccscc;

  delete scc;

  return 0;
}
