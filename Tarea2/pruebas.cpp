#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>

#include "dictionary.hpp"
#include "hashing.hpp"
#include "patricia.hpp"
#include "ternario.hpp"



int main(int argc, char const *argv[])
{
	typedef unsigned int uint;
	//Useful variables.
	ifstream text;
	fstream formatted;
	string word, fword;
	int filesize = 0, unique_count, ejemplos = 13, next, pos;
	uint i = 0;
	chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;
    Hashing **h;
    Ternario **t;
    Ptrie **p;
    vector<string> words, unique_words;
    vector<int> positions;

    h = new Hashing*[ejemplos];
    t = new Ternario*[ejemplos];
    p = new Ptrie*[ejemplos];

    for (int k = 0; k < ejemplos; k++) {
		text.open("ejemplo"+to_string(k+1)+".txt");
		formatted.open("formateado"+to_string(k+1)+".txt", ios::in | ios::out | ios::trunc);
		cout << "Formateando texto " << k << "\n";

		while (text >> word) {
			transform(word.begin(), word.end(), word.begin(), ::tolower);
			i = 0;
			while (i < word.length()) {
				if (word[i] == '.' || 
					word[i] == ',' || 
					word[i] == ':' || 
					word[i] == ';' || 
					word[i] == '-' || 
					word[i] == '<' || 
					word[i] == '>' ||
					word[i] == '\"' || 
					word[i] == '\'' || 
					word[i] == '(' || 
					word[i] == ')' || 
					word[i] == '{' || 
					word[i] == '}' || 
					word[i] == '[' || 
					word[i] == ']' || 
					word[i] == '_' || 
					word[i] == '+' || 
					word[i] == '=' || 
					word[i] == '*' || 
					word[i] == '!' || 
					word[i] == '~' || 
					word[i] == '@' || 
					word[i] == '$' || 
					word[i] == '\%' || 
					word[i] == '^' || 
					word[i] == '&' || 
					word[i] == '/' || 
					word[i] == '\\' || 
					word[i] == '|' || 
					word[i] == '#' ||
					word[i] == '?') {
					word.erase(i, 1);
				} else {
					i++;
				}
			}

			if (word.length() > 0) {
				filesize++;
				formatted << word << " ";
			}
		}
		text.close();

		formatted.seekp(0, ios::beg);
		h[k] = new Hashing(filesize);
		//t[k] = new Ternario();
		p[k] = new Ptrie();

		words.clear();
		positions.clear();
		unique_words.clear();

		pos = 0;
		while (formatted >> word) {
			words.push_back(word);
			positions.push_back(pos);
			pos += word.length() + 1;
		}

		cout << "Archivo con " << (int) (filesize) << " palabras validas.\n";
		int pos = 0;
		begin = chrono::high_resolution_clock::now();

		for (uint j = 0; j < words.size(); j++) {
			h[k]->insert(words.at(j), positions.at(j));
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		unique_count = h[k]->unique_count();
		unique_words = h[k]->unique_values();

		cout << unique_count << " palabras distintas\n\n";
		cout << "Tiempos de construccion:\n";
		cout << "Hashing: " << elapsed.count() * 1000 << " ms\n";
		/*
		pos = 0;
		begin = chrono::high_resolution_clock::now();
		for (uint j = 0; j < words.size(); j++) {
			t[k]->insert(words.at(j));
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		cout << "Ternario: " << elapsed.count() * 1000 << " ms\n";
		*/
		pos = 0;
		begin = chrono::high_resolution_clock::now();
		for (uint j = 0; j < words.size(); j++) {
			p[k]->insert(words.at(j), positions.at(j));
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		cout << "Patricia: " << elapsed.count() * 1000 << " ms\n";

		cout << "hashing_" << k << " = [";
		for (int j = 0; j < unique_count / 10; j++) {
			next = rand() % unique_count;
			begin = chrono::high_resolution_clock::now();
			h[k]->search(words.at(next));
			end = chrono::high_resolution_clock::now();
			elapsed = end - begin;
			cout << words.at(next).length() << ", " << elapsed.count() << ";\n";
		}
		cout << "]\n";

		formatted.seekp(0, ios::beg);
		formatted.close();
		filesize = 0;
	}

	
	return 0;
}
