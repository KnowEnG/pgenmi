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

table::table(const string& _filename) : filename(_filename), num_non_zero(), num_features(0), num_rows(0), num_cols(0), pval(0), col_names(), row_names(), feat_names(){
	readfile(filename);
}


// Print data table
void table::print(){
	if (num_features == 0) {
		return;
	}

	cout << col_names[0];
	for (int i = 1; i < col_names.size(); i++){
		cout << "\t" << col_names[i];
	}
	cout << "\n";

	for (int i = 0; i < num_rows; i++){
		// print rowname and pvalue
		cout << row_names[i] << "\t" << pval[i];
		// print rest of data
		for (int j = 0; j < num_features; j++){
			cout << "\t" << data[i][j];
		}
		cout << "\n";
	}
}


long double table::_cast_string_to_long_double(const vector<string>& elem, int index, int num_rows){
	try {
		return stold(elem[index]);
	} catch (const out_of_range& err) {
		cerr << "Out of range error: " << err.what() << "\n";
		cerr << "Trying to cast element (" << num_rows <<  ", " << index << ") = " << elem[index] << "  to long double\n";
		exit(1);
	}
	return -1;
}

/**
 *	Read file
 */
int table::readfile(const string& filename) {
	ifstream ifile(filename.c_str());
	if (!ifile.is_open()){
		cerr << "Error: " << filename << " could not be read\n";
		exit(2);
	} else if (ifile.peek() == ifstream::traits_type::eof()){
		cerr << "Error: " << filename << " is empty.\n";
		exit(3);
	}
	// Get col_names from header
	string line;
	ifile.clear();
	ifile.seekg(0, ios::beg);
	getline(ifile, line);
	col_names = split(line, '\t');

	// Get number of columns from 1st data entry
	getline(ifile, line);
	int num_cols = split(line, '\t').size();
	num_features = num_cols - 2;

	// Get feature names from col_names and num_features
	int offset = col_names.size() - num_features;

	for (int i = 0; i < num_features; i++){
		feat_names.push_back(col_names[i + offset]);
	}

	// Resize data sizes matrix to (num_features) and pval matrix to be ()
	num_non_zero.resize(num_features, 0);
	pval.resize(0);

	// Reset and skip header
	ifile.clear();
	ifile.seekg(0, ios::beg);
	getline(ifile, line);

	// For each data line, split by tab
	while(getline(ifile,line)) {
		vector<string> elem = split(line, '\t');

		// Check if it has the correct number of columns (num_features + gene + pval)
		if (elem.size() != (num_features + 2)){
			cerr << "Error: Row " << num_rows + 1 << " has " << elem.size() << " not " << num_features << " elements.\n";
			data.clear();
			col_names.clear();
			row_names.clear();
			pval.clear();
			num_non_zero.clear();
			num_rows = 0;
			num_features = 0;
			exit(4);
		}

		//Get row_names for first element and push_back a vector of (num_features) zeros.
		row_names.push_back(elem[0]);
		data.push_back(vector<long double>(num_features,0));
		num_rows++;

		//Get pvalue safely from second element
		pval.push_back(_cast_string_to_long_double(elem, 1, num_rows - 1));

		// For each feature
		for (int i = 0; i < num_features; i++){
			// Get value
			data[num_rows - 1][i] = _cast_string_to_long_double(elem, i+2, num_rows - 1);
			if (data[num_rows - 1][i] != 0) {
				num_non_zero[i]++;
			}
		}
	}
	return 0;
}
