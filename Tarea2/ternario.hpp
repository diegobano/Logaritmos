#ifndef _TERNARIO_INCLUDED_
#define _TERNARIO_INCLUDED_

#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <fstream>
#include <iostream>
#include <vector>


using namespace std;

class Ternario {
	private:
		char splitchar;
		vector<int> values;
		Ternario *lowchild;
		Ternario *equalchild;
		Ternario *highchild;
	public:
		Ternario ();
		void insert(string, int);
		vector<int> search(string);
		char getSplitChar();
};

#endif
