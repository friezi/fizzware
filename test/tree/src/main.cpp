#include "main.hpp"

using namespace std;
using namespace exc;
using namespace ds;

typedef nofree<int> nfi;
typedef pair<int,PrimList *> PrimKey;

// some Deleters, which don't delete anything
class nodel{

public:

  void operator()(int){}
};

class nodels{

public:

  void operator()(string){}
};

// some Less-Comparators
class mod2{

public:

  bool operator()(int x, int y){

    return ( (x%2) < (y%2) );
  }
};

// compare number of elements
class plless{

public:

  bool operator()(PrimKey p1, PrimKey p2){

    return ( p1.second->elements() < p2.second->elements() );
  }
};

// One Deleter which deletes the key
class plfree{

public:

  void operator()(PrimKey p){

    delete p.second;
    cout << "Primlist " << p.first << " deleted\n";
  }
};

// a tree consisting of PrimKeys
typedef BTree< PrimKey,plless,plfree > PrimTree;

int main(int argc, char **argv){

  // some trees
  BTree< string,less<string>, nodels > tree;
  BTree<int,mod2,nodel> modtree;
  BTree< int,less<int>,nfi > lesstree;

  try{

    PrimTree pltree;

    lesstree.insert(3);
    lesstree.insert(2);

    for ( BTree<int,less<int>,nfi>::iterator it = lesstree.begin(); it != lesstree.end(); ++it )
      cout << "it:  key: " << (*it) << endl;

    lesstree.show();
    cout << endl;

    modtree.insert(5);
    modtree.insert(7);
    modtree.insert(6);
    modtree.insert(8);
    modtree.insert(4);
    modtree.insert(3);

    modtree.show();

    for ( BTree<int,mod2,nodel>::iterator it = modtree.begin(); it != modtree.end(); ++it )
      cout << "it:  key: " << (*it) << endl;

    modtree.erase(5);
    modtree.insert(2);
    modtree.show();
    for ( BTree<int,mod2,nodel>::iterator it = modtree.begin(); it != modtree.end(); ++it )
      cout << "it:  key: " << (*it) << endl;


    tree.insert("ich");
    tree.insert("ach");
    tree.insert("aah");
    tree.insert("huch");
    tree.insert("uch");
    tree.show();

    tree.erase("ich");

    tree.show();

    PrimList *pl15 = new PrimList(15);
    PrimList *pl4 = new PrimList(4);

    pltree.insert(PrimKey(5,new PrimList(5)));
    pltree.insert(PrimKey(30,new PrimList(30)));
    pltree.insert(PrimKey(1,new PrimList(1)));
    pltree.insert(PrimKey(0,new PrimList(0)));
    pltree.insert(PrimKey(-1,new PrimList(-1)));
    pltree.insert(PrimKey(4,pl4));
    pltree.insert(PrimKey(150,new PrimList(150)));
    pltree.insert(PrimKey(15,pl15));

    // iterate over the elements in the tree
    for ( PrimTree::iterator it = pltree.begin(); it != pltree.end(); ++it ){
      cout << "limit: " << (*it).first << "\tprimenumbers: ";
      for ( long i = 1; i <= (*it).second->elements(); i++ )
	cout << (*it).second->get(i) << " ";
      cout << endl;
    }

//     pltree.erase(PrimKey(4,pl4));
    pltree.erase(PrimKey(15,pl15));
  }
  catch (ExceptionBase &e){
    e.show();
  }
  
  
  return 0;
}
