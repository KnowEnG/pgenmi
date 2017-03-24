#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "helper.h"

using namespace std;

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

long double sig(long double _w) {
	return (long double) 1.0/(1 + exp(-1 * _w));
}

long double sig(const vector<long double>& _a, const vector<long double>& _w){
	long double s = 0;
	for (int i = 0; i < _w.size(); i++){
		s += _a[i] * _w[i];
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

string vec2str(const vector<long double>& v) {
	stringstream s;
	s.precision(15);
	s << v[0];
	for (int i = 1; i < v.size(); i++){
		s << "\t" << v[i];
	}
	return s.str();	
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

