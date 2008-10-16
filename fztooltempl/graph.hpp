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

#ifndef _FZTT_GRAPH_HPP_
#define _FZTT_GRAPH_HPP_

#include <map>
#include <set>
#include <sstream>
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

  template<typename TNode> class SCCSet;

  template<typename TNode> class SCCGraphComponent;

  template<typename TNode> class node_iterator{

  public:

    /**
       @brief returns the object the iterator is pointing at
       @return the object
    */
    virtual TNode operator*() = 0;

    /**
       @brief postincrement
    */
    virtual void operator++(int) = 0;
    
    /**
       @brief equality
       @return true if iterators are pointing to the same position (resp. object), false otherwise
    */
    virtual bool operator==(const node_iterator<TNode> *it_rval) = 0;
    
    bool operator!=(const node_iterator<TNode> *it_rval){ return !( *this == it_rval ); }
    
    virtual ~node_iterator(){}

  };

  /**
     This is a wrapper-class for an node_iterator. Since GraphableBase is an abstract class and a user must define its own subclasses of node_iterator and
     neighbour_iterator it's impossible for the beginNodesPtr() - and beginNeighboursPtr() - method
     to return a class-object, they have to return a pointer which won't be destructed automatically. iterator just
     contains this pointer and destroys the referenced object automatically on destruction. Use beginNodes() and beginNeighbours() (resp. end...()) for creating
     iterators.
     @attention Destruction of two different iterators containing the same reference will result (of course) in a segmentation-fault!
     @brief A wrapper-class for an abstract_iterator.
  */
  template<typename TNode> class iterator{
    
  private:
    
    node_iterator<TNode> *it;
    
    bool delete_on_destruction;
    
    iterator(){}
    
  public:
    
    iterator(node_iterator<TNode> *it) : it(it), delete_on_destruction(true){}
    
    /**
       Only the newly created object will destroy the referenced pointer
       @internal
    */
    iterator(const iterator<TNode> & iter){
      
      this->it = iter.it;
      const_cast<iterator<TNode> &>(iter).delete_on_destruction = false;
      this->delete_on_destruction = true;
    
    }
    
    ~iterator(){
        
      if ( delete_on_destruction == true )
	delete it;

    }
      
    void operator++(int){ (*it)++; }
    TNode operator*(){ return **it; }
    bool operator!=(const iterator<TNode> & it_rval){ return ( *this->it != it_rval.it ); }
    bool operator==(const iterator<TNode> & it_rval){ return ( *this->it == it_rval.it ); }

  };

  /**
     This class does not define any graph-structure in the way of a data-structure, instead a class can be a graph in a
     functional way, i.e. offers functions which are necessary (and hopefully enough) for any graph-structure.
     So you resp. your graph-class has to provide several methods which are important to execute graph-algorithms.
     You can define any graph-structure you want, just define the purely virtual methods, which are mainly iterators for
     neighbours and nodes.\n\n
  */
  template<typename TNode> class Graphable{

  public:

    virtual node_iterator<TNode> * beginNodesPtr() = 0;

    virtual node_iterator<TNode> * endNodesPtr() = 0;

    virtual node_iterator<TNode> * beginNeighboursPtr(TNode node) = 0;

    virtual node_iterator<TNode> * endNeighboursPtr(TNode node) = 0;

    virtual size_t maxNodes() = 0;

    virtual ~Graphable(){}

    /** @name access-methods*/
    //@{
    /**
       @brief creates an iterator pointing to the first node
       @return the iterator (wrapped)
    */
    iterator<TNode> beginNodes(){ return iterator<TNode>(beginNodesPtr()); }
    
    /**
       @brief creates an iterator pointing beyond the last node
       @return the iterator (wrapped)
    */
    iterator<TNode> endNodes(){ return iterator<TNode>(endNodesPtr()); }
    
    /**
       @brief creates an iterator pointing to the first neighbour of a node
       @param node the node
       @return the iterator (wrapped)
    */
    iterator<TNode> beginNeighbours(const TNode node){ return iterator<TNode>(beginNeighboursPtr(node)); }
    
    /**
       @brief creates an iterator pointing beyond the last neighbour of a node
       @param node the node
       @return the iterator (wrapped)
    */
    iterator<TNode> endNeighbours(const TNode node){ return iterator<TNode>(endNeighboursPtr(node)); }

  };

  /**
     @brief finds strongly connected componets and does some processing on it
  */
  template<typename TNode> class SCCProcessor{

  private:

    typedef std::map< TNode, unsigned int, std::less<TNode> > Valuemap;

  protected:

    std::stack<TNode> nodestack;

    Valuemap values;

    Graphable<TNode> *graph;

  public:

    SCCProcessor(Graphable<TNode> *graph){ this->graph = graph; }

    virtual ~SCCProcessor(){}
    
    /** @name provided algorithm*/
    //@{
    /**
       @brief for finding and doing some processing on strongly connected components (Tarjan)
    */
    void find_scc();

  protected:
    
    //@}

    /** @name implement these methods */
    //@{
    
    virtual void prepare() = 0;
    
    virtual void processFoundCycle() = 0;
    
    virtual void processCycleNode(TNode node) = 0;
    
    //@}
    
  private:

    // basically Tarjan's algorithm
    unsigned int scc_visit(const TNode node, unsigned int id);
    
  };

  template<typename TNode>
  void SCCProcessor<TNode>::find_scc(){

    unsigned int id = 0;
    unsigned int min;

    while ( !nodestack.empty() ){
      nodestack.pop();
    }

    values.clear();

    // set all values to 0 -> no node is visited yet

    for ( iterator<TNode> ndit = graph->beginNodes(); ndit != graph->endNodes(); ndit++ ){
      values[*ndit] = 0;
    }
    
    prepare();

    // since not every node must have a neighbour which points to it, we have to start from all unvisited nodes

    for ( iterator<TNode> ndit = graph->beginNodes(); ndit != graph->endNodes(); ndit++ ){
      
      if ( values[*ndit] == 0 ){
	min = scc_visit(*ndit,id);
      }

    }

  }

  template<typename TNode>
  unsigned int SCCProcessor<TNode>::scc_visit(const TNode node, unsigned int id){
  
    unsigned int m = 0, min;

    values[node] = ++id;

    min = id;

    nodestack.push(node);

    for ( iterator<TNode> nit = graph->beginNeighbours(node); nit != graph->endNeighbours(node); nit++ ){
    
      m = (!values[*nit]) ? scc_visit(*nit,id) : values[*nit];

      if ( m < min )
	min = m;
    
    }

    if ( min == values[node] ){

      processFoundCycle();

      TNode topnode;

      do{

	topnode = nodestack.top();
	nodestack.pop();

	values[topnode] = graph->maxNodes() + 1;

	processCycleNode(topnode);

      } while ( topnode != node );

    }
  
    return min;
    
  }

  /**
     @brief finds strongly connected components and stores the nodes in component sets
  */
  template<typename TNode> class SCCCollector : public SCCProcessor<TNode>{

  public:

    SCCCollector(Graphable<TNode> *graph) : SCCProcessor<TNode>(graph){}

    virtual ~SCCCollector(){}

  protected:
    
    SCCSet<TNode> sccset;

    SCCGraphComponent<TNode> * component;
    
    virtual void prepare(){
      sccset.clear();
    }
    
    virtual void processFoundCycle(){
      component = sccset.newComponent();
    }
    
    virtual void processCycleNode(TNode node){
      component->insertComponentNode(node);
    }

  public:

    SCCSet<TNode> & getSCCSet(){ return sccset; }

  };

  /**
     The graph will be accessible via getSCCSet().
     @brief finds strongly connected components and constructs a component graph.
  */
  template<typename TNode> class SCCGraphConstructor : public SCCCollector<TNode>{

  public:

    SCCGraphConstructor(Graphable<TNode> *graph) : SCCCollector<TNode>(graph){}

  protected:

    typedef std::map< TNode, SCCGraphComponent<TNode> *, std::less<TNode> > Componentmap;

    Componentmap nodecomponents;
    
    virtual void prepare(TNode node){

      SCCCollector<TNode>::prepare();

      nodecomponents.clear();

      for ( iterator<TNode> ndit = this->graph->beginNodes(); ndit != this->graph->endNodes(); ndit++ ){
	nodecomponents[node] = 0;
      }

    }

    virtual void processCycleNode(TNode node){

      SCCCollector<TNode>::processCycleNode(node);

      nodecomponents[node] = this->component;

      for ( iterator<TNode> nit = this->graph->beginNeighbours(node); nit != this->graph->endNeighbours(node); nit++ ){

	SCCGraphComponent<TNode> * neighbourcomponent;

	if ( ( neighbourcomponent = nodecomponents[*nit] ) != 0 )
	  if ( neighbourcomponent != this->component)
	    this->component->insertNeighbour(neighbourcomponent);

      }
    }

  public:

  };
  
  /**
     It stores all containing nodes. Besides a list containing
     all neighbours in the resulting component-graph will be filled if the proper option in find_scc() is provided.
     @brief This class represents a strongly connected component.
     @since v1.98
  */
  template<typename TNode> class SCCGraphComponent{
    
  public:

    SCCGraphComponent(){}
    
    typedef std::set< TNode, std::less<TNode> > Nodeset;
    typedef std::set< SCCGraphComponent<TNode> *, std::less<SCCGraphComponent<TNode> *> > Neighbourset;
    typedef typename Nodeset::iterator nodeiterator;
    typedef typename Neighbourset::iterator neighbouriterator;

  private:
    
    unsigned int id;
    
    Nodeset nodes;
    Neighbourset neighbours;
    
    // copy-constructor not allowed
    SCCGraphComponent(const SCCGraphComponent<TNode> &){}
    
  public:

    /**
       @brief Default-constructor
       @param id a number identifing the component
    */
    SCCGraphComponent(unsigned int id) : id(id){}

    /**
       @brief inserts a node to the component's node-list
       @param componentnode the node which should be a member of the component
       @return a pair consisting of an iterator to the inserted node and a bool-value expressing if insertion was first time made.
    **/
    std::pair<typename Nodeset::iterator, bool> insertComponentNode(const TNode componentnode){ return nodes.insert(componentnode); }

    /**
       @brief erases a node from the component's node-list
       @param componentnode the node to be erased from list
       @return resulting size of the list
    */
    size_t eraseComponentnode(const TNode componentnode){ return nodes.erase(componentnode); }

    /**
       @brief inserts a component to the component's neighbour-list
       @param neighbour the component which should be a neighbour of the component
       @return a pair consisting of an iterator to the inserted component and a bool-value expressing if insertion was first time made.
    **/
    std::pair<typename Neighbourset::iterator, bool> insertNeighbour(SCCGraphComponent * neighbour){ return neighbours.insert(neighbour); }

    /**
       @brief erases a component from the component's neighbour-list
       @param neighbour the component to be erased from list
       @return resulting size of the list
    */
    size_t eraseNeighbour(SCCGraphComponent * neighbour){ return neighbours.erase(neighbour); }

    /**
       @brief iterator pointing to beginning of contained nodes of the component
       @return the iterator
    */
    typename Nodeset::iterator beginNodes(){ return nodes.begin(); }

    /**
       @brief iterator pointing to end of contained nodes of components
       @return the iterator
    */
    typename Nodeset::iterator endNodes(){ return nodes.end(); }
    
    /**
       @brief iterator pointing to beginning of neighbours of the component
       @return the iterator
    */
    typename Neighbourset::iterator beginNeighbours(){ return neighbours.begin(); }

    /**
       @brief iterator pointing to end of neighbours of the component
       @return the iterator
    */
    typename Neighbourset::iterator endNeighbours(){ return neighbours.end(); }

    /**
       @brief the numerical id of the component
       @return the id
    */
    inline unsigned int getId() const { return id; }

    std::string toString(){

      std::ostringstream sbuffer;

      sbuffer << "{";

      for ( typename Nodeset::iterator it = beginNodes(); it != endNodes(); it++ ){

	if ( it != beginNodes() )
	  sbuffer << ",";
	sbuffer << *it;

      }

      sbuffer << "}";
      
      return sbuffer.str();
      
    }
    
  };

  /**
     Besides this graph represents a component-dag of strongly connected components. I.e. The sccs form nodes in a graph where scc1 is neighbour of scc2
     (for any two vertices scc1 and scc2) if any of the included nodes of scc1 is neighbour of any included node of scc2.
     @brief The purpose of this class is to store the strongly connected components of a graph.
     @since v1.98
  */ 
  template<typename TNode> class SCCSet : Graphable<SCCGraphComponent<TNode> *>{
  
  public:
  
    typedef std::list<SCCGraphComponent<TNode> *> Componentlist;

  private:
  
    Componentlist components;

    int componentcounter;

    SCCSet(const SCCSet<TNode> &){}

  public:
    
    class NodeIterator : public node_iterator<SCCGraphComponent<TNode> *>{

      friend class SCCSet<TNode>;

    private:

      typename SCCSet<TNode>::Componentlist::iterator it;

      NodeIterator(typename SCCSet<TNode>::Componentlist::iterator it) : it(it){}

    public:

      ~NodeIterator(){}

      // Implement the virtual methods.
      void operator++(int){ it++; }
      SCCGraphComponent<TNode> * operator*(){ return *it; }
      bool operator==(const node_iterator<SCCGraphComponent<TNode> *> * it_rval){ return ( this->it == (static_cast<const NodeIterator *>(it_rval))->it ); }    

    };
    
    class NeighbourIterator : public node_iterator<SCCGraphComponent<TNode> *>{

      friend class SCCSet<TNode>;

    private:

      typename SCCGraphComponent<TNode>::neighbouriterator it;

      void setIt(typename SCCGraphComponent<TNode>::neighbouriterator it_rval){ it = it_rval; }

    public:

      ~NeighbourIterator(){}

      // Implement the virtual methods.
      void operator++(int){ it++; }
      SCCGraphComponent<TNode> * operator*(){ return *it; }
      bool operator==(const node_iterator<SCCGraphComponent<TNode> *> *it_rval){ return ( this->it == (static_cast<const NeighbourIterator *>(it_rval))->it ); }
    
    };

  public:

    SCCSet() : componentcounter(0){}
    
    ~SCCSet(){ clear(); }

    void clear(){

      for ( typename Componentlist::iterator it = components.begin(); it != components.end(); it++ )
	delete *it;
      
      components.clear();
      componentcounter = 0;

    }

    /**
       @brief constructs a new and empty (no nodes containing) component
       @return pointer to the newly created component
    */
    SCCGraphComponent<TNode> * newComponent(){

      SCCGraphComponent<TNode> *component = new SCCGraphComponent<TNode>(++componentcounter);

      components.push_back(component);
      return component;

    }
    
    /**
       @brief size, i.e. number of contained components in the graph
       @return the size
    */
    int size(){ return componentcounter; }


    typename std::list<SCCGraphComponent<TNode> *>::iterator beginComponents(){ return components.begin(); }
    typename std::list<SCCGraphComponent<TNode> *>::iterator endComponents(){ return components.end(); }
    
    // We have to provide the GraphableBase-class Tstgraph with the implementation of the following pure virtual methods.

    node_iterator<SCCGraphComponent<TNode> *> * beginNodesPtr(){

      return new NodeIterator(components.begin());

    }

    node_iterator<SCCGraphComponent<TNode> *> * endNodesPtr(){

      return new NodeIterator(components.end());

    }

    // note: the "const" must be placed after the pointer-declaration. Otherwise the compiler misinterprets the meaning
    node_iterator<SCCGraphComponent<TNode> *> * beginNeighboursPtr(SCCGraphComponent<TNode> *node){

      NeighbourIterator *it = new NeighbourIterator();

      typename Componentlist::iterator cit = find(components.begin(),components.end(),node);
    
      it->setIt((*cit)->beginNeighbours());

      return it;

    }
    
    node_iterator<SCCGraphComponent<TNode> *> * endNeighboursPtr(SCCGraphComponent<TNode> *node){
      
      NeighbourIterator *it = new NeighbourIterator();
      
      typename Componentlist::iterator cit = find(components.begin(),components.end(),node);
      
      it->setIt((*cit)->endNeighbours());
      
      return it;
      
    }
    
    size_t maxNodes(){ return components.size(); }
    
  };
  
}


#endif
