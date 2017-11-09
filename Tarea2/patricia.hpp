#ifndef _PATRICIA_INCLUDED_
#define _PATRICIA_INCLUDED_

#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "dictionary.hpp"

using namespace std;

class Ptrie : public Dictionary {
  bool isLeaf;
  string label;
  vector<int> values;
  vector<Ptrie *> children;
  pair<pair<int, Ptrie *>, pair<string::iterator, string::iterator>>
      search_it(string::iterator, string::iterator);

public:
  Ptrie(bool, string, vector<int>, vector<Ptrie *>);
  Ptrie(string, int);
  Ptrie();
  vector<int> search(string);
  void insert(string, int);
  string getName() {return "patricia_";};
};

#endif
