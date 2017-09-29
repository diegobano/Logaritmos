// Example program
#include <iostream>
#include <list>
#include <string>
using namespace std;

#define M 2

struct Rectangle {
  int xMin, yMin;
  int xMax, yMax;
};

struct Node {
  int isLeaf;
  int numKeys;
  Rectangle MBR[M];
  Node *children[M];
};

int intersect(Rectangle r1, Rectangle r2) {
  if (r1.xMax < r2.xMin || r2.xMax < r1.xMin || r1.yMax < r2.yMin ||
      r2.yMax < r1.yMin)
    return false;
  else
    return true;
}

void searchRec(Rectangle C, Node *node, list<Rectangle> &result) {
  if (node == NULL) {
    return;
  }
  for (int i = 0; i < node->numKeys; i++) {
    if (intersect(C, node->MBR[i])) {
      if (node->isLeaf) {
        result.push_back(node->MBR[i]);
        cout << result.size() << "\n";
      } else
        searchRec(C, node->children[i], result);
    }
  }
}

list<Rectangle> search(Rectangle C, Node *root) {
  list<Rectangle> result;
  searchRec(C, root, result);
  cout << &result << "\n";
  return result;
}

int main(int argc, char *argv[]) {
  // test searchRec
  Rectangle r1 = {-6, -2, 2, 5};
  Rectangle r2 = {2, -7, 5, -3};
  Rectangle r3 = {-6, -2, 2, 2};
  Rectangle r4 = {-6, 2, 2, 5};
  Rectangle r5 = {2, -7, 5, -3};
  Rectangle r6 = {-5, -2, -3, 1};
  Rectangle r7 = {-2, -2, 1, 1};
  Rectangle r8 = {-4, 3, -1, 4};
  Rectangle r9 = {3, -6, 4, -4};

  Node *nodes = (Node *)malloc(5 * sizeof(Node));
  nodes[4].isLeaf = true;
  nodes[4].numKeys = 1;
  nodes[4].MBR[0] = r9;
  nodes[3].isLeaf = true;
  nodes[3].numKeys = 1;
  nodes[3].MBR[0] = r8;
  nodes[2].isLeaf = true;
  nodes[2].numKeys = 2;
  nodes[2].MBR[0] = r6;
  nodes[2].MBR[1] = r7;
  nodes[1].isLeaf = false;
  nodes[1].numKeys = 1;
  nodes[1].MBR[0] = r5;
  nodes[1].children[0] = &nodes[4];
  nodes[0].isLeaf = false;
  nodes[0].numKeys = 2;
  nodes[0].MBR[0] = r3;
  nodes[0].MBR[1] = r4;
  nodes[0].children[0] = &nodes[2];
  nodes[0].children[1] = &nodes[3];

  Node *root = (Node *)malloc(sizeof(Node));
  root->isLeaf = false;
  root->numKeys = 2;
  root->MBR[0] = r1;
  root->MBR[1] = r2;
  root->children[0] = &nodes[0];
  root->children[1] = &nodes[1];

  Rectangle s = {1, -8, 6, 1};

  list<Rectangle> result = search(s, root);
  cout << &result << "\n";
  cout << result.size() << "\n";
  printf("%i %i %i %i\n", result.front().xMin, result.front().yMin,
         result.front().xMax, result.front().yMax);
  printf("%i %i %i %i\n", result.back().xMin, result.back().yMin,
         result.back().xMax, result.back().yMax);

  return 0;
}
