#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <vector>

#include "hashing.hpp"
#include "patricia.hpp"
#include "ternario.hpp"

int main(int argc, char const *argv[])
{
	ifstream text, text2, text3;
	fstream formatted, form2, form3;
	string word, fword;
	int filesize = 0;
	int ejemplos = 13;
	unsigned int i = 0;
	chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;
    Hashing *h;

    h = new Hashing[ejemplos];

    for (int k = 0; k < ejemplos; k++) {
		text.open("ejemplo"+to_string(i+1)+".txt");
		formatted.open("formateado"+to_string(i+1)+".txt", ios::in | ios::out | ios::trunc);
		cout << "Formateando texto " << i << "\n";

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
		h[i] = new Hashing(2 * filesize);

		cout << "Creando Hashing con " << (int) (2 * filesize) << " llaves posibles\n";
		int pos = 0;
		begin = chrono::high_resolution_clock::now();
		while (formatted >> word) {
			h[i].insert(word, pos);
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		cout << h[i].unique_count() << " palabras distintas : " << elapsed.count() * 1000 << "ms\n";
	}

	
	return 0;
}
