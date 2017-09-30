// Example program
#include <iostream>
#include <limits>
#include <list>
#include <stdlib.h>
#include <string>
using namespace std;

#define M 256
#define MAX_INT numeric_limits<int>::max()

struct Rectangle {
  int xMin, yMin;
  int xMax, yMax;
};

struct Rtree {
  int isLeaf;
  int numKeys;
  Rectangle MBR[M + 1];
  Rtree *children[M + 1];
};

int getArea(Rectangle c) { return (c.xMax - c.xMin) * (c.yMax - c.yMin); }

int getPerimeter(Rectangle c) {
  return 2 * (c.xMax - c.xMin + c.yMax - c.yMin);
}

int intersect(Rectangle r1, Rectangle r2) {
  if (r1.xMax < r2.xMin || r2.xMax < r1.xMin || r1.yMax < r2.yMin ||
      r2.yMax < r1.yMin)
    return false;
  else
    return true;
}

void searchRec(Rectangle C, Rtree *node, list<Rectangle> &result) {
  if (node == NULL) {
    return;
  }
  for (int i = 0; i < node->numKeys; i++) {
    if (intersect(C, node->MBR[i])) {
      if (node->isLeaf)
        result.push_back(node->MBR[i]);
      else
        searchRec(C, node->children[i], result);
    }
  }
}

list<Rectangle> search(Rectangle C, Rtree *root) {
  list<Rectangle> result;
  searchRec(C, root, result);
  return result;
}

void fit(Rectangle C, Rectangle &in, Rectangle &out) {
  out.xMin = C.xMin < in.xMin ? C.xMin : in.xMin;
  out.xMax = C.xMax > in.xMax ? C.xMax : in.xMax;
  out.yMin = C.yMin < in.yMin ? C.yMin : in.yMin;
  out.yMax = C.yMax > in.yMax ? C.yMax : in.yMax;
}

int getMBRgrowth(Rectangle C, Rectangle MBR) {
  Rectangle n;
  fit(C, MBR, n);
  a1 = getArea(n);
  a2 = getArea(MBR);
  return a1 - a2;
}

void split(Rtree *node, Rtree *father) {
  // Implementar heuristica
}

void insert(Rectangle C, Rtree *node, Rtree *father) {
  int smaller = 0;
  int growth = MAX_INT;
  int tmp, a1, a2;
  if (!node->isLeaf) {
    for (int i = 0; i < node->numKeys; i++) {
      tmp = getMBRgrowth(C, node->MBR[i]);
      if (tmp < growth) {
        smaller = i;
        growth = tmp;
      } else if (tmp == growth) {
        a1 = getArea(node->MBR[smaller]);
        a2 = getArea(node->MBR[i]);
        if (a1 > a2) {
          smaller = i;
          growth = tmp;
        } else if (a1 == a2) {
          int r = rand() % 2;
          smaller = r ? i : smaller;
          growth = r ? tmp : growth;
        }
      }
    }
    fit(C, node->MBR[smaller], node->MBR[smaller]);
    insert(C, node->children[smaller], &node);
  } else {
    node->MBR[node->numKeys++] = C;
  }
  if (node->numKeys > M) {
    split(node, father);
  }
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

  Rtree *nodes = (Rtree *)malloc(5 * sizeof(Rtree));
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

  Rtree *root = (Rtree *)malloc(sizeof(Rtree));
  root->isLeaf = false;
  root->numKeys = 2;
  root->MBR[0] = r1;
  root->MBR[1] = r2;
  root->children[0] = &nodes[0];
  root->children[1] = &nodes[1];

  Rectangle s = {1, -8, 6, 1};

  list<Rectangle> result = search(s, root);
  printf("%i %i %i %i\n", result.front().xMin, result.front().yMin,
         result.front().xMax, result.front().yMax);
  printf("%i %i %i %i\n", result.back().xMin, result.back().yMin,
         result.back().xMax, result.back().yMax);

  return 0;
}
