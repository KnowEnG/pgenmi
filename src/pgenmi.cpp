#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include "helper.h"
#include "table.h"
#include "pgm.h"
#include "trainer.h"

void print_usage();
void default_masks(vector<vector<int>>&, vector<string>&, const vector<string>&, const bool, const bool, const bool, const bool);
void parse_custom_models(vector<vector<int>>&, vector<string>&, const string&, const string&, int);

//static int verbose;

using namespace std;


int main (int argc, char **argv) {
  int c = 0;
  bool verbose = false;
  bool all_disable = false;
  bool full_disable = false;
  bool null_disable = false;
  bool without_singles_enable = false;
  bool singles_enable = false;
  int num_restarts = 100;

  string tf = "TF";
  string drug = "DRUG";
  string model_list_str = "";
  string model_names_str = "";

  // Loop until break
  while (true) {

      static struct option long_options[] = {
        /* These options set a flag. */
        {"all_disable",  no_argument, 0, 'a'},
        {"full_disable", no_argument, 0, 'f'},
        {"singles",      no_argument, 0, 's'},
        {"without_singles",      no_argument, 0, 'w'},
        {"help",         no_argument, 0, 'h'},
        {"null_disable", no_argument, 0, 'n'},
        {"verbose",      no_argument, 0, 'v'},
        //{"brief",   no_argument,       &verbose, 0},
        {"drug",   required_argument, 0, 'd'},
        {"names", required_argument, 0,  'i'},
        {"models", required_argument, 0, 'm'},
        {"restarts",     required_argument, 0, 'r'},
        {"tf",     required_argument, 0, 't'},
        {0, 0, 0, 0}
      };

      /* getopt_long stores the option index here. */
      int opt_index = 0;

      //c = getopt_long (argc, argv, "abc:d:f:m:", long_options, &opt_index);
      c = getopt_long (argc, argv, "afhnsvwd:i:m:r:t:", long_options, &opt_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c) {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[opt_index].flag != 0) {
            break;
          }

          cerr << "option " << long_options[opt_index].name;
          if (optarg) {
            cerr << " with arg " << optarg;
          }
          cerr << "\n";
          break;

        case 'a':
          all_disable = true;
          break;

        case 'd':
          drug = string(optarg);
          break;

        case 'f':
          full_disable = true;
          break;

        case 'h':
          print_usage();
          exit(0);
          break;

        case 'i':
          model_names_str = string(optarg);
          break;

        case 'm':
          model_list_str = string(optarg);
          break;

        case 'n':
          null_disable = true;
          break;

        case 'r':
          num_restarts = stoi(optarg);
          break;

        case 's':
          singles_enable = true;
          break;

        case 't':
          tf = string(optarg);
          break;

        case 'v':
          verbose = true;
          break;

        case 'w':
          without_singles_enable = true;
          break;

        case '?':
          /* getopt_long already printed an error message. */
          print_usage();
          break;

        default:
          abort ();
        }
    }

  // Make sure flags agree for all, null, and full
  if (all_disable) {
    full_disable = true;
    null_disable = true;
  } else if (full_disable && null_disable){
    all_disable = true;
  }

  if (verbose) {
    cerr << "Option --full_disable: " << full_disable << "\n";
    cerr << "Option --null_disable: " << null_disable << "\n";
    cerr << "Option --all_disable: " << all_disable << "\n";
    cerr << "Option --singles: " << singles_enable << "\n";
    cerr << "Option --without_singles: " << without_singles_enable << "\n";
    cerr << "Option --tf: " << tf << "\n";
    cerr << "Option --drug: " << drug << "\n";
    cerr << "Option --verbose: " << verbose << "\n";
    cerr << "Option --names: " << model_names_str << "\n";
    cerr << "Option --models: " << model_list_str << "\n";
    cerr << "Option --restarts: " << num_restarts << "\n";
  }

  int num_args = argc - optind;

  if (num_args != 1) {
    cerr << "Error: Need 1 argument (FILE), but given " << num_args << ".\n";
    print_usage();
    exit(1);
  }

   // Required arguments non option ARV elements
  string filename = string(argv[optind]);

  // Set precision
  cout.precision(15);
  cerr.precision(15);

  // Read datatable
 	table dt(filename);
  int num_features = dt.num_features;

  if (verbose) {
    cerr << "Filename: " << filename << "\n";
    cerr << "Num Features: " << num_features << "\n";
    cerr << "Num Samples(rows): " << dt.num_rows << "\n";
    cerr << "Feature Names: " << vec2str(dt.feat_names) << "\n";
  }

  // Make model feature masks and names
  vector<vector<int>> model_masks;
  vector<string> model_names;
  default_masks(model_masks, model_names, dt.feat_names, full_disable, null_disable, singles_enable, without_singles_enable);
  parse_custom_models(model_masks, model_names, model_list_str, model_names_str, num_features);
  assert(model_names.size() == model_masks.size());

  vector<pgm> models = train_with_restart(dt, model_masks, model_names, num_restarts);
  if (models.size() == 0) {
    cout << "No models were tested.\n";
    return 1;
  }
  cout << "TF\tDRUG\t" << models[0].header();
  for (int i = 0; i < models.size(); i++){
    cout << "\n" << tf << "\t" << drug << "\t" << models[0].to_string();
  }
  cout << "\n";
  return 0;
}


// Generate default model masks from flags
void default_masks(vector<vector<int>>& model_masks, vector<string>& model_names, const vector<string>& feat_names, const bool full_disable, const bool null_disable, const bool singles_enable, const bool without_singles_enable){
	int num_features = feat_names.size();
	if (num_features == 0) {
		return;
	}
	vector<int> ones(num_features, 1);
	vector<int> zeros(num_features, 0);
	zeros[0] = 1;
	if (!full_disable) {
		model_masks.push_back(ones);
		model_names.push_back("H_ALL");
	}
	if (!null_disable) {
		model_masks.push_back(zeros);
		model_names.push_back("H_" + feat_names[0]);
	}
	if (singles_enable) {
		for (int i = 1; i < num_features; i++){
			model_masks.push_back(zeros);
			model_masks[model_masks.size() - 1][i] = 1;
			model_names.push_back("H_" + feat_names[i]);
		}
	}
	if (without_singles_enable) {
		for (int i = 1; i < num_features; i++){
			model_masks.push_back(ones);
			model_masks[model_masks.size() - 1][i] = 0;
			model_names.push_back("H_ALL_WO_" + feat_names[i]);
		}
	}
	return;
}


void parse_custom_models(vector<vector<int>>& model_masks, vector<string>& model_names, const string& model_list_str, const string& model_names_str, int num_features) {
  int num_default_masks = model_masks.size();
  model_list_to_mask(model_masks, model_list_str, num_features);
  int num_total_masks = model_masks.size();
  int num_custom_masks = num_total_masks - num_default_masks;
  if (num_custom_masks == 0){
    return;
  }

  // Parse custom names
  vector<string> custom_names = split(model_names_str, ',');
  int num_custom_names = custom_names.size();

  //Get minimum of custom names and models
  int num_names_to_add = (num_custom_masks < num_custom_names) ? num_custom_masks : num_custom_names;
  //Add the minimum of names and models
  for (int i = 0; i < num_names_to_add; i++){
    model_names.push_back(custom_names[i]);
  }

  //Generate custom names if there aren't enough
  //If too many, ignore them.
  if (num_custom_names < num_custom_masks){
    int i = model_names.size();
    stringstream ss;
    while(i < num_total_masks){
      ss << "H_CUSTOM_" << i ;
      model_names.push_back(ss.str());
      ss.str("");
      i++;
    }
  }
  return;
}


void print_usage() {
  cerr << "Usage: pgenmi [OPTION] ... [FILE]\n";
  cerr << "\n";
  cerr << "  Run the pgenmi model on pvalues and features in FILE.\n";
  cerr << "\n";
  cerr << "  DEFAULT: run models on all (H1) and no (H0) features.\n";
  cerr << "    (excluding intercept which is always a feature).\n";
  cerr << "  This can be changed using options.\n";
  cerr << "\n";
  cerr << "  Results output to STDOUT.\n";
  cerr << "\n";
  cerr << "  -a,  --all_disable      Disable both H1 and H0 model runs.\n";
  cerr << "  -d,  --drug             Drug name (Default is DRUG).\n";
  cerr << "  -f,  --full_disable     Disable full (H1) model.\n";
  cerr << "  -h,  --help             Print this usage guide.\n";
  cerr << "  -n,  --null_disable     Disable null (H0) model.\n";
  cerr << "  -r,  --restarts=NUM     Restart model runs NUM times. \n";
  cerr << "  -s,  --singles          Run all models with only one feature\n";
  cerr << "                            and intercept active.\n";
  cerr << "  -t,  --tf               TF name (Default is TF).\n";
  cerr << "  -w,  --without_singles  Run all models missing only one feature\n";
  cerr << "                            excluding the intercept.\n";
  cerr << "\n";
  cerr << "  -i,  --names=\"CSV\"      A CSV of model names corresponding\n";
  cerr << "                            to and in the order of --models.\n";
  cerr << "                          If --models unset, this is ignored.\n";
  cerr << "                          Default model names generated if there\n";
  cerr << "                            more models than names, for the extra\n";
  cerr << "                            models without names.\n";
  cerr << "                          The CSV should be in double quotes.\n";
  cerr << "                          Example: --names=\"H2,H3\".\n";
  cerr << "\n";
  cerr << "  -m,  --models=\"CSV\"     Run models on CSV feature lists.\n";
  cerr << "                          Features are column indices in FILe \n";
  cerr << "                            using 1 based indexing.\n";
  cerr << "                          A model is a comma seperated value\n";
  cerr << "                            (CSV) list of feature indices.\n";
  cerr << "                          To specify multiple models, seperate\n";
  cerr << "                            individual CSV models by '|'.\n";
  cerr << "                          The entire INDICES list should be in\n";
  cerr << "                            double quotes.\n";
  cerr << "\n";
  cerr << "                          Example: --models=\"1,2|3\"\n";
  cerr << "                            This says run two models: \n";
  cerr << "                            The 1st on features 1 and 2.\n";
  cerr << "                            The 2nd on features 1 and 3.\n";
  cerr << "                            Note that the 2nd list omitted 1.\n";
  cerr << "                            But, feature 1 (intercept) is\n";
  cerr << "                              auto included when omitted.\n";
}