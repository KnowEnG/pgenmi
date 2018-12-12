#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "helper.h"

using namespace std;

vector<int> string_to_mask(const string& model, int num_features){
	vector<string> elem = split(model, ',');
	vector<int> mask(num_features, 0);
	mask[0] = 1;
	for (int i = 0; i < elem.size(); i++) {
		// Maybe clean string here or something
		int col = stoi(elem[i]);
		if (col > 0 && col <= num_features){
			mask[col - 1] = 1;
		}
	}
	return mask;
}


void model_list_to_mask(vector<vector<int>>& model_masks, const string& model_list_str, int num_features){
	vector<string> elem = split(model_list_str, '|');
	for (int i = 0; i < elem.size(); i++){
		model_masks.push_back(string_to_mask(elem[i], num_features));
	}
	return;
}


long double l2norm(const long double a, const long double b){
	return abs(a - b);
}


long double l2norm(const vector<long double>& a, const vector<long double>&b){
	int n = a.size();
	long double ssd = 0;
	long double diff = 0;
	for (int i = 0; i < n; i++){
		diff = a[i] - b[i];
		ssd += diff*diff;
	}
	return sqrt(ssd);
}


int sum(const vector<int>& x) {
	int s = 0;
	for (int i = 0; i < x.size(); i++) {
		s += x[i];
	}
	return s;
}


long double sig(long double w) {
	return (long double) 1.0/(1 + exp(-1 * w));
}


long double sig(const vector<long double>& x, const vector<long double>& w){
	long double s = 0;
	for (int i = 0; i < w.size(); i++){
		s += x[i] * w[i];
	}
	return (long double) 1.0/(1 + exp(-1 * s));
}


long double sig(const vector<long double>& x, const vector<long double>& w, const vector<int>& mask){
	long double s = 0;
	for (int i = 0; i < w.size(); i++){
		s += x[i] * w[i] * mask[i];
	}
	return (long double) 1.0/(1 + exp(-1 * s));
}


long double dbeta(long double y, long double alpha) {
	long double beta = tgamma(alpha)*tgamma(1.0)/tgamma(alpha + 1);
	return std::pow(y, alpha - 1)/beta;
}


vector<string> & split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}


long double file2alpha(const string& s) {
	//synth.alpha_0.1.t_1.w_-1.44043714599684_1.76122314715758_0.982014360371977.pi_0.6_0.6.tsv
	vector<string> elems;
	elems = split(s, '_', elems);
	string alpha_raw = elems[1];
	// Ignore first occurence of . in 0.1.t
	// Replace .t
	unsigned int pos = alpha_raw.find(".", alpha_raw.find(".") + 1);
	alpha_raw.replace(pos, 2, "");
	return stold(alpha_raw);
}


long double file2pi(const string& s) {
	//synth.alpha_0.1.t_1.w_-1.44043714599684_1.76122314715758_0.982014360371977.pi_0.6_0.6.tsv
	vector<string> elems;
	elems = split(s, '_', elems);
	string str = elems[elems.size() - 1];
	unsigned int pos = str.find(".tsv");
	str.replace(pos, 4, "");
	return stold(str);
}


long double file2trial(const string& s) {
	//synth.alpha_0.1.t_1.w_-1.44043714599684_1.76122314715758_0.982014360371977.pi_0.6_0.6.tsv
	vector<string> elems;
	elems = split(s, '_', elems);
	string str = elems[2];
	unsigned int pos = str.find(".");
	str.replace(pos,2,"");
	return stold(str);
}


vector<long double> file2weight(const string& s, unsigned int natt) {
	vector<string> elems;
	elems = split(s, '_', elems);
	vector<long double> w(natt, 0);
	for (int i = 3; i < 3 + natt; i++){
		w[i-3] = stold(elems[i].c_str());
	}
	return w;
}

