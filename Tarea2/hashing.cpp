#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>

#include "hashing.hpp"

Hashing::Hashing(int size, int seed) {
	this->size = size;
	keys = new string[size];
	values = new vector<int>[size];
	this->seed = seed;
	for (int i = 0; i < this->size; i++) {
		keys[i] = "";
	}
}

Hashing::Hashing(int size) {
	this->size = size;
	keys = new string[size];
	values = new vector<int>[size];
	this->seed = 131;
	for (int i = 0; i < this->size; i++) {
		keys[i] = "";
	}
}

Hashing::Hashing() {
	size = 1000;
	keys = new string[size];
	values = new vector<int>[size];
	seed = 131;
	for (int i = 0; i < this->size; i++) {
		keys[i] = "";
	}
}

Hashing::~Hashing() {
	delete[] keys;
	delete[] values;
}

int Hashing::hash(string s) {
	unsigned int sum = 0;
	for (unsigned int i = 0; i < s.length(); i++) {
		sum = (sum * this->seed) + s[i];
	}
	return (int) (sum % this->size);
}

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

int Hashing::count(string s) {
	vector<int> res = this->search(s);
	return res.size();
}