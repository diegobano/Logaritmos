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

int main(int argc, char const *argv[])
{
	ifstream text, text2, text3;
	fstream formatted, form2, form3;
	string word, fword;
	int filesize = 0;
	unsigned int i = 0;
	chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;


	if (argc == 2) {
		text.open("ejemplo.txt");
		formatted.open("formateado.txt", ios::in | ios::out | ios::trunc);
		text2.open("ejemplo3.txt");

	} else if (argc == 3) {

	} else if (argc == 4) {

	} else {
		text.open("ejemplo.txt");

	}

	cout << "Formateando texto\n";

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
	Hashing h((int) (2.5 * filesize));

	cout << "Creando Hashing con " << (int) (2.5 * filesize) << " llaves posibles\n";
	int pos = 0;


	begin = chrono::high_resolution_clock::now();
	while (formatted >> word) {
		h.insert(word, pos);
		pos += word.length() + 1;
	}
	end = chrono::high_resolution_clock::now();
	elapsed = end - begin;
	cout << filesize << " palabras : " << elapsed.count() * 1000 << "ms\n";
	cout << "Contando ocurrencias de jason\n";
	cout << h.count("jason") << "\n";
	cout << "Tamaño: " << sizeof(h) << "\n";
	return 0;
}
