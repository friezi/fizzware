/*
    Copyright (C) 1999-2004 Friedemann Zintel

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
   @file datastructures.hpp
   @author Friedemann Zintel
*/

#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP
#include <utility>
#include <map>
#include <exception.hpp>

/**
   @brief Contains templates for datastructures
   @ingroup datastructures
*/

namespace ds{
  
  /**
     @brief to administer memory-blocks
     @internal
  */
  template<typename T>
  class MemBlock{
    
  public:
    
    MemBlock<T> *next;
    T *buf;
    
    // Konstruktor:
    MemBlock(unsigned long blksize) throw (Exception_T);
    
    // Destruktor:
    ~MemBlock() throw (Exception_T);
    
  };
  
  /**
     @brief for holding pointers
  */
  template<typename T>
  class MemPointer{
    
  private:
    T *ptr;
    bool clearflag;
    
  public:
    
    /**
       @param clear_on_exit if true, free() will be called for internal pointer
    */
    MemPointer( bool clear_on_exit = true ) : ptr((T *)0), clearflag(clear_on_exit) {}
    
    MemPointer(const MemPointer &mempointer) { this->ptr = mempointer.ptr; }
    
    /**
       If clear_on_exit was true, memory is freed
    */
    ~MemPointer() {
      if ( clearflag == true )
	free(ptr);
      ptr = (T *)0;
    }
    
    /**
       @brief get the memory-pointer
       @return memory-pointer
    */
    T *get() { return ptr; }
    
    /**
       @brief return status of clear_on_exit-flag
       @return clear_on_exit-flag
    */
    bool getClearflag() { return clearflag; }
    
    /**
       @brief set clear_on_exit-flag
       @param clear_on_exit true or false
    */
    void setClearflag(bool clear_on_exit) { clearflag = clear_on_exit; }
    
    /**
       @brief resets the internal pointer to 0, can free the memory
       @param freemem if true, memory will be freed
    */
    void clear ( bool freemem = false ) {
      if ( freemem == true )
	free(ptr);
      ptr = (T *)0;
    }
    
    MemPointer &operator=(const MemPointer &mempointer) { 
      this->ptr = mempointer.ptr;
      return *this;
    }
    
    MemPointer &operator=(const T * const mempointer) {
      this->ptr = (T *)mempointer;
      return *this;
    }
    
  };
  
  /**
     @brief An extensible Buffer of unspecified size
  */
  template<typename T>
  class Buffer{
    
  private:
    
    MemBlock<T> *memblock;
    unsigned long blksize;
    unsigned long offs;
    unsigned long nmb;
    unsigned long count;
    
  public:
    
    // Konstruktor:
    /**
       @param blksize size of a block in the buffer
    */
    Buffer(unsigned long blksize) : memblock(NULL), blksize(blksize), offs(0), nmb(0), count(0){}
    
    // Destruktor:
    /**
       @exception Exception_T
    */
    ~Buffer() throw (Exception_T);
    
    // Element in den Puffer schreiben
    /**
       @brief put a new object in the buffer
       @param c reference to the object
       @exception Exception_T
    */
    void put(T &c) throw (Exception_T);
    
    // Bloecke zu einem Block zusammenfuegen (NULL terminiert)
    /**
       @brief return a MemPointer of a continuous memory-block, filled with the merged buffer-blocks
       @return the MemPointer for the memory
       @remark No '\\0' will be added, so take care of this by yourself if you want
       to convert it to a string, e.g. use put(0);
       @exception Exception< Buffer<T> >
    */
    MemPointer<T> merge() throw (Exception_T);
    
    // Anzahl der Bloecke
    /**
       @brief Get the number of blocks in the buffer
       @return number of blocks
    */
    unsigned long blocks(){ return nmb; }
    
    // Anzahl der geschriebenen Elemente (Ohne NULL-Byte am Ende)
    /**
       @brief Get the total number of elements of type T in the buffer
       @return number of elements
    */
    unsigned long elements() { return count; }
    
    /**
       @brief Clears the buffer and resets all variables
    */
    void clear();
    
  };
  
  /**
    @brief The base-node class
  **/
  class Node{
    
  public:

    Node *next; 
    Node *prev; 

    Node(Node *next,Node *prev) : next(next),prev(prev){}
  };

  
  /**
     It stores a key of the template-classtype TKey.
    @brief The base-class for a branch of a tree
  **/
  template< typename TKey, typename TComp, typename TDel >
  class KeyNode : public Node{

  public:
    TKey key;

    KeyNode(KeyNode<TKey,TComp,TDel> *next,KeyNode<TKey,TComp,TDel> *prev, TKey &key)
      : Node(next,prev) ,key(key){}

    ~KeyNode();
    
  };

  /**
     The template-parameters are
      - TKey:\n
        the class-type of the key
      - TComp:\n
        Comparator: the class-type for a function-object, containing the comparison-function. It must be
	a binary function, both parameters of type TKey, returning a bool.
	Besides, the operator bool TKey::operator=(TKey) must be defined.
      - TDel:\n
        Deleter : the class-type for a function-object, containing the delete-function. It will be called on destructor-call
	and can delete the key (if it's a pointer or something more complicated). Usefull if the key should be deleted automatically on
	destruction of the BinaryTreeBranch. If the key should not be destructed just leave the function-body empty of the Deleter-class.

     @brief A class for a branch of a binary-tree
     @see STL-documentation on the use of function-objects.
  **/
  template< typename TKey, typename TComp, typename TDel >
  class BinaryTreeBranch : public KeyNode<TKey,TComp,TDel>{

  public:

    typedef std::pair< BinaryTreeBranch<TKey,TComp,TDel> *, BinaryTreeBranch<TKey,TComp,TDel> *> Tail;

    BinaryTreeBranch<TKey,TComp,TDel> *left;
    BinaryTreeBranch<TKey,TComp,TDel> *right;
    BinaryTreeBranch<TKey,TComp,TDel> *parent;

    BinaryTreeBranch(TKey &key)
      : KeyNode<TKey,TComp,TDel>(0,0,key), left(0),right(0),parent(0){}

    BinaryTreeBranch(KeyNode<TKey,TComp,TDel> *next,KeyNode<TKey,TComp,TDel> *prev,
		     BinaryTreeBranch<TKey,TComp,TDel> *left,BinaryTreeBranch<TKey,TComp,TDel> *right,
		     BinaryTreeBranch<TKey,TComp,TDel> *parent,TKey &key)
      : KeyNode<TKey,TComp,TDel>(next,prev,key), left(left),right(right),parent(parent){}

    void insert(TKey key);
    
    BinaryTreeBranch<TKey,TComp,TDel> * getFirst();
    BinaryTreeBranch<TKey,TComp,TDel> * getLast();

    void concat(BinaryTreeBranch<TKey,TComp,TDel> *node);
    BinaryTreeBranch<TKey,TComp,TDel> * cutRight();
    BinaryTreeBranch<TKey,TComp,TDel> * cutLeft();
    void breakRight();
    void breakLeft();

    BinaryTreeBranch<TKey,TComp,TDel> * getBranch(TKey key);
        
    void show();
    
  };

  /**
    @brief A class for a branch of a multiple-tree
    @attention not yet supported!
    @todo supporting MultipleTreeBranch
  **/
  template< typename TKey, typename TComp, typename TDel >
  class MultipleTreeBranch : virtual KeyNode<TKey,TComp,TDel>{

  public:

    std::map<MultipleTreeBranch<TKey,TComp,TDel>,TKey,TComp> children;

    MultipleTreeBranch(KeyNode<TKey,TComp,TDel> *next,KeyNode<TKey,TComp,TDel> *prev, TKey &key)
      : KeyNode<TKey,TComp,TDel>(next,prev,key){}
  };

  /**
     @brief A class for holding keynodes. Useful for defining your own tree-structures
  */
  template< template<typename,typename,typename> class TKNode, typename TKey, typename TComp, typename TDel >
  class KeyNodeContainer{

  public:

    class iterator{

      friend class KeyNodeContainer;

    private :
      
      TKNode<TKey,TComp,TDel> *current;

      iterator(TKNode<TKey,TComp,TDel> * kn) : current(kn){}
      
    public:
      
      iterator() : current((TKNode<TKey,TComp,TDel> *)0){}
      
      void operator++();
      void operator--();
      TKey & operator*(){ return current->key; }
      bool operator==(KeyNodeContainer::iterator it){ return (this->current == it.current); }
      bool operator!=(KeyNodeContainer::iterator it){ return (this->current != it.current); }
      void operator=(KeyNodeContainer::iterator it){ this->current = it.current; }
      
    };
    
    TKNode<TKey,TComp,TDel> *root;
    TKNode<TKey,TComp,TDel> *first;

    KeyNodeContainer() : root(0),first(0){}
    
    ~KeyNodeContainer();

    iterator begin();
    iterator end();
  };
  
  /** 
      A template-class for binary trees
      @brief A binary Tree, which stores keys
      @see BinaryTreeBranch for information about the templateclass-parameters
  */
  template< typename TKey,typename TComp, typename TDel >
  class BTree : public KeyNodeContainer< BinaryTreeBranch,TKey,TComp,TDel >{

    public:

    unsigned long count;
    
    BTree() : KeyNodeContainer< BinaryTreeBranch,TKey,TComp,TDel >(),count(0){}

    void insert(TKey key);
    void erase(TKey key);
    bool empty(){ return ( count == 0 ); }

    void show();
    
  };

  template< typename TKey, typename TComp, typename TDel >
  class MTree : KeyNodeContainer< MultipleTreeBranch,TKey,TComp,TDel >{
  };

  template< typename T >
  class nofree{

  public:

    void operator()(T){}

  };
  
}

template< typename TKey, typename TComp, typename TDel >
ds::KeyNode<TKey,TComp,TDel>::
~KeyNode(){

  TDel del;

  del(key);
}
  
template< template<typename,typename,typename> class TKNode,typename TKey, typename TComp, typename TDel >
ds::KeyNodeContainer<TKNode,TKey,TComp,TDel>::
~KeyNodeContainer<TKNode,TKey,TComp,TDel>(){

  TKNode<TKey,TComp,TDel> *curr = this->first;
  TKNode<TKey,TComp,TDel> *next = 0;

  while ( curr ){
    
    next = (TKNode<TKey,TComp,TDel> *)curr->next;
    delete curr;
    curr = next;
  }
}
  
template< template<typename,typename,typename> class TKNode,typename TKey, typename TComp, typename TDel >
typename ds::KeyNodeContainer<TKNode,TKey,TComp,TDel>::iterator
ds::KeyNodeContainer<TKNode,TKey,TComp,TDel >::
begin(){
    
  typename KeyNodeContainer<TKNode,TKey,TComp,TDel>::iterator it(first);
  return it;
}
  
template< template<typename,typename,typename> class TKNode,typename TKey, typename TComp, typename TDel >
typename ds::KeyNodeContainer<TKNode,TKey,TComp,TDel>::iterator
ds::KeyNodeContainer<TKNode,TKey,TComp,TDel >::
end(){
  
  typename KeyNodeContainer<TKNode,TKey,TComp,TDel>::iterator it;
  return it;
}

template< template<typename,typename,typename> class TKNode,typename TKey, typename TComp, typename TDel >
void
ds::KeyNodeContainer<TKNode,TKey,TComp,TDel>::iterator::
operator++(){

  if ( current != 0 )
    current = (TKNode<TKey,TComp,TDel> *)current->next;
}

template< template<typename,typename,typename> class TKNode,typename TKey, typename TComp, typename TDel >
void
ds::KeyNodeContainer<TKNode,TKey,TComp,TDel>::iterator::
operator--(){

  if ( current != 0 )
    current = (TKNode<TKey,TComp,TDel> *)current->prev;
}

template<typename T>
ds::MemBlock<T>::
MemBlock(unsigned long blksize) throw (Exception_T) : next(0){
  
  if (!(buf = (T *)calloc(blksize,sizeof(T))))
    throw Exception< ds::MemBlock<T> >("MemBlock(): calloc() failed!");
}

template<typename T>
ds::MemBlock<T>::
~MemBlock() throw (Exception_T){
  
  if (buf)
    free(buf);
  else
    throw Exception< ds::MemBlock<T> >("~MemBlock(): buf==NULL!");
  
}

template<typename T>
ds::Buffer<T>::
~Buffer() throw (Exception_T){
  
  ds::MemBlock<T> *curr,*next;
  
  if( !memblock )
    return;
  
  curr = memblock;
  
  while (curr){
    
    next = curr->next;
    delete curr;
    curr = next;
  }
}

template<typename T>
void
ds::Buffer<T>::
put(T &c) throw (Exception_T){
  
  ds::MemBlock<T> *curr;
  
  /* bis zum letzten Block vorarbeiten */
  if ( memblock ){
    curr = memblock;
    while (curr->next)
      curr = curr->next;
  }
  else{
    memblock = new ds::MemBlock<T>(blksize);
    curr = memblock;
    nmb++;
  }
  
  /* eventuell neuen Block allokieren */
  if ( offs == blksize ){
    curr->next = new ds::MemBlock<T>(blksize);
    nmb++;
    
    curr = curr->next;
    offs=0;
  }
  
  /* Zeichen schreiben */
  curr->buf[offs]=c;
  count++;
  
  offs++;
}

template<typename T>
ds::MemPointer<T>
ds::Buffer<T>::
merge() throw (Exception_T){
  
  unsigned long units=0, pos=0;
  ds::MemBlock<T> *curr;
  ds::MemPointer<T> mempointer(false);
  T *block;
  
  if ( !memblock )
    return 0;
  
  curr = memblock;
  
  units = nmb*blksize;
  
  if ( !(block = (T *)calloc(1,units*sizeof(T)+1)) )
    throw Exception< ds::Buffer<T> >("merge(): calloc failed!");
  
  curr = memblock;
  
  /* alle Bloecke kopieren */
  while ( curr ){
    if ( !curr->buf )
      throw Exception< ds::Buffer<T> >("merge(): curr->buf==NULL!");
    memcpy(&block[pos],curr->buf,blksize*sizeof(T));
    pos += blksize*sizeof(T);
    curr = curr->next;
  }
  
  mempointer = block;
  
  return mempointer;
}

template< typename T >
void
ds::Buffer<T>::
clear(){
  
  ds::MemBlock<T> *curr,*next;
  
  if ( memblock ){
    
    curr = memblock;
    
    while (curr){
      
	next = curr->next;
	delete curr;
	curr = next;
    }
  }
  memblock = NULL;
  offs = count = 0;
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BinaryTreeBranch<TKey,TComp,TDel>::
insert(TKey key){
  
  TComp comp;
  
  if ( comp(key,this->key) == true ){
    if ( left != NULL )
      left->insert(key);
    else{

      left = new ds::BinaryTreeBranch<TKey,TComp,TDel>(key);

      left->parent = this;
      
      left->next = this;
      left->prev = this->prev;
      
      if ( this->prev )
	this->prev->next = left;

      this->prev = left;

    }
  }
  else{

    if ( right != NULL )
      right->insert(key);
    else{

      right = new ds::BinaryTreeBranch<TKey,TComp,TDel>(key);

      right->parent = this;

      right->next = this->next;
      right->prev = this;

      if ( this->next )
	this->next->prev = right;
      
      this->next = right;
    }
  }
}

template< typename TKey, typename TComp, typename TDel >
ds::BinaryTreeBranch<TKey,TComp,TDel> * 
ds::BinaryTreeBranch<TKey,TComp,TDel>::
getFirst(){
  
  BinaryTreeBranch<TKey,TComp,TDel> * current = this;
  
  while ( current->prev )
    current = (BinaryTreeBranch<TKey,TComp,TDel> *)current->prev;

  return current;
}

template< typename TKey, typename TComp, typename TDel >
ds::BinaryTreeBranch<TKey,TComp,TDel> *
ds::BinaryTreeBranch<TKey,TComp,TDel>::
getLast(){
  
  BinaryTreeBranch<TKey,TComp,TDel> * current = this;
  
  while ( current->next )
    current = (BinaryTreeBranch<TKey,TComp,TDel> *)current->next;

  return current;
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BinaryTreeBranch<TKey,TComp,TDel>::
breakLeft(){

  ds::BinaryTreeBranch<TKey,TComp,TDel> * current = this;
  ds::BinaryTreeBranch<TKey,TComp,TDel> * leftmost = current;
  ds::BinaryTreeBranch<TKey,TComp,TDel> * rightmost = current;
  ds::BinaryTreeBranch<TKey,TComp,TDel> * parent = this->parent;

  if ( parent == 0 )
    return;

  while ( leftmost->left )
    leftmost = leftmost->left;

  while ( rightmost->right )
    rightmost = rightmost->right;

  rightmost->next->prev = leftmost->prev;
  if ( leftmost->prev )
    leftmost->prev->next = rightmost->next;

  rightmost->next = 0;
  leftmost->prev = 0;
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BinaryTreeBranch<TKey,TComp,TDel>::
breakRight(){

  ds::BinaryTreeBranch<TKey,TComp,TDel> * current = this;
  ds::BinaryTreeBranch<TKey,TComp,TDel> * leftmost = current;
  ds::BinaryTreeBranch<TKey,TComp,TDel> * rightmost = current;
  ds::BinaryTreeBranch<TKey,TComp,TDel> * parent = this->parent;

  if ( parent == 0 )
    return;

  while ( leftmost->left )
    leftmost = leftmost->left;

  while ( rightmost->right )
    rightmost = rightmost->right;

  leftmost->prev->next = rightmost->next;
  if ( rightmost->next )
    rightmost->next->prev = leftmost->prev;

  rightmost->next = 0;
  leftmost->prev = 0;
}
  
template< typename TKey, typename TComp, typename TDel >
ds::BinaryTreeBranch<TKey,TComp,TDel> *
ds::BinaryTreeBranch<TKey,TComp,TDel>::
cutRight(){

  BinaryTreeBranch<TKey,TComp,TDel> * right = 0;

  if ( this->right == 0 )
    return 0;

  right = this->right;

  right->breakRight();
  
  this->right->parent = 0;
  this->right = 0;

  return right;
}

template< typename TKey, typename TComp, typename TDel >
ds::BinaryTreeBranch<TKey,TComp,TDel> *
ds::BinaryTreeBranch<TKey,TComp,TDel>::
cutLeft(){

  BinaryTreeBranch<TKey,TComp,TDel> * left = 0;

  if ( this->left == 0 )
    return 0;

  left = this->left;

  left->breakLeft();

  this->left->parent = 0;
  this->left = 0;
  

  return left;
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BinaryTreeBranch<TKey,TComp,TDel>::
concat(BinaryTreeBranch<TKey,TComp,TDel> *node){
  
  TComp comp;
  
  if ( node == NULL )
    return;
  
  if ( comp(node->key,this->key) == true ){

    if ( left != NULL )
      left->concat(node);
    else{

      left = node;
      node->parent = this;

      if ( this->prev ){

	this->prev->next = node->getFirst();
	this->prev->next->prev = this->prev;

      }

      this->prev = node->getLast();
      this->prev->next = this;
      
    }
  }
  else{

    if ( right != NULL )
      right->concat(node);
    else{

      right = node;
      node->parent = this;

      if ( this->next ){

	this->next->prev = node->getLast();
	this->next->prev->next = this->next;

      }

      this->next = node->getFirst();
      this->next->prev = this;
      
    }
  }
}

template< typename TKey, typename TComp, typename TDel >
ds::BinaryTreeBranch<TKey,TComp,TDel> *
ds::BinaryTreeBranch<TKey,TComp,TDel>::
getBranch(TKey key){
  
  TComp comp;
 
  if ( key == this->key )
    return this;
  
  else if ( comp(key,this->key) == true ){
    
    if ( this->left )
      return this->left->getBranch(key);
    else
      return 0;

  }
  else{
    
    if ( this->right )
      return this->right->getBranch(key);
    else
      return 0;
    
  }
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BinaryTreeBranch<TKey,TComp,TDel>::
show(){
  
  if ( left != NULL )
    left->show();
  
  std::cout << this->key << std::endl;
  
  if ( right != NULL )
    right->show();
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BTree<TKey,TComp,TDel>::
insert(TKey key){
  
  if ( count == 0 ){

    this->root = new BinaryTreeBranch<TKey,TComp,TDel>(key);
    this->first = this->root;

  }
  else{

    this->root->insert(key);

    if ( this->first->left != 0 )
      this->first = this->first->left;

  }
  
  count++;
}

template< typename TKey, typename TComp, typename TDel >
void
ds::BTree<TKey,TComp,TDel>::
erase(TKey key){
  
  BinaryTreeBranch<TKey,TComp,TDel> *node = 0;
  BinaryTreeBranch<TKey,TComp,TDel> *right = 0;
  BinaryTreeBranch<TKey,TComp,TDel> *left = 0;
  BinaryTreeBranch<TKey,TComp,TDel> *parent = 0;
  
  if ( this->root == NULL )
    return;
  
  node = this->root->getBranch(key);

  if ( node == NULL )
    return;
    
  if ( node == this->first )
    this->first = (BinaryTreeBranch<TKey,TComp,TDel> *)node->next;

  left = node->cutLeft();
  right = node->cutRight();
  parent = node->parent;
  
  if ( parent ){
    
    if ( parent->right == node )
      parent->cutRight();
    else
      parent->cutLeft();
  }

  if ( left ){

    if ( parent )
      parent->concat(left);
    
    else
      this->root = left;
    
    left->concat(right);

  }
  else if ( right ){

    if ( parent )
     parent->concat(right);

    else
      this->root = right;

  }
  else{

    if ( node->next )
      node->next->prev = node->prev;

    if ( node->prev )
      node->prev->next = node->next;
    
  }

  if ( this->first == NULL )
    this->root = NULL;

  delete node;
  count--;

}

template< typename TKey, typename TComp, typename TDel >
void
ds::BTree<TKey,TComp,TDel>::
show(){

  if ( this->root != NULL )
    this->root->show();
}

#endif
