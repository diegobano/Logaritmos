#ifndef _HASHING_INCLUDED_
#define _HASHING_INCLUDED_

#include <algorithm>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>

using namespace std;

class Hashing
{
private:
	int seed, size;
	string *keys;
	vector<int> *values;
public:
	Hashing(int, int);
	Hashing(int);
	Hashing();
	~Hashing();
	int hash(string);
	void insert(string, int);
	vector<int> search(string);
	int count(string);
	int unique_count();
};

#endif
