#ifndef _DICTIONARY_INCLUDED_
#define _DICTIONARY_INCLUDED_

#include <string>
#include <vector>

using namespace std;

class Dictionary
{
public:
	virtual void insert(string, int, int) = 0;
	virtual vector<int> search(string, int) = 0;
	virtual string getName() = 0;
	virtual int similarity() = 0;
};

#endif