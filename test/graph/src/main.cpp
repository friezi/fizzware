#include "main.hpp"

using namespace std;
using namespace exc;
using namespace graph;
using namespace ds;

// in the following we will define two different graph-representations

// // the nodes will be stored in a map with lists holding the neighbours
// class Tstgraph : public Graphable<int>{

// public:

//   typedef map< int, list<int> *, less<int> > Nodemap;

// private:

//   int * tempnode;
  
//   // our definition of nodes
//   Nodemap * neighbours;

// public:

//   // Here we derive from the nested classes GraphableBase::node_iterator and GraphableBase::neighbour_iterator. We have to implement the pure virtual (abstract)
//   // methods which are declared in GraphableBase::iterator.

//   class tg_node_iterator : public node_iterator{

//     friend class Tstgraph;

//   private:

//     int count;

//     int **tempnode;

//     void setCount(int rval_count){ count = rval_count; }

//   public:

//     tg_node_iterator(int **tempnode) : tempnode(tempnode){}

//     ~tg_node_iterator(){}

//     // Implement the virtual methods.
//     void operator++(int){ count++; }
//     TNode operator*(){

//       if ( *tempnode )
// 	delete *tempnode;

//       *tempnode = new int(count);
//       return *tempnode;

// }
//     bool operator==(const abstract_iterator & it_rval){ return ( this->count == ((tg_node_iterator &)it_rval).count ); }
    

//   };

//   class tg_neighbour_iterator : public neighbour_iterator{

//     friend class Tstgraph;

//   private:

//     list<int>::iterator it;

//     void setIt(list<int>::iterator it_rval){ it = it_rval; }

//   public:

//     ~tg_neighbour_iterator(){}

//     // Implement the virtual methods.
//     void operator++(int){ it++; }
//     TNode operator*(){ return &(*it); }
//     bool operator==(const abstract_iterator & it_rval){ return ( this->it == ((tg_neighbour_iterator &)it_rval).it ); }
    

//   };

//   Tstgraph() : tempnode(0){ 

//     neighbours = new Nodemap();

//   }

//   ~Tstgraph(){

//     for ( Nodemap::iterator it = neighbours->begin(); it != neighbours->end(); it++ )
//       delete (*it).second;
    
//     delete neighbours;

//     if ( tempnode )
//       delete tempnode;
    
//   }

//   Nodemap * getNodes(){ return neighbours; }

//   // The following methods are only necessary for our local GraphableBase-class Tstgraph. 

//   void insertNode(int node){ 
    
//     (*neighbours)[node] = new list<int>();
  
//   }

//   void insertEdge(int node, int neighbour){

//     (*neighbours)[node]->push_back(neighbour);

//   }

//   void removeEdge(int node, int neighbour){

//     list<int> * neighbourlist = (*neighbours)[node];

//     list<int>::iterator it = find(neighbourlist->begin(),neighbourlist->end(),neighbour);

//     if ( it != neighbourlist->end() )
//       neighbourlist->erase(it);

//   }

//   // We have to provide the GraphableBase-class Tstgraph with the implementation of the following pure virtual methods.

//   node_iterator * beginNodesPtr(){

//     tg_node_iterator * it = new tg_node_iterator(&tempnode);
    
//     it->setCount(0);

//     return it;

//   }
//   node_iterator * endNodesPtr(){

//     tg_node_iterator * it = new tg_node_iterator(&tempnode);
   
//     it->setCount(maxNodes());

//     return it;

//   }

//   neighbour_iterator * beginNeighboursPtr(const TNode node){

//     tg_neighbour_iterator * it = new tg_neighbour_iterator();
    
//     it->setIt((*neighbours->find(*((int *)node))).second->begin());

//     return it;

//   }
//   neighbour_iterator * endNeighboursPtr(const TNode node){

//     tg_neighbour_iterator * it = new tg_neighbour_iterator();
   
//     it->setIt((*neighbours->find(*((int *)node))).second->end());

//     return it;

//   }

//   unsigned int maxNodes(){ return neighbours->size(); }

// };

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
  
class MatrixNodeIterator : public node_iterator<unsigned int *>{
  
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
    
  void operator++(int){ 
     
    pos > 0 && pos < maxpos ? pos++ : pos = 0;
      
  }
  
  unsigned int *operator*(){ return &nodes[pos-1]; }
    
  bool operator==(const node_iterator<unsigned int *> *it_rval){ return ( this->pos == (static_cast<const MatrixNodeIterator *>(it_rval))->pos ); }

};

class MatrixNeighbourIterator : public node_iterator<unsigned int *>{

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

  void operator++(int){ 

    if ( node == 0 || neighbour == 0 )
      return;

    nextNeighbour();

  }

  unsigned int * operator*(){ return &nodes[neighbour-1]; }

  bool operator==(const node_iterator<unsigned int *> * it_rval){
    return ( this->neighbour == (static_cast<const MatrixNeighbourIterator *>(it_rval))->neighbour );
  }
  
};

class GraphableMatrix : public Graphable<unsigned int *>{
  
protected:
  
  MatrixGraph & graph;
  
public:
  
  GraphableMatrix(MatrixGraph &graph) : graph(graph){}
  
  node_iterator<unsigned int *> * beginNodesPtr(){ return new MatrixNodeIterator(graph.getNodes(),graph.maxNodes()); }
  node_iterator<unsigned int *> * endNodesPtr(){ return new MatrixNodeIterator(graph.getNodes(),graph.maxNodes(),0); }
  
  node_iterator<unsigned int *> * beginNeighboursPtr(unsigned int * node){
    
    MatrixNeighbourIterator * neighbours = new MatrixNeighbourIterator(graph.getNodes(),graph.getAdjacencyMatrix(),*(unsigned int *)node,graph.maxNodes());
    
    neighbours->nextNeighbour();
    
    return static_cast<node_iterator<unsigned int *> *>(neighbours);
    
  }
  
  node_iterator<unsigned int *> * endNeighboursPtr(unsigned int * node){
    return new MatrixNeighbourIterator(graph.getNodes(),graph.getAdjacencyMatrix(),*(unsigned int*)node,graph.maxNodes());
  }
  
  size_t maxNodes(){ return graph.maxNodes(); }
  
  
};

int main(int argc, char **argv){
  
  int max = 100;
    //    SCCGraph *scc;

//     Tstgraph tg;

//     // just build up a graph-structure

//     tg.insertNode(1);
//     tg.insertNode(0);
//     tg.insertNode(2);
//     tg.insertNode(3);
//     tg.insertNode(4);
//     tg.insertNode(5);
//     tg.insertNode(6);
//     tg.insertNode(7);
//     tg.insertNode(8);
//     tg.insertNode(9);
//     tg.insertNode(10);
//     tg.insertNode(11);
  
//     tg.insertEdge(0,1);
//     tg.insertEdge(0,2);

//     tg.insertEdge(2,0);
//     tg.insertEdge(1,3);
//     tg.insertEdge(3,4);
//     tg.insertEdge(3,1);
//     tg.insertEdge(4,1);
//     tg.insertEdge(5,3);
//     tg.insertEdge(2,6);
//     tg.insertEdge(6,7);
//     tg.insertEdge(7,8);
//     tg.insertEdge(8,5);
//     tg.insertEdge(8,9);
//     tg.insertEdge(10,9);
//     tg.insertEdge(9,11);
//     tg.insertEdge(11,10);
//     tg.insertEdge(3,7);

  
//     // print the graph-structure for demonstration
//     for ( Tstgraph::Nodemap::iterator nm_it = tg.getNodes()->begin(); nm_it != tg.getNodes()->end(); nm_it++ ){

//       cout << "neighbours from node " << (*nm_it).first << ": ";

//       for ( Tstgraph::iterator ni = tg.beginNeighbours(&((void *)((*nm_it).first))); ni != tg.endNeighbours(&((void *)((*nm_it).first))); ni++ )
//         cout << *ni << " ";

//       cout << endl;

//     }


//     // find all strongly connected components
//     scc = tg.find_scc(true);

//     for ( SCCGraph::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){
      
//       cout << "component " << (*scc_it)->getId() << ": {";
//       for ( SCCGraphComponent::nodeiterator c_it = (*scc_it)->beginNodes(); c_it != (*scc_it)->endNodes(); c_it++ ){

//         if ( c_it != (*scc_it)->beginNodes() )
//   	cout << ",";
//         cout << *c_it;

//       }
//       cout << "}" << endl;
//     }

//     cout << "componentgraph:" << endl;

//     for ( SCCGraph::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){

//       cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
//       for ( SCCGraph::iterator nb_it = scc->beginNeighbours(*scc_it); nb_it != scc->endNeighbours(*scc_it); nb_it++ )
//         cout << (*nb_it)->getId() << " ";
//       cout << endl;

//     }

//     delete scc;

//     Tstgraph cplg;

//     cout << endl << "building a bigger graph ... " << flush;
  
//     for ( int i = 0; i < max; i++ )
//       cplg.insertNode(i);

//     for ( int i = 0; i < max; i++ )
//       for ( int j = 0; j < max; j++ )
//         if ( j != i )
//   	cplg.insertEdge(i,j);

//     for ( int i = max/2-1; i < max ; i++ )
//       if ( i != max/2 )
//         cplg.removeEdge(max/2-1,i);

//     for ( int i = 0 ; i < max/2-1; i++ )
//       for ( int j = max/2; j < max; j++ )
//         cplg.removeEdge(i,j);

//     for ( int i = max/2; i < max; i++ )
//       for ( int j=0; j < max/2; j++ )
//         cplg.removeEdge(i,j);

//     cout << "finished" << endl << flush;

//     cout << "constructing component-graph ... " << flush;

//     scc = cplg.find_scc(true);

//     cout << "finished" << endl << flush;

//     for ( SCCGraph::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){
      
//       cout << "component " << (*scc_it)->getId() << ": {";
//       for ( SCCGraphComponent::nodeiterator c_it = (*scc_it)->beginNodes(); c_it != (*scc_it)->endNodes(); c_it++ ){

//         if ( c_it != (*scc_it)->beginNodes() )
//   	cout << ",";
//         cout << *c_it;

//       }
//       cout << "}" << endl;
//     }

//     cout << "componentgraph:" << endl;

//     for ( SCCGraph::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){

//       cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
//       for ( SCCGraph::iterator nb_it = scc->beginNeighbours(&(*scc_it)); nb_it != scc->endNeighbours(&(*scc_it)); nb_it++ )
//         cout << (*nb_it)->getId() << " ";
//       cout << endl;
//     }

//     // scc is a graph itself so we can call find_scc() as well on this graph (won't make any difference because a dag just stays a dag; only for
//     // demonstrative purpose).
//     SCCGraph * sccscc = scc->find_scc(true);

//     cout << endl << "componentgraph's componentgraph:" << endl;

//     for ( SCCGraph::iterator scc_it = sccscc->beginNodes(); scc_it != sccscc->endNodes(); scc_it++ ){

//       cout << "neighbours of component " << (*scc_it)->getId() << ": ";
    
//       for ( SCCGraph::iterator nb_it = sccscc->beginNeighbours(*scc_it); nb_it != sccscc->endNeighbours(*scc_it); nb_it++ )
//         cout << (*nb_it)->getId() << " ";
//       cout << endl;
//     }

//     delete sccscc;

//     delete scc;

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

  cout << "finished" << endl;

  cout << "adjacencymatrix:" << endl;

  mgraph.showAdjacencymatrix();

  cout << "constructing component-graph ... " << flush;

  GraphableMatrix graph(mgraph);
  SCCGraphConstructor<unsigned int *> sccGraphConstructor(&graph);

  sccGraphConstructor.find_scc();

  cout << "finished" << endl << flush;

  for ( list<SCCGraphComponent<unsigned int *> *>::iterator it = sccGraphConstructor.getSCCSet().beginComponents();
	it != sccGraphConstructor.getSCCSet().endComponents(); it++ ){
    cout << (*it)->toString() << endl;
  }

//   // we can consider scc as GraphableBase ...
//   for ( GraphableBase::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){
      
//     // ...then we have to cast the administered nodes explicitly, ...
//     SCCGraphComponent * component = (SCCGraphComponent *)(*scc_it);
//     // ... (but it makes the generic handling of different GraphableBase-objects flexible resp. possible) ...

//     cout << "component " << component->getId() << ": {";
//     for ( SCCGraphComponent::nodeiterator c_it = component->beginNodes(); c_it != component->endNodes(); c_it++ ){

//       if ( c_it != component->beginNodes() )
// 	cout << ",";
//       cout << *(unsigned int *)(*c_it);

//     }
//     cout << "}" << endl;
//   }

//   cout << "componentgraph:" << endl;

//   // ... or we can consider it as SCCGraph (as derived from Graphable<SomeClass>); note the difference in the return-values of the iterators.
//   // This version is type-safe.
//   for ( SCCGraph::iterator scc_it = scc->beginNodes(); scc_it != scc->endNodes(); scc_it++ ){
      
//     SCCGraphComponent * component = *scc_it;

//     cout << "neighbours of component " << component->getId() << ": ";
    
//     for ( SCCGraph::iterator nb_it = scc->beginNeighbours(component); nb_it != scc->endNeighbours(component); nb_it++ )
//       cout << (*nb_it)->getId() << " ";
//     cout << endl;
//   }
  
//   delete scc;

  return 0;
}
