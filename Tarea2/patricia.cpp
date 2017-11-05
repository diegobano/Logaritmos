// patricia.cpp
#include "patricia.hpp"

pair<pair<int, Ptrie *>, pair<string::iterator, string::iterator>>
search_it(Ptrie *node, string::iterator it_key, string::iterator end) {

  // iterate over children
  for (vector<Ptrie *>::iterator it = node->children.begin();
       it != node->children.end(); ++it) {

    // compare label with key
    pair<string::iterator, string::iterator> compare =
        mismatch((*it)->label.begin(), (*it)->label.end(), it_key);

    // if label is prefix of key
    if (compare.first == (*it)->label.end()) {
      // if key reach end
      if (compare.second == end && (*it)->isLeaf)
        return make_pair(make_pair(1, node), make_pair(it_key, it_key));
      else
        return search_it(*it, compare.second, end);
    }
    // key ended on label
    if (compare.first != (*it)->label.begin())
      return make_pair(make_pair(2, *it), compare);
  }
  // only stop on leaf at the beggining due to implementation
  if (node->isLeaf)
    return make_pair(make_pair(3, node), make_pair(it_key, it_key));
  // stopped on node
  else
    return make_pair(make_pair(4, node), make_pair(it_key, it_key));
}

bool search(Ptrie *root, string key) {
  key += "$";
  return search_it(root, key.begin(), key.end()).first.first == 1;
}

void insert(Ptrie *root, string key) {
  key += "$";
  pair<pair<int, Ptrie *>, pair<string::iterator, string::iterator>> sch =
      search_it(root, key.begin(), key.end());
  // key ended on label
  if (sch.first.first == 2) {
    Ptrie *node = new (Ptrie), *leaf = new (Ptrie);

    node->isLeaf = sch.first.second->isLeaf;
    node->label = string(sch.second.first, sch.first.second->label.end());
    node->children = sch.first.second->children;

    leaf->isLeaf = true;
    leaf->label = string(sch.second.second, key.end());

    sch.first.second->isLeaf = false;
    sch.first.second->label =
        string(sch.first.second->label.begin(), sch.second.first);
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
    Ptrie *node = new (Ptrie);

    node->isLeaf = true;
    node->label = key;

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
        Ptrie *ins = new (Ptrie);
        ins->isLeaf = true;
        ins->label = string(sch.second.second, key.end());
        // insert new child before current iterator
        sch.first.second->children.insert(it, ins);
        break;
      }
    }
  }
}

/*
int main(int argc, char const *argv[]) {

  Ptrie *test_root = new (Ptrie);
  test_root->isLeaf = true;

  insert(test_root, "romane");
  insert(test_root, "romanus");
  insert(test_root, "romulus");
  insert(test_root, "rubens");
  insert(test_root, "ruber");
  insert(test_root, "rubicon");
  insert(test_root, "rubicundus");

  cout << search(test_root, "ruber") << endl;
  return 0;
}
*/