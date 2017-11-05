#ifndef _PATRICIA_INCLUDED_
#define _PATRICIA_INCLUDED_

#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

struct Ptrie {
  int isLeaf;
  string label;
  vector<Ptrie *> children;
};

pair<pair<int, Ptrie *>, pair<string::iterator, string::iterator>> search_it(Ptrie *node, string::iterator it_key, string::iterator end);

bool search(Ptrie *root, string key);

void insert(Ptrie *root, string key);

#endif