// Example program
#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

#define M 203
#define MAX_INT numeric_limits<int>::max()

struct Rectangle {
  int xMin, yMin;
  int xMax, yMax;
};

struct Rtree {
  int isLeaf;
  int numKeys;
  Rectangle MBR[M + 1];
  int children[M + 1];
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

int getMBRarea(Rectangle C, Rectangle MBR) {
  Rectangle n;
  fit(C, MBR, n);
  return getArea(n);
}

int getMBRgrowth(Rectangle C, Rectangle MBR) {
  return getMBRarea(C, MBR) - getArea(MBR);
}

void addMBRtoRectangle(Rtree &rtree, Rectangle MBR, int child, Rectangle rec) {
  addMBR(rtree, MBR, childIndex);
  fit(MBR, rec, rec);
}

void addMBR(Rtree &rtree, Rectangle MBR, int child) {
  rtree.MBR[rtree.numKeys] = MBR;
  rtree.children[rtree.numKeys] = child;
  rtree.numKeys++
}

void split(int node, int father, int childPos) {
  Rtree rtNode, rtFather;
  fstream fNode(to_string(node), ios::in | ios::out | ios::binary);
  fNode.read((char *)&rtNode, sizeof(Rtree));

  // LinearSplit
  int xMinRange, xMaxRange, yMinRange, yMaxRange;
  int xMinHigh, xMaxLow, yMinHigh, yMaxLow;
  xMinRange = yMinRange = xMinHigh = yMinHigh = MAX_INT;
  xMaxRange = yMaxRange = xMaxLow = yMaxLow = -MAX_INT;

  int xMinHighIndex, xMaxLowIndex, yMinHighIndex, yMaxLowIndex;
  xMinHighIndex = xMaxLowIndex = yMinHighIndex = yMaxLowIndex = 0;

  vector<int> MBRset(M + 1);

  for (int i = 0; i < rtNode.numKeys; i++) {
    MBRset.push_back(i);
    if (rtNode.MBR[i].xMin < xMinRange)
      xMinRange = rtNode.MBR[i].xMin;
    if (rtNode.MBR[i].xMax > xMaxRange)
      xMaxRange = rtNode.MBR[i].xMax;
    if (rtNode.MBR[i].yMin < yMinRange)
      yMinRange = rtNode.MBR[i].yMin;
    if (rtNode.MBR[i].yMax > yMaxRange)
      yMaxRange = rtNode.MBR[i].yMax;

    if (rtNode.MBR[i].xMax < xMinHigh) {
      xMinHigh = rtNode.MBR[i].xMax;
      xMinHighIndex = i;
    }
    if (rtNode.MBR[i].xMin > xMaxLow) {
      xMaxLow = rtNode.MBR[i].xMin;
      xMaxLowIndex = i;
    }
    if (rtNode.MBR[i].yMax < yMinHigh) {
      yMinHigh = rtNode.MBR[i].yMax;
      yMinHighIndex = i;
    }
    if (rtNode.MBR[i].yMin > yMaxLow) {
      yMaxLow = rtNode.MBR[i].yMin;
      yMaxLowIndex = i;
    }
  }
  int xNormDist = (xMaxLow - xMinHigh) / (xMaxRange - xMinRange);
  int yNormDist = (yMaxLow - yMinHigh) / (yMaxRange - yMinRange);

  int firstIndex = xNormDist > yNormDist ? xMinHighIndex : yMinHighIndex;
  int secondIndex = xNormDist > yNormDist ? xMaxLowIndex : yMaxLowIndex;

  MBRset.erase(MBRset.begin() + max(firstIndex, secondIndex));
  MBRset.erase(MBRset.begin() + min(firstIndex, secondIndex));
  random_shuffle(MBRset.begin(), MBRset.end());

  Rtree first, second;
  Rectangle firstRec, secondRec;

  first.isLeaf = rtNode.isLeaf;
  first.numKeys = 1;
  first.MBR[0] = firstRec = rtNode.MBR[firstIndex];
  first.children[0] = rtNode.children[firstIndex];

  second.isLeaf = rtNode.isLeaf;
  second.numKeys = 1;
  second.MBR[0] = secondRec = rtNode.MBR[secondIndex];
  second.children[0] = rtNode.children[secondIndex];

  int firstGrowth, secondGrowth;
  int firstArea, secondArea;
  int maxSize = M + 1 - m;

  for (vector<int>::iterator it = MBRset.begin(); it != MBRset.end(); ++it) {

    if (first->numKeys == maxSize) {
      // add to second
      addMBRtoRectangle(second, rtNode.MBR[*it], rtNode.children[*it],
                        secondRec);
    } else if (second->numKeys == maxSize) {
      // add to first
      addMBRtoRectangle(first, rtNode.MBR[*it], rtNode.children[*it], firstRec);
    } else {
      firstGrowth = getMBRgrowth(rtNode.MBR[*it], firstRec);
      secondGrowth = getMBRgrowth(rtNode.MBR[*it], secondRec);
      if (firstGrowth == secondGrowth) {
        firstArea = getMBRrea(rtNode.MBR[*it], firstRec);
        secondArea = getMBRarea(rtNode.MBR[*it], secondRec);
        if (firstArea == secondArea) {
          if (first->numKeys == second->numKeys) {
            if (rand() % 2) {
              addMBRtoRectangle(first, rtNode.MBR[*it], rtNode.children[*it],
                                firstRec);
              continue;
            }
          } else if (first->numKeys < second->numKeys) {
            addMBRtoRectangle(first, rtNode.MBR[*it], rtNode.children[*it],
                              firstRec);
            continue;
          }
        } else if (firstArea < secondArea) {
          addMBRtoRectangle(first, rtNode.MBR[*it], rtNode.children[*it],
                            firstRec);
          continue;
        }
      } else if (firstGrowth < secondGrowth) {
        addMBRtoRectangle(first, rtNode.MBR[*it], rtNode.children[*it],
                          firstRec);
        continue;
      }
      addMBRtoRectangle(second, rtNode.MBR[*it], rtNode.children[*it],
                        secondRec);
    }
  }

  fNode.seekp(0, ios::beg);
  fNode.write((char *)&first, sizeof(Rtree));
  fNode.close();

  fstream fNewNode(to_string(N_CHILD), ios::in | ios::out | ios::binary);
  fNewNode.write((char *)&second, sizeof(Rtree));
  fNewNode.close();

  fstream fFather(to_string(father), ios::in | ios::out | ios::binary);
  fFather.read((char *)&rtFather, sizeof(Rtree));
  rtFather.MBR[childPos] = firstRec;
  rtFather.MBR[rtFather.numKeys] = secondRec;
  rtFather.childIndex[rtFather.numKeys] = N_CHILD;
  rtFather.numKeys++;
  fFather.seekp(0, ios::beg);
  fFather.write((char *)&rtFather, sizeof(Rtree));
  fFather.close();
  N_CHILD++;
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
    insert(C, node->children[smaller], node);
  } else {
    smaller = node->numKeys;
    node->MBR[node->numKeys++] = C;
  }
  if (node->numKeys > M) {
    split(node, father, smaller);
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

  Rtree nodes[5];
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

  Rtree root;
  root.isLeaf = false;
  root.numKeys = 2;
  root.MBR[0] = r1;
  root.MBR[1] = r2;
  root.children[0] = &nodes[0];
  root.children[1] = &nodes[1];

  Rectangle s = {1, -8, 6, 1};

  list<Rectangle> result = search(s, &root);
  printf("%i %i %i %i\n", result.front().xMin, result.front().yMin,
         result.front().xMax, result.front().yMax);
  printf("%i %i %i %i\n", result.back().xMin, result.back().yMin,
         result.back().xMax, result.back().yMax);

  // cout << sizeof(Rtree) << "\n";
  return 0;
}
