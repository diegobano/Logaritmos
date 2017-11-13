// patricia.cpp
#include "patricia.hpp"

Patricia::Patricia(bool isLeaf, string label, vector<Patricia *> children,
                   vector<vector<int>> positions)
    : isLeaf(isLeaf), label(label), children(children), positions(positions) {}

Patricia::Patricia(string key, string label, int value, int text)
    : isLeaf(true), label(label) {
  expand_positions(key, text);
  positions[text].push_back(value);
}

Patricia::Patricia(int size) : isLeaf(true) {}

Patricia::Patricia() : Patricia(1024) {}

Patricia::~Patricia() {}

void Patricia::expand_positions(string key, int text) {
  for (; int(positions.size()) < text + 1; positions.push_back(vector<int>()))
    ;
}

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
    int cmp = compare(child->label.begin(), key, child->label.size());

    // if label is prefix of key
    if (!cmp) {
      // if key reach end
      if (!*(key + child->label.size()))
        return make_tuple(1, child, key);
      else
        return child->search_it(key + child->label.size());
    }
    // if key ended on label
    if (cmp != int(child->label.size()))
      return make_tuple(2, child, key);
  }
  // only stop on leaf at the beggining due to implementation
  if (this->isLeaf)
    return make_tuple(3, this, key);
  // stopped on node
  else
    return make_tuple(4, this, key);
}

vector<int> Patricia::search(string key, int text) {
  key += "$";
  int rc;
  Patricia *node;
  string::iterator key_pos;
  tie(rc, node, key_pos) = this->search_it(key.begin());
  vector<int> res;
  if (rc == 1 && int(node->positions.size()) > text)
    res = node->positions[text];
  return res;
}

void Patricia::insert(string key, int value, int text) {
  key += "$";
  int rc;
  Patricia *node;
  string::iterator key_pos;
  tie(rc, node, key_pos) = this->search_it(key.begin());

  // key founded
  if (rc == 1) {
    node->expand_positions(key, text);
    node->positions[text].push_back(value);
  }
  // key ended on label
  else if (rc == 2) {
    int count = 0;
    string::iterator label_pos = node->label.begin();
    for (; *label_pos == *key_pos; label_pos++, key_pos++, count++)
      ;
    Patricia *new_node = new Patricia(node->isLeaf, node->label.substr(count),
                                      node->children, node->positions);
    Patricia *leaf =
        new Patricia(key, key.substr(key_pos - key.begin()), value, text);
    node->isLeaf = false;
    node->label = node->label.substr(0, count);
    node->children.clear();
    node->positions.clear();
    if (new_node->label[0] < leaf->label[0]) {
      node->children.push_back(new_node);
      node->children.push_back(leaf);
    } else {
      node->children.push_back(leaf);
      node->children.push_back(new_node);
    }

  }
  // stopped on leaf
  else if (rc == 3) {
    // cout << "rc == 3" << endl;
    Patricia *leaf = new Patricia(key, key, value, text);

    node->isLeaf = false;
    node->children.push_back(leaf);
  }
  // stopped on node
  else if (rc == 4) {
    // iterate over children
    for (vector<Patricia *>::iterator child_it = node->children.begin();
         child_it != node->children.end(); ++child_it) {
      if ((*child_it)->label[0] > *key_pos) {
        Patricia *leaf =
            new Patricia(key, key.substr(key_pos - key.begin()), value, text);
        // insert new child before current iterator
        node->children.insert(child_it, leaf);
        break;
      }
    }
  }
}

int Patricia::similarity() {
  // by default compare text 0 and 1
  int text_1 = 0, text_2 = 1;
  int res = 0;
  // iterate over children
  for (Patricia *&child : this->children) {
    if (child->isLeaf) {
      int texts = child->positions.size();
      int word_t1 = text_1 < texts ? child->positions[text_1].size() : 0;
      int word_t2 = text_2 < texts ? child->positions[text_2].size() : 0;
      res += abs(word_t1 - word_t2);
    } else
      res += child->similarity();
  }
  return res;
}
/*
int main(int argc, char const *argv[]) {

  Patricia *test_root = new Patricia(100);
  // test_root->insert("hola", 1, 0);
  // test_root->insert("holi", 2, 0);

  test_root->insert("romane", 1, 0);
  test_root->insert("romanus", 2, 0);
  test_root->insert("romulus", 3, 0);
  test_root->insert("rubens", 4, 0);
  test_root->insert("ruber", 5, 0);
  test_root->insert("rubicon", 6, 0);
  test_root->insert("rubicundus", 7, 0);
  test_root->insert("ruber", 8, 0);
  test_root->insert("romanus", 9, 0);
  test_root->insert("rubicundus", 10, 0);

  Patricia *test_root2 = new Patricia(100);
  test_root2->insert("romane", 1, 0);
  test_root2->insert("romanus", 2, 0);
  test_root2->insert("romulus", 3, 0);
  test_root2->insert("rubens", 4, 0);
  test_root2->insert("ruber", 5, 0);
  test_root2->insert("rubicon", 6, 0);
  test_root2->insert("rubicundus", 7, 0);
  test_root2->insert("ruber", 8, 0);
  test_root2->insert("romanus", 9, 0);
  test_root2->insert("rubicundus", 10, 0);

  test_root->insert("romane", 1, 1);
  test_root->insert("romanus", 2, 1);
  test_root->insert("romulus", 3, 1);
  test_root->insert("rubens", 4, 1);
  test_root->insert("ruber", 5, 1);
  test_root->insert("rubicon", 6, 1);
  test_root->insert("rubicundus", 7, 1);
  test_root->insert("ruber", 8, 1);
  test_root->insert("romanus", 9, 1);
  test_root->insert("rubicundus", 10, 1);
  cout << "search: " << test_root->search("ruber", 0).size() << endl;
  cout << "similarity: " << test_root->similarity() << endl;
  return 0;
}
*/
