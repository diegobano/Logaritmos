#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define M 2

typedef struct {
  int xMin, yMin;
  int xMax, yMax;
} Rectangle;

typedef struct Node {
  int isLeaf;
  int numKeys;
  Rectangle MBR[M];
  struct Node *children[M];
} Node;

int intersect(Rectangle r1, Rectangle r2) {
  if (r1.xMax < r2.xMin || r2.xMax < r1.xMin || r1.yMax < r2.yMin ||
      r2.yMax < r1.yMin)
    return FALSE;
  else
    return TRUE;
}

int searchCount(Rectangle C, Node *node) {
  if (node == NULL) {
    return 0;
  }
  int count = 0;
  for (int i = 0; i < node->numKeys; i++) {
    if (intersect(C, node->MBR[i])) {

      if (node->isLeaf) {
        count += 1;
        // printf("%i %i %i %i\n", node->MBR[i].xMin, node->MBR[i].yMin,
        //       node->MBR[i].xMax, node->MBR[i].yMax);
      } else
        count += searchCount(C, node->children[i]);
    }
  }
  return count;
}

int searchRec(Rectangle C, Node *node, Rectangle *result, int index) {
  if (node == NULL) {
    return 0;
  }
  int nextIndex = index;
  for (int i = 0; i < node->numKeys; i++) {
    if (intersect(C, node->MBR[i])) {
      if (node->isLeaf)
        result[nextIndex++] = node->MBR[i];
      else
        nextIndex = searchRec(C, node->children[i], result, nextIndex);
    }
  }
  return nextIndex;
}

Rectangle *search(Rectangle C, Node *root) {
  printf("SC %i\n", searchCount(C, root));
  Rectangle *result =
      (Rectangle *)malloc(searchCount(C, root) * sizeof(Rectangle));
  printf("SR %i\n", searchRec(C, root, result, 0));
  // searchRec(C, root, result, 0);
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
  nodes[4].isLeaf = TRUE;
  nodes[4].numKeys = 1;
  nodes[4].MBR[0] = r9;
  nodes[3].isLeaf = TRUE;
  nodes[3].numKeys = 1;
  nodes[3].MBR[0] = r8;
  nodes[2].isLeaf = TRUE;
  nodes[2].numKeys = 2;
  nodes[2].MBR[0] = r6;
  nodes[2].MBR[1] = r7;
  nodes[1].isLeaf = FALSE;
  nodes[1].numKeys = 1;
  nodes[1].MBR[0] = r5;
  nodes[1].children[0] = &nodes[4];
  nodes[0].isLeaf = FALSE;
  nodes[0].numKeys = 2;
  nodes[0].MBR[0] = r3;
  nodes[0].MBR[1] = r4;
  nodes[0].children[0] = &nodes[2];
  nodes[0].children[1] = &nodes[3];

  Node *root = (Node *)malloc(sizeof(Node));
  root->isLeaf = FALSE;
  root->numKeys = 2;
  root->MBR[0] = r1;
  root->MBR[1] = r2;
  root->children[0] = &nodes[0];
  root->children[1] = &nodes[1];

  Rectangle s = {1, -8, 6, 1};

  Rectangle *r = search(s, root);

  printf("%i %i %i %i\n", r[0].xMin, r[0].yMin, r[0].xMax, r[0].yMax);
  printf("%i %i %i %i\n", r[1].xMin, r[1].yMin, r[1].xMax, r[1].yMax);

  return 0;
}
