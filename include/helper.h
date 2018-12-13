#ifndef _HELPER_H
#define _HELPER_H

#include <cmath>
#include <string>
#include <vector>
#include "templated_helper.h";

using namespace std;

vector<int> string_to_mask(const string&, int);
void model_list_to_mask(vector<vector<int>>&, const string&, int);
long double l2norm(const long double a, const long double b);
long double l2norm(const vector<long double>&, const vector<long double>&);
int sum(const vector<int>& x);
long double sig(long double w);
long double sig(const vector<long double>&, const vector<long double>&);
long double sig(const vector<long double>&, const vector<long double>&,
                const vector<int>&);
long double dbeta(long double y, long double alpha);
vector<string> split(const string&, char delim);
vector<string>& split(const string&, char delim, vector<string>&);
long double file2alpha(const string&);
long double file2pi(const string& s);
long double file2trial(const string& s);
vector<long double> file2weight(const string&, unsigned int);

#endif
