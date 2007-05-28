#include <graph.hpp>

using namespace graph;

// basically Tarjan's algorithm
unsigned int graph::Graphable::scc_visit(const TNode node, SCCGraph & scc, std::stack<TNode> & nodestack, Valuemap & values, Componentmap & nodecomponents, 
	  unsigned int id, bool construct_scc_graph){
  
  unsigned int m = 0, min;

  values[node] = ++id;

  min = id;

  nodestack.push(node);

  for ( iterator nit = beginNeighbours(node); nit != endNeighbours(node); nit++ ){
    
    m = (!values[*nit]) ? scc_visit(*nit,scc,nodestack,values,nodecomponents,id,construct_scc_graph) : values[*nit];

    if ( m < min )
      min = m;
    
  }

  if ( min == values[node] ){

    SCCGraphComponent * component = scc.newComponent();

    TNode topnode = (TNode) 0;

    do{

      topnode = nodestack.top();
      component->insertComponentnode(topnode);
      nodestack.pop();

      values[topnode] = maxNodes() + 1;

      if ( construct_scc_graph == true ){
	// construct scc-graph

	nodecomponents[topnode] = component;

	for ( iterator nit = beginNeighbours(topnode); nit != endNeighbours(topnode); nit++ ){

	  SCCGraphComponent * tn_component;

	  if ( ( tn_component = nodecomponents[*nit] ) != 0 )
	    if ( tn_component != component)
	      component->insertNeighbour(tn_component);

	}

      }

    } while ( topnode != node );

  }
  
  return min;
    
}

graph::SCCGraph * graph::Graphable::find_scc(bool construct_scc_graph = false){

  graph::SCCGraph *scc = new graph::SCCGraph();
  std::stack<TNode> nodestack;
  Valuemap values;
  Componentmap nodecomponents;
  unsigned int id = 0;
  unsigned int min;

  // set all values to 0 -> no node is visited yet

  for ( iterator ndit = beginNodes(); ndit != endNodes(); ndit++ ){

    values[*ndit] = 0;

    if ( construct_scc_graph == true )
      nodecomponents[*ndit] = 0;

  }

  // since not every node must have a neighbour which points to it, we have to start from all unvisited nodes

  for ( iterator ndit = beginNodes(); ndit != endNodes(); ndit++ ){
      
    if ( values[*ndit] == 0 )
      min = scc_visit(*ndit,*scc,nodestack,values,nodecomponents,id,construct_scc_graph);

  }
      

  return scc;

}
