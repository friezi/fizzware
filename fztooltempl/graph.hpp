/*
  Copyright (C) 1999-2005 Friedemann Zintel

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

#include <list>
#include <stack>
#include <exception.hpp>

/**
   @example graph_example
*/

/**
  @brief graph-functions and -algorithms
*/
namespace graph{

  /**
    This class does not define any graph-structure in the way of a data-structure, instead a class can be a graph in a
    functional way, i.e. offers functions which are necessary (and hopefully enough) for any graph-structure.
    So you resp. your graph-class has to provide several methods which are important to execute graph-algorithms.
    You can define any graph-structure you want, just define the purely virtual methods, which are mainly iterators for
    neighbours and nodes. Furthermore you have to extend the nested classes neighbour_iterator and node_iterator and override
    methods from their super-class iterator. With these methods any structure can behave as a graph-structure.
    @see Example @ref graph_example
    @brief if a class extends Graphable it has access to graph-algorithms
  */
  template <typename TNode>
  class Graphable{

  private:

    typedef map< TNode, unsigned int, less<TNode> > Valuemap;

  protected:

    /**
      @brief To implement Graphable you have to override methods from this class through extending the derived
      classes node_iterator and neighbour_iterator
    */
    class iterator{

      friend Graphable<TNode>;

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

      friend Graphable<TNode>;

    public:

      virtual ~node_iterator(){}
      
    };

    /**
      @brief To implement Graphable you have to extend this class and override
      methods from the super-class iterator
    */
    class neighbour_iterator : public iterator{

      friend Graphable<TNode>;

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
      @return a list of components containing the connected nodes
    */
    list< list<TNode> > * find_scc();
    //@}

  private:

    unsigned int scc_visit(TNode node, list< list<TNode> > & scc, stack<TNode> & nodestack, Valuemap & values, unsigned int id);
    
  };
  
}

// algorithm from Tarjan
template< typename TNode >
unsigned int
graph::Graphable<TNode>::
scc_visit(TNode node, list< list<TNode> > & scc, stack<TNode> & nodestack, Valuemap & values, unsigned int id){
  
  unsigned int m = 0, min;

  values[node] = ++id;

  min = id;

  nodestack.push(node);

  temp_iterator nit_begin(neighboursBegin(node));
  temp_iterator nit_end(neighboursEnd(node));

  for ( neighbour_iterator *nit = (neighbour_iterator *)nit_begin.get(); *nit != *nit_end.get(); (*nit)++ ){
    
    m = (!values[**nit]) ? scc_visit(**nit,scc,nodestack,values,id) : values[**nit];

    if ( m < min )
      min = m;
    
  }


  if ( min == values[node] ){

    list<TNode> new_scc;

    scc.push_back(new_scc);

    TNode current = 0;

    do{

      current = nodestack.top();
      scc.back().push_back(current);
      nodestack.pop();

      values[current] = maxNodes() + 1;

    } while ( current != node );


  }
  
  return min;
    
}

template< typename TNode >
list< list<TNode> > *
graph::Graphable<TNode>::
find_scc(){

  list< list<TNode> > *scc = new list< list<TNode> >();
  stack<TNode> nodestack;
  Valuemap values;
  unsigned int id = 0;
  unsigned int min;

  // set all values to 0 -> no node is visited yet
  {
    temp_iterator ndit_begin(nodesBegin());
    temp_iterator ndit_end(nodesEnd());

    for ( node_iterator *ndit = (node_iterator *)ndit_begin.get(); *ndit != *ndit_end.get(); (*ndit)++ )
      values[**ndit] = 0;

  }

  // since not every node must have a neighbour which points to it, we have to start from all unvisited nodes
  {
    temp_iterator ndit_begin(nodesBegin());
    temp_iterator ndit_end(nodesEnd());

    for ( node_iterator *ndit = (node_iterator *)ndit_begin.get(); *ndit != *ndit_end.get(); (*ndit)++ ){
      
      if ( values[**ndit] == 0 )
	min = scc_visit(**ndit,*scc,nodestack,values,id);

    }
      
  }

  return scc;

}


#endif
