// patricia.cpp
#include "patricia.hpp"

vector<string> words;
unordered_map<string, vector<int>> positions;

Patricia::Patricia(bool isLeaf, string::iterator pointer, int length,
                   vector<Patricia *> children)
    : isLeaf(isLeaf), pointer(pointer), length(length), children(children) {}

Patricia::Patricia(string key, int start, int value) : isLeaf(true) {
  words.push_back(key);
  pointer = words.back().begin() + start;
  length = words.back().end() - pointer;
  positions[key].push_back(value);
}

Patricia::Patricia(int size) : isLeaf(true) { words = vector<string>(size); }

Patricia::Patricia() : Patricia(1024) {}

int Patricia::compare(string::iterator it1, string::iterator it2, int length) {
  for (; length > 0 && *it1++ == *it2++; length--)
    ;
  return length;
}

tuple<int, Patricia *, string::iterator>
Patricia::search_it(string::iterator key) {

  // iterate over children
  for (Patricia *&child : this->children) {

    // compare label with key
    int cmp = compare(child->pointer, key, child->length);

    // if label is prefix of key
    if (!cmp) {
      // if key reach end
      if (!*(key + child->length))
        return make_tuple(1, child, key);
      else
        return child->search_it(key + child->length);
    }
    // if key ended on label
    if (cmp != child->length)
      return make_tuple(2, child, key);
  }
  // only stop on leaf at the beggining due to implementation
  if (this->isLeaf)
    return make_tuple(3, this, key);
  // stopped on node
  else
    return make_tuple(4, this, key);
}

vector<int> Patricia::search(string key) {
  // key += "$";
  int rc = get<0>(this->search_it(key.begin()));
  vector<int> res;
  if (rc == 1)
    res = positions[key];
  return res;
}

void Patricia::insert(string key, int value) {
  // key += "$";
  int rc;
  Patricia *node;
  string::iterator key_pos;
  tie(rc, node, key_pos) = this->search_it(key.begin());

  // key founded
  if (rc == 1) {
    positions[key].push_back(value);
  }
  // key ended on label
  else if (rc == 2) {
    int count = 0;
    string::iterator pointer = node->pointer;
    for (; *node->pointer == *key_pos; node->pointer++, key_pos++, count++)
      ;
    Patricia *new_node = new Patricia(node->isLeaf, node->pointer,
                                      node->length - count, node->children);
    Patricia *leaf = new Patricia(key, key_pos - key.begin(), value);
    node->isLeaf = false;
    node->pointer -= count;
    node->length = count;
    node->children.clear();
    if (*new_node->pointer < *leaf->pointer) {
      node->children.push_back(new_node);
      node->children.push_back(leaf);
    } else {
      node->children.push_back(leaf);
      node->children.push_back(new_node);
    }

  }
  // stopped on leaf
  else if (rc == 3) {
    Patricia *leaf = new Patricia(key, 0, value);

    node->isLeaf = false;
    node->children.push_back(leaf);
  }
  // stopped on node
  else if (rc == 4) {
    // iterate over children

    for (vector<Patricia *>::iterator child_it = node->children.begin();
         child_it != node->children.end(); ++child_it) {
      if (*(*child_it)->pointer > *key_pos) {
        Patricia *leaf = new Patricia(key, key_pos - key.begin(), value);
        // insert new child before current iterator
        node->children.insert(child_it, leaf);
        break;
      }
    }
  }
}
/*
int main(int argc, char const *argv[]) {

  Patricia *test_root = new Patricia(100);

  test_root->insert("romane$", 1);
  test_root->insert("romanus$", 2);
  test_root->insert("romulus$", 3);
  test_root->insert("rubens$", 4);
  test_root->insert("ruber$", 5);
  test_root->insert("rubicon$", 6);
  test_root->insert("rubicundus$", 7);
  test_root->insert("ruber$", 8);
  test_root->insert("romanus$", 9);
  test_root->insert("rubicundus$", 10);
  cout << test_root->search("ruber$").size() << endl;
  return 0;
}
*/
