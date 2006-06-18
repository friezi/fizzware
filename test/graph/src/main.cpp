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
    int & operator*(){ return count; }
    bool operator!=(const iterator & it_rval){ return ( this->count != ((tg_node_iterator &)it_rval).count ); }
    bool operator==(const iterator & it_rval){ return ( this->count == ((tg_node_iterator &)it_rval).count ); }
    

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
    int & operator*(){ return *it; }
    bool operator!=(const iterator & it_rval){ return ( this->it != ((tg_neighbour_iterator &)it_rval).it ); }
    bool operator==(const iterator & it_rval){ return ( this->it == ((tg_neighbour_iterator &)it_rval).it ); }
    

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

  void insertNeighbour(int node, int neighbour){

    (*nodes)[node]->push_back(neighbour);

  }

  // We have to provide the Graphable-class Tstgraph with the implementation of the following pure virtual methods.

  node_iterator * nodesBegin(){

    tg_node_iterator * it = new tg_node_iterator();
    
    it->setCount(0);

    return it;

  }
  node_iterator * nodesEnd(){

    tg_node_iterator * it = new tg_node_iterator();
   
    it->setCount(maxNodes());

    return it;

  }

  neighbour_iterator * neighboursBegin(const int & node){

    tg_neighbour_iterator * it = new tg_neighbour_iterator();
    
    it->setIt((*nodes->find(node)).second->begin());

    return it;

  }
  neighbour_iterator * neighboursEnd(const int & node){

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
  

  tg.insertNeighbour(0,1);
  tg.insertNeighbour(0,2);

  tg.insertNeighbour(2,0);
  tg.insertNeighbour(1,3);
  tg.insertNeighbour(3,4);
  tg.insertNeighbour(3,1);
  tg.insertNeighbour(4,1);
  tg.insertNeighbour(5,3);
  tg.insertNeighbour(2,6);
  tg.insertNeighbour(6,7);
  tg.insertNeighbour(7,8);
  tg.insertNeighbour(8,5);
  tg.insertNeighbour(8,9);
  tg.insertNeighbour(10,9);
  tg.insertNeighbour(9,11);
  tg.insertNeighbour(11,10);

  
  // print the graph-structure for demonstration
  for ( Tstgraph::Nodemap::iterator nm_it = tg.getNodes()->begin(); nm_it != tg.getNodes()->end(); nm_it++ ){

    cout << "neighbours from node " << (*nm_it).first << ": ";

    {

      // because we put the temp_iterators in a separat block, they and their contents will be destroyed automatically
      // at end of the block.

      Graphable<int>::temp_iterator ni_begin(tg.neighboursBegin((*nm_it).first));
      Graphable<int>::temp_iterator ni_end(tg.neighboursEnd((*nm_it).first));

      for ( Tstgraph::tg_neighbour_iterator *ni = (Tstgraph::tg_neighbour_iterator *)ni_begin.get(); *ni != *ni_end.get(); (*ni)++ )
	cout << **ni << " ";

    }

    cout << endl;

  }

  list< list<int> > *scc;

  // find all strongly connected components
  scc = tg.find_scc();

  int count = 1;
  for ( list< list<int> >::iterator scc_it = scc->begin(); scc_it != scc->end(); scc_it++, count++ ){
    cout << "component " << count << ": ";
    for ( list<int>::iterator c_it = (*scc_it).begin(); c_it != (*scc_it).end(); c_it++ ){
      cout << *c_it << " ";
    }
    cout << endl;
  }

  delete scc;
  
  return 0;
}
