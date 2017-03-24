#ifndef _PGM_H
#define _PGM_H

#include <ostream>
#include <random>
#include <string>
#include <vector>
#include "table.h"

using namespace std;

class model0 {
	public: 
	vector<vector<long double> > post;	
	long double w;
	long double init_w;
	long double alpha;
	long double init_alpha;
	long double init_ll;
	unsigned int iter;
	long double train_ll;
	long double train_lll;
	unsigned int natt;
};

class model1 {
	public: 
	vector<long double> w;
	vector<long double> init_w;
	vector<vector<long double> > post;	
	long double alpha;
	long double init_alpha;
	long double init_ll;
	unsigned int iter;
	long double train_ll;
	long double train_lll;
	unsigned int natt;
};


class pgm {
	public:
		static uniform_real_distribution<long double> unif;
		static random_device rdev;
		static mt19937 reng;
		static long double lbalpha;
		static long double ubalpha;
		static unsigned int max_iter;
		static unsigned int min_iter;
		static unsigned int nrestart;
		static unsigned int maxneg;
		static long double beta; 	
		// Attributes
		model0 h0;
		model1 h1;
		long double eps;
		vector<long double> eta;
		
		// Methods
		pgm(const table & dt, long double _eps=0.0001);
		void estepH0(const table &);
		void estepH1(const table &);
		long double alpha_mstep(const table &, const vector<vector<long double> >&);
		long double w_mstepH0(const table&);
		long double w_mstep_stochH0(const table&);
		vector<long double> w_mstepH1(const table&);
		vector<long double> w_mstep_stochH1(const table&);
		long double loglikH0(const table&);
		long double loglikH1(const table&);
		void trainH0(const table&);
		void trainH1(const table&);

		// Static Methods
		static long double loglik(const table&, const vector<long double>&, const long double);
		static pgm trainRestart(const table& dt);

		//Friend Methods
		 friend ostream& operator<< (ostream&, const pgm& );
};

#endif
