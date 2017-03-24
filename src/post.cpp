#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "helper.h"
#include "table.h"
#include "pgm.h"

using namespace std;

int main(int argc, char** argv) {
	//Argument Handling
	if (argc != 3) {
		return -1;
	}
	string pfile	=	argv[1];
	string dfile	=	argv[2];
	cout.precision(15);

	// Parameters for H1
	vector<long double> h1w;
	long double h0w;
	long double h1alpha;	
	long double h0alpha;
	
	// Process PFILE
	ifstream ifile(pfile.c_str());
	if (!ifile.is_open()){
		cout << "Error: " << pfile << " could not be read\n";
		return 1;
	} else if (ifile.peek() == ifstream::traits_type::eof()){
		cout << "Error: " << pfile << " is empty.\n";
		return 2;
	}
	//Get contents of PFILE
	string line;
	ifile.clear();
	ifile.seekg(0, ios::beg);
	getline(ifile, line);	
	ifile.close();
	vector<string> args = split(line, '\t');
	int nannot	=	(args.size() - 19)/2;
	int bindex	=	9 + nannot;
	int	eindex	=	bindex + nannot + 1;
	for (int i = bindex; i < eindex; i++){
		h1w.push_back(stold(args[i]));
	}
	h1alpha = stold(args[eindex]);
	h0alpha	=	stold(args[args.size() - 1]);
	h0w			=	stold(args[args.size() - 2]);
	
	// Get Posterior Estimates
	table dt(dfile);
	pgm model(dt);

	model.h1.w = h1w;
	model.h1.alpha	=	h1alpha;
	model.estepH1(dt);	
	model.h0.w	=	h0w;		
	model.h0.alpha	=	h0alpha;
	model.estepH0(dt);

	cout << "ENSG\tPVAL\tPOSTH1\tPOSTH0\tPOSTODDS\tA0";
	for (int j = 1; j < dt.ncol; j++){
		cout << "\t" << "A" << j;
	}
	cout << "\n";
	for (int i = 0; i < dt.nrow; i++){
		cout << dt.rows[i] << "\t" << dt.y[i] << "\t";
		cout << model.h1.post[i][1] << "\t" << model.h0.post[i][1] << "\t";
		cout << model.h1.post[i][1]/model.h0.post[i][1] << "\t" << dt.annot[i][0];
		for (int j = 1; j < dt.ncol; j++){
			cout << "\t" << dt.annot[i][j];
		}
		cout << "\n";
	}
	return 0;
};

