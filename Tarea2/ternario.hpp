#ifndef _TERNARIO_INCLUDED_
#define _TERNARIO_INCLUDED_

#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <fstream>
#include <iostream>

using namespace std;

class Ternario {
	private:
		char splitchar;
		Ternario *lowchild;
		Ternario *equalchild;
		Ternario *highchild;
	public:
		Ternario ();
		Ternario (char);
		void insert(string);
		int search(string);
};

#endif
