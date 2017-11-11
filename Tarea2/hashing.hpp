#ifndef _HASHING_INCLUDED_
#define _HASHING_INCLUDED_

#include <algorithm>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>
#include "dictionary.hpp"

using namespace std;

class Hashing : public Dictionary
{
private:
	int seed, size;
	string *keys;
	vector<int> **values;
public:
	Hashing(int, int);
	Hashing(int);
	Hashing();
	~Hashing();
	int hash(string);
	void insert(string, int, int);
	vector<int> search(string, int);
	int count(string);
	int unique_count();
	vector<string> unique_values();
	string getName() {return "hashing_";};
};

#endif
