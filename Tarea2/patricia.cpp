// patricia.cpp
#include "patricia.hpp"

Ptrie::Ptrie(bool isLeaf, string label, vector<int> values,
             vector<Ptrie *> children)
    : isLeaf(isLeaf), label(label), values(values), children(children) {}
Ptrie::Ptrie(string label, int value) : isLeaf(true), label(label) {
  values.push_back(value);
}
Ptrie::Ptrie() : isLeaf(true) {}

pair<pair<int, Ptrie *>, pair<string::iterator, string::iterator>>
Ptrie::search_it(string::iterator it_key, string::iterator end) {

  // iterate over children
  for (vector<Ptrie *>::iterator it = this->children.begin();
       it != this->children.end(); ++it) {

    // compare label with key
    pair<string::iterator, string::iterator> compare =
        mismatch((*it)->label.begin(), (*it)->label.end(), it_key);

    // if label is prefix of key
    if (compare.first == (*it)->label.end()) {
      // if key reach end
      if (compare.second == end && (*it)->isLeaf)
        return make_pair(make_pair(1, *it), make_pair(it_key, it_key));
      else
        return (*it)->search_it(compare.second, end);
    }
    // key ended on label
    if (compare.first != (*it)->label.begin())
      return make_pair(make_pair(2, *it), compare);
  }
  // only stop on leaf at the beggining due to implementation
  if (this->isLeaf)
    return make_pair(make_pair(3, this), make_pair(it_key, it_key));
  // stopped on node
  else
    return make_pair(make_pair(4, this), make_pair(it_key, it_key));
}

vector<int> Ptrie::search(string key) {
  key += "$";
  pair<int, Ptrie *> sch_it = this->search_it(key.begin(), key.end()).first;
  vector<int> res;
  if (sch_it.first == 1)
    res = sch_it.second->values;
  return res;
}

void Ptrie::insert(string key, int value) {
  key += "$";
  pair<pair<int, Ptrie *>, pair<string::iterator, string::iterator>> sch =
      this->search_it(key.begin(), key.end());

  // key founded
  if (sch.first.first == 1) {
    sch.first.second->values.push_back(value);
  }
  // key ended on label
  else if (sch.first.first == 2) {
    Ptrie *node =
        new Ptrie(sch.first.second->isLeaf,
                  string(sch.second.first, sch.first.second->label.end()),
                  sch.first.second->values, sch.first.second->children);

    Ptrie *leaf = new Ptrie(string(sch.second.second, key.end()), value);

    sch.first.second->isLeaf = false;
    sch.first.second->label =
        string(sch.first.second->label.begin(), sch.second.first);
    sch.first.second->values.clear();
    sch.first.second->children.clear();
    if (node->label.compare(leaf->label) < 0) {
      sch.first.second->children.push_back(node);
      sch.first.second->children.push_back(leaf);
    } else {
      sch.first.second->children.push_back(leaf);
      sch.first.second->children.push_back(node);
    }

  }
  // stopped on leaf
  else if (sch.first.first == 3) {
    Ptrie *node = new Ptrie(key, value);

    sch.first.second->isLeaf = false;
    sch.first.second->children.push_back(node);

  }
  // stopped on node
  else if (sch.first.first == 4) {
    // iterate over children
    for (vector<Ptrie *>::iterator it = sch.first.second->children.begin();
         it != sch.first.second->children.end(); ++it) {
      // if label > key
      if (!lexicographical_compare(((*it)->label).begin(), ((*it)->label).end(),
                                   sch.second.second, key.end())) {
        Ptrie *ins = new Ptrie(string(sch.second.second, key.end()), value);
        // insert new child before current iterator
        sch.first.second->children.insert(it, ins);
        break;
      }
    }
  }
}
/*
int main(int argc, char const *argv[]) {

  Ptrie *test_root = new Ptrie();

  test_root->insert("romane", 1);
  test_root->insert("romanus", 2);
  test_root->insert("romulus", 3);
  test_root->insert("rubens", 4);
  test_root->insert("ruber", 5);
  test_root->insert("rubicon", 6);
  test_root->insert("rubicundus", 7);
  test_root->insert("ruber", 8);
  test_root->insert("romanus", 9);
  test_root->insert("rubicundus", 10);

  cout << test_root->search("rubero").size() << endl;
  return 0;
}
*/
