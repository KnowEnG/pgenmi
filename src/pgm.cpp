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
unsigned int pgm::max_loglik_decr = 20;
unsigned int pgm::queue_size = 20;

pgm::pgm(const table& dt, const vector<int> mask, const string& name, long double _eps) : mask(mask), name(name), eps(_eps), eta(), num_features(0), init(), final(), w_names() {
	//Get total features and masked
	// Sample alpha and w and get number of non_zero_entries for each feature
	// Set learning rate to be proprotional to features that are non-zero
	num_features = sum(mask);
	w_names = dt.feat_names;
	init.alpha = 1;
	init.w.resize(dt.num_features, 1);
	eta.resize(dt.num_features, 0);
	for (int i = 0; i < dt.num_features; i++){
		init.w[i] = mask[i];
		eta[i] = 2/((long double) dt.num_non_zero[i]);
	}
	for (int i = 0; i < dt.num_rows; i++){
		init.post.push_back(vector<long double>(2,0));
		init.post[i][1] = 1;
	}
	final = init;
}


// Initialize parameters from static uniform random number generators
void pgm::initialize_params() {
	init.alpha = unif(reng);
	for (int i = 0; i < mask.size(); i++){
		init.w[i] = mask[i] * (unif(reng) - 0.5);
	}
	for (int i = 0; i < init.post.size(); i++){
		init.post[i][0] = unif(reng);
		init.post[i][1] = 1 - init.post[i][0];
	}

	// Get loglikelihood for initial loglik (loglik) and lastloglik (lll)
	init.loglik = 0;
	init.loglik_prev = init.loglik;
	final = init;
}


//Initialize parameters from pgm h. This resets both initial and final models.
void pgm::initialize_params(const table& dt,const pgm& h){
	init = h.init;
	for (int i = 0; i < mask.size(); i++){
		init.w[i] = mask[i] * h.init.w[i];
	}
	init.loglik = loglikelihood(dt, init);
	init.loglik_prev = init.loglik;
	final = init;
}


void pgm::update_loglikelihoods(const table& dt) {
	init.loglik_prev = init.loglik;
	init.loglik = loglikelihood(dt, init);
	final.loglik_prev = final.loglik;
	final.loglik = loglikelihood(dt, final);
	return;
}


// Use only model weights to update posterior
void pgm::estep(const table& dt, model& h){
	long double g1 = 0;
	long double j0 = 0;
	long double j1 = 0;
	for (int i = 0; i < dt.num_rows; i++){
		g1 = sig(dt.data[i], h.w, mask);
		j0 = (1 - g1);
		j1 = dbeta(dt.pval[i], h.alpha) * g1;
		h.post[i][0] = j0/(j0 + j1);
		h.post[i][1] = j1/(j0 + j1);
	}
	return;
}


//Use only posterior to maximize alpha
long double pgm::alpha_mstep(const table &dt, const vector<vector<long double> >& post){
	long double numer = 0;
	long double denom = 0;
	for (int i = 0; i < dt.num_rows; i++){
		numer += post[i][1];
		denom += post[i][1] * log(dt.pval[i]);
	}
	long double ret = -1 * numer/denom;
	if (ret < 0) { ret = lbalpha; }
	if (ret > 1) { ret = ubalpha; }
	return ret;
}


// Use only posterior to maximize w through gradient descent
vector<long double> pgm::w_mstep(const table& dt, const model& h) {
	vector<long double> w = h.w;
	vector<long double> w_next(dt.num_features, 0);
	vector<long double> s(dt.num_features, 0);
	vector<long double> c(dt.num_features, 0);
	long double norm = eps + 1;

	// While distance between w and w_next >= eps
	while(norm >= eps) {
		fill(s.begin(), s.end(), 0);
		fill(c.begin(), c.end(), 0);
		for (int j = 0; j < dt.num_features; j++){
			if (mask[j]) {
				for (int i = 0; i < dt.num_rows; i++){
					if (dt.data[i][j] != 0) {
						s[j] += sig(dt.data[i], w, mask) + h.post[i][0];
						c[j] = c[j] + 1;
					}
				}
			}
		}
		for (int i = 0; i < dt.num_features; i++){
			w_next[i] = mask[i] * (w[i] + eta[i] * (c[i] - s[i]));
		}
		norm = l2norm(w,w_next);
		//cout << norm << "\t" << vec2str(_w) << "\t" << vec2str(_wn) << "\n";
		w = w_next;
	}
	return w;
};


//Computes loglikelihood of data in dt for model
long double pgm::loglikelihood(const table& dt, const model& h) {
	long double loglik = 0;
	for (int i = 0; i < dt.num_rows; i++){
		loglik += log(sig(dt.data[i], h.w, mask) * (dbeta(dt.pval[i], h.alpha) - 1) + 1);
	}
	return loglik;
}


//Computes loglikelihood of data in dt using _w and _alpha
long double pgm::loglikelihood(const table& dt, const vector<long double>& w, const long double alpha) {
	long double loglik = 0;
	for (int i = 0; i < dt.num_rows; i++){
		loglik += log(sig(dt.data[i], w) * (dbeta(dt.pval[i], alpha) - 1) + 1);
	}
	return loglik;
}

void pgm::train(const table& dt) {
	train(dt, final);
	return;
}

/*
 * Train a model on dt using a more robust methodology
 * Tolerate some decreases in a window of
 * Tolerate in a window a fixed number of decreases or something.
 */
void pgm::train(const table& dt, model& h) {
	h.iter = 0;
	h.loglik = 0;
	h.loglik_prev = 0;
	int num_loglik_decr = 0;
	model max_h = h;
	queue<int> q;
	// while we haven't decrease maximally and we are within iteration bounds
	while ((num_loglik_decr < max_loglik_decr || h.iter < min_iter) && h.iter <max_iter) {
		//E-step update h.post given h.w, h.mask. and h.alpha
		estep(dt, h);

		//M-step: Maximize h.w and h.alpha given h.post
		h.alpha = alpha_mstep(dt, h.post);
		h.w = w_mstep(dt, h);

		//Log Likelihood
		h.loglik_prev  = h.loglik;
		h.loglik   = loglikelihood(dt, h);

		// Check max ll
		if (h.loglik > max_h.loglik) {
			max_h = h;
		}

		// Update queue if loglikelihood decreasing

		// Make sure you have at least min_iter data points
		if (h.iter > min_iter) {
			int loglik_decr = int((h.loglik -h.loglik_prev) < 0);
			q.push(loglik_decr);
			num_loglik_decr = num_loglik_decr + 1 - loglik_decr;
			if (q.size() == queue_size){
				num_loglik_decr = num_loglik_decr + q.front() - 1;
				q.pop();
			}
		}
		h.iter++;
	}
	h.iter++;
	h = max_h;
	return;
}

string pgm::header() {
	stringstream ss;
	ss << "NAME" << "\t" << "NUM_FEATURES" << "\t";
	ss << vec2str(w_names, "MASK_") << "\t";
	ss << "INIT_ITER" << "\t" << "INIT_LOGLIK" << "\t";
	ss << "INIT_LOGLIK_PREV" << "\t" << vec2str(w_names, "INIT_") << "\t";
	ss << "INIT_ALPHA" << "\t";
	ss << "TRAINED_ITER" << "\t" << "TRAINED_LOGLIK" << "\t";
	ss << "TRAINED_LOGLIK_PREV" << "\t" << vec2str(w_names, "TRAINED_") << "\t";
	ss << "TRAINED_ALPHA";
	return ss.str();
}

string pgm::to_string(){
	stringstream ss;
	ss << name << "\t" << num_features << "\t" << vec2str(mask) << "\t";
	ss << init.iter << "\t";
	ss << init.loglik << "\t" << init.loglik_prev << "\t";
	ss << vec2str(init.w) << "\t" << init.alpha << "\t";
	ss << final.iter << "\t";
	ss << final.loglik << "\t" << final.loglik_prev << "\t";
	ss << vec2str(final.w) << "\t" << final.alpha << "\t";
	return ss.str();
}
