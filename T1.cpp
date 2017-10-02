// Example program
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>
using namespace std;

#define M 25
#define m 10
#define MAX_INT numeric_limits<int>::max()

int ROOT = 1;
int N_CHILD = 2;
string path = "Rtree/";

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
    return !(r1.xMax < r2.xMin || r2.xMax < r1.xMin || r1.yMax < r2.yMin ||
             r2.yMax < r1.yMin);
}

void searchRec(Rectangle C, int nFile, list<Rectangle> &result) {
    Rtree node;
    if (nFile == 0) {
        return;
    }

    fstream n(path + to_string(nFile), ios::in | ios::out | ios::binary);
    n.read((char *)&node, sizeof(Rtree));
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

void addMBR(Rtree &rtree, Rectangle MBR, int child) {
    rtree.MBR[rtree.numKeys] = MBR;
    rtree.children[rtree.numKeys] = child;
    rtree.numKeys++;
}

void addMBRtoRectangle(Rtree &rtree, Rectangle MBR, int child, Rectangle rec) {
    addMBR(rtree, MBR, child);
    fit(MBR, rec, rec);
}

int getUselessArea(Rectangle d1, Rectangle d2) {
    Rectangle aux;
    fit(d1, d2, aux);
    return getArea(aux) - getArea(d1) - getArea(d2);
}

void LinearSplit(int node, int father, int childPos) {

    Rtree rtNode, rtFather;
    fstream fNode(path + to_string(node), ios::in | ios::out | ios::binary);
    fNode.read((char *)&rtNode, sizeof(Rtree));

    // LinearSplit
    int xMinRange, xMaxRange, yMinRange, yMaxRange;
    int xMinHigh, xMaxLow, yMinHigh, yMaxLow;
    xMinRange = yMinRange = xMinHigh = yMinHigh = MAX_INT;
    xMaxRange = yMaxRange = xMaxLow = yMaxLow = -MAX_INT;

    int xMinHighIndex, xMaxLowIndex, yMinHighIndex, yMaxLowIndex;
    xMinHighIndex = xMaxLowIndex = yMinHighIndex = yMaxLowIndex = 0;

    vector<int> MBRset;
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
    cout << "firstIndex: " << firstIndex << "\nsecondIndex: " << secondIndex
         << '\n';
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
        if (first.numKeys == maxSize) {
            // add to second
            addMBRtoRectangle(second, rtNode.MBR[*it], rtNode.children[*it],
                              secondRec);
        } else if (second.numKeys == maxSize) {
            // add to first
            addMBRtoRectangle(first, rtNode.MBR[*it], rtNode.children[*it],
                              firstRec);
        } else {
            firstGrowth = getMBRgrowth(rtNode.MBR[*it], firstRec);
            secondGrowth = getMBRgrowth(rtNode.MBR[*it], secondRec);
            if (firstGrowth == secondGrowth) {
                firstArea = getMBRarea(rtNode.MBR[*it], firstRec);
                secondArea = getMBRarea(rtNode.MBR[*it], secondRec);
                if (firstArea == secondArea) {
                    if (first.numKeys == second.numKeys) {
                        if (rand() % 2) {
                            addMBRtoRectangle(first, rtNode.MBR[*it],
                                              rtNode.children[*it], firstRec);
                            continue;
                        }
                    } else if (first.numKeys < second.numKeys) {
                        addMBRtoRectangle(first, rtNode.MBR[*it],
                                          rtNode.children[*it], firstRec);
                        continue;
                    }
                } else if (firstArea < secondArea) {
                    addMBRtoRectangle(first, rtNode.MBR[*it],
                                      rtNode.children[*it], firstRec);
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
    cout << "second: " << second.numKeys << "\nfirst: " << first.numKeys
         << '\n';
    fNode.seekp(0, ios::beg);
    fNode.write((char *)&first, sizeof(Rtree));
    fNode.close();
    cout << "N_CHILD: " << N_CHILD << '\n';
    fstream fNewNode(path + to_string(N_CHILD),
                     ios::out | ios::trunc | ios::binary);
    fNewNode.write((char *)&second, sizeof(Rtree));
    fNewNode.close();

    if (father == -1) {
        cout << "father -1\n";
        Rtree newRoot;
        newRoot.isLeaf = false;
        newRoot.numKeys = 2;
        newRoot.MBR[0] = firstRec;
        newRoot.children[0] = node;
        newRoot.MBR[1] = secondRec;
        newRoot.children[1] = N_CHILD;
        ROOT = ++N_CHILD;
        fstream fNewRoot(path + to_string(ROOT),
                         ios::out | ios::trunc | ios::binary);
        fNewRoot.write((char *)&newRoot, sizeof(Rtree));
        fNewRoot.close();
        cout << "ROOT: " << ROOT << '\n';
    } else {

        fstream fFather(path + to_string(father),
                        ios::in | ios::out | ios::binary);
        fFather.read((char *)&rtFather, sizeof(Rtree));
        rtFather.MBR[childPos] = firstRec;
        rtFather.MBR[rtFather.numKeys] = secondRec;
        rtFather.children[rtFather.numKeys] = N_CHILD;
        rtFather.numKeys++;
        fFather.seekp(0, ios::beg);
        fFather.write((char *)&rtFather, sizeof(Rtree));
        fFather.close();
    }
    N_CHILD++;
}

void QuadraticSplit(int nFile, int fFile, int childPos) {
    cout << "Haciendo split de " << nFile << endl;
    Rectangle d1, d2, aux1, aux2;
    int pos1, pos2, curr, newRoot;
    int max = 0, ua, d, a1, a2;
    Rtree newNode1, newNode2;
    Rtree node, father;
    fstream f, n;

    if (fFile < 0) {
        cout << "Creando nueva raiz en " << N_CHILD << endl;
        newRoot = N_CHILD++;
        f.open(path + to_string(newRoot), ios::out | ios::trunc | ios::binary);
    } else {
        f.open(path + to_string(fFile), ios::in | ios::out | ios::binary);
        f.read((char *)&father, sizeof(Rtree));
    }
    n.open(path + to_string(nFile), ios::in | ios::out | ios::binary);
    n.read((char *)&node, sizeof(Rtree));

    // Setting initial values for new nodes
    newNode1.numKeys = 0;
    newNode1.isLeaf = node.isLeaf;
    newNode2.numKeys = 0;
    newNode2.isLeaf = node.isLeaf;

    int placed[node.numKeys];

    for (int i = 0; i < node.numKeys; i++) placed[i] = 0;

    for (int i = 0; i < node.numKeys; i++) {
        for (int j = i + 1; j < node.numKeys; j++) {
            ua = getUselessArea(node.MBR[i], node.MBR[j]);
            if (ua > max) {
                d1 = node.MBR[i];
                d2 = node.MBR[j];
                pos1 = i;
                pos2 = j;
                max = ua;
            }
        }
    }

    while (1) {
        cout << "Nodo 1 tiene " << newNode1.numKeys << 
        " elementos y Nodo 2 tiene " << newNode2.numKeys << " elementos\n";
        // newNode1 full
        if (newNode1.numKeys >= M - m + 1) {
            cout << "Nodo 1 lleno con " << newNode1.numKeys << " elementos\n";
            for (int i = 0; i < node.numKeys; i++) {
            	cout << "Nodo 1 tiene " << newNode1.numKeys << 
        " elementos y Nodo 2 tiene " << newNode2.numKeys << " elementos\n";
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
            cout << "Nodo 2 lleno con " << newNode2.numKeys << " elementos\n";
            for (int i = 0; i < node.numKeys; i++) {
            	cout << "Nodo 1 tiene " << newNode1.numKeys << 
        " elementos y Nodo 2 tiene " << newNode2.numKeys << " elementos\n";
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

        // Mark as placed
        placed[curr] = 1;
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
            a1 = getArea(aux1);
            a1 = getArea(aux2);
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
    }

    cout << "Es hoja el nodo original que se partio?: " << node.isLeaf << endl;
    // Update/reuse old node with overflow
    cout << "Reciclando nodo en " << nFile << " con " << newNode1.numKeys << " hijos\n";
    cout << "Es hoja nuevo nodo 1?: " << newNode1.isLeaf << endl;
    n.seekp(0, ios::beg);
    n.write((char *)&newNode1, sizeof(Rtree));
    n.close();

    // Create new node in secondary memory
    cout << "Creando nuevo nodo en " << N_CHILD << " con " << newNode2.numKeys << " hijos\n";
    cout << "Es hoja nuevo nodo 2?: " << newNode2.isLeaf << endl;
    fstream newN(path + to_string(N_CHILD), ios::out | ios::binary);
    newN.write((char *)&newNode2, sizeof(Rtree));
    newN.close();

    cout << "Actualizando al padre: " << 
    (fFile < 0 ? newRoot : fFile) << endl;
    // Update and save father
    if (fFile < 0) {
        father.MBR[0] = d1;
        father.children[0] = ROOT;
        father.numKeys = 1;
        father.isLeaf = 0;
        ROOT = newRoot;
    } else {
        father.MBR[childPos] = d1;
        f.seekp(0, ios::beg);
    }
    father.MBR[father.numKeys] = d2;
    father.children[father.numKeys++] = N_CHILD++;
    cout << "Padre quedo con " << father.numKeys << " elementos\n";
    f.write((char *)&father, sizeof(Rtree));
    f.close();
}

void insert(Rectangle C, int nFile, int fFile, int childPos,
            void (*split)(int, int, int)) {
    int smaller = 0;
    int growth = MAX_INT;
    int tmp, a1, a2;
    Rtree node;
    cout << "Abriendo archivo " << nFile << endl;
    cout << "Abriendo archivo " << path + to_string(nFile) << endl;
    fstream n(path + to_string(nFile), ios::in | ios::out | ios::binary);
    n.read((char *)&node, sizeof(Rtree));
    cout << node.isLeaf << endl;
    cout << node.numKeys << endl;
    if (!node.isLeaf) {
        cout << "Archivo " << nFile << " no es una hoja\n";
        for (int i = 0; i < node.numKeys; i++) {
            tmp = getMBRgrowth(C, node.MBR[i]);
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
        cout << "Actualizando MBR de hijo: " << smaller << "\n";
        fit(C, node.MBR[smaller], node.MBR[smaller]);
        cout << "Actualizando archivo\n";
        n.seekp(0, ios::beg);
        n.write((char *)&node, sizeof(Rtree));
        n.close();
        cout << "Insertar en hijo " << node.children[smaller] << endl;
        insert(C, node.children[smaller], nFile, smaller, split);
    } else {
        cout << "Archivo " << nFile << " SI es una hoja\n";
        cout << "Contiene " << node.numKeys << " elementos\n";
        node.MBR[node.numKeys++] = C;
        cout << "-------numKeys: " << node.numKeys << "\n";
        cout << "Actualizando archivo\n";
        n.seekp(0, ios::beg);
        n.write((char *)&node, sizeof(Rtree));
        n.close();
    }

    n.open(path + to_string(nFile), ios::in | ios::binary);
    n.read((char*)&node, sizeof(Rtree));
    n.close();
    // Check correct size
    cout << "Verificando invariante de " << nFile << " con " << node.numKeys << " elementos\n";
    if (node.numKeys > M) {
        cout << ">>>split: " << nFile << " " << fFile << " " << childPos
             << "\n";
        split(nFile, fFile, childPos);
    }
}

void insert(Rectangle C, int root, void (*split)(int, int, int)) {
    insert(C, root, -1, -1, split);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int n = 1 << atoi(argv[1]);
    cout << n << '\n';
    Rtree init;
    init.isLeaf = 1;
    init.numKeys = 0;

    cout << "Creando raiz inicial\n";
    fstream r(path + to_string(ROOT), ios::out | ios::trunc | ios::binary);
    r.write((char *)&init, sizeof(Rtree));
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

    cout << "Insertando en raiz utilizando QuadraticSplit\n";
    cout << "Insertando en raiz utilizando QuadraticSplit\n";
    auto begin = chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        cout << "Inserting " << i << "\n";
        // insert(rs[i], ROOT, QuadraticSplit);
        insert(rs[i], ROOT, QuadraticSplit);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - begin;
    cout << "Insercion de " << n << " elementos terminada en "
         << elapsed.count() * 1000 << "ms\n";
    cout << N_CHILD << " paginas creadas\n";
    return 0;
}
