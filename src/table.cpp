#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include "helper.h"
#include "table.h"

using namespace std;

table::table(const string& _filename) : filename(_filename), annot_sizes(), ncol(0), nrow(0){
	readfile(filename);
}

/**
 * Print data table
 */
void table::print(){
	if (ncol == 0) { 
		return;
	}
	cout << cols[0];
	for (int i = 1; i < ncol + 1; i++){
		cout << "\t" << cols[i];
	}
	cout << "\n";
	for (int i = 0; i < nrow; i++){
		cout << rows[i] << "\t" << y[i];
		for (int j = 0; j < ncol; j++){
			cout << "\t" << annot[i][j];
		}
		cout << "\n";
	}
}

/**
 *	Read file
 */
int table::readfile(const string& filename) {
	ifstream ifile(filename.c_str());
	if (!ifile.is_open()){
		cout << "Error: " << filename << " could not be read\n";
		return 1;
	} else if (ifile.peek() == ifstream::traits_type::eof()){
		cout << "Error: " << filename << " is empty.\n";
		return 2;
	}
	string line;
	ifile.clear();
	ifile.seekg(0, ios::beg);
	getline(ifile, line);	
	cols = split(line, '\t');
	// header has one fewer column
	ncol = cols.size() - 1;
	nrow = 0;
	annot_sizes.resize(ncol, 0);
	y.resize(0);
	while(getline(ifile,line)) {
		vector<string> elem = split(line, '\t');
		unsigned int nelem = elem.size();
		if (nelem != (ncol + 2)){
			cout << "Error: Row " << nrow + 1 << " has " << nelem << " not " << ncol << " elements.\n";
			annot.clear();
			cols.clear();
			rows.clear();
			nrow = 0;
			ncol = 0;
			return 3;
		}
		rows.push_back(elem[0]);
		annot.push_back(vector<long double>(ncol,0));
		long double p = 0;
		try {
			p = stold(elem[1]);
		} catch (const out_of_range& err) {
			cerr << "Out of range error: " << err.what() << "\n";
			cerr << "Trying to cast element (" << nrow <<  ", 1) = " << elem[1] << "  to long double\n";
			exit(1);
		}
		y.push_back(p);
		for (int i = 0; i < ncol; i++){
			try {
				annot[nrow][i] = stold(elem[i+2]);
				if (annot[nrow][i] == 1) {
					annot_sizes[i] = annot_sizes[i] + 1;
				}
			} catch (const out_of_range& err) {
				cerr << "Out of range error: " << err.what() << "\n";
				cerr << "Trying to cast element (" << nrow << ", " << (ncol + 2) << ") = " << elem[i + 2] << "  to long double\n";
				exit(1);
			}
		}
		nrow++;
	}
	return 0;
}


