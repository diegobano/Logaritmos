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

void similitud(Dictionary *d, vector<string> &words, vector<int> &positions, int n, int k) {
	int ss;
	double res;
	chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;

	for (int i = 0; i < words.size(); i++) {
		d->insert(words.at(i), positions.at(i), 1);
		//cout << "Insertando " << i << "/" << words.size() << endl;
	}
	//cout << "Calculando similitud\n";

	begin = chrono::high_resolution_clock::now();
	ss = d->similarity();
	res = 1 - (((double) ss) / ((double) n));
	end = chrono::high_resolution_clock::now();
	elapsed = end - begin;
	cout << "sum = " << ss << "; total = " << n << endl;
	cout << d->getName() << "similarity_" << k << " = " << elapsed.count() << " : " << res << "\n";
}

void doSearch(Dictionary *d, vector<string> words, int n, int k) {
	int next;
	double search_times[30][2];

	chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;
	for (int j = 0; j < 30; j++) {
		search_times[j][0] = 0;
		search_times[j][1] = 0;
	}

	cout << d->getName() << k << " = [";
	for (int j = 0; j < n / 10; j++) {
		next = rand() % n;
		begin = chrono::high_resolution_clock::now();
		d->search(words.at(next), 0);
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		search_times[words.at(next).length()][0] += elapsed.count() * 1000;
		search_times[words.at(next).length()][1]++;
	}
	for (int j = 0; j < 30; j++) {
		if (search_times[j][1] != 0) {
			cout << j << ", " << search_times[j][0]/search_times[j][1] << ";\n";
		}
	}
	cout << "]\n";
}


int main(int argc, char const *argv[])
{
	typedef unsigned int uint;
	//Useful variables.
	ifstream text;
	fstream formatted;
	string word, fword;
	string *names;
	int filesize = 0, unique_count, ejemplos, pos;
	uint i = 0;

	chrono::high_resolution_clock::time_point begin, end;
    chrono::duration<double> elapsed;
    Hashing **h;
    Ternario **t;
    Patricia **p;
    vector<string> words, unique_words, r_words;
    vector<int> positions;

    if (argc > 1) {
    	cout << "Utilizando textos entregados por consola\n\n";
    	ejemplos = argc - 1;
    	names = new string[ejemplos];
    	copy_n(argv+1, ejemplos, names);
    } else {
    	cout << "Utilizando textos por defecto\n\n";
    	ejemplos = 13;
    	names = new string[ejemplos];
    	for (int k = 0; k < ejemplos; k++) {
    		names[k] = "ejemplo"+to_string(k+1)+".txt";
    	}
    }

    h = new Hashing*[ejemplos];
    t = new Ternario*[ejemplos];
    p = new Patricia*[ejemplos];
    cout << ejemplos << endl;
    for (int k = 0; k < ejemplos; k++) {
    	//Formateo del archivo de entrada
		text.open(names[k]);
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
		t[k] = new Ternario();

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
			h[k]->insert(words.at(j), positions.at(j), 0);
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		unique_count = h[k]->unique_count();
		unique_words = h[k]->unique_values();

		cout << unique_count << " palabras distintas\n\n";
		cout << "Tiempos de construccion:\n";
		cout << "Hashing: " << elapsed.count() * 1000 << " ms\n";
		
		pos = 0;
		begin = chrono::high_resolution_clock::now();
		for (uint j = 0; j < words.size(); j++) {
			t[k]->insert(words.at(j), positions.at(j), 0);
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		cout << "Ternario: " << elapsed.count() * 1000 << " ms\n";
		
		p[k] = new Patricia(unique_count);
		pos = 0;
		begin = chrono::high_resolution_clock::now();
		for (uint j = 0; j < words.size(); j++) {
			p[k]->insert(words.at(j), positions.at(j), 0);
			pos += word.length() + 1;
		}
		end = chrono::high_resolution_clock::now();
		elapsed = end - begin;
		cout << "Patricia: " << elapsed.count() * 1000 << " ms\n\n";

		cout << "Tiempos de busqueda:\n";

		doSearch(h[k], words, unique_count, k);
		doSearch(t[k], words, unique_count, k);
		doSearch(p[k], words, unique_count, k);

		if (k > 0) {
			cout << "\nTiempos de recorrido:\n";
			similitud(h[k-1], words, positions, filesize, k);
			similitud(t[k-1], words, positions, filesize, k);
			similitud(p[k-1], words, positions, filesize, k);
		}
		cout << "Iteracion actual: " << k << endl;
		formatted.seekp(0, ios::beg);
		formatted.close();
		filesize = 0;
	}

	
	return 0;
}
