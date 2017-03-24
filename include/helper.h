#ifndef _HELPER_H
#define _HELPER_H

#include <cmath>
#include <vector>
#include <string>

using namespace std;

long double l2norm(const long double a, const long double b);
long double l2norm(const vector<long double>&, const vector<long double>&);
long double sig(long double _w);
long double sig(const vector<long double>&, const vector<long double>&);
long double dbeta(long double y, long double alpha);
string vec2str(const vector<long double>&);
vector<string>& split(const string&, char delim, vector<string>&);
vector<string>  split(const string&, char delim);
long double file2alpha(const string&);
long double file2pi(const string& s);
long double file2trial(const string& s);
vector<long double> file2weight(const string&, unsigned int);

#endif
