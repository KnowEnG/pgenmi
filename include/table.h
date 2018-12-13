#ifndef _TABLE_H
#define _TABLE_H

#include <string>
#include <vector>

using namespace std;

class table {
 public:
  // Attributes
  string filename;
  unsigned int num_cols;
  unsigned int num_features;
  unsigned int num_rows;
  vector<vector<long double> > data;
  vector<long double> pval;
  vector<string> feat_names;
  vector<string> col_names;
  vector<string> row_names;
  vector<int> num_non_zero;

  // Methods
  void print();
  int readfile(const string&);
  table(const string&);

 private:
  long double _cast_string_to_long_double(const vector<string>& elem, int index,
                                          int num_rows);
};

#endif
