/*
  Copyright (C) 1999-2008 Friedemann Zintel

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

#ifndef FZTOOLTEMPL_DATASTRUCTURES_HPP
#define FZTOOLTEMPL_DATASTRUCTURES_HPP

#include <utility>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <fztooltempl/exception.hpp>
#include <fztooltempl/utils.hpp>

/**
   @brief Contains templates for datastructures
   @ingroup datastructures
*/

namespace ds{

  /**
     Actually very similar to auto_pointer<T>, except that you can prevent auto-delettion later on.
     @brief for holding pointers
  */
  template<typename T>
  class MemPointer{
    
  private:
    T *ptr;
    bool clearflag;
    
  public:
    
    /**
       @param clear_on_exit if true, free() will be called for internal pointer on destruction of object
    */
    MemPointer( bool clear_on_exit = true ) : ptr((T *)0), clearflag(clear_on_exit) {}
    
    /**
       @brief Will allocate a clear memory-area of blksize-bytes
       @param blksize the size of memory to be allocated
       @param clear_on_exit if true, free() will be called for internal pointer on destruction of object
    */
    MemPointer(unsigned long blksize, bool clear_on_exit ) throw (exc::ExceptionBase);
    
    // attention: probably due to a bug in g++ (occured with V4.0.1) sometimes the ptr will be freed during copy-construction despite
    // setting clearflag to false!!!
    MemPointer(const MemPointer &mempointer) { this->ptr = mempointer.ptr; }
    
    /**
       If clear_on_exit was true, memory is freed
    */
    ~MemPointer() {
      clear(clearflag);
    }
    
    /**
       @brief get the memory-pointer
       @return memory-pointer
    */
    T *get() const { return ptr; }
    
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
	if ( ptr )
	  free(ptr);

      ptr = (T *)0;

    }
    
    MemPointer &operator=(const MemPointer & mempointer){ 

      this->ptr = mempointer.get();
      return *this;

    }
    
    MemPointer &operator=(const T * const mempointer){

      this->ptr = (T *)mempointer;
      return *this;

    }

    /**
       checks if contained pointers point to the same memory-address
       @param mempointer a mempointer to be compared to
       @return true, if both point to the same memory-address, false otherwise
    */
    bool operator==(const MemPointer & mempointer){ return this->get() == mempointer.get(); }
 
    /**
       checks if contained pointers point to a different memory-address
       @param mempointer a mempointer to be compared to
       @return true, if both point to a different memory-address, false otherwise
    */
    bool operator!=(const MemPointer & mempointer){ return this->get() != mempointer.get(); }

    /**
       returns the content of the contained pointer
       @return content of pointer
    */
    T operator*(){ return *this->get(); }

    /**
       returns the pointer itself
       @return the pointer
    */
    T* operator->(){ return this->get(); }

    /**
       @return true, if contained pointer is a null-pointer
    */
    bool operator!(){ return ( this->get() == (T *)0 ); }
    
  };

  /**
     @brief A stack with iterator-functionality
  */
  template<typename T>
  class Stack : public std::list<T>{
    
  public:
    
    Stack() : std::list<T>(){}
    
    /**
       @brief pushes the element on the stack
       @param elem element to be pushed
    */
    void push(T elem){ this->push_back(elem); }
    
    /**
       @brief pops the top-element from the stack and returns it
       @return top-most element
       @throw Exception< Stack<T> >
    */
    T pop() throw (exc::Exception< Stack<T> >);
	   
    /**
       @brief returns the top-most element
       @return top-most element
       @throw Exception< Stack<T> >
    */
    T top() throw (exc::Exception< Stack<T> >);
	   
    /**
       If stack is empty, end() will be returned
       @brief returns an iterator to the top-most element
       @return top-element-iterator
    */
    typename Stack::const_iterator topIter() const { return --this->end(); }

    /**
       For handling local stacks on top of the stack. Local emptyness relative to the
       given iterator (local bottom) means that the iterator-element is the top-most on stack, i. e.
       nop other elements are on top of it.
       @brief checks for local emptyness
       @param it the local bottom
       @return true if local empty
    */
    bool localEmpty(const typename std::list<T>::const_iterator & it) const { return ( it == topIter() ); }

    /**
       @brief prepends the top-most elements (upto iterator position) to a list
       @param l the list
       @param bottom iterator to local bottom
    */
    void popToListPrepend(std::list<T> & l, const typename Stack::const_iterator & bottom);

    /**
       @brief appends the top-most elements (upto iterator position) to a list
       @param l the list
       @param bottom iterator to local bottom
    */
    void popToListAppend(std::list<T> & l, const typename Stack::const_iterator & bottom);
	     
  };

  /**
     The buffer will consist of several blocks of user-defined size which will be allocated
     dynamically.
     @brief A dynamically growing buffer.
  */
  template<typename T>
  class DynamicBuffer{
    
  private:
    
    std::list< MemPointer<T> *> blocklist;
    unsigned long blksize;
    unsigned long offs;
    unsigned long blocks;
    unsigned long elements;
    
  private:
    
    MemPointer<T> * newBlock();

  public:
    
    // Konstruktor:
    /**
       @param blksize size of a block in the buffer
    */
    DynamicBuffer(unsigned long blksize) throw (exc::Exception< DynamicBuffer<T> >) : blksize(blksize), offs(0), blocks(0), elements(0){
      
      if ( !blksize )
	throw exc::Exception< DynamicBuffer<T> >("DynamicBuffer(): blksize is zero!");
      
    }
    
    // Destruktor:
    /**
       @exception ExceptionBase
    */
    ~DynamicBuffer() throw (exc::ExceptionBase);
    
    // Element in den Puffer schreiben
    /**
       @brief put a new object in the buffer
       @param c reference to the object
       @exception ExceptionBase
    */
    void put(const T &c) throw (exc::ExceptionBase);
    
    // Bloecke zu einem Block zusammenfuegen (NULL terminiert)
    /**
       @brief return a pointer to a continuous memory-block, filled with the merged buffer-blocks.
       @return the pointer for the memory
       @remark If the buffer is empty, zero will be returned. The block might be bigger than the filled area.
       No '\\0' will be added, so take care of this by yourself if you want
       to convert it into a string, e.g. use put(0). After merging the buffer will be cleared and reset to initial state.
       @exception Exception< DynamicBuffer<T> >
    */
    T *merge() throw (exc::ExceptionBase);

    /**
       @brief returns a certain element
       @return the element
    */
    T get(unsigned long number) throw (exc::Exception< ds::DynamicBuffer<T> >,exc::ExceptionBase);
    
    // Anzahl der Bloecke
    /**
       @brief Get the number of blocks in the buffer
       @return number of blocks
    */
    unsigned long getBlocks(){ return blocks; }
    
    // Anzahl der geschriebenen Elemente (Ohne NULL-Byte am Ende)
    /**
       @brief Get the total number of elements of type T in the buffer
       @return number of elements
    */
    unsigned long getElements() { return elements; }
    
    /**
       @brief Clears the buffer and resets all variables
    */
    void clear();
    
  };

  /**
     @brief For storing a fixed number of elements
     @since v2.1
  */
  template <typename T>
  class RingBuffer{

  protected:

    size_t size;

    size_t elements;

    unsigned int overwriting_mode;

    size_t first;

    size_t last;

    T *buffer;

  public:

    /**
       @brief In this mode an exception will be raised if a new element should be
       added to a full buffer. The buffer won't change.
    */
    static const unsigned int ALERT_OVERWRITING = 0;

    /**
       @brief In this mode no exception will be raised if a new element will be
       added to a full buffer. The first element of the buffer will be skipped.
    */
    static const unsigned int QUIET_OVERWRITING = 1;

  protected:

    void assertInBounds(size_t index) throw (exc::Exception<RingBuffer>);

    void next(size_t & reference);

    void forward(size_t & reference, size_t number);

    void reset();

    size_t getIndex(size_t position);

  public:

    /**
       
       @param size the size of the buffer i.e. number of elements the buffer can store simultaneously
    */
    RingBuffer(size_t size, unsigned int overwriting_mode = ALERT_OVERWRITING);

    ~RingBuffer();

    /**
       @brief stores the element at the end of the queue
       @param element the element to be stored
       @throw Exception<RingBuffer>
    */
    void enqueue(T element) throw (exc::Exception<RingBuffer>);

    /**
       @brief removes and returns the first element of the queue
       @return the first element
       @throw Exception<RingBuffer>
    */
    T & dequeue() throw (exc::Exception<RingBuffer>);

    /**
       @brief returns the element at a position of the buffer
       @param position the position of the element
       @return reference to the element at position
       @throw Exception<RingBuffer>
    */
    T & operator[](size_t position) throw (exc::Exception<RingBuffer>);

    /**
       @brief skips the first number elements of the buffer
       @param number the number of elements to be skipped
       @throw Exception<RingBuffer>
    */
    void skip(size_t number) throw (exc::Exception<RingBuffer>);

    /**
       @brief returns the number of stored elements
       @return number of elements
    */
    size_t getElements(){ return elements; }

    /**
       @brief checks if the buffer is empty
       @return true if buffer is empty
    */
    bool isEmpty(){ return getElements() == 0; }

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
  template< typename TKey, typename TLess, typename TDel >
  class KeyNode : public Node{

  public:
    TKey key;

    KeyNode(KeyNode<TKey,TLess,TDel> *next,KeyNode<TKey,TLess,TDel> *prev, TKey &key)
      : Node(next,prev) ,key(key){}

    ~KeyNode();
    
  };

  /**
     The template-parameters are
     - TKey:\n
     the class-type of the key
     - TLess:\n
     Less-Comparator: the class-type for a function-object (let's say less), containing the comparison-function. It must be
     a binary function, both parameters of type TKey, returning a bool. It has to return true on less(x,y) iff (!) x < y.
     - TDel:\n
     Deleter : the class-type for a function-object, containing the delete-function. It will be called on destructor-call
     and can delete the key (if it's a pointer or something more complicated). Usefull if the key should be deleted automatically on
     destruction of the BinaryTreeBranch. If the key should not be destructed just leave the function-body empty of the Deleter-class.
     @see nofree

     @brief A class for a branch of a binary-tree
     @see STL-documentation on the use of function-objects.
  **/
  template< typename TKey, typename TLess, typename TDel >
  class BinaryTreeBranch : public KeyNode<TKey,TLess,TDel>{

  public:

    typedef std::pair< BinaryTreeBranch<TKey,TLess,TDel> *, BinaryTreeBranch<TKey,TLess,TDel> *> Tail;

    BinaryTreeBranch<TKey,TLess,TDel> *left;
    BinaryTreeBranch<TKey,TLess,TDel> *right;
    BinaryTreeBranch<TKey,TLess,TDel> *parent;

    BinaryTreeBranch(TKey &key)
      : KeyNode<TKey,TLess,TDel>(0,0,key), left(0),right(0),parent(0){}

    BinaryTreeBranch(KeyNode<TKey,TLess,TDel> *next,KeyNode<TKey,TLess,TDel> *prev,
		     BinaryTreeBranch<TKey,TLess,TDel> *left,BinaryTreeBranch<TKey,TLess,TDel> *right,
		     BinaryTreeBranch<TKey,TLess,TDel> *parent,TKey &key)
      : KeyNode<TKey,TLess,TDel>(next,prev,key), left(left),right(right),parent(parent){}

    void insert(TKey key);
    
    BinaryTreeBranch<TKey,TLess,TDel> * getFirst();
    BinaryTreeBranch<TKey,TLess,TDel> * getLast();

    void concat(BinaryTreeBranch<TKey,TLess,TDel> *node);
    BinaryTreeBranch<TKey,TLess,TDel> * cutRight();
    BinaryTreeBranch<TKey,TLess,TDel> * cutLeft();
    void breakRight();
    void breakLeft();

    BinaryTreeBranch<TKey,TLess,TDel> * getBranch(TKey key);
        
    void show();
    
  };

  /**
     @brief A class for a branch of a multiple-tree
     @attention not yet supported!
     @todo supporting MultipleTreeBranch
  **/
  template< typename TKey, typename TLess, typename TDel >
  class MultipleTreeBranch : virtual KeyNode<TKey,TLess,TDel>{

  public:

    std::map<MultipleTreeBranch<TKey,TLess,TDel>,TKey,TLess> children;

    MultipleTreeBranch(KeyNode<TKey,TLess,TDel> *next,KeyNode<TKey,TLess,TDel> *prev, TKey &key)
      : KeyNode<TKey,TLess,TDel>(next,prev,key){}
  };

  /**
     @brief A class for holding keynodes. Useful for defining your own tree-structures
  */
  template< template<typename,typename,typename> class TKNode, typename TKey, typename TLess, typename TDel >
  class KeyNodeContainer{

  public:

    class iterator{

      friend class KeyNodeContainer;

    private :
      
      TKNode<TKey,TLess,TDel> *current;

      iterator(TKNode<TKey,TLess,TDel> * kn) : current(kn){}
      
    public:
      
      iterator() : current((TKNode<TKey,TLess,TDel> *)0){}
      
      void operator++();
      void operator--();
      TKey & operator*(){ return current->key; }
      bool operator==(KeyNodeContainer::iterator it){ return (this->current == it.current); }
      bool operator!=(KeyNodeContainer::iterator it){ return (this->current != it.current); }
      void operator=(KeyNodeContainer::iterator it){ this->current = it.current; }
      
    };
    
    TKNode<TKey,TLess,TDel> *root;
    TKNode<TKey,TLess,TDel> *first;

    KeyNodeContainer() : root(0),first(0){}
    
    ~KeyNodeContainer();

    iterator begin();
    iterator end();
  };
  
  /** 
      @example btree_example
  */
  /** 
      A template-class for binary trees
      @brief A binary Tree, which stores keys
      @see BinaryTreeBranch for information about the templateclass-parameters
  */
  template< typename TKey,typename TLess, typename TDel >
  class BTree : public KeyNodeContainer< BinaryTreeBranch,TKey,TLess,TDel >{

  public:

    unsigned long count;
    
    BTree() : KeyNodeContainer< BinaryTreeBranch,TKey,TLess,TDel >(),count(0){}

    void insert(TKey key);
    void erase(TKey key);
    bool empty(){ return ( count == 0 ); }

    void show();
    
  };

  template< typename TKey, typename TLess, typename TDel >
  class MTree : KeyNodeContainer< MultipleTreeBranch,TKey,TLess,TDel >{
  };

  /**
     @brief A Deletor-classtemplate which omits deletion of the key.
  */   
  template< typename T >
  class nofree{

  public:

    void operator()(T){}

  };


  /**
     @brief A bit-matrix
  **/
  class BitMatrix{

  private:

    unsigned long rows;
    unsigned long columns;

    MemPointer<char> bitmatrix;

  public:

    /**
       This will setup a bit-matrix of the given dimensions. counting of the elements starts from 0 to rows-1, resp. columns-1.
       @param rows number of rows
       @param columns number of columns
    **/
    BitMatrix(unsigned long rows, unsigned long columns) throw (exc::Exception<BitMatrix>) : rows(rows),columns(columns){

      if ( !(bitmatrix = (char *)::calloc(1,(size_t)((rows*columns)/sizeof(char)) + ((rows*columns)%sizeof(char)? 1 : 0))) )
	throw exc::Exception<BitMatrix>("calloc() failed!");

    }

    /**
       Sets the bit at the given position. counting starts at 0 in each dimension
       @param row the row-position
       @param column the column-position
    **/
    void setBit(unsigned long row, unsigned long column) throw (exc::Exception<BitMatrix>){
      
      if ( row < 0 || row >= rows  || column < 0 || column >= columns )
	throw exc::Exception<BitMatrix>("OutOfBounds");

      bitmatrix.get()[(unsigned long)((row*columns+column)/sizeof(char))] |= (1L << (row*columns+column)%sizeof(char));
      
    }      
    
    /**
       Clears the bit at the given position. counting starts at 0 in each dimension
       @param row the row-position
       @param column the column-position
    **/
    void clearBit(unsigned long row, unsigned long column) throw (exc::Exception<BitMatrix>){
      
      if ( row < 0 || row >= rows  || column < 0 || column >= columns )
	throw exc::Exception<BitMatrix>("OutOfBounds");

      bitmatrix.get()[(unsigned long)((row*columns+column)/sizeof(char))] &= ~(1L << (row*columns+column)%sizeof(char));
      
    }   
    
    /**
       ClearsReturns the bit at the given position. counting starts at 0 in each dimension
       @param row the row-position
       @param column the column-position
       @return 0 or 1 if bit is unset resp. set
    **/
    char getBit(unsigned long row, unsigned long column) throw (exc::Exception<BitMatrix>){
      
      if ( row < 0 || row >= rows  || column < 0 || column >= columns )
	throw exc::Exception<BitMatrix>("OutOfBounds");

      return (char)(bitmatrix.get()[(unsigned long)((row*columns+column)/sizeof(char))] & (1L << (row*columns+column)%sizeof(char)));
      
    }  

    /**
       @brief string-representation
       @return the string
       @since v1.98
    */
    std::string toString(){

      std::ostringstream sstream;

      for ( unsigned long row = 0; row < rows; row++ ){
	for ( unsigned long column = 0; column < columns; column++ )
	  sstream << (int)getBit(row,column);
	sstream << std::endl;
      }

      return sstream.str();

    }

    /**
       @brief shows the bitmatrix on standart-output
       @since v1.98
    */
    void show(){ std::cout << toString() << std::flush; }

  };
  
}

template< typename TKey, typename TLess, typename TDel >
ds::KeyNode<TKey,TLess,TDel>::
~KeyNode(){

  TDel del;

  del(key);
}
  
template< template<typename,typename,typename> class TKNode,typename TKey, typename TLess, typename TDel >
ds::KeyNodeContainer<TKNode,TKey,TLess,TDel>::
~KeyNodeContainer<TKNode,TKey,TLess,TDel>(){

  TKNode<TKey,TLess,TDel> *curr = this->first;
  TKNode<TKey,TLess,TDel> *next = 0;

  while ( curr ){
    
    next = (TKNode<TKey,TLess,TDel> *)curr->next;
    delete curr;
    curr = next;
  }
}
  
template< template<typename,typename,typename> class TKNode,typename TKey, typename TLess, typename TDel >
typename ds::KeyNodeContainer<TKNode,TKey,TLess,TDel>::iterator
ds::KeyNodeContainer<TKNode,TKey,TLess,TDel >::
begin(){
    
  typename KeyNodeContainer<TKNode,TKey,TLess,TDel>::iterator it(first);
  return it;
}
  
template< template<typename,typename,typename> class TKNode,typename TKey, typename TLess, typename TDel >
typename ds::KeyNodeContainer<TKNode,TKey,TLess,TDel>::iterator
ds::KeyNodeContainer<TKNode,TKey,TLess,TDel >::
end(){
  
  typename KeyNodeContainer<TKNode,TKey,TLess,TDel>::iterator it;
  return it;
}

template< template<typename,typename,typename> class TKNode,typename TKey, typename TLess, typename TDel >
void
ds::KeyNodeContainer<TKNode,TKey,TLess,TDel>::iterator::
operator++(){

  if ( current != 0 )
    current = (TKNode<TKey,TLess,TDel> *)current->next;
}

template< template<typename,typename,typename> class TKNode,typename TKey, typename TLess, typename TDel >
void
ds::KeyNodeContainer<TKNode,TKey,TLess,TDel>::iterator::
operator--(){

  if ( current != 0 )
    current = (TKNode<TKey,TLess,TDel> *)current->prev;
}

template<typename T>
ds::MemPointer<T>::
MemPointer(unsigned long blksize,bool clear_on_exit ) throw (exc::ExceptionBase) : clearflag(clear_on_exit){

  if ( blksize == 0 )
    throw exc::Exception< ds::MemPointer<T> >("MemPointer(): blksize is zero!");
  
  if ( !(ptr = (T *)calloc(blksize,sizeof(T))) )
    throw exc::Exception< ds::MemPointer<T> >("MemPointer(): calloc() failed!");
}

template<typename T>
T
ds::Stack<T>::
top() throw (exc::Exception<Stack<T> >){
  
  if ( this->empty() )
    throw exc::Exception< Stack<T> >("pop() failed: Stack is empty!");
  
  return this->back();
  
}

template<typename T>
T
ds::Stack<T>::
pop() throw (exc::Exception< Stack<T> >) {
  
  if ( this->empty() )
    throw exc::Exception< Stack<T> >("pop() failed: Stack is empty!");
  
  T last = this->back();
  this->pop_back();
  return last; 
  
}

template<typename T>
void
ds::Stack<T>::
popToListPrepend(std::list<T> & l, const typename Stack::const_iterator & bottom){
  
  for ( typename Stack::const_iterator it = topIter(); it != bottom; it = topIter() )
    l.push_front(pop());
  
}

template<typename T>
void
ds::Stack<T>::
popToListAppend(std::list<T> & l, const typename Stack::const_iterator & bottom){
  
  for ( typename Stack::const_iterator it = topIter(); it != bottom; it = topIter() )
    l.push_back(pop());
  
}

template<typename T>
ds::DynamicBuffer<T>::
~DynamicBuffer() throw (exc::ExceptionBase){
  clear();
}

template<typename T>
ds::MemPointer<T> *
ds::DynamicBuffer<T>::
newBlock(){
      
  ds::MemPointer<T> *block;
  
  block = new ds::MemPointer<T>(blksize,true);
  blocklist.push_back(block);
  blocks++;
  
  return block;

}

template<typename T>
void
ds::DynamicBuffer<T>::
put(const T &c) throw (exc::ExceptionBase){
  
  ds::MemPointer<T> *curr;
  
  /* bis zum letzten Block vorarbeiten */
  if ( !blocklist.empty() ){
    
    /* eventuell neuen Block allokieren */
    if ( offs == blksize ){

      curr = newBlock();
      offs=0;
      
    } else
      curr = blocklist.back();

  } else
    curr = newBlock();
  
  /* insert element */
  curr->get()[offs]=c;
  elements++;
  
  offs++;
}

template< typename T >
T
ds::DynamicBuffer<T>::
get(unsigned long number) throw (exc::Exception< ds::DynamicBuffer<T> >,exc::ExceptionBase){

  unsigned long blocknumber;
  unsigned long position;

  if ( number < 1 || number > getElements() )
    throw exc::Exception< ds::DynamicBuffer<T> >("get(): index out of bounds!");

  blocknumber = ((unsigned long)(number/blksize)) + 1;
  position = (number-1)%blksize;
  
  typename std::list< ds::MemPointer<T> >::iterator it = blocklist.begin();

  for ( unsigned long i = 0; i < blocknumber; it++, i++ )
    if ( it == blocklist.end() )
      break;

  if ( it == blocklist.end() )
    throw exc::Exception< ds::DynamicBuffer<T> >("get(): internal error: blocklist at end!");

  return (*it)->get()[position];

}

template<typename T>
T *
ds::DynamicBuffer<T>::
merge() throw (exc::ExceptionBase){
  
  unsigned long pos=0, full_blocks;
  T *block;
  
  if ( blocklist.empty() == true )
    return 0;
  
  full_blocks = (unsigned long)((getElements()-1)/blksize);
  
  if ( blocks > 1 ){
    
    if ( !(block = (T *)calloc(getElements(),sizeof(T))) )
      throw exc::Exception< ds::DynamicBuffer<T> >("merge(): calloc failed!");
    
    unsigned long blocknumber = 0;

    for ( typename std::list< ds::MemPointer<T> *>::iterator it = blocklist.begin(); it != blocklist.end(); it++, blocknumber++ ){
      
      if ( !(*it)->get() )
	throw exc::Exception< ds::DynamicBuffer<T> >("merge(): ptr == NULL!");
      
      if ( blocknumber < full_blocks ){
	
	memcpy(&block[pos],(*it)->get(),blksize*sizeof(T));
	pos += blksize*sizeof(T);
	
      } else{
	
	memcpy(&block[pos],(*it)->get(),offs);
	pos += offs;

      }	

    }
    
  } else{
    // only one block is filled
    // we can safely return the already filled memory-area
    
    block = blocklist.front()->get();
    
    // make sure that the memory will not be erased twice
    blocklist.front()->setClearflag(false);;
    
  }
  
  clear();
  
  return block;
  
}

template< typename T >
void
ds::DynamicBuffer<T>::
clear(){

  for ( typename std::list< ds::MemPointer<T> *>::iterator it = blocklist.begin(); it != blocklist.end(); it++ )
    delete (*it);

  blocklist.clear();
  
  offs = elements = blocks = 0;

}

template <typename T>
ds::RingBuffer<T>::
RingBuffer(size_t size, unsigned int overwriting_mode) : size(size), elements(0), overwriting_mode(overwriting_mode){

  buffer = new T[size];
  reset();

}

template <typename T>
ds::RingBuffer<T>::
~RingBuffer(){

  delete [] buffer;

}

template <typename T>
void
ds::RingBuffer<T>::
assertInBounds(size_t index) throw (exc::Exception<RingBuffer>){

  std::string thisMethod = "assertInBounds()";

  if ( index >= elements || index < 0 ){

    std::ostringstream stream;
    stream << thisMethod << ": object " << this << ": index " << index << " out of bounds!";
    throw exc::Exception<RingBuffer>(stream.str());

  }

}

template <typename T>
void
ds::RingBuffer<T>::
next(size_t & reference){

  if ( reference == size - 1 )
    reference = 0;

  else
    reference++;

}

template <typename T>
void
ds::RingBuffer<T>::
forward(size_t & reference, size_t number){

  if ( (size - 1) - number >= reference )
    reference += number;

  else
    reference = (number - (size - reference));

}

template <typename T>
void
ds::RingBuffer<T>::
reset(){

  first = 0;
  last = first - 1;

}

template <typename T>
size_t
ds::RingBuffer<T>::
getIndex(size_t position){

  if ( (size - 1) - position >= first )
    return first + position;
  else
    return position - (size - first);
 
}

template <typename T>
void
ds::RingBuffer<T>::
enqueue(T element) throw (exc::Exception<RingBuffer>){

  std::string thisMethod = "enqueue()";

  if ( elements < size )
    elements++;

  else {
    
    if ( overwriting_mode != QUIET_OVERWRITING )
      throw exc::Exception<RingBuffer>(thisMethod + " buffer is full!");

    next(first);

  }

  next(last);

  buffer[last] = element;

}

template <typename T>
T &
ds::RingBuffer<T>::
dequeue() throw (exc::Exception<RingBuffer>){

  std::string thisMethod = "dequeue()";

  if ( isEmpty() == true )
    throw exc::Exception<RingBuffer>(thisMethod + ": buffer is empty!");

  T & element = buffer[first];

  elements--;

  if ( isEmpty() == true )
    reset();
  else
    next(first);

  return element;

}

template <typename T>
T &
ds::RingBuffer<T>::
operator[](size_t position) throw (exc::Exception<RingBuffer>){

  assertInBounds(position);

  return buffer[getIndex(position)];

}

template <typename T>
void
ds::RingBuffer<T>::
skip(size_t number) throw (exc::Exception<RingBuffer>){

  if ( number == 0 )
    return;

  assertInBounds(number-1);

  if ( number == elements )
    reset();
  
  else
    forward(first,number);
  
  elements -= number;

}

template< typename TKey, typename TLess, typename TDel >
void
ds::BinaryTreeBranch<TKey,TLess,TDel>::
insert(TKey key){
  
  TLess less;
  
  if ( less(key,this->key) == true ){
    if ( left != NULL )
      left->insert(key);
    else{

      left = new ds::BinaryTreeBranch<TKey,TLess,TDel>(key);

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

      right = new ds::BinaryTreeBranch<TKey,TLess,TDel>(key);

      right->parent = this;

      right->next = this->next;
      right->prev = this;

      if ( this->next )
	this->next->prev = right;
      
      this->next = right;
    }
  }
}

template< typename TKey, typename TLess, typename TDel >
ds::BinaryTreeBranch<TKey,TLess,TDel> * 
ds::BinaryTreeBranch<TKey,TLess,TDel>::
getFirst(){
  
  BinaryTreeBranch<TKey,TLess,TDel> * current = this;
  
  while ( current->prev )
    current = (BinaryTreeBranch<TKey,TLess,TDel> *)current->prev;

  return current;
}

template< typename TKey, typename TLess, typename TDel >
ds::BinaryTreeBranch<TKey,TLess,TDel> *
ds::BinaryTreeBranch<TKey,TLess,TDel>::
getLast(){
  
  BinaryTreeBranch<TKey,TLess,TDel> * current = this;
  
  while ( current->next )
    current = (BinaryTreeBranch<TKey,TLess,TDel> *)current->next;

  return current;
}

template< typename TKey, typename TLess, typename TDel >
void
ds::BinaryTreeBranch<TKey,TLess,TDel>::
breakLeft(){

  ds::BinaryTreeBranch<TKey,TLess,TDel> * current = this;
  ds::BinaryTreeBranch<TKey,TLess,TDel> * leftmost = current;
  ds::BinaryTreeBranch<TKey,TLess,TDel> * rightmost = current;
  ds::BinaryTreeBranch<TKey,TLess,TDel> * parent = this->parent;

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

template< typename TKey, typename TLess, typename TDel >
void
ds::BinaryTreeBranch<TKey,TLess,TDel>::
breakRight(){

  ds::BinaryTreeBranch<TKey,TLess,TDel> * current = this;
  ds::BinaryTreeBranch<TKey,TLess,TDel> * leftmost = current;
  ds::BinaryTreeBranch<TKey,TLess,TDel> * rightmost = current;
  ds::BinaryTreeBranch<TKey,TLess,TDel> * parent = this->parent;

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
  
template< typename TKey, typename TLess, typename TDel >
ds::BinaryTreeBranch<TKey,TLess,TDel> *
ds::BinaryTreeBranch<TKey,TLess,TDel>::
cutRight(){

  BinaryTreeBranch<TKey,TLess,TDel> * right = 0;

  if ( this->right == 0 )
    return 0;

  right = this->right;

  right->breakRight();
  
  this->right->parent = 0;
  this->right = 0;

  return right;
}

template< typename TKey, typename TLess, typename TDel >
ds::BinaryTreeBranch<TKey,TLess,TDel> *
ds::BinaryTreeBranch<TKey,TLess,TDel>::
cutLeft(){

  BinaryTreeBranch<TKey,TLess,TDel> * left = 0;

  if ( this->left == 0 )
    return 0;

  left = this->left;

  left->breakLeft();

  this->left->parent = 0;
  this->left = 0;
  

  return left;
}

template< typename TKey, typename TLess, typename TDel >
void
ds::BinaryTreeBranch<TKey,TLess,TDel>::
concat(BinaryTreeBranch<TKey,TLess,TDel> *node){
  
  TLess less;
  
  if ( node == NULL )
    return;
  
  if ( less(node->key,this->key) == true ){

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

template< typename TKey, typename TLess, typename TDel >
ds::BinaryTreeBranch<TKey,TLess,TDel> *
ds::BinaryTreeBranch<TKey,TLess,TDel>::
getBranch(TKey key){
  
  TLess less;

  // check for equality 
  if ( less(key,this->key) == false && less(this->key,key) == false )
    return this;
  
  else if ( less(key,this->key) == true ){
    
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

template< typename TKey, typename TLess, typename TDel >
void
ds::BinaryTreeBranch<TKey,TLess,TDel>::
show(){
  
  if ( left != NULL )
    left->show();
  
  std::cout << this->key << std::endl;
  
  if ( right != NULL )
    right->show();
}

template< typename TKey, typename TLess, typename TDel >
void
ds::BTree<TKey,TLess,TDel>::
insert(TKey key){
  
  if ( count == 0 ){

    this->root = new BinaryTreeBranch<TKey,TLess,TDel>(key);
    this->first = this->root;

  }
  else{

    this->root->insert(key);

    if ( this->first->left != 0 )
      this->first = this->first->left;

  }
  
  count++;
}

template< typename TKey, typename TLess, typename TDel >
void
ds::BTree<TKey,TLess,TDel>::
erase(TKey key){
  
  BinaryTreeBranch<TKey,TLess,TDel> *node = 0;
  BinaryTreeBranch<TKey,TLess,TDel> *right = 0;
  BinaryTreeBranch<TKey,TLess,TDel> *left = 0;
  BinaryTreeBranch<TKey,TLess,TDel> *parent = 0;
  
  if ( this->root == NULL )
    return;
  
  node = this->root->getBranch(key);

  if ( node == NULL )
    return;
    
  if ( node == this->first )
    this->first = (BinaryTreeBranch<TKey,TLess,TDel> *)node->next;

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

template< typename TKey, typename TLess, typename TDel >
void
ds::BTree<TKey,TLess,TDel>::
show(){

  if ( this->root != NULL )
    this->root->show();
}

#endif
