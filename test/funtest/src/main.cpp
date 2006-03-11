#include "main.hpp"

using namespace std;

bool f(string s){

  cout << s << "\n";
  return true;
}

bool sz(string s){

  return (s.size() == 6);
}

int add2(int i){
  return i+2;
}

void sim_add2(int i){
  cout << i+2 << endl;
}

string rev(string s){

  string ns;

  for (string::reverse_iterator it = s.rbegin(); it != s.rend(); it++)
    ns += *it;
  return ns;

}

bool even(int x){

  return (x%2 == 0);
}

class Jip{};

class Sib : SubException<TestDieZweite,Jip>{};

int main(int agc, char *argv[]){

  list<string> l,m;

  Exception<TestDieZweite> e;

  l.push_back("Friede");
  l.push_back("Mann");
  l.push_back("123456");
  
  cout << "list:\n";
  list<string>::iterator i = l.begin();
  while ( i != l.end())
    cout << *i++ << "\n";
  cout << endl;
 
  cout << "list:map_same()\n";
  fun::map_same(rev,l);
  
  for (list<string>::iterator begin = l.begin(); begin != l.end(); begin++)
    cout << *begin << "\n";
  
  cout << "list:filter_same()\n";
  fun::filter_same(sz,l);
  
  for (list<string>::iterator begin = l.begin(); begin != l.end(); begin++)
    cout << *begin << "\n";
  cout << endl;
  
  vector<int> v;
  vector<int> *w;

  for (int i = 0; i < 10; i++)
    v.push_back(i);

  cout << "vector:\n";
  for (vector<int>::iterator begin = v.begin(); begin != v.end(); begin++)
    cout << *begin << "\n";
  cout << endl;

//    mapit(f,l);
  cout << "vector:map_new()\n";
  w = fun::map_new(add2,v);

  for (vector<int>::iterator begin = w->begin(); begin != w->end(); begin++)
    cout << *begin << "\n";
  cout << endl;

  delete w;
  cout << "\n";

//    w = fun::filter_new(even,v);

  cout << "vector:filter_same()\n";
  fun::filter_same(even,v);

  for (vector<int>::iterator begin = v.begin(); begin != v.end(); begin++)
    cout << *begin << endl;
  cout << endl;

  cout << "Set:\n";
  Set<int> menge;

  menge.insert(1);
  menge.insert(2);
  menge.insert(3);
  for (Set<int>::iterator begin = menge.begin(); begin != menge.end(); begin++)
    cout << *begin << "\n";
  cout << endl;
  
  cout << "Set: map_unchg()\n";
  fun::map_unchg(sim_add2,menge);
  cout << endl;
  
  set<int,less<int> > *menge2;
  
  menge2 = (set<int,less<int> > *)fun::map_new(add2,menge);
  cout << "Set: map_new()\n";
  for (set<int,less<int> >::iterator begin = menge2->begin(); begin != menge2->end(); begin++)
    cout << *begin << "\n";
  cout << endl;
  
  delete menge2;

  IntSet menge3;

  cout << "IntSet:\n";
  menge3.insert(5);
  menge3.insert(6);
  menge3.insert(7);
  for (IntSet::iterator begin = menge3.begin(); begin != menge3.end(); begin++)
    cout << *begin << "\n";
  cout << endl;
  
  
  cout << "IntSet: map_unchg()\n";
  fun::map_unchg(sim_add2,menge3);
  cout << endl;

  cout << "IntSet:filter_same()\n";

  fun::filter_same(even,menge3);
  
  for (IntSet::iterator begin = menge3.begin(); begin != menge3.end(); begin++)
    cout << *begin << "\n";
  cout << endl;

  SubException<TestDieZweite,TestDieZweite> se;
  SubException<Jip,Jip> je;
  SubException<int,double> ide;
  SubException<string,char> sce;
  SubException<bool,unsigned int> buie;
  SubException<bool,int> bie;
  Sib sib;
  Jip *jip = (Jip *)(&sib);
  Exception<Jip> *ejip = (Exception<Jip> *)(&sib);

  cout << typeid(e).name() << "\n";
  cout << typeid(se).name() << "\n";
  cout << typeid(je).name() << "\n";
  cout << typeid(ide).name() << "\n";
  cout << typeid(sce).name() << "\n";
  cout << typeid(buie).name() << "\n";
  cout << typeid(bie).name() << "\n";
  cout << typeid(sib).name() << "\n";
  cout << typeid(jip).name() << "\n";
  cout << typeid(ejip).name() << "\n";
  cout << typeid(TestDieZweite).name() << "\n";
  
  return 0;

}
