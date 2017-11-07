#include "ternario.hpp"

Ternario::Ternario () {
	splitchar = '$';
	lowchild = equalchild = highchild = NULL;
}

vector<int> Ternario::search(string word){
	Ternario *p = this;
	int n = word.length();
	int i = 0;
	vector<int> res;
	while (p != NULL) {
		if (word[i] < p->splitchar) {
			p = p->lowchild;
		}
		else if (word[i] == p->splitchar) {
			if (++i == n) {
				res = p->values;
				break;
			}
			p = p->equalchild;
		}
		else {
			p = p->highchild;
		}
	}
	return res;
}

void Ternario::insert(string word, int index){
	int n = word.length();
	int i = 0;
	Ternario *p = this;
	while (i < n) {
		if (p->splitchar == '$') {
		    p->splitchar = word[i];
		}
		if (word[i] < p->splitchar) {
		    if (p->lowchild == NULL) {
		        p->lowchild = new Ternario();
		    }
			p = p->lowchild;
		}
		else if (word[i] == p->splitchar) {
			if (++i < n) {
				if (p->equalchild == NULL) {
			        p->equalchild = new Ternario();
			    }
				p = p->equalchild;
			}
		}
		else {
		    if (p->highchild == NULL) {
		        p->highchild = new Ternario();
		    }
			p = p->highchild;
		}
	}
	if (p->values.size() < 100) {
		p->values.push_back(index);
	}
}

char Ternario::getSplitChar(){
	return this->splitchar;
}

void display(vector<int> &v) {
    int n = v.size();
    for(int i = 0; i < n; i++)
    {
        cout << v[i] << " ";
    }
    cout << endl;
}
/*
int main() {
	Ternario root;
	cout << root.getSplitChar() << endl;
	root.insert("hola", 1);
	root.insert("hola", 2);
	root.insert("hola", 3);
	vector<int> v1 = root.search("hola");
	vector<int> v2 = root.search("chao");
	cout << "Searching \"hola\", result = ";
	display(v1);
	cout << "Searching \"chao\", result = ";
	display(v2);
	return 0;
}*/