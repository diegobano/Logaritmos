// Example program
#include <iostream>
#include <fstream>
#include <chrono>
#include <stdlib.h>
#include <list>
#include <string>
#include <limits>
using namespace std;

#define M 203
#define m 81
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
	//cout << "Haciendo split de " << nFile << endl;
	Rectangle d1, d2, aux1, aux2;
	int pos1, pos2, curr, newRoot;
	int max = 0, ua, d, a1, a2;
	Rtree newNode1, newNode2;
	Rtree node, father;
	fstream f, n;

	if (fFile < 0) {
		//cout << "Creando nueva raiz en " << N_CHILD << endl;
		newRoot = N_CHILD++;
		f.open(to_string(newRoot), ios::out | ios::trunc | ios::binary);
	} else {
		f.open(to_string(fFile), ios::in | ios::out | ios::binary);
		f.read((char*)&father, sizeof(Rtree));
	}
	n.open(to_string(nFile), ios::in | ios::out | ios::binary);
	n.read((char*)&node, sizeof(Rtree));

	// Setting initial values for new nodes
	newNode1.numKeys = 0;
	newNode1.isLeaf = node.isLeaf;
	newNode2.numKeys = 0;
	newNode2.isLeaf = node.isLeaf;

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
		//cout << "Nodo 1 tiene " << newNode1.numKeys << " elementos y Nodo 2 tiene " << newNode2.numKeys << " elementos\n";
		// newNode1 full
		if (newNode1.numKeys >= M - m + 1) {
			//cout << "Nodo 1 lleno con " << newNode1.numKeys << " elementos\n";
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
			//cout << "Nodo 2 lleno con " << newNode2.numKeys << " elementos\n";
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
	//cout << "Reciclando nodo en " << nFile << endl;
	//cout << "Hoja: " << newNode1.isLeaf << endl;
	n.seekp(0, ios::beg);
	n.write((char*)&newNode1, sizeof(Rtree));
	n.close();

	// Create new node in secondary memory
	//cout << "Creando nuevo nodo en " << N_CHILD << endl;
	//cout << "Hoja: " << newNode2.isLeaf << endl;
	fstream newN(to_string(N_CHILD), ios::out | ios::binary);
	newN.write((char*)&newNode2, sizeof(Rtree));
	newN.close();

	//cout << "Actualizando al padre: " << (fFile < 0 ? newRoot : fFile) << endl;
	// Update and save father
	if (fFile < 0) {
		father.MBR[0] = d1;
		father.children[0] = root;
		father.numKeys = 1;
		father.isLeaf = 0;
		root = newRoot;
	} else {
		father.MBR[childPos] = d1;
		f.seekp(0, ios::beg);
	}
	father.MBR[father.numKeys] = d2;
	father.children[father.numKeys++] = N_CHILD++;
	//cout << "Padre quedo con " << father.numKeys << " elementos\n";
	f.write((char*)&father, sizeof(Rtree));
	f.close();
}



void insert(Rectangle c, int nFile, int fFile, int childPos, void (*split)(int, int, int)) {
	int smaller = 0;
	int growth = MAX_INT;
	int tmp, a1, a2;
	Rtree node;
	//cout << "Abriendo archivo " << nFile << endl;
	fstream n(to_string(nFile), ios::in | ios::out | ios::binary);
	n.read((char*)&node, sizeof(Rtree));
	//cout << node.isLeaf << endl;
	//cout << node.numKeys << endl;
	if (!node.isLeaf) {
		//cout << "Archivo " << nFile << " no es una hoja\n";
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
		//cout << "Actualizando MBR de hijo\n";
		fit(c, node.MBR[smaller], node.MBR[smaller]);
		//cout << "Actualizando archivo\n";
		n.seekp(0, ios::beg);
		n.write((char*)&node, sizeof(Rtree));
		n.close();
		//cout << "Insertar en hijo " << node.children[smaller] << endl;
		insert(c, node.children[smaller], nFile, smaller, split);
	} else {
		//cout << "Archivo " << nFile << " SI es una hoja\n";
		//cout << "Contiene " << node.numKeys << " elementos\n";
		node.MBR[node.numKeys++] = c;
		//cout << "Actualizando archivo\n";
		n.seekp(0, ios::beg);
		n.write((char*)&node, sizeof(Rtree));
		n.close();
	}
	// Check correct size
	//cout << "Verificando invariante\n";
	if (node.numKeys > M) {
		split(nFile, fFile, childPos);
	}
}

void insert(Rectangle c, int root, void (*split)(int, int, int)) {
	insert(c, root, -1, -1, split);
}

int main(int argc, char *argv[]) {
	int n = 1<<15;
	Rtree init;
	init.isLeaf = 1;
	init.numKeys = 0;

	cout << "Creando raiz inicial\n";
	fstream r(to_string(root), ios::out | ios::trunc | ios::binary);
	r.write((char*)&init, sizeof(Rtree));
	r.close();

	cout << "Creando rectangulos para insertar en el arbol\n";
	Rectangle rs[n];
	for (int i = 0; i < n; i++) {
		rs[i].xMin = rand() % 500001;
		rs[i].yMin = rand() % 500001;
		rs[i].xMax = rs[i].xMin + rand() % 100 + 1;
		rs[i].yMax = rs[i].yMin + rand() % 100 + 1;
	}
	cout << "Rectangulos creados\n";
	cout << "Insertando en raiz\n";
	auto begin = chrono::high_resolution_clock::now();
	for (int i = 0; i < n; i++) {
		insert(rs[i], root, QuadraticSplit);
	}
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = end - begin;
	cout << "Insercion de " << n << " elementos terminada en " << elapsed.count() * 1000 << "ms\n";
	return 0;
}
