#ifndef _PATRICIA_INCLUDED_
#define _PATRICIA_INCLUDED_


#include <cstring>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "dictionary.hpp"

using namespace std;

class Patricia : public Dictionary {
  bool isLeaf;
  string::iterator pointer;
  int length;
  vector<Patricia *> children;
  tuple<int, Patricia *, string::iterator> search_it(string::iterator);
  int compare(string::iterator, string::iterator, int);

public:
  Patricia(bool, string::iterator, int, vector<Patricia *>);
  Patricia(string, int, int);
  Patricia(int);
  Patricia();
  vector<int> search(string, int);
  void insert(string, int, int);
  string getName() {return "patricia_";};
  int similarity();
};

#endif
