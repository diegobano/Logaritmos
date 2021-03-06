#ifndef _TERNARIO_INCLUDED_
#define _TERNARIO_INCLUDED_

#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <fstream>
#include <iostream>
#include <vector>
#include "dictionary.hpp"


using namespace std;

class Ternario : public Dictionary {
	private:
		char splitchar;
		vector<int> text1;
		vector<int> text2;
		Ternario *lowchild;
		Ternario *equalchild;
		Ternario *highchild;
	public:
		Ternario();
		void insert(string, int, int);
		vector<int> search(string, int);
		char getSplitChar();
		string getName() {return "ternario_";};
		int similarity();
		vector<int> getText1();
		vector<int> getText2();
		Ternario* getLowchild();
		Ternario* getEqualchild();
		Ternario* getHighchild();


};

#endif
