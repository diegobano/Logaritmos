// patricia.cpp
#include "patricia.hpp"

string *words;
int i_words;
unordered_map<string, vector<vector<int>>> positions;

Patricia::Patricia(bool isLeaf, string::iterator pointer, int length,
                   string *word, vector<Patricia *> children)
    : isLeaf(isLeaf), pointer(pointer), length(length), word(word),
      children(children) {}

Patricia::Patricia(string key, int start, int value, int text) : isLeaf(true) {
  words[i_words] = key;
  pointer = words[i_words].begin() + start;
  length = words[i_words].end() - pointer;
  word = &words[i_words];
  i_words++;
  expand_positions(&key, text);
  positions[key][text].push_back(value);
}

Patricia::Patricia(int size) : isLeaf(true) {
  words = new string[size];
  i_words = 0;
}

Patricia::Patricia() : Patricia(1024) {}

Patricia::~Patricia() { delete[] words; }

void Patricia::expand_positions(string *key, int text) {
  for (; positions[*key].size() < text + 1;
       positions[*key].push_back(vector<int>()))
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

vector<int> Patricia::search(string key, int text) {
  int rc = get<0>(this->search_it(key.begin()));
  vector<int> res;
  if (rc == 1 && positions[key].size() > text)
    res = positions[key][text];
  return res;
}

void Patricia::insert(string key, int value, int text) {
  int rc;
  Patricia *node;
  string::iterator key_pos;
  tie(rc, node, key_pos) = this->search_it(key.begin());

  // key founded
  if (rc == 1) {
    expand_positions(&key, text);
    positions[key][text].push_back(value);
  }
  // key ended on label
  else if (rc == 2) {
    int count = 0;
    for (; *node->pointer == *key_pos; node->pointer++, key_pos++, count++)
      ;
    Patricia *new_node =
        new Patricia(node->isLeaf, node->pointer, node->length - count,
                     node->word, node->children);
    Patricia *leaf = new Patricia(key, key_pos - key.begin(), value, text);
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
    Patricia *leaf = new Patricia(key, 0, value, text);

    node->isLeaf = false;
    node->children.push_back(leaf);
  }
  // stopped on node
  else if (rc == 4) {
    // iterate over children

    for (vector<Patricia *>::iterator child_it = node->children.begin();
         child_it != node->children.end(); ++child_it) {
      if (*(*child_it)->pointer > *key_pos) {
        Patricia *leaf = new Patricia(key, key_pos - key.begin(), value, text);
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
      int texts = positions[*child->word].size();
      int word_t1 = text_1 < texts ? positions[*child->word][text_1].size() : 0;
      int word_t2 = text_2 < texts ? positions[*child->word][text_2].size() : 0;
      res += abs(word_t1 - word_t2);
    } else
      res += child->similarity();
  }
  return res;
}
/*
int main(int argc, char const *argv[]) {

  Patricia *test_root = new Patricia(100);

  test_root->insert("romane", 1, 0);
  test_root->insert("romanus", 2, 0);
  test_root->insert("romulus", 3, 0);
  test_root->insert("rubens", 4, 0);
  test_root->insert("ruber", 5, 0);
  test_root->insert("rubicon", 6, 0);
  test_root->insert("rubicundus", 7, 0);
  test_root->insert("ruber", 8, 1);
  test_root->insert("romanus", 9, 0);
  test_root->insert("rubicundus", 10, 0);
  cout << "search: " << test_root->search("ruber", 1).size() << endl;
  cout << "similarity: " << test_root->similarity() << endl;
  delete test_root;
  return 0;
}
*/
