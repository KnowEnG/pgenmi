#ifndef _PGM_H
#define _PGM_H

#include <ostream>
#include <random>
#include <string>
#include <vector>
#include "table.h"

using namespace std;

class model {
 public:
  vector<long double> w;
  vector<vector<long double> > post;
  long double alpha;
  long double loglik;
  long double loglik_prev;
  unsigned int iter;
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
  static unsigned int max_loglik_decr;
  static unsigned int queue_size;

  // Attributes
  long double eps;
  string name;
  vector<string> w_names;
  vector<long double> eta;
  vector<int> mask;
  model final;
  model init;
  unsigned int num_features;

  // Constructor
  pgm(const table& dt, const vector<int> mask, const string& name = "PGM",
      long double _eps = 0.0001);

  // Initializers should be called before training
  void initialize_params();
  void initialize_params(const table&, const pgm&);

  // Training Methods
  void train(const table&);
  void train(const table&, model&);
  void estep(const table&, model&);
  long double alpha_mstep(const table&, const vector<vector<long double> >&);
  vector<long double> w_mstep(const table&, const model&);
  void update_loglikelihoods(const table&);

  // Helper methods
  long double loglikelihood(const table&, const model&);
  string header();
  string to_string();

  // Static Methods
  static long double loglikelihood(const table&, const vector<long double>&,
                                   const long double);

  // Friend Methods
  // friend ostream& operator<< (ostream&, const pgm& );
};

#endif
