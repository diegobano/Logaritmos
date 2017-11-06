#include "ternario.hpp"

Ternario::Ternario () {
	splitchar = '$';
	lowchild = equalchild = highchild = NULL;
}

Ternario::Ternario (char c) {
	splitchar = c;
	lowchild = equalchild = highchild = NULL;
}

int Ternario::search(string word){
	Ternario *p = this;
	int n = word.length();
	int i = 0;
	while (p != NULL) {
		if (word[i] < p->splitchar) {
			p = p->lowchild;
		}
		else if (word[i] == p->splitchar) {
			if (++i == n) {
				return 1;
			}
			p = p->equalchild;
		}
		else {
			p = p->highchild;
		}
	}
	return 0;
}

void Ternario::insert(string word){
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
		    if (p->equalchild == NULL) {
		        p->equalchild = new Ternario();
		    }
			p = p->equalchild;
			i++;
		}
		else {
		    if (p->highchild == NULL) {
		        p->highchild = new Ternario();
		    }
			p = p->highchild;
		}
	}
}
/*
int main() {
	Ternario root;
	cout << root->splitchar << endl;
	root.insert("hola");
	cout << "Searching \"hola\", result = "  << root.search("hola") << endl;
	cout << "Searching \"chao\", result = "  << root.search("chao") << endl ;
	return 0;

}*/