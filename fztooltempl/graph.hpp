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

  typedef void * TNode;

  class SCCGraph;

  class SCCGraphComponent;

  /**
     This class does not define any graph-structure in the way of a data-structure, instead a class can be a graph in a
     functional way, i.e. offers functions which are necessary (and hopefully enough) for any graph-structure.
     So you resp. your graph-class has to provide several methods which are important to execute graph-algorithms.
     You can define any graph-structure you want, just define the purely virtual methods, which are mainly iterators for
     neighbours and nodes.\n\n
     Furthermore you have to extend the nested classes neighbour_iterator and node_iterator and override
     methods from their super-class abstract_iterator. With these methods any data-structure can behave as a graph-structure.\n\n
     For generic handling of objects instantiated from different GraphableBase-subclasses use this type as a baseclass.\n
     For type-safe operation derive not directly from this class but rather from the subclass Graphable.
     @brief If a class extends GraphableBase it has access to graph-algorithms
  */
  class GraphableBase{

  private:

    typedef std::map< TNode, unsigned int, std::less<TNode> > Valuemap;
    typedef std::map< TNode, SCCGraphComponent *, std::less<TNode> > Componentmap;

  protected:

    /**
       @brief To implement GraphableBase you have to override methods from this class by extending the derived
       classes node_iterator and neighbour_iterator
    */
    class abstract_iterator{

      friend class GraphableBase;

    public:

      virtual ~abstract_iterator(){}

      /** @name override these iterator-functions */
      //@{
 
      /**
	 @brief postincrement
      */
      virtual void operator++(int) = 0;

      /**
	 @brief returns the referenced object the iterator is pointing at
	 @return the referenced object
      */
      virtual TNode operator*() = 0;
      
      /**
	 @brief equality
	 @return true if iterators are pointing to the same position (resp. object), false otherwise
      */
      virtual bool operator==(const abstract_iterator & it_rval) = 0;
      //@}
 
      /** @name derived functions */
      //@{
      /**
	 @brief unequality
	 @return true if iterators are not pointing to the same position (resp. object), false otherwise
      */
      bool operator!=(const abstract_iterator & it_rval) { return !(*this == it_rval); }
      //@}      

    };

  public:

    /**
       @name extend these classes*/
    //@{
    /**
       @brief To implement GraphableBase you have to extend this class and override
       methods from the super-class abstract_iterator
    */
    class node_iterator : public abstract_iterator{

      friend class GraphableBase;

    public:

      virtual ~node_iterator(){}
      
    };

    /**
       @brief To implement GraphableBase you have to extend this class and override
       methods from the super-class abstract_iterator
    */
    class neighbour_iterator : public abstract_iterator{

      friend class GraphableBase;

    public:

      virtual ~neighbour_iterator(){}
      
    };
    //@}

    /**
       This is a wrapper-class for an abstract_iterator. Since GraphableBase is an abstract class and a user must define its own subclasses of node_iterator and
       neighbour_iterator it's impossible for the beginNodesPtr() - and beginNeighboursPtr() - method
       to return a class-object, they have to return a pointer which won't be destructed automatically. iterator just
       contains this pointer and destroys the referenced object automatically on destruction. Use beginNodes() and beginNeighbours() (resp. end...()) for creating
       iterators.
       @attention Destruction of two different iterators containing the same reference will result (of course) in a segmentation-fault!
       @brief A wrapper-class for an abstract_iterator.
    */
    class iterator{

    private:

      abstract_iterator *it;

      bool delete_on_destruction;

      iterator(){}

    public:

      iterator(abstract_iterator *it, bool delete_on_destruction = true) : it(it), delete_on_destruction(delete_on_destruction){}

      /**
	 This should never be used because copied iterators will point to the same abstract_iterators. Multiple deletion will
	 most probably result in segmentation-fault or bus-error.
	 @internal
      */
      iterator(const iterator & it){
	
	this->it = it.it;
	this->delete_on_destruction = true;

      }

      ~iterator(){
	
	if ( delete_on_destruction == true )
	  delete it;

      }
      
      void operator++(int){ (*it)++; }
      TNode operator*(){ return **it; }
      bool operator!=(const iterator & it_rval){ return ( *this->it != *it_rval.it ); }
      bool operator==(const iterator & it_rval){ return ( *this->it == *it_rval.it ); }

    };

    virtual ~GraphableBase(){}

    /**
       @name override these methods*/
    //@{
    /**
       This method must be overridden. This is only for internal use. Use beginNodes() which takes
       advantage of this method.
       @brief points to the first entry
       @return pointer to the node_iterator
       @internal
    */
    virtual node_iterator * beginNodesPtr() = 0;

    /**
       This method must be overridden. This is only for internal use. Use endNodes() which takes
       advantage of this method.
       @brief points to the point beyond the last entry
       @return pointer to a node_iterator
       @internal
    */
    virtual node_iterator * endNodesPtr() = 0;

    /**
       This method must be overridden. This is only for internal use. Use beginNeighbours() which takes
       advantage of this method.
       @brief points to the first entry
       @param node a node neighbours should be searched for
       @return pointer to a neighbour_iterator
       @internal
    */
    virtual neighbour_iterator * beginNeighboursPtr(const TNode node) = 0;

    /**
       This method must be overridden. This is only for internal use. Use endNeighbours() which takes
       advantage of this method.
       @brief points to the point beyond the last entry
       @param node a node neighbours should be searched for
       @return pointer to a neighbour_iterator
       @internal
    */
    virtual neighbour_iterator * endNeighboursPtr(const TNode node) = 0;

    /**
       @brief returns the maximum number of nodes in the structure
       @return maximum number
    */
    virtual unsigned int maxNodes() = 0;
    //@}

    /** @name provided algorithms*/
    //@{
    /**
       @brief for finding strongly connected components (Tarjan)
       @param construct_scc_graph if set the component-graph of the sccs will be built.
       @return a list resp. graph-structure of components containing the connected nodes
    */
    SCCGraph * find_scc(bool construct_scc_graph);

    //@}

    /** @name access-methods*/
    //@{
    /**
       @brief creates an iterator pointing to the first node
       @return the iterator (wrapped)
    */
    iterator beginNodes(){ return iterator(beginNodesPtr(),false); }
    
    /**
       @brief creates an iterator pointing beyond the last node
       @return the iterator (wrapped)
    */
    iterator endNodes(){ return iterator(endNodesPtr(),false); }
    
    /**
       @brief creates an iterator pointing to the first neighbour of a node
       @param node the node
       @return the iterator (wrapped)
    */
    iterator beginNeighbours(const TNode node){ return iterator(beginNeighboursPtr(node),false); }
    
    /**
       @brief creates an iterator pointing beyond the last neighbour of a node
       @param node the node
       @return the iterator (wrapped)
    */
    iterator endNeighbours(const TNode node){ return iterator(endNeighboursPtr(node),false); }

    //@}

  private:

    unsigned int scc_visit(const TNode node, SCCGraph & scc, std::stack<TNode> & nodestack, Valuemap & values, Componentmap & nodecomponents,
			   unsigned int id, bool construct_scc_graph);
    
  };

  /**
     To create your own Graph-structures it is recommended to derive from this class instead of the baseclass GraphableBase.
     @brief This subclass to GraphableBase makes operations more type-safe.
  */
  template<typename TTNode> class Graphable : public GraphableBase{

  public:
    
    class iterator : public GraphableBase::iterator{

    public:

      iterator(abstract_iterator *it, bool delete_on_destruction = true) : GraphableBase::iterator::iterator(it,delete_on_destruction){}

      /**
	 This should never be used because copied iterators will point to the same abstract_iterators. Multiple deletion will
	 most probably result in segmentation-fault or bus-error.
	 @internal
      */
      iterator(const iterator & it) : GraphableBase::iterator::iterator(it){}

      TTNode * operator*(){ return (TTNode *)(this->GraphableBase::iterator::operator*()); }

    };

    /**
       @brief creates an iterator pointing to the first node
       @return the iterator (wrapped)
    */
    iterator beginNodes(){ return iterator(beginNodesPtr(),false); }
    
    /**
       @brief creates an iterator pointing beyond the last node
       @return the iterator (wrapped)
    */
    iterator endNodes(){ return iterator(endNodesPtr(),false); }
    
    /**
       @brief creates an iterator pointing to the first neighbour of a node
       @param node the node
       @return the iterator (wrapped)
    */
    iterator beginNeighbours(const TNode node){ return iterator(beginNeighboursPtr(node),false); }
    
    /**
       @brief creates an iterator pointing beyond the last neighbour of a node
       @param node the node
       @return the iterator (wrapped)
    */
    iterator endNeighbours(const TNode node){ return iterator(endNeighboursPtr(node),false); }

  };

  /**
     It stores all containing nodes. Besides a list containing
     all neighbours in the resulting component-graph will be filled if the proper option in find_scc() is provided.
     @brief This class represents a strongly connected component.
     @since v1.98
  */
  class SCCGraphComponent{

  public:

    typedef std::set< TNode, std::less<TNode> > Nodeset;
    typedef std::set< SCCGraphComponent *, std::less<SCCGraphComponent *> > Neighbourset;

  private:

    unsigned int id;

    Nodeset nodes;
    Neighbourset neighbours;

    // copy-constructor not allowed
    SCCGraphComponent(const SCCGraphComponent &){}

  public:

    typedef Nodeset::iterator nodeiterator;
    typedef Neighbourset::iterator neighbouriterator;

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
    std::pair<Nodeset::iterator, bool> insertComponentnode(const TNode componentnode){ return nodes.insert(componentnode); }

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
    std::pair<Neighbourset::iterator, bool> insertNeighbour(SCCGraphComponent * neighbour){ return neighbours.insert(neighbour); }

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
    Nodeset::iterator beginNodes(){ return nodes.begin(); }

    /**
       @brief iterator pointing to end of contained nodes of components
       @return the iterator
    */
    Nodeset::iterator endNodes(){ return nodes.end(); }
    
    /**
       @brief iterator pointing to beginning of neighbours of the component
       @return the iterator
    */
    Neighbourset::iterator beginNeighbours(){ return neighbours.begin(); }

    /**
       @brief iterator pointing to end of neighbours of the component
       @return the iterator
    */
    Neighbourset::iterator endNeighbours(){ return neighbours.end(); }

    /**
       @brief the numerical id of the component
       @return the id
    */
    inline unsigned int getId() const { return id; }
   
  };

  /**
     Besides this graph represents a component-dag of strongly connected components. I.e. The sccs form nodes in a graph where scc1 is neighbour of scc2
     (for any two vertices scc1 and scc2) if any of the included nodes of scc1 is neighbour of any included node of scc2.
     @brief The purpose of this class is to store the strongly connected components of a graph.
     @since v1.98
  */ 
  class SCCGraph : public Graphable<SCCGraphComponent>{

  private:
    
    typedef std::list<SCCGraphComponent *> Componentlist;

    Componentlist components;

    int componentcounter;

    SCCGraph(const SCCGraph &){}

  public:
    
    class node_iterator : public GraphableBase::node_iterator{

      friend class SCCGraph;

    private:

      SCCGraph::Componentlist::iterator it;

      node_iterator(SCCGraph::Componentlist::iterator it) : it(it){}

    public:

      ~node_iterator(){}

      // Implement the virtual methods.
      void operator++(int){ it++; }
      TNode operator*(){ return *it; }
      bool operator==(const GraphableBase::abstract_iterator & it_rval){ return ( this->it == ((node_iterator &)it_rval).it ); }
    

    };
    
    class neighbour_iterator : public GraphableBase::neighbour_iterator{

      friend class SCCGraph;

    private:

      SCCGraphComponent::neighbouriterator it;

      void setIt(SCCGraphComponent::neighbouriterator it_rval){ it = it_rval; }

    public:

      ~neighbour_iterator(){}

      // Implement the virtual methods.
      void operator++(int){ it++; }
      TNode operator*(){ return *it; }
      bool operator==(const GraphableBase::abstract_iterator & it_rval){ return ( this->it == ((neighbour_iterator &)it_rval).it ); }
    
    };

    SCCGraph() : componentcounter(1){}
    
    ~SCCGraph(){

      for ( Componentlist::iterator it = components.begin(); it != components.end(); it++ )
	delete *it;

    }

    /**
       @brief constructs a new and empty (no nodes containing) component
       @return pointer to the newly created component
    */
    SCCGraphComponent * newComponent(){

      SCCGraphComponent *component = new SCCGraphComponent(componentcounter++);

      components.push_back(component);
      return component;

    }
    
    /**
       @brief size, i.e. number of contained components in the graph
       @return the size
    */
    int size(){ return componentcounter; }
    
    // We have to provide the GraphableBase-class Tstgraph with the implementation of the following pure virtual methods.

    GraphableBase::node_iterator * beginNodesPtr(){

      return new node_iterator(components.begin());

    }

    GraphableBase::node_iterator * endNodesPtr(){

      return new node_iterator(components.end());

    }

    // note: the "const" must be placed after the pointer-declaration. Otherwise the compiler misinterprets the meaning
    GraphableBase::neighbour_iterator * beginNeighboursPtr(const TNode node){

      neighbour_iterator * it = new neighbour_iterator();

      Componentlist::iterator cit = find(components.begin(),components.end(),node);
    
      it->setIt((*cit)->beginNeighbours());

      return it;

    }

    GraphableBase::neighbour_iterator * endNeighboursPtr(const TNode node){

      neighbour_iterator * it = new neighbour_iterator();

      Componentlist::iterator cit = find(components.begin(),components.end(),node);
    
      it->setIt((*cit)->endNeighbours());

      return it;

    }

    unsigned int maxNodes(){ return components.size(); }
    
  };
  
}


#endif
