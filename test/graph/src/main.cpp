#include "main.hpp"

using namespace std;
using namespace exc;
using namespace graph;
using namespace ds;

// in the following we will define two different graph-representations

// the nodes will be stored in a map with lists holding the neighbours
class Tstgraph : public Graphable<int>{

public:

  typedef map< int, list<int> *> Nodemap;

private:
  
  // our definition of nodes
  Nodemap *neighbours;

public:

  // Here we derive from the nested classes graph::abstract_node_iterator and put it inside our Graphable.
  // We have to implement the pure virtual (abstract)
  // methods which are declared in GraphableBase::iterator.

  class tg_abstract_node_iterator : public abstract_node_iterator<int>{

    friend class Tstgraph;

  private:

    Nodemap::iterator it;
    
  public:

    tg_abstract_node_iterator(const Nodemap::iterator it) : it(it){}
    tg_abstract_node_iterator(const tg_abstract_node_iterator & it) : it(it.it){}

    ~tg_abstract_node_iterator(){}

    // Implement the virtual methods.
    void operator++(int) throw(exc::ExceptionBase){ ++it; }
    int operator*() throw(exc::ExceptionBase){ return (*it).first; }
    bool operator==(const abstract_node_iterator<int> * it_rval) throw(exc::ExceptionBase){
      return ( (*this->it).first == (*(static_cast<const tg_abstract_node_iterator *>(it_rval))->it).first );
    }
    

  };

  class tg_neighbour_iterator : public abstract_node_iterator<int>{
  
    friend class Tstgraph;
  
  private:

    list<int>::iterator it;

    void setIt(const list<int>::iterator it_rval){ it = it_rval; }

  public:

    ~tg_neighbour_iterator(){}

    // Implement the virtual methods.
    void operator++(int) throw(exc::ExceptionBase){ ++it; }
    int operator*() throw(exc::ExceptionBase){ return *it; }
    bool operator==(const abstract_node_iterator<int> * it_rval) throw(exc::ExceptionBase){
      return ( this->it == (static_cast<const tg_neighbour_iterator *>(it_rval))->it );
    }
    

  };

  Tstgraph(){ 

    neighbours = new Nodemap();

  }

  ~Tstgraph(){

    for ( Nodemap::iterator it = neighbours->begin(); it != neighbours->end(); it++ )
      delete (*it).second;
    
    delete neighbours;
    
  }

  Nodemap * getNodes(){ return neighbours; }

  // The following methods are only necessary for our local GraphableBase-class Tstgraph. 

  void insertNode(int node){ 
    
    Nodemap::iterator old = neighbours->find(node);

    if ( old != neighbours->end() )
      delete (*old).second;
    
    (*neighbours)[node] = new list<int>();
  
  }

  void insertEdge(int node, int neighbour){

    (*neighbours)[node]->push_back(neighbour);

  }

  void removeEdge(int node, int neighbour){
    
    Nodemap::iterator nit = neighbours->find(node);
    
    if ( nit != neighbours->end() ){
      
      list<int> *neighbourlist = (*nit).second;
      
      list<int>::iterator it = find(neighbourlist->begin(),neighbourlist->end(),neighbour);
      
      if ( it != neighbourlist->end() )
	neighbourlist->erase(it);
      
    }
    
  }
  
  // We have to provide the GraphableBase-class Tstgraph with the implementation of the following pure virtual methods.

  abstract_node_iterator<int> * beginNodesPtr(){

    return new tg_abstract_node_iterator(neighbours->begin());

  }
  abstract_node_iterator<int> * endNodesPtr(){

    return new tg_abstract_node_iterator(neighbours->end());

  }

  abstract_node_iterator<int> * beginNeighboursPtr(const int node){

    tg_neighbour_iterator * it = new tg_neighbour_iterator();
    
    it->setIt((*neighbours->find(node)).second->begin());
    
    return it;

  }
  abstract_node_iterator<int> * endNeighboursPtr(const int node){

    tg_neighbour_iterator * it = new tg_neighbour_iterator();
   
    it->setIt((*neighbours->find(node)).second->end());

    return it;

  }

  size_t maxNodes(){ return neighbours->size(); }

};

// a graph represented by a matrix (note that the nodes must be accessible objects)
class MatrixGraph{

private:
  
  unsigned int number_nodes;

  unsigned int *nodes;

  BitMatrix *adjacencymatrix;

public:

  MatrixGraph(unsigned int number_nodes) : number_nodes(number_nodes){

    nodes = new unsigned int[number_nodes];
    
    for ( unsigned int i = 0; i< number_nodes; i++ )
      nodes[i] = i+1;
    
    adjacencymatrix = new BitMatrix(number_nodes,number_nodes);
    
  }
  
  ~MatrixGraph(){
    
    delete adjacencymatrix;
    delete nodes;
    
  }

  unsigned int maxNodes(){ return number_nodes; }

  unsigned int *getNodes(){ return nodes; }

  BitMatrix * getAdjacencyMatrix(){ return adjacencymatrix; }

  void insertEdge(int node, int neighbour){

    adjacencymatrix->setBit(node-1,neighbour-1);

  }

  void removeEdge(int node, int neighbour){

    adjacencymatrix->clearBit(node-1,neighbour-1);

  }

  void showAdjacencymatrix(){ adjacencymatrix->show(); }

};
  
class MatrixNodeIterator : public abstract_node_iterator<unsigned int *>{
  
private:
  
  unsigned int *nodes;
  
  unsigned int maxpos;
  
  unsigned int pos;

public:

  MatrixNodeIterator(unsigned int *nodes, unsigned int maxpos, unsigned int pos) : nodes(nodes), maxpos(maxpos), pos(pos){}

  MatrixNodeIterator(unsigned int *nodes, unsigned int maxpos) : nodes(nodes), maxpos(maxpos){
      
    if ( 1 > maxpos )
      this->pos = 0;
    else
      this->pos = 1;
      
  }
    
  void operator++(int) throw(exc::ExceptionBase){ 
    pos > 0 && pos < maxpos ? pos++ : pos = 0;
  }
  
  unsigned int *operator*() throw(exc::ExceptionBase){
    return &nodes[pos-1];
  }
    
  bool operator==(const abstract_node_iterator<unsigned int *> *it_rval) throw(exc::ExceptionBase){
    return ( this->pos == (static_cast<const MatrixNodeIterator *>(it_rval))->pos );
  }

};

class MatrixNeighbourIterator : public abstract_node_iterator<unsigned int *>{

private:

  unsigned int *nodes;

  BitMatrix *adjacencymatrix;

  unsigned int maxnodes;

  unsigned int node;
    
  unsigned int neighbour;

public:

  MatrixNeighbourIterator(unsigned int *nodes, BitMatrix *adjacencymatrix, unsigned int node, unsigned int maxnodes)
    : nodes(nodes),
      adjacencymatrix(adjacencymatrix),
      maxnodes(maxnodes){
    
    if ( node > maxnodes )
      this->node = 0;
    else
      this->node = node;

    neighbour = 0;

  }

  void nextNeighbour(){

    if ( node == 0 )
      return;

    neighbour++;

    while ( neighbour <= maxnodes ) {

      if ( adjacencymatrix->getBit(node-1,neighbour-1) == 1 )
	return;

      neighbour++;

    }

    neighbour = 0;
      
  }

  void operator++(int) throw(exc::ExceptionBase){ 

    if ( node == 0 || neighbour == 0 )
      return;

    nextNeighbour();

  }

  unsigned int * operator*() throw(exc::ExceptionBase){
    return &nodes[neighbour-1];
  }

  bool operator==(const abstract_node_iterator<unsigned int *> * it_rval) throw(exc::ExceptionBase){
    return ( this->neighbour == (static_cast<const MatrixNeighbourIterator *>(it_rval))->neighbour );
  }
  
};

class GraphableMatrix : public Graphable<unsigned int *>{
  
protected:
  
  MatrixGraph & graph;
  
public:
  
  GraphableMatrix(MatrixGraph &graph) : graph(graph){}
  
  abstract_node_iterator<unsigned int *> * beginNodesPtr(){ return new MatrixNodeIterator(graph.getNodes(),graph.maxNodes()); }
  abstract_node_iterator<unsigned int *> * endNodesPtr(){ return new MatrixNodeIterator(graph.getNodes(),graph.maxNodes(),0); }
  
  abstract_node_iterator<unsigned int *> * beginNeighboursPtr(unsigned int *node){
    
    MatrixNeighbourIterator *neighbours = new MatrixNeighbourIterator(graph.getNodes(),graph.getAdjacencyMatrix(),
								      *(unsigned int *)node,graph.maxNodes());
    
    neighbours->nextNeighbour();
    
    return neighbours;
    
  }
  
  abstract_node_iterator<unsigned int *> * endNeighboursPtr(unsigned int *node){
    return new MatrixNeighbourIterator(graph.getNodes(),graph.getAdjacencyMatrix(),*(unsigned int*)node,graph.maxNodes());
  }
  
  size_t maxNodes(){ return graph.maxNodes(); }
  
  
};

int main(int argc, char **argv){
  
  int max = 100;

  try{

    {
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

	for ( node_iterator<int> ni = tg.beginNeighbours((*nm_it).first); ni != tg.endNeighbours((*nm_it).first); ni++ )
	  cout << *ni << " ";

	cout << endl;

      }

      SCCGraphConstructor<int> graphconstructor(&tg);

      // find all strongly connected components
      graphconstructor.find_scc();

      SCCStructure<int> & scc = graphconstructor.getSCCStructure();
	
      for ( node_iterator<SCCGraphComponent<int> *> scc_it = scc.beginNodes(); scc_it != scc.endNodes(); scc_it++ ){
	  
	cout << "component " << (*scc_it)->getId() << ": {";
	for ( SCCGraphComponent<int>::nodeiterator c_it = (*scc_it)->beginNodes(); c_it != (*scc_it)->endNodes(); c_it++ ){

	  if ( c_it != (*scc_it)->beginNodes() )
	    cout << ",";
	  cout << *c_it;

	}
	cout << "}" << endl;
      }

      cout << "componentgraph:" << endl;

      for ( node_iterator<SCCGraphComponent<int> *> scc_it = scc.beginNodes(); scc_it != scc.endNodes(); scc_it++ ){

	cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
	for ( node_iterator<SCCGraphComponent<int> *> nb_it = scc.beginNeighbours(*scc_it); nb_it != scc.endNeighbours(*scc_it); nb_it++ )
	  cout << (*nb_it)->getId() << " ";
	cout << endl;

      }

    }

    {
      Tstgraph cplg;

      cout << endl << "building a bigger graph ... " << flush;
  
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

      SCCGraphConstructor<int> gcons(&cplg);

      gcons.find_scc();

      SCCStructure<int> & scc = gcons.getSCCStructure();

      cout << "finished" << endl << flush;

      for ( node_iterator<SCCGraphComponent<int> *> scc_it = scc.beginNodes(); scc_it != scc.endNodes(); scc_it++ ){
      
	cout << "component " << (*scc_it)->getId() << ": {";
	for ( SCCGraphComponent<int>::nodeiterator c_it = (*scc_it)->beginNodes(); c_it != (*scc_it)->endNodes(); c_it++ ){

	  if ( c_it != (*scc_it)->beginNodes() )
	    cout << ",";
	  cout << *c_it;

	}
	cout << "}" << endl;
      }

      cout << "componentgraph:" << endl;

      for ( node_iterator<SCCGraphComponent<int> *> scc_it = scc.beginNodes(); scc_it != scc.endNodes(); scc_it++ ){

	cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
	for ( node_iterator<SCCGraphComponent<int> *> nb_it = scc.beginNeighbours(*scc_it); nb_it != scc.endNeighbours(*scc_it); nb_it++ )
	  cout << (*nb_it)->getId() << " ";
	cout << endl;
      }

      // scc is a graph itself so we can call find_scc() as well on this graph (won't make any difference because a dag just stays a dag; only for
      // demonstrative purpose).

      SCCGraphConstructor<SCCGraphComponent<int> *> gc2(&scc);

      gc2.find_scc();

      SCCStructure<SCCGraphComponent<int> *> & sccscc = gc2.getSCCStructure();

      cout << endl << "componentgraph's componentgraph:" << endl;

      for ( node_iterator<SCCGraphComponent<SCCGraphComponent<int> *> *> scc_it = sccscc.beginNodes(); scc_it != sccscc.endNodes(); scc_it++ ){

	cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
	for ( node_iterator<SCCGraphComponent<SCCGraphComponent<int> *> *> nb_it = sccscc.beginNeighbours(*scc_it);
	      nb_it != sccscc.endNeighbours(*scc_it); nb_it++ )
	  cout << (*nb_it)->getId() << " ";
	cout << endl;
      }
    }
  
    // at last a Graph represented by a Matrix
    cout << endl << "building Matrixgraph ... ";

    MatrixGraph mgraph(max);
  
    for ( int i = 1; i <= max; i++ )
      for ( int j = 1; j <= max; j++ )
	if ( j != i )
	  mgraph.insertEdge(i,j);
  
    for ( int i = max/2-1; i <= max ; i++ )
      if ( i != max/2 )
	mgraph.removeEdge(max/2-1,i);
  
    for ( int i = 1 ; i <= max/2-1; i++ )
      for ( int j = max/2; j <= max; j++ )
	mgraph.removeEdge(i,j);
  
    for ( int i = max/2; i <= max; i++ )
      for ( int j=1; j <= max/2; j++ )
	mgraph.removeEdge(i,j);

    mgraph.insertEdge(1,50);

    cout << "finished" << endl;

    cout << "adjacencymatrix:" << endl;

    mgraph.showAdjacencymatrix();

    cout << "constructing component-graph ... " << flush;

    GraphableMatrix graph(mgraph);
    SCCGraphConstructor<unsigned int *> sccGraphConstructor(&graph);

    sccGraphConstructor.find_scc();

    cout << "finished" << endl << flush;

    SCCStructure<unsigned int *> & scc = sccGraphConstructor.getSCCStructure();

    // we can consider scc as Graphable ...
    for ( node_iterator<SCCGraphComponent<unsigned int *> *> scc_it = scc.beginNodes(); scc_it != scc.endNodes(); scc_it++ ){
      
      SCCGraphComponent<unsigned int *> * component = *scc_it;

      cout << "component " << component->getId() << ": {";
      for ( SCCGraphComponent<unsigned int *>::Nodeset::iterator c_it = component->beginNodes(); c_it != component->endNodes(); c_it++ ){

	if ( c_it != component->beginNodes() )
	  cout << ",";
	cout << *(unsigned int *)(*c_it);

      }
      cout << "}" << endl;
    }

    cout << "componentgraph:" << endl;

    // ... or we can consider it as SCCStructure (as derived from Graphable<SomeClass>); note the difference in the return-values of the iterators.
    // This version is type-safe.
    for ( node_iterator<SCCGraphComponent<unsigned int *> *> scc_it = scc.beginNodes(); scc_it != scc.endNodes(); scc_it++ ){
      
      SCCGraphComponent<unsigned int *> * component = *scc_it;

      cout << "neighbours of component " << component->getId() << ": ";
    
      for ( node_iterator<SCCGraphComponent<unsigned int *> *> nb_it = scc.beginNeighbours(component); nb_it != scc.endNeighbours(component); nb_it++ )
	cout << (*nb_it)->getId() << " ";
      cout << endl;
    }

  } catch (ExceptionBase &e){
    e.show();
  }

  return 0;
}
