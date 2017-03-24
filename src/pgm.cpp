#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "helper.h"
#include "table.h"
#include "pgm.h"

using namespace std;

uniform_real_distribution<long double> pgm::unif(0, 1);
random_device pgm::rdev;
mt19937 pgm::reng(rdev());

long double pgm::lbalpha = 0.000001;
long double pgm::ubalpha = 1 - lbalpha;
unsigned int pgm::max_iter = 300;
unsigned int pgm::min_iter = 25;
unsigned int pgm::nrestart = 100; // usually 100
unsigned int pgm::maxneg   = 20;
long double pgm::beta = 0.0001;

/**
 *
 */
pgm::pgm(const table& dt, long double _eps) : eps(_eps), eta(), h0(), h1() { 
	eta.resize(dt.ncol);
	for (int i = 0; i < dt.ncol; i++){
		eta[i] = 2/((long double) dt.annot_sizes[i]);
	}
	h0.natt = 1;
	h1.natt = dt.ncol;
	h1.w.resize(h1.natt);
	for (int i = 0; i < dt.nrow; i++){
		h0.post.push_back(vector<long double>(2,0));	
		h1.post.push_back(vector<long double>(2,0));	
		h1.post[i][0] = unif(reng);
		h1.post[i][1] = 1 - h1.post[i][0];
		h0.post[i][0] = h1.post[i][0];
		h0.post[i][1] = 1 - h0.post[i][0];
	}
	h1.alpha = unif(reng);
	h1.init_alpha = h1.alpha;
	h0.alpha = h1.alpha;
	h0.init_alpha = h0.alpha;
	
	// Set model specific
	for (int i = 0; i < h1.natt; i++){
		h1.w[i] = unif(reng) - 0.5;
	}
	h0.w = h1.w[0];
	h0.init_w = h0.w;
	h1.init_w = h1.w;
	h0.init_ll = loglikH0(dt);
	h1.init_ll = loglikH1(dt);
}

/**
 * Compute Expectations for null.
 */
void pgm::estepH0(const table &dt) {
	long double g1 = 0;
	long double j0 = 0;
	long double j1 = 0;
	for (int i = 0; i < dt.nrow; i++){
		g1 = sig(h0.w);
		j0 = (1 - g1);
		j1 = dbeta(dt.y[i], h0.alpha) * g1;
		h0.post[i][0] = j0/(j0 + j1);
		h0.post[i][1] = j1/(j0 + j1);
	}
	return;
}

/**
 * Compute Expectations for alternative.
 */
void pgm::estepH1(const table &dt) {
	long double g1 = 0;
	long double j0 = 0;
	long double j1 = 0;
	for (int i = 0; i < dt.nrow; i++){
		g1 = sig(dt.annot[i], h1.w);
		j0 = (1 - g1);
		j1 = dbeta(dt.y[i], h1.alpha) * g1;
		h1.post[i][0] = j0/(j0 + j1);
		h1.post[i][1] = j1/(j0 + j1);
	}
	return;
}


/**
 * Compute new alpha given posterior
 */
long double pgm::alpha_mstep(const table &dt, const vector<vector<long double> >& post) {
	long double numer = 0;
	long double denom = 0;
	for (int i = 0; i < dt.nrow; i++){
		numer += post[i][1];
		denom += post[i][1] * log(dt.y[i]);
	}
	long double ret = -1 * numer/denom;
	if (ret < 0) { ret = lbalpha; }
	if (ret > 1) { ret = ubalpha; }
	return ret;
}

long double pgm::w_mstep_stochH0(const table& dt) {
	long double _w = h0.w;
	long double _wn = _w;
	long double norm = eps + 1;
	while(norm >= eps) {
		long double s = 0;
		for (int i = 0; i < dt.nrow; i++){
			_wn += eta[0] * (1 - sig(_wn) - h0.post[i][0]) + beta * (_wn - _w);
		}
		norm = l2norm(_w,_wn);
		//cout << norm << "\t" << _w << "\t" << _wn << "\n";
		_w = _wn;
	}
	return _w; 

}

/**
 * Gradient descent to find new w for H0
 */
long double pgm::w_mstepH0(const table& dt) {
	long double _w = h0.w;
	long double _wn = 0;
	long double norm = eps + 1;
	while(norm >= eps) {
		long double s = 0;
		for (int i = 0; i < dt.nrow; i++){
			s += sig(_w) + h0.post[i][0];
		}
		for (int i = 0; i < dt.ncol; i++){
			_wn = _w + eta[0] * (dt.nrow - s);
		}
		norm = l2norm(_w,_wn);
	//	cout << norm << "\t" << _w << "\t" << _wn << "\n";
		_w = _wn;
	}
	return _w; 
};

/**
 * Gradient descent to find new w
 */
vector<long double> pgm::w_mstepH1(const table& dt) {
	vector<long double> _w = h1.w;
	vector<long double> _wn(h1.natt, 0);
	vector<long double> s(h1.natt, 0);
	vector<long double> c(h1.natt, 0);
	long double norm = eps + 1;
	while(norm >= eps) {
		fill(s.begin(), s.end(),     0);
		fill(c.begin(), c.end(),     0);
		for (int i = 0; i < dt.nrow; i++){
			for (int j = 0; j < h1.natt; j++){
				if (dt.annot[i][j] != 0) {
					s[j] += sig(dt.annot[i], _w) + h1.post[i][0];
					c[j] = c[j] + 1;
				}
			}
		}
		for (int i = 0; i < dt.ncol; i++){
			_wn[i] = _w[i] + eta[i] * (c[i] - s[i]);
		}
		norm = l2norm(_w,_wn);
		//cout << norm << "\t" << vec2str(_w) << "\t" << vec2str(_wn) << "\n";
		_w = _wn;
	}
	return _w; 
};

/**
 * Gradient descent to find new w
 */
vector<long double> pgm::w_mstep_stochH1(const table& dt) {
	vector<long double> _w = h1.w;
	vector<long double> _wn = _w;
	long double norm = eps + 1;
	while(norm >= eps) {
		for (int i = 0; i < dt.nrow; i++){
			for (int j = 0; j < h1.natt; j++){
				if (dt.annot[i][j] != 0) {
					_wn[j] += eta[j] * (1 - sig(dt.annot[i], _wn) - h1.post[i][0]) + beta * (_wn[j] - _w[j]);
				}
			}
		}
		norm = l2norm(_w,_wn);
		//cout << norm << "\t" << vec2str(_w) << "\t" << vec2str(_wn) << "\n";
		_w = _wn;
	}
	return _w; 
};

/**
 * Computes loglikelihood of data in dt for null model
 */
long double pgm::loglikH0(const table& dt) {
	long double ll = 0;
	for (int i = 0; i < dt.nrow; i++){
		ll += log(sig(h0.w) * (dbeta(dt.y[i], h0.alpha) - 1) + 1);
	}
	return ll;
}

/**
 * Computes loglikelihood of data in dt for alternative model
 */
long double pgm::loglikH1(const table& dt) {
	long double ll = 0;
	for (int i = 0; i < dt.nrow; i++){
		ll += log(sig(dt.annot[i], h1.w) * (dbeta(dt.y[i], h1.alpha) - 1) + 1);
	}
	return ll;
}

/**
 * Computes loglikliehood of data in dt using _w and _alpha
 */
long double pgm::loglik(const table& dt, const vector<long double>& _w, const long double _alpha) {
	long double ll = 0;
	for (int i = 0; i < dt.nrow; i++){
		ll += log(sig(dt.annot[i], _w) * (dbeta(dt.y[i], _alpha) - 1) + 1);
	}
	return ll;
}

/*
 * Train a model on dt using a more robust methodology
 * Change fixed threshold to percentage threshold
 * Tolerate in a window a fixed number of decreases or something.
 */
void pgm::trainH0(const table& dt) {
	h0.iter = 0;
	h0.train_ll = 0;
	h0.train_lll = 0;
	long double dll  = 0;
	queue<int> q;
	int nneg = 0;
	bool thresh = true;
	long double max_lll = 0;
	long double max_ll = h0.init_ll;
	long double max_alpha = h0.init_alpha;
	long double max_w = h0.init_w;
	while ((nneg < maxneg || h0.iter < min_iter) && h0.iter < max_iter) {
		//E-step
		estepH0(dt);
	
		//M-step
		//h0.w     = w_mstepH0(dt);
		h0.w     = w_mstepH0(dt);
		h0.alpha = alpha_mstep(dt, h0.post);
		
		//Log Likelihood
		h0.train_lll  = h0.train_ll;
		h0.train_ll   = loglikH0(dt);
		dll  = h0.train_ll - h0.train_lll;

		// Check max ll
		if (h0.train_ll > max_ll) {
			max_lll = h0.train_lll;
			max_ll =  h0.train_ll;
			max_alpha = h0.alpha;
			max_w = h0.w;
		}
		
		// Update queue if loglikelihood decreasing
		if (dll < 0){
			q.push(0);
			nneg++;
		} else {
			q.push(1);
		}

		// Make sure you have at least min_iter data points
		if ((h0.iter >= min_iter) && (!q.empty())) {
			if (q.front() == 0){
				nneg--;
			}
			q.pop();
		}
		if (h0.iter > min_iter && thresh) {
			thresh = false;
		//	cout << "HERE\n";
		}
		//cout << ll << "\t" << lll << "\t" << dll << "\t" << nneg << "\n";
		h0.iter++;
	}
	h0.train_lll = max_lll;
	h0.train_ll = max_ll;
	h0.alpha = max_alpha;
	h0.w = max_w; 
	return;
}


/*
 * Train a model on dt using a more robust methodology
 * Change fixed threshold to percentage threshold
 * Tolerate in a window a fixed number of decreases or something.
 */
void pgm::trainH1(const table& dt) {
	h1.iter = 0;
	h1.train_ll = 0;
	h1.train_lll = 0;
	long double dll  = 0;
	queue<int> q;
	int nneg = 0;
	bool thresh = true;
	long double max_lll = 0;
	long double max_ll = h1.init_ll;
	long double max_alpha = h1.init_alpha;
	vector<long double> max_w = h1.init_w;
	while ((nneg < maxneg || h1.iter < min_iter) && h1.iter < max_iter) {
		//E-step
		estepH1(dt);
	
		//M-step
		//h1.w     = w_mstepH1(dt);
		h1.w     = w_mstepH1(dt);
		h1.alpha = alpha_mstep(dt, h1.post);

		//Log Likelihood
		h1.train_lll  = h1.train_ll;
		h1.train_ll   = loglikH1(dt);
		dll  = h1.train_ll - h1.train_lll;

		// Check max ll
		if (h1.train_ll > max_ll) {
			max_lll = h1.train_lll;
			max_ll =  h1.train_ll;
			max_alpha = h1.alpha;
			max_w = h1.w;
		}

		// Update queue if loglikelihood decreasing
		if (dll < 0){
			q.push(0);
			nneg++;
		} else {
			q.push(1);
		}

		// Make sure you have at least min_iter data points
		if ((h1.iter >= min_iter) && (!q.empty())) {
			if (q.front() == 0){
				nneg--;
			}
			q.pop();
		}
		if (h1.iter > min_iter && thresh) {
			thresh = false;
		//	cout << "HERE\n";
		}
		//cout << ll << "\t" << lll << "\t" << dll << "\t" << nneg << "\n";
		h1.iter++;
	}
	h1.train_lll = max_lll;
	h1.train_ll = max_ll;
	h1.alpha = max_alpha;
	h1.w = max_w; 
	return;
}

/*
 * Train a model on dt
 * Change fixed threshold to percentage threshold
 * Tolerate in a window a fixed number of decreases or something.
void pgm::train(const table& dt){
	int i = 0;
	long double dll  = 0;
	long double lll  = 0;
	long double llll = 0;
	long double ll   = 0;
	long double _alpha = 0;
	vector<long double> _w;
	while ((dll > eps || i < min_iter) && i < max_iter) {
		//E-step
		estep(dt);
		//M-step
		_alpha = alpha_mstep(dt);
		_w     = w_mstep(dt);
		//Log Likelihood
		llll = lll;
		lll  = ll;
		ll   = loglik(dt);	
		dll  = ll - lll;
		if (dll < 0){ 
			ll  = lll;
			lll = llll;
			break; 
		}
		alpha = _alpha;
		w = _w; 
		i++;
	}
	train_iter = i;
	train_lll  = lll;
	train_ll   = ll;
	return;
}
*/

/**
 * Train multiple PGM models with restart.
 * Return best according to ll.
 */
pgm pgm::trainRestart(const table& dt) {
	pgm best(dt);
	best.trainH1(dt);
	best.trainH0(dt);
	//cout << "N\t" << best.h1.train_ll << "\t" << best.h0.train_ll << "\t" << best.h1.train_ll - best.h0.train_ll << "\n";
	for (int i = 0; i < nrestart; i++){
		if (i % 15 == 0) {
			//cerr << i/((double) nrestart) << "% ..";
		}
		pgm mod(dt);	
		mod.trainH1(dt);
		mod.trainH0(dt);
		if (mod.h0.train_ll > best.h0.train_ll) {
			best.h0 = mod.h0;
		}
		if (mod.h1.train_ll > best.h1.train_ll) {
			best.h1 = mod.h1;
		}
		//cout << i << "\t" <<  best.h1.train_ll << "\t" << best.h0.train_ll << "\t" << best.h1.train_ll - best.h0.train_ll << "\n";
	}
	//cerr << "\n";
	return best;
}

/**
 * Print out pgm
 */
ostream& operator<< (ostream& out, const pgm& m) {
	return out;
}
