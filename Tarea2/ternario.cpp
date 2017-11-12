#include "ternario.hpp"

Ternario::Ternario () {
	splitchar = '$';
	lowchild = equalchild = highchild = NULL;
}

vector<int> Ternario::search(string word, int text){
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
				if (text == 0) {
					res = p->text1;
				}
				else if (text == 1){
					res = p->text2;
				}
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

void Ternario::insert(string word, int index, int text){
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
	if (text == 0) {
		p->text1.push_back(index);
	}
	else if (text == 1) {
		p->text2.push_back(index);
	}
}


char Ternario::getSplitChar(){
	return this->splitchar;
}

vector<int> Ternario::getText1(){
	return this->text1;
}

vector<int> Ternario::getText2(){
	return this->text2;
}

Ternario* Ternario::getLowchild(){
	return this->lowchild;
}

Ternario* Ternario::getEqualchild(){
	return this->equalchild;
}

Ternario* Ternario::getHighchild(){
	return this->highchild;
}


int similarityF(Ternario* node) {
	if (node == NULL) {
		return 0;
	}
	int sizetext1 = node->getText1().size();
	int sizetext2 = node->getText2().size();
	int result = abs(sizetext1 - sizetext2);
	int reslow = similarityF(node->getLowchild());
	int resequal = similarityF(node->getEqualchild());
	int reshigh = similarityF(node->getHighchild());
	return result + reslow + resequal + reshigh;
}

int Ternario::similarity() {
	Ternario* p = this;
	return similarityF(p);
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
	root.insert("hola", 1, 1);
	root.insert("hola", 2, 1);
	root.insert("hola", 3, 2);
	root.insert("hola", 4, 2);
	root.insert("hola", 5, 2);
	vector<int> v1 = root.search("hola", 1);
	vector<int> v2 = root.search("hola", 2);
	vector<int> v3 = root.search("chao", 1);
	cout << "Searching \"hola\" in text 1, result = ";
	display(v1);
	cout << "Searching \"hola\" in text 2, result = ";
	display(v2);
	cout << "Searching \"chao\" in text 1, result = ";
	display(v3);

	int similar = root.similarity();
	cout << "Similarity = " << similar << endl ;
	return 0;
}*/