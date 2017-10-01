// Example program
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <list>
#include <string>
#include <limits>
using namespace std;

#define M 203
#define m M*4/10
#define MAX_INT numeric_limits<int>::max()

int root = 1;
int N_CHILD = 2;

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

int getArea(Rectangle c) {
	return (c.xMax - c.xMin) * (c.yMax - c.yMin);
}

int getPerimeter(Rectangle c) {
	return 2 * (c.xMax - c.xMin) + 2 * (c.yMax - c.yMin);
}

int intersect(Rectangle r1, Rectangle r2) {
  if (r1.xMax < r2.xMin || r2.xMax < r1.xMin || r1.yMax < r2.yMin ||
      r2.yMax < r1.yMin)
    return false;
  else
    return true;
}

void searchRec(Rectangle C, int nFile, list<Rectangle> &result) {
	Rtree node;
  if (nFile == 0) {
    return;
  }
	fstream n(to_string(nFile), ios::in | ios::out | ios::binary);
	n.read((char*)&node, sizeof(Rtree));
	n.close();
  for (int i = 0; i < node.numKeys; i++) {
    if (intersect(C, node.MBR[i])) {
      if (node.isLeaf) {
        result.push_back(node.MBR[i]);
        cout << result.size() << "\n";
      } else
        searchRec(C, node.children[i], result);
    }
  }
}

list<Rectangle> search(Rectangle C, int root) {
  list<Rectangle> result;
  searchRec(C, root, result);
  cout << &result << "\n";
  return result;
}

void fit(Rectangle c, Rectangle &mbr, Rectangle &out) {
	out.xMin = c.xMin < mbr.xMin ? c.xMin : mbr.xMin;
	out.xMax = c.xMax > mbr.xMax ? c.xMax : mbr.xMax;
	out.yMin = c.yMin < mbr.yMin ? c.yMin : mbr.yMin;
	out.yMax = c.yMax > mbr.yMax ? c.yMax : mbr.yMax;
}

int getMBRgrowth(Rectangle c, Rectangle mbr) {
	Rectangle n;
	int a1, a2;
	fit(c, mbr, n);
	a1 = getArea(n);
	a2 = getArea(mbr);
	return a1 - a2;
}

int getUselessArea(Rectangle d1, Rectangle d2) {
	Rectangle aux;
	fit(d1, d2, aux);
	return getArea(aux) - getArea(d1) - getArea(d2);
}

void QuadraticSplit(int nFile, int fFile, int childPos) {
	Rectangle d1, d2, aux1, aux2;
	int pos1, pos2, curr;
	int max = 0, ua, d, a1, a2;
	Rtree newNode1, newNode2;
	Rtree node, father;

	if (fFile < 0) {
		root = N_CHILD++;
		fFile = root;
		fstream f(to_string(fFile), ios::in | ios::out | ios::binary);
	} else {
		fstream f(to_string(fFile), ios::in | ios::out | ios::binary);
		f.read((char*)&father, sizeof(Rtree));
	}
	fstream n(to_string(nFile), ios::in | ios::out | ios::binary);
	n.read((char*)&node, sizeof(Rtree));

	int placed[node.numKeys];
	for (int i = 0; i < node.numKeys; i++) {
		for (int j = i + 1; j < node.numKeys; j++) {
			ua = getUselessArea(node.MBR[i], node.MBR[j]);
			if (ua > max) {
				d1 = node.MBR[i];
				d2 = node.MBR[j];
				pos1 = i; pos2 = j;
				max = ua;
			}
		}
	}
	
	while (1) {
		// newNode1 full
		if (newNode1.numKeys >= M - m + 1) {
			for (int i = 0; i < node.numKeys; i++) {
				if (!placed[i]) {
					placed[i] = 1;
					newNode2.MBR[newNode2.numKeys] = node.MBR[i];
					newNode2.children[newNode2.numKeys++] = node.children[i];
				}
			}
			break;
		}
		// newNode2 full
		if (newNode2.numKeys >= M - m + 1) {
			for (int i = 0; i < node.numKeys; i++) {
				if (!placed[i]) {
					placed[i] = 1;
					newNode1.MBR[newNode1.numKeys] = node.MBR[i];
					newNode1.children[newNode1.numKeys++] = node.children[i];
				}
			}
			break;
		}

		max = 0;
		for (int i = 0; i < node.numKeys; i++) {
			if (!placed[i]) {
				fit(node.MBR[i], d1, aux1);
				fit(node.MBR[i], d2, aux2);
				d = abs(getArea(aux1) - getArea(aux2));
				if (d > max) {
					max = d;
					curr = i;
				}
			}
		}

		a1 = getMBRgrowth(node.MBR[curr], d1);
		a2 = getMBRgrowth(node.MBR[curr], d2);
		if (a1 < a2) {
			newNode1.children[newNode1.numKeys] = node.children[curr];
			newNode1.MBR[newNode1.numKeys++] = node.MBR[curr];
			fit(node.MBR[curr], d1, d1);
		} else if (a2 < a1) {
			newNode2.children[newNode2.numKeys] = node.children[curr];
			newNode2.MBR[newNode2.numKeys++] = node.MBR[curr];
			fit(node.MBR[curr], d2, d2);
		} else {
			fit(node.MBR[curr], d1, aux1);
			fit(node.MBR[curr], d2, aux2);
			a1 = getArea(aux1); a1 = getArea(aux2);
			if (a1 < a2) {
				newNode1.children[newNode1.numKeys] = node.children[curr];
				newNode1.MBR[newNode1.numKeys++] = node.MBR[curr];
				fit(node.MBR[curr], d1, d1);
			} else if (a2 < a1) {
				newNode2.children[newNode2.numKeys] = node.children[curr];
				newNode2.MBR[newNode2.numKeys++] = node.MBR[curr];
				fit(node.MBR[curr], d2, d2);
			} else {
				if (newNode1.numKeys < newNode2.numKeys) {
					newNode1.children[newNode1.numKeys] = node.children[curr];
					newNode1.MBR[newNode1.numKeys++] = node.MBR[curr];
					fit(node.MBR[curr], d1, d1);
				} else {
					newNode2.children[newNode2.numKeys] = node.children[curr];
					newNode2.MBR[newNode2.numKeys++] = node.MBR[curr];
					fit(node.MBR[curr], d2, d2);
				}
			}
		}
		// Mark as placed
		placed[curr] = 1;
	}
	// Update/reuse old node with overflow
	n.seekp(0, ios::beg);
	n.write((char*)&newNode1, sizeof(Rtree));
	n.close();

	// Create new node in secondary memory
	fstream newN(to_string(N_CHILD), ios::out | ios::binary);
	newN.write((char*)&newNode2, sizeof(Rtree));
	newN.close();

	// Update and save father
	if (fFile < 0) {
		father.MBR[0] = d1;
		father.numKeys = 1;
	} else {
		father.MBR[childPos] = d1;
	}
	father.MBR[father.numKeys] = d2;
	father.children[father.numKeys++] = N_CHILD++;
	f.seekp(0, ios::beg);
	f.write((char*)&father, sizeof(Rtree));
	f.close();
}



void insert(Rectangle c, int nFile, int fFile, int childPos, void (*split)(int, int, int)) {
	int smaller = 0;
	int growth = MAX_INT;
	int tmp, a1, a2;
	Rtree node;
	fstream n(to_string(nFile), ios::in | ios::out | ios::binary);
	n.read((char*)&node, sizeof(Rtree));
	if (!node.isLeaf) {
		for(int i=0; i < node.numKeys; i++) {
			tmp = getMBRgrowth(c, node.MBR[i]);
			if (tmp < growth) {
				smaller = i;
				growth = tmp;
			} else if (tmp == growth) {
				a1 = getArea(node.MBR[smaller]);
				a2 = getArea(node.MBR[i]);
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
		fit(c, node.MBR[smaller], node.MBR[smaller]);
		n.seekp(0, ios::beg);
		n.write((char*)&node, sizeof(Rtree));
		n.close();
		insert(c, node.children[smaller], nFile, smaller, split);
	} else {
		smaller = node.numKeys;
		node.MBR[node.numKeys++] = c;
		n.seekp(0, ios::beg);
		n.write((char*)&node, sizeof(Rtree));
		n.close();
	}
	// Check correct size
	if (node.numKeys > M) {
		split(nFile, fFile, childPos);
	}
}

void insert(Rectangle c, int root, void (*split)(int, int, int)) {
	insert(c, root, -1, -1, split);
}

int main(int argc, char *argv[]) {
	int n = 1<<9;
	Rectangle rs[n];
	for (int i = 0; i < n; i++) {
		rs[i].xMin = rand() % 500001;
		rs[i].yMin = rand() % 500001;
		rs[i].xMax = rs[i].xMin + rand() % 100 + 1;
		rs[i].yMax = rs[i].yMin + rand() % 100 + 1;
	}

	for (int i = 0; i < n; i++) {
		insert(rs[i], root, QuadraticSplit);
	}
	return 0;
}
