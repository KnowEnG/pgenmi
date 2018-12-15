#include <assert.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "helper.h"
#include "pgm.h"
#include "table.h"

using namespace std;

void print_usage();

string header(const vector<model>&);

vector<model> parse_params(int, char**);

int main(int argc, char** argv) {
  // Argument Handling
  if (argc < 2) {
    print_usage();
    return -1;
  }

  //Parse inputs
  string filename = string(argv[1]);
  int num_models = argc - 2;
  vector<model> models = parse_params(num_models, argv + 2);

  assert(num_models == models.size());
  assert(num_models > 0);

  // Load data
  table dt(filename);
  cout.precision(15);

  //Get posteriors
  for (int i = 0; i < num_models; i++){
    models[i].post = pgm::posterior(dt, models[i].w, models[i].alpha);
  }

  // Print results
  cout << dt.header() << "\t" << header(models) << "\n";
  for (int i = 0; i < dt.num_rows; i++) {
    cout << dt.row2str(i) << "\t";
    cout << models[0].post[i][1];
    for (int j = 1; j < num_models; j++){
      cout << "\t" << models[j].post[i][1];
    }
    if (num_models > 1) {
      cout << "\t" << models[0].post[i][1] / models[1].post[i][1];
      for (int k = 2; k < num_models; k++){
        cout << "\t" << models[0].post[i][1] / models[k].post[i][1];
      }
    }
    cout << "\n";
  }
  return 0;
};

void print_usage(){
  cout << "Usage: postodds filename param_str ...\n";
  cout << "\n";
  cout << "This program calculates posteriors for all models in param_str.\n";
  cout << " It accepts multiple strings for parameters, each a model.\n";
  cout << " It computes posterior odds for the first model to all others .\n";
  cout << " as follows: \"w0,w1,w2,..,wn:alpha:[name]";
  cout <<
  cout << "Args: \n";
  cout << "  filename    This is the path to a pgenmi input data file, with\n";
  cout << "              pvalues and feature vectors for genes.\n";
  cout << "              See README.md for formatting.\n";
  cout << "  param_str   A string corresponding to a model with alpha and \n";
  cout << "              weights=[w0,w1,w2,...,wn]. The format is as follows \n";
  cout << "              for a three weight model w=[w0,w1,w2] alpha=a:\n";
  cout << "                Ex param_str: \"w0,w1,w2:a\"\n";
  cout << "              You can also optionally specify a name for models\n";
  cout << "                Ex param_str: \"w0,w1,w2:a:name\"\n";
  cout << "              You can list any number of models in quotes in this\n";
  cout << "              format:\n";
  cout << "                Ex: \"w10,w11:a1:name1\" \"w20,w21:a2:name2\"\n";
  cout << "              NOTE: Each listed model must have the same number\n";
  cout << "                of weights and must be compatible with the data\n";
  cout << "                in filename (i.e. must have the same dimensions).\n";
}


string header(const vector<model>& models){
  stringstream ss;
  int num_models = models.size();
  if (num_models == 0) {
    return "";
  }
  ss << "POST_" << models[0].name;
  for (int i = 1; i < num_models; i++){
    ss << "\tPOST_" << models[i].name;
  }

  if (num_models > 1) {
    ss << "\tPOSTODDS_" << models[0].name << "-" << models[1].name;
    for (int i = 2; i < num_models; i++) {
      ss << "\tPOSTODDS_" << models[0].name << "-" << models[i].name;
    }
  }
  return ss.str();
}

vector<model> parse_params(int num_models, char** models_str){
  vector<model> models;
  if (num_models == 0) {
    return models;
  }
  for (int i = 0; i < num_models; i++){
    vector<string> elem = split(string(models_str[i]),':');
    int num_elem = elem.size();
    assert(num_elem == 2 || num_elem == 3);
    vector<long double> w = str_vec2ld_vec(split(elem[0], ','));
    long double alpha = str2ld(elem[1]);
    if (num_elem == 2) {
      stringstream ss;
      ss << "H_" << i;
      models.push_back(model(w, alpha, ss.str()));
    } else {
      models.push_back(model(w, alpha, string(elem[2])));
    }
  }
  return models;
}