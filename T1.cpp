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

#define M 203
#define m 81
#define MAX_INT numeric_limits<int>::max()

int ROOT = 1;
int N_CHILD = 2;
int disk = 0;
string SharedPath = "Rtree/";
string Qpath = "QuadraticRtree/";
string Lpath = "LinearRtree/";

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

void searchRec(Rectangle C, int nodeFile, list<Rectangle> &result, string path) {
    Rtree node;
    Rectangle r;
    fstream n(path + to_string(nodeFile), ios::in | ios::out | ios::binary);
    n.read((char *)&node, sizeof(Rtree));
    n.close();
    disk += 1;
    for (int i = 0; i < node.numKeys; i++) {
    	r = node.MBR[i];
        if (intersect(C, node.MBR[i])) {
            if (node.isLeaf) {
                result.push_back(node.MBR[i]);
            } else
                searchRec(C, node.children[i], result, path);
        }
    }
}

list<Rectangle> search(Rectangle C, int root, string path) {
    list<Rectangle> result;
    searchRec(C, root, result, path);
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

void LinearSplit(int nodeFile, int fatherFile, int childPos, string path) {

    Rtree node, father;
    fstream fNode(path + to_string(nodeFile), ios::in | ios::out | ios::binary);
    fNode.read((char *)&node, sizeof(Rtree));

    // LinearSplit
    int xMinRange, xMaxRange, yMinRange, yMaxRange;
    int xMinHigh, xMaxLow, yMinHigh, yMaxLow;
    xMinRange = yMinRange = xMinHigh = yMinHigh = MAX_INT;
    xMaxRange = yMaxRange = xMaxLow = yMaxLow = -MAX_INT;

    int xMinHighIndex, xMaxLowIndex, yMinHighIndex, yMaxLowIndex;
    xMinHighIndex = xMaxLowIndex = yMinHighIndex = yMaxLowIndex = 0;

    vector<int> MBRset;
    for (int i = 0; i < node.numKeys; i++) {
        MBRset.push_back(i);
        if (node.MBR[i].xMin < xMinRange)
            xMinRange = node.MBR[i].xMin;
        if (node.MBR[i].xMax > xMaxRange)
            xMaxRange = node.MBR[i].xMax;
        if (node.MBR[i].yMin < yMinRange)
            yMinRange = node.MBR[i].yMin;
        if (node.MBR[i].yMax > yMaxRange)
            yMaxRange = node.MBR[i].yMax;
        if (node.MBR[i].xMax < xMinHigh) {
            xMinHigh = node.MBR[i].xMax;
            xMinHighIndex = i;
        }
        if (node.MBR[i].xMin > xMaxLow) {
            xMaxLow = node.MBR[i].xMin;
            xMaxLowIndex = i;
        }
        if (node.MBR[i].yMax < yMinHigh) {
            yMinHigh = node.MBR[i].yMax;
            yMinHighIndex = i;
        }
        if (node.MBR[i].yMin > yMaxLow) {
            yMaxLow = node.MBR[i].yMin;
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
    // cout << "firstIndex: " << firstIndex << "\nsecondIndex: " << secondIndex
    // << '\n';
    Rtree first, second;
    Rectangle firstRec, secondRec;

    first.isLeaf = node.isLeaf;
    first.numKeys = 1;
    first.MBR[0] = firstRec = node.MBR[firstIndex];
    first.children[0] = node.children[firstIndex];

    second.isLeaf = node.isLeaf;
    second.numKeys = 1;
    second.MBR[0] = secondRec = node.MBR[secondIndex];
    second.children[0] = node.children[secondIndex];

    int firstGrowth, secondGrowth;
    int firstArea, secondArea;
    int maxSize = M + 1 - m;

    for (vector<int>::iterator it = MBRset.begin(); it != MBRset.end(); ++it) {
        if (first.numKeys == maxSize) {
            // add to second
            addMBRtoRectangle(second, node.MBR[*it], node.children[*it],
                              secondRec);
        } else if (second.numKeys == maxSize) {
            // add to first
            addMBRtoRectangle(first, node.MBR[*it], node.children[*it],
                              firstRec);
        } else {
            firstGrowth = getMBRgrowth(node.MBR[*it], firstRec);
            secondGrowth = getMBRgrowth(node.MBR[*it], secondRec);
            if (firstGrowth == secondGrowth) {
                firstArea = getMBRarea(node.MBR[*it], firstRec);
                secondArea = getMBRarea(node.MBR[*it], secondRec);
                if (firstArea == secondArea) {
                    if (first.numKeys == second.numKeys) {
                        if (rand() % 2) {
                            addMBRtoRectangle(first, node.MBR[*it],
                                              node.children[*it], firstRec);
                            continue;
                        }
                    } else if (first.numKeys < second.numKeys) {
                        addMBRtoRectangle(first, node.MBR[*it],
                                          node.children[*it], firstRec);
                        continue;
                    }
                } else if (firstArea < secondArea) {
                    addMBRtoRectangle(first, node.MBR[*it],
                                      node.children[*it], firstRec);
                    continue;
                }
            } else if (firstGrowth < secondGrowth) {
                addMBRtoRectangle(first, node.MBR[*it], node.children[*it],
                                  firstRec);
                continue;
            }
            addMBRtoRectangle(second, node.MBR[*it], node.children[*it],
                              secondRec);
        }
    }
    // cout << "second: " << second.numKeys << "\nfirst: " << first.numKeys
    // << '\n';
    fNode.seekp(0, ios::beg);
    fNode.write((char *)&first, sizeof(Rtree));
    fNode.close();
    // cout << "N_CHILD: " << N_CHILD << '\n';
    fstream fNewNode(path + to_string(N_CHILD),
                     ios::out | ios::trunc | ios::binary);
    fNewNode.write((char *)&second, sizeof(Rtree));
    fNewNode.close();

    if (fatherFile == -1) {
        // cout << "fatherFile -1\n";
        Rtree newRoot;
        newRoot.isLeaf = false;
        newRoot.numKeys = 2;
        newRoot.MBR[0] = firstRec;
        newRoot.children[0] = nodeFile;
        newRoot.MBR[1] = secondRec;
        newRoot.children[1] = N_CHILD;
        ROOT = ++N_CHILD;
        fstream fNewRoot(path + to_string(ROOT),
                         ios::out | ios::trunc | ios::binary);
        fNewRoot.write((char *)&newRoot, sizeof(Rtree));
        fNewRoot.close();
        // cout << "ROOT: " << ROOT << '\n';
    } else {

        fstream fFather(path + to_string(fatherFile),
                        ios::in | ios::out | ios::binary);
        fFather.read((char *)&father, sizeof(Rtree));
        father.MBR[childPos] = firstRec;
        father.MBR[father.numKeys] = secondRec;
        father.children[father.numKeys] = N_CHILD;
        father.numKeys++;
        fFather.seekp(0, ios::beg);
        fFather.write((char *)&father, sizeof(Rtree));
        fFather.close();
    }
    N_CHILD++;
}

void QuadraticSplit(int nodeFile, int fatherFile, int childPos, string path) {
    Rectangle d1, d2, aux1, aux2;
    int pos1, pos2, curr, newRoot;
    int max = 0, ua, d, a1, a2;
    Rtree newNode1, newNode2;
    Rtree node, father;
    fstream f, n;

    // Reading father and current node
    if (fatherFile < 0) {
        newRoot = N_CHILD++;
        f.open(path + to_string(newRoot), ios::out | ios::trunc | ios::binary);
    } else {
        f.open(path + to_string(fatherFile), ios::in | ios::out | ios::binary);
        f.read((char *)&father, sizeof(Rtree));
    }
    n.open(path + to_string(nodeFile), ios::in | ios::out | ios::binary);
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
    	// Node 1 full, insert everything else in node 2
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

        // Node 2 full, insert everything else in node 1
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

        // Pick best MBR to insert
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

        // Add selected Rectangle to optimal MBR
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

    // Update/reuse old node with overflow
    n.seekp(0, ios::beg);
    n.write((char *)&newNode1, sizeof(Rtree));
    n.close();

    // Create new node in secondary memory
    fstream newN(path + to_string(N_CHILD), ios::out | ios::binary);
    newN.write((char *)&newNode2, sizeof(Rtree));
    newN.close();

    // Update and save father
    if (fatherFile < 0) {
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
    f.write((char *)&father, sizeof(Rtree));
    f.close();
}

void insert(Rectangle C, int nodeFile, int fatherFile, int childPos,
            void (*split)(int, int, int, string), string path) {
    int smaller = 0;
    int growth = MAX_INT;
    int tmp, a1, a2;
    Rtree node;
    fstream n(path + to_string(nodeFile), ios::in | ios::out | ios::binary);
    n.read((char *)&node, sizeof(Rtree));
    if (!node.isLeaf) {
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
        fit(C, node.MBR[smaller], node.MBR[smaller]);
        n.seekp(0, ios::beg);
        n.write((char *)&node, sizeof(Rtree));
        n.close();
        insert(C, node.children[smaller], nodeFile, smaller, split, path);
    } else {
        node.MBR[node.numKeys++] = C;
        n.seekp(0, ios::beg);
        n.write((char *)&node, sizeof(Rtree));
        n.close();
    }

    n.open(path + to_string(nodeFile), ios::in | ios::binary);
    n.read((char*)&node, sizeof(Rtree));
    n.close();
    // Check correct size
    if (node.numKeys > M) {
        split(nodeFile, fatherFile, childPos, path);
    }
}

void insert(Rectangle C, int root, void (*split)(int, int, int, string), string path) {
    insert(C, root, -1, -1, split, path);
}

double getFilledPercentage(string path) {
	double sum;
	fstream n;
	Rtree node;

	for (int i = 1; i < N_CHILD; i++) {
		if (i == ROOT) continue;
		n.open(path + to_string(i), ios::in | ios::binary);
		n.read((char*)&node, sizeof(Rtree));
		n.close();
		sum += (double)(node.numKeys * 100) / (double)M;
		if (node.numKeys < m && i != ROOT) {
			cout << "Nodo " << i << " tiene " << node.numKeys << " elementos\n";
		}
	}

	return sum / (double)(N_CHILD - 1);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;
    Rtree init;
    Rectangle r;
	list<Rectangle> res;
    double fill;
    int n, sum;
    fstream root;

    // Creating initial empty roots in all folders
    init.isLeaf = 1;
    init.numKeys = 0;

    root.open(SharedPath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
    root.write((char*)&init, sizeof(Rtree));
    root.close();

    root.open(Qpath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
    root.write((char*)&init, sizeof(Rtree));
    root.close();

    root.open(Lpath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
    root.write((char*)&init, sizeof(Rtree));
    root.close();

	cout << "LinearSplit:\n";
	for (int k = 9; k < 20; k++) {
		n = 1 << k;
    	begin = chrono::high_resolution_clock::now();
	    for (int i = 0; i < n; i++) {
	        r.xMin = rand() % 500001;
	        r.yMin = rand() % 500001;
	        r.xMax = r.xMin + rand() % 100 + 1;
	        r.yMax = r.yMin + rand() % 100 + 1;

	        insert(r, ROOT, LinearSplit, SharedPath);
	    }
    	end = chrono::high_resolution_clock::now();
    	elapsed = end - begin;
    	fill = getFilledPercentage(SharedPath);
    	cout << k << " : " << elapsed.count() * 1000 << "ms : ";
    	cout << fill << "\% : " << (N_CHILD - 1) * (20 * M + 8) << " bytes\n";
    	// Reseting global variables and emptying root
    	N_CHILD = 2;
    	ROOT = 1;
    	root.open(SharedPath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
	    root.write((char*)&init, sizeof(Rtree));
	    root.close();
	}

	cout << "QuadraticSplit:\n";
	for (int k = 9; k < 20; k++) {
		n = 1 << k;
    	begin = chrono::high_resolution_clock::now();
	    for (int i = 0; i < n; i++) {
	        r.xMin = rand() % 500001;
	        r.yMin = rand() % 500001;
	        r.xMax = r.xMin + rand() % 100 + 1;
	        r.yMax = r.yMin + rand() % 100 + 1;

	        insert(r, ROOT, QuadraticSplit, SharedPath);
	    }
    	end = chrono::high_resolution_clock::now();
    	elapsed = end - begin;
    	fill = getFilledPercentage(SharedPath);
    	cout << k << " : " << elapsed.count() * 1000 << "ms : ";
    	cout << fill << "\% : " << (N_CHILD - 1) * (20 * M + 8) << " bytes\n";
    	// Reseting global variables and emptying root
    	N_CHILD = 2;
    	ROOT = 1;
    	root.open(SharedPath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
	    root.write((char*)&init, sizeof(Rtree));
	    root.close();
	}
	
	cout << "Search test:\n";
	for (int k = 9; k < 16; k++) {
		n = 1 << k;
		cout << "k = " << k << endl;
		Rectangle rs[n];
		Rectangle sr[n/10];
	    for (int i = 0; i < n; i++) {
	        rs[i].xMin = rand() % 500001;
	        rs[i].yMin = rand() % 500001;
	        rs[i].xMax = rs[i].xMin + rand() % 100 + 1;
	        rs[i].yMax = rs[i].yMin + rand() % 100 + 1;
	    }


		for (int i = 0; i < n/10; i++) {
	        sr[i].xMin = rand() % 500001;
	        sr[i].yMin = rand() % 500001;
	        sr[i].xMax = sr[i].xMin + rand() % 300 + 1;
	        sr[i].yMax = sr[i].yMin + rand() % 300 + 1;
		}

	    for (int i = 0; i < n; i++)
	        insert(rs[i], ROOT, LinearSplit, Lpath);    

	    sum = 0;
		begin = chrono::high_resolution_clock::now();
		for (int i = 0; i < n/10; i++){
		    res = search(sr[i], ROOT, Lpath);
		    sum += res.size();
		}
	    end = chrono::high_resolution_clock::now();
		elapsed = end - begin;

	    cout << "Linear : " << elapsed.count() * 1000 << "ms : ";
		cout << (double)disk / (n/10) << " acs/srch : " <<  sum << " found\n";

	    N_CHILD = 2;
	    ROOT = 1;
	    disk = 0;

	    root.open(Lpath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
	    root.write((char*)&init, sizeof(Rtree));
	    root.close();

		for (int i = 0; i < n; i++)
	        insert(rs[i], ROOT, QuadraticSplit, Qpath);

		sum = 0;
		begin = chrono::high_resolution_clock::now();
		for (int i = 0; i < n/10; i++){
		    res = search(sr[i], ROOT, Qpath);
		    sum += res.size();
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;

	    cout << "Quadratic : " << elapsed.count() * 1000 << "ms : ";
		cout << (double)disk / (n/10) << " acs/srch : " << sum << " found\n";
		
		N_CHILD = 2;
	    ROOT = 1;
	    disk = 0;
	    
	    root.open(Qpath + to_string(ROOT), ios::out | ios::trunc | ios::binary);
	    root.write((char*)&init, sizeof(Rtree));
	    root.close();
	}

    return 0;
}
