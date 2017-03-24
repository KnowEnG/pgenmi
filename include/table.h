#ifndef _TABLE_H
#define _TABLE_H

#include<string>
#include<vector>

using namespace std;

class table {
	public:

		// Attributes
		string filename;
		unsigned int ncol;
		unsigned int nrow;
		vector<vector<long double> > annot;
		vector<long double> y;
		vector<string> cols;
		vector<string> rows;

		vector<int> annot_sizes;	
		// Methods
		void print();
		int readfile(const string&);
		table(const string&);

	private:

		// Methods
};

#endif
