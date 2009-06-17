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

#ifndef FZTOOLTEMPL_GRAPH_HPP
#define FZTOOLTEMPL_GRAPH_HPP

#include <map>
#include <set>
#include <sstream>
#include <list>
#include <stack>
#include <utility>
#include <fztooltempl/exception.hpp>

/**
   @example graph_example
*/

/**
   @brief graph-functions and -algorithms
*/
namespace graph{

  /**
     @defgroup algorithm Algorithms
     @defgroup structure Structural Classes
  */

  template<typename TNode> class SCCStructure;

  template<typename TNode> class SCCGraphComponent;

  /**
     @ingroup structure
     @brief \ref structure an abstract node_iterator class
  */
  template<typename TNode> class abstract_node_iterator{

  public:

    /**
       @brief returns the object the iterator is pointing at
       @return the object
    */
    virtual TNode operator*() throw(exc::ExceptionBase) = 0;

    /**
       @brief postincrement
    */
    virtual void operator++(int) throw(exc::ExceptionBase) = 0;
    
    /**
       @brief equality
       @return true if iterators are pointing to the same position (resp. object), false otherwise
    */
    virtual bool operator==(const abstract_node_iterator<TNode> *it_rval) throw(exc::ExceptionBase) = 0;
    
    bool operator!=(const abstract_node_iterator<TNode> *it_rval) throw(exc::ExceptionBase) { return !( *this == it_rval ); }
    
    virtual ~abstract_node_iterator(){}

  };

  /**
     This is a wrapper-class for an abstract_node_iterator. Since Graphable is an abstract class and a user must define its own subclasses of
     abstract_node_iterator it's impossible for the beginNodesPtr() - and beginNeighboursPtr() - method
     to return a class-object, they have to return a pointer which won't be destructed automatically. node_iterator just
     contains this pointer and destroys the referenced object automatically on destruction. Use beginNodes() and beginNeighbours() (resp. end...()) for creating
     iterators.
     @ingroup structure
     @attention Destruction of two different iterators containing the same reference will result (of course) in a segmentation-fault!
     @brief \ref structure A wrapper-class for an abstract_node_iterator.
  */
  template<typename TNode> class node_iterator{
    
  private:
    
    abstract_node_iterator<TNode> *it;
    
    bool delete_on_destruction;
    
    node_iterator(){}
    
  public:
    
    node_iterator(abstract_node_iterator<TNode> *it) : it(it), delete_on_destruction(true){}
    
    /**
       Only the newly created object will destroy the referenced pointer
       @internal
    */
    node_iterator(const node_iterator<TNode> & iter){
      
      this->it = iter.it;
      const_cast<node_iterator<TNode> &>(iter).delete_on_destruction = false;
      this->delete_on_destruction = true;
    
    }
    
    ~node_iterator(){
        
      if ( delete_on_destruction == true )
	delete it;

    }
      
    void operator++(int) throw(exc::ExceptionBase) { (*it)++; }
    TNode operator*() throw(exc::ExceptionBase) { return **it; }
    bool operator!=(const node_iterator<TNode> & it_rval) throw(exc::ExceptionBase) { return ( *this->it != it_rval.it ); }
    bool operator==(const node_iterator<TNode> & it_rval) throw(exc::ExceptionBase) { return ( *this->it == it_rval.it ); }

  };

  /**
     This class does not define any graph-structure in the way of a data-structure, instead a class can be a graph in a
     functional way, i.e. offers functions which are necessary (and hopefully enough) for any graph-structure.
     So you resp. your graph-class has to provide several methods which are important to execute graph-algorithms.
     You can define any graph-structure you want, just define the purely virtual methods, which are mainly iterators for
     neighbours and nodes.\n\n
     @ingroup structure
     @brief \ref structure For mapping a class to a graph-structure
  */
  template<typename TNode> class Graphable{

  public:
    /** @name abstract methods*/
    //@{
    
    /**
       ...
       @brief returns a pointer to a begin-iterator of the nodes
       @return pointer
    */
    virtual abstract_node_iterator<TNode> * beginNodesPtr() = 0;

    /**
       ...
       @brief returns a pointer to an end-iterator of the nodes
       @return pointer
    */
    virtual abstract_node_iterator<TNode> * endNodesPtr() = 0;

    /**
       ...
       @brief returns a pointer to a begin-iterator of the neighbours
       @return pointer
    */
    virtual abstract_node_iterator<TNode> * beginNeighboursPtr(TNode node) = 0;

    /**
       ...
       @brief returns a pointer to an end-iterator of the neighbours
       @return pointer
    */
    virtual abstract_node_iterator<TNode> * endNeighboursPtr(TNode node) = 0;

    /**
       ...
       @brief number of nontained nodes in the graph
       @return nmber of nodes
    */
    virtual size_t maxNodes() = 0;

    //@}

    virtual ~Graphable(){}

    /** @name access-methods*/
    //@{

    /**
       ...
       @brief creates an node_iterator pointing to the first node
       @return the node_iterator (wrapped)
    */
    node_iterator<TNode> beginNodes(){ return node_iterator<TNode>(beginNodesPtr()); }
    
    /**
       ...
       @brief creates an node_iterator pointing beyond the last node
       @return the node_iterator (wrapped)
    */
    node_iterator<TNode> endNodes(){ return node_iterator<TNode>(endNodesPtr()); }
    
    /**
       ...
       @brief creates an node_iterator pointing to the first neighbour of a node
       @param node the node
       @return the node_iterator (wrapped)
    */
    node_iterator<TNode> beginNeighbours(const TNode node){ return node_iterator<TNode>(beginNeighboursPtr(node)); }
    
    /**
       ...
       @brief creates an node_iterator pointing beyond the last neighbour of a node
       @param node the node
       @return the node_iterator (wrapped)
    */
    node_iterator<TNode> endNeighbours(const TNode node){ return node_iterator<TNode>(endNeighboursPtr(node)); }

    //@}
  };

  /**
     @ingroup algorithm
     @brief \ref algorithm finds strongly connected components and does some processing on it
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
    void find_scc() throw(exc::ExceptionBase);
    
    //@}

  protected:

    /** @name implement these methods */
    //@{
    
    /**
       intended to do some initialization before the search starts.
       @brief will be called initially by find_scc()
    */
    virtual void prepareFind() throw (exc::ExceptionBase) = 0;
    
    /**
       ...
       @brief will be called on each fully identified component
    */
    virtual void processComponent() throw (exc::ExceptionBase) = 0;
    
    /**
       ...
       @brief will be called on each node of the component
    */
    virtual void processComponentNode(TNode node) throw (exc::ExceptionBase) = 0;
    
    //@}
    
  private:

    // basically Tarjan's algorithm
    unsigned int scc_visit(const TNode node, unsigned int id) throw(exc::ExceptionBase);
    
  };

  template<typename TNode>
  void SCCProcessor<TNode>::find_scc() throw(exc::ExceptionBase){

    unsigned int id = 0;
    unsigned int min;

    while ( !nodestack.empty() ){
      nodestack.pop();
    }

    values.clear();

    // set all values to 0 -> no node is visited yet

    for ( node_iterator<TNode> ndit = graph->beginNodes(); ndit != graph->endNodes(); ndit++ ){
      values[*ndit] = 0;
    }
    
    prepareFind();

    // since not every node must have a neighbour which points to it, we have to start from all unvisited nodes

    for ( node_iterator<TNode> ndit = graph->beginNodes(); ndit != graph->endNodes(); ndit++ ){
      
      if ( values[*ndit] == 0 ){
	min = scc_visit(*ndit,id);
      }

    }

  }

  template<typename TNode>
  unsigned int SCCProcessor<TNode>::scc_visit(const TNode node, unsigned int id) throw(exc::ExceptionBase){
  
    unsigned int m = 0, min, value;

    values[node] = ++id;

    min = id;

    nodestack.push(node);

    for ( node_iterator<TNode> nit = graph->beginNeighbours(node); nit != graph->endNeighbours(node); nit++ ){

      value =  values[*nit];
      m = (!value) ? scc_visit(*nit,id) : value;

      if ( m < min )
	min = m;
    
    }

    if ( min == values[node] ){

      processComponent();

      TNode topnode;

      do{

	topnode = nodestack.top();
	nodestack.pop();

	values[topnode] = graph->maxNodes() + 1;

	processComponentNode(topnode);

      } while ( topnode != node );

    }
  
    return min;
    
  }

  /**
     @ingroup algorithm
     @brief \ref algorithm finds strongly connected components and stores the nodes in component sets
  */
  template<typename TNode> class SCCCollector : public SCCProcessor<TNode>{

  public:

    SCCCollector(Graphable<TNode> *graph) : SCCProcessor<TNode>(graph){}

    virtual ~SCCCollector(){}

  protected:
    
    SCCStructure<TNode> sccstructure;

    SCCGraphComponent<TNode> * component;
    
    virtual void prepareFind() throw (exc::ExceptionBase){
      sccstructure.clear();
    }
    
    virtual void processComponent() throw (exc::ExceptionBase){
      component = sccstructure.newComponent();
    }
    
    virtual void processComponentNode(TNode node) throw (exc::ExceptionBase){
      component->insertComponentNode(node);
    }

  public:

    SCCStructure<TNode> & getSCCStructure(){ return sccstructure; }

  };

  /**
     The graph will be accessible via getSCCStructure().
     @ingroup algorithm
     @brief \ref algorithm finds strongly connected components and constructs a component graph.
  */
  template<typename TNode> class SCCGraphConstructor : public SCCCollector<TNode>{

  public:

    SCCGraphConstructor(Graphable<TNode> *graph) : SCCCollector<TNode>(graph){}

  protected:

    typedef std::map< TNode, SCCGraphComponent<TNode> *, std::less<TNode> > Componentmap;

    Componentmap nodecomponents;
    
    virtual void prepareFind() throw (exc::ExceptionBase){

      SCCCollector<TNode>::prepareFind();

      nodecomponents.clear();

      for ( node_iterator<TNode> ndit = this->graph->beginNodes(); ndit != this->graph->endNodes(); ndit++ ){
	nodecomponents[*ndit] = 0;
      }

    }

    virtual void processComponentNode(TNode node) throw (exc::ExceptionBase){

      SCCCollector<TNode>::processComponentNode(node);

      nodecomponents[node] = this->component;

      for ( node_iterator<TNode> nit = this->graph->beginNeighbours(node); nit != this->graph->endNeighbours(node); nit++ ){

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
     @ingroup structure
     @brief \ref structure This class represents a strongly connected component.
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

    std::string toString() throw(exc::Exception< abstract_node_iterator<TNode> >){

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
     Besides this class can represent a component-dag of strongly connected components (in combination with SCCGraphConstructor).
     I.e. The sccs form nodes in a graph where scc1 is neighbour of scc2
     (for any two vertices scc1 and scc2) if any of the included nodes of scc1 is neighbour of any included node of scc2.
     @ingroup structure
     @brief \ref structure The purpose of this class is to store the strongly connected components of a graph.
     @since v1.98
  */ 
  template<typename TNode> class SCCStructure : public Graphable<SCCGraphComponent<TNode> *>{
  
  public:
  
    typedef std::list<SCCGraphComponent<TNode> *> Componentlist;

  private:
  
    Componentlist components;

    int componentcounter;

    SCCStructure(const SCCStructure<TNode> &){}

  public:
    
    class NodeIterator : public abstract_node_iterator<SCCGraphComponent<TNode> *>{

      friend class SCCStructure<TNode>;

    private:

      typename SCCStructure<TNode>::Componentlist::iterator it;

      NodeIterator(typename SCCStructure<TNode>::Componentlist::iterator it) : it(it){}

    public:

      ~NodeIterator(){}

      // Implement the virtual methods.
      void operator++(int) throw(exc::ExceptionBase){ it++; }
      SCCGraphComponent<TNode> * operator*() throw(exc::ExceptionBase){ return *it; }
      bool operator==(const abstract_node_iterator<SCCGraphComponent<TNode> *> * it_rval) throw(exc::ExceptionBase){
	return ( this->it == static_cast<const NodeIterator *>(it_rval)->it );
      }    

    };
    
    class NeighbourIterator : public abstract_node_iterator<SCCGraphComponent<TNode> *>{

      friend class SCCStructure<TNode>;

    private:

      typename SCCGraphComponent<TNode>::neighbouriterator it;

      void setIt(typename SCCGraphComponent<TNode>::neighbouriterator it_rval){ it = it_rval; }

    public:

      ~NeighbourIterator(){}

      // Implement the virtual methods.
      void operator++(int) throw(exc::ExceptionBase){ it++; }
      SCCGraphComponent<TNode> * operator*() throw(exc::ExceptionBase){ return *it; }
      bool operator==(const abstract_node_iterator<SCCGraphComponent<TNode> *> *it_rval) throw(exc::ExceptionBase){
	return ( this->it == static_cast<const NeighbourIterator *>(it_rval)->it );
      }
    
    };

  public:

    SCCStructure() : componentcounter(0){}
    
    ~SCCStructure(){ clear(); }

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

    abstract_node_iterator<SCCGraphComponent<TNode> *> * beginNodesPtr(){

      return new NodeIterator(components.begin());

    }

    abstract_node_iterator<SCCGraphComponent<TNode> *> * endNodesPtr(){

      return new NodeIterator(components.end());

    }

    // note: the "const" must be placed after the pointer-declaration. Otherwise the compiler misinterprets the meaning
    abstract_node_iterator<SCCGraphComponent<TNode> *> * beginNeighboursPtr(SCCGraphComponent<TNode> *node){

      NeighbourIterator *it = new NeighbourIterator();

      typename Componentlist::iterator cit = find(components.begin(),components.end(),node);
    
      it->setIt((*cit)->beginNeighbours());

      return it;

    }
    
    abstract_node_iterator<SCCGraphComponent<TNode> *> * endNeighboursPtr(SCCGraphComponent<TNode> *node){
      
      NeighbourIterator *it = new NeighbourIterator();
      
      typename Componentlist::iterator cit = find(components.begin(),components.end(),node);
      
      it->setIt((*cit)->endNeighbours());
      
      return it;
      
    }
    
    size_t maxNodes(){ return components.size(); }
    
  };
  
}


#endif
