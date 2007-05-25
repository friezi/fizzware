/*
  Copyright (C) 1999-2007 Friedemann Zintel

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  For any questions, contact me at
  friezi@cs.tu-berlin.de
*/

/**
   @file graph.hpp
   @author Friedemann Zintel
*/

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <map>
#include <set>
#include <list>
#include <stack>
#include <utility>
#include <exception.hpp>

/**
   @example graph_example
*/

/**
   @brief graph-functions and -algorithms
*/
namespace graph{

  /**
     It stores all containing nodes. Besides a list containing
     all neighbours in the resulting component-graph will be filled if the proper option in find_scc() is provided.
     @brief This class represents a strongly connected component.
  */

  template <typename TNode>
  class SCCGraphComponent{

  public:

    typedef std::set< TNode, std::less<TNode> > Nodeset;
    typedef std::set< SCCGraphComponent<TNode> *, std::less<SCCGraphComponent<TNode> *> > Neighbourset;

  private:

    unsigned int id;

    Nodeset nodes;
    Neighbourset neighbours;

    // copy-constructor not allowed
    SCCGraphComponent(const SCCGraphComponent<TNode> &){}

  public:

    typedef typename Nodeset::iterator nodeiterator;
    typedef typename Neighbourset::iterator neighbouriterator;

    SCCGraphComponent(unsigned int id) : id(id){}

    std::pair<typename Nodeset::iterator, bool> insertComponentnode(const TNode & componentnode){ return nodes.insert(componentnode); }
    size_t eraseComponentnode(const TNode & componentnode){ return nodes.erase(componentnode); }

    std::pair<typename Neighbourset::iterator, bool> insertNeighbour(SCCGraphComponent<TNode> * neighbour){ return neighbours.insert(neighbour); }
    size_t eraseNeighbour(const SCCGraphComponent<TNode> * neighbour){ return neighbours.erase(neighbour); }

    typename Nodeset::iterator beginNodes(){ return nodes.begin(); }
    typename Nodeset::iterator endNodes(){ return nodes.end(); }
    
    typename Neighbourset::iterator beginNeighbours(){ return neighbours.begin(); }
    typename Neighbourset::iterator endNeighbours(){ return neighbours.end(); }

    inline unsigned int getId(){ return id; }
   
  };

  /**
     Besides this graph represents a component-dag of strongly connected components. I.e. The sccs form nodes in a graph where scc1 is neighbour of scc2
     (for any two vertices scc1 and scc2) if any of the included nodes of scc1 is neighbour of any included node of scc2.
     @brief The purpose of this class is to store the strongly connected components of a graph.
  */ 
  template <typename TNode>
  class SCCGraph{


  private:
    
    typedef std::list< SCCGraphComponent<TNode> * > Componentlist;

    Componentlist components;

    int componentcounter;

  public:

    typedef typename Componentlist::iterator iterator;

    SCCGraph(const SCCGraph &){}

    SCCGraph() : componentcounter(1){}
    
    ~SCCGraph(){

      for ( typename Componentlist::iterator it = components.begin(); it != components.end(); it++ )
	delete *it;

    }

    SCCGraphComponent<TNode> * newComponent(){

      SCCGraphComponent<TNode> *component = new SCCGraphComponent<TNode>(componentcounter++);

      components.push_back(component);
      return component;

    }

    iterator begin(){ return components.begin(); }
    iterator end(){ return components.end(); }
    
    int size(){ return componentcounter; }

  };

  /**
     This class does not define any graph-structure in the way of a data-structure, instead a class can be a graph in a
     functional way, i.e. offers functions which are necessary (and hopefully enough) for any graph-structure.
     So you resp. your graph-class has to provide several methods which are important to execute graph-algorithms.
     You can define any graph-structure you want, just define the purely virtual methods, which are mainly iterators for
     neighbours and nodes. Furthermore you have to extend the nested classes neighbour_iterator and node_iterator and override
     methods from their super-class iterator. With these methods any data-structure can behave as a graph-structure.
     @brief If a class extends Graphable it has access to graph-algorithms
     @pre For an object of type Graphable<Anytype> two objects t1 and t2 of type Anytype must be comparable by t1<t2. Thus "<" must be overloaded for
     the class Anytype.
  */
  template <typename TNode>
  class Graphable{

  private:

    typedef std::map< TNode, unsigned int, std::less<TNode> > Valuemap;
    typedef std::map< TNode, SCCGraphComponent<TNode> *, std::less<TNode> > Componentmap;

  protected:

    /**
       @brief To implement Graphable you have to override methods from this class by extending the derived
       classes node_iterator and neighbour_iterator
    */
    class iterator{

      friend class Graphable<TNode>;

    public:

      virtual ~iterator(){}

      /** @name common iterator-functions */
      //@{
      virtual void operator++() = 0;
      virtual void operator++(int) = 0;
      virtual TNode & operator*() = 0;
      virtual bool operator!=(const iterator & it_rval) = 0;
      virtual bool operator==(const iterator & it_rval) = 0;
      //@}
      
    };

  public:

    /**
       @brief To implement Graphable you have to extend this class and override
       methods from the super-class iterator
    */
    class node_iterator : public iterator{

      friend class Graphable<TNode>;

    public:

      virtual ~node_iterator(){}
      
    };

    /**
       @brief To implement Graphable you have to extend this class and override
       methods from the super-class iterator
    */
    class neighbour_iterator : public iterator{

      friend class Graphable<TNode>;

    public:

      virtual ~neighbour_iterator(){}
      
    };

    /**
       Since Graphable is an abstract class it's impossible for the neighboursBegin() - and neighboursEnd() - method
       to return a class-object, it has to return a pointer which won't be destructed automatically. temp_iterator just
       contains this pointer and destroys it automatically on destruction.
       @attention Destruction of two different temp_iterators containing the same reference will result (of course) in a segmentation-fault!
       @brief very useful for creating temporary iterators which are destructed automatically
    */
    class temp_iterator{

      iterator *it;

      temp_iterator(){}

      temp_iterator(const temp_iterator &){}

    public:

      temp_iterator(iterator *it) : it(it){}

      ~temp_iterator(){ delete it; }

      iterator * get(){ return it; }

    };

    virtual ~Graphable(){}


    /**
       @name override these methods
    */
    //@{
    /**
       @brief points to the first entry
       @return the iterator
    */
    virtual node_iterator * nodesBegin() = 0;

    /**
       @brief points to the point beyond the last entry
       @return the iterator
    */
    virtual node_iterator * nodesEnd() = 0;

    /**
       @brief points to the first entry
       @param node a node neighbours should be searched for
       @return the iterator
    */
    virtual neighbour_iterator * neighboursBegin(const TNode & node) = 0;

    /**
       @brief points to the point beyond the last entry
       @param node a node neighbours should be searched for
       @return the iterator
    */
    virtual neighbour_iterator * neighboursEnd(const TNode & node) = 0;

    /**
       @brief should return the maximum number of nodes in the structure
       @return maximum number
    */
    virtual unsigned int maxNodes() = 0;

    /**
       @brief should return the starting-node for some graph-algorithms
       @return starting node
    */
    virtual TNode startNode() = 0;
    //@}

    /**
       @name provided algorithms
    */
    //@{
    /**
       @brief for finding strongly connected components (Tarjan)
       @param construct_scc_graph if set the component-graph of the sccs will be built.
       @return a list resp. graph-structure of components containing the connected nodes
    */
    SCCGraph<TNode> * find_scc(bool construct_scc_graph);
    //@}

  private:

    unsigned int scc_visit(TNode node, SCCGraph<TNode> & scc, std::stack<TNode> & nodestack, Valuemap & values, Componentmap & nodecomponents,
			   unsigned int id, bool construct_scc_graph);
    
  };
  
}

// basically Tarjan's algorithm
template< typename TNode >
unsigned int
graph::Graphable<TNode>::
scc_visit(TNode node, SCCGraph<TNode> & scc, std::stack<TNode> & nodestack, Valuemap & values, Componentmap & nodecomponents, 
	  unsigned int id, bool construct_scc_graph){
  
  unsigned int m = 0, min;

  values[node] = ++id;

  min = id;

  nodestack.push(node);

  temp_iterator nit_begin(neighboursBegin(node));
  temp_iterator nit_end(neighboursEnd(node));

  for ( neighbour_iterator *nit = (neighbour_iterator *)nit_begin.get(); *nit != *nit_end.get(); (*nit)++ ){
    
    m = (!values[**nit]) ? scc_visit(**nit,scc,nodestack,values,nodecomponents,id,construct_scc_graph) : values[**nit];

    if ( m < min )
      min = m;
    
  }


  if ( min == values[node] ){

    SCCGraphComponent<TNode> * component = scc.newComponent();

    TNode topnode = 0;

    do{

      topnode = nodestack.top();
      component->insertComponentnode(topnode);
      nodestack.pop();

      values[topnode] = maxNodes() + 1;

      if ( construct_scc_graph == true ){
	// construct scc-graph

	nodecomponents[topnode] = component;

	temp_iterator tnit_begin(neighboursBegin(topnode));
	temp_iterator tnit_end(neighboursEnd(topnode));

	for ( neighbour_iterator *tnit = (neighbour_iterator *)tnit_begin.get(); *tnit != *tnit_end.get(); (*tnit)++ ){

	  SCCGraphComponent<TNode> * cn_component;

	  if ( ( cn_component = nodecomponents[**tnit] ) != 0 )
	    if ( cn_component != component)
	      component->insertNeighbour(cn_component);

	}

      }

    } while ( topnode != node );


  }
  
  return min;
    
}

template< typename TNode >
graph::SCCGraph<TNode> *
graph::Graphable<TNode>::
find_scc(bool construct_scc_graph = false){

  graph::SCCGraph<TNode> *scc = new graph::SCCGraph<TNode>();
  std::stack<TNode> nodestack;
  Valuemap values;
  Componentmap nodecomponents;
  unsigned int id = 0;
  unsigned int min;

  // set all values to 0 -> no node is visited yet
  {
    temp_iterator ndit_begin(nodesBegin());
    temp_iterator ndit_end(nodesEnd());

    for ( node_iterator *ndit = (node_iterator *)ndit_begin.get(); *ndit != *ndit_end.get(); (*ndit)++ ){

      values[**ndit] = 0;

      if ( construct_scc_graph == true )
	nodecomponents[**ndit] = 0;

    }

  }

  // since not every node must have a neighbour which points to it, we have to start from all unvisited nodes
  {
    temp_iterator ndit_begin(nodesBegin());
    temp_iterator ndit_end(nodesEnd());

    for ( node_iterator *ndit = (node_iterator *)ndit_begin.get(); *ndit != *ndit_end.get(); (*ndit)++ ){
      
      if ( values[**ndit] == 0 )
	min = scc_visit(**ndit,*scc,nodestack,values,nodecomponents,id,construct_scc_graph);

    }
      
  }

  return scc;

}


#endif
