#ifndef _PATRICIA_INCLUDED_
#define _PATRICIA_INCLUDED_

#include "dictionary.hpp"
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Patricia : public Dictionary {
  bool isLeaf;
  string::iterator pointer;
  int length;
  string *word;
  vector<Patricia *> children;
  void expand_positions(string *, int);
  tuple<int, Patricia *, string::iterator> search_it(string::iterator);
  int compare(string::iterator, string::iterator, int);

public:
  Patricia(bool, string::iterator, int, string *, vector<Patricia *>);
  Patricia(string, int, int, int);
  Patricia(int);
  Patricia();
  ~Patricia();
  vector<int> search(string, int);
  void insert(string, int, int);
  string getName() { return "patricia_"; };
  int similarity();
};

#endif
