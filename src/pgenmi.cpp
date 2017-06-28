#include <iostream>
#include <string>
#include "helper.h"
#include "table.h"
#include "pgm.h"

using namespace std;

int main(int argc, char** argv) {
  //Argument Handling
 	string filename = argv[1];
 	string drug = argv[2];
 	string tf = argv[3];
 	cout.precision(15);
 	table dt(filename);
 	pgm m = pgm::trainRestart(dt);
	double lambda = 2 * (m.h1.train_ll - m.h0.train_ll);
	cout << drug << "\t" << tf << "\t";
	cout << lambda << "\t" << m.h1.train_ll << "\t" << m.h1.train_lll << "\t" << m.h1.init_ll << "\t";
	cout << m.h1.iter << "\t" << vec2str(m.h1.init_w) << "\t" << m.h1.init_alpha << "\t";
	cout << vec2str(m.h1.w) << "\t" << m.h1.alpha << "\t";
	cout << m.h0.train_ll << "\t" << m.h0.train_lll << "\t" << m.h0.init_ll << "\t";
	cout << m.h0.iter << "\t" << m.h0.init_w << "\t" << m.h0.init_alpha << "\t";
	cout << m.h0.w << "\t" << m.h0.alpha << "\n";
	return 0;
}


