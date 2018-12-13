#ifndef _TEMPLATED_HELPER_H
#define _TEMPLATED_HELPER_H
#include <string>
#include <vector>

using namespace std;
template <typename T>
string vec2str(const vector<T>& v, const string prefix = "");

template <typename T>
string vec2str(const vector<T>& v, const string prefix) {
  stringstream s;
  s.precision(15);
  s << prefix << v[0];
  for (int i = 1; i < v.size(); i++) {
    s << "\t" << prefix << v[i];
  }
  return s.str();
}
#endif
