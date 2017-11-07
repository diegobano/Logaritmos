#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>

#include "hashing.hpp"

/*
Hashing constructor
	size (int): Size of the hash table
	seed (int): Arbitrary number to use in the hash value generator.
Creates a Hashing table with the given size and seed values.
*/
Hashing::Hashing(int size, int seed) {
	this->size = size;
	keys = new string[size];
	values = new vector<int>[size];
	this->seed = seed;
	for (int i = 0; i < this->size; i++) {
		keys[i] = "";
	}
}

/*
Hashing constructor:
	size (int): Size of the hash table
Creates a Hashing table with default arbitrary seed and a given size.
*/
Hashing::Hashing(int size) {
	this->size = size;
	keys = new string[size];
	values = new vector<int>[size];
	this->seed = 131;
	for (int i = 0; i < this->size; i++) {
		keys[i] = "";
	}
}

/*
Hashing constructor:
Creates a Hashing table with default arbitrary seed and size.
*/
Hashing::Hashing() {
	size = 1000;
	keys = new string[size];
	values = new vector<int>[size];
	seed = 131;
	for (int i = 0; i < this->size; i++) {
		keys[i] = "";
	}
}

/*
Destructor
*/
Hashing::~Hashing() {
	delete[] keys;
	delete[] values;
}

/*
hash:
	s (string): String used to calculate hash value.
	return The value of the hash for the given string.
*/
int Hashing::hash(string s) {
	unsigned int sum = 0;
	for (unsigned int i = 0; i < s.length(); i++) {
		sum = (sum * this->seed) + s[i];
	}
	return (int) (sum % this->size);
}

/*
insert:
	s (string): String to be inserted in hashing table.
	pos (int): Position of the string in the text it belongs to (0 if it doesn't belong to any text).
Inserts the given string to the Hashing table using the Linear Probing strategy on collisioning hash values.
*/
void Hashing::insert(string s, int pos) {
	int index = this->hash(s);
	int ini = index;

	while (1) {
		if (this->values[index].size() <= 0) {
			this->keys[index] = s;
			this->values[index].push_back(pos);
			break;
		} else if (this->keys[index] == s) {
			this->values[index].push_back(pos);
			break;
		}else {
			index = (index + 1) % this->size;
			if (index == ini) exit(1);
		}
	}
}

/*
searh:
	s (string): String value to look for in the table.
Returns the positions vector for the given string (empty vector if the string is not in the table)
*/
vector<int> Hashing::search(string s) {
	int index = this->hash(s);
	int ini = index;
	vector<int> res;
	while (1) {
		if (this->keys[index] == s) {
			res =  this->values[index];
			break;
		} else if (this->keys[index] == "" || index == ini){
			break;
		} else {
			index = (index + 1) % this->size;
		}
	}
	return res;
}

/*
count:
	s (string): String value to look for in the table.
Returns the amount of ocurrences of the string in the text.
*/
int Hashing::count(string s) {
	vector<int> res = this->search(s);
	return res.size();
}

/*
unique_count:
Returns the number of different words inserted into the object.
*/
int Hashing::unique_count() {
	int cnt = 0;
	for (int i = 0; i < this->size; i++) {
		if (this->values[i].size() > 0)
			cnt++;
	}
	return cnt;
}

/*
unique_values:
Returns a vector containing all the unique values stored in its table.
*/
vector<string> Hashing::unique_values() {
	vector<string> words;
	for (int i = 0; i < this->size; i++)
		if (this->values[i].size() > 0)
			words.push_back(this->keys[i]);
	return words;
}