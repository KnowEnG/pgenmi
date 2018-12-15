## pGENMi v2 Code

This github page contains pGENMi code to associate TFs with Drugs. The original paper in which v1 was applied is published in Genome Research https://www.ncbi.nlm.nih.gov/pubmed/29898900.

### Making
Modify Makefile with your platforms specific needs. This code uses C++11.
The given Makefile assumes a .obj and bin directory. Simply make all as below:

    make all
  
This should produce pgenmi which is the main program that produces output on the likelihoods for various models, by default two: one with all (H_ALL) features and one with only the intercept (H_0 or H_NULL or H_NULL_X0).

### Input Data

The format of the data to pgmllr is TSV. It includes a header like below:

GENE    PVAL  A0  A1  A2

ENSG00000000001 0.01 1 1 0

ENSG00000000002 0.30 1 0 1

ENSG00000000003 0.04 1 1 1

...

ENSG00000000500 1.00  1 0 0

The header should include PVAL in the first column and then A#, where the # indexes the regulatory evidence for the column.
All subsequent datalines should begin with a symbol in the first column, p-value in the second column, a 1 for the third column, and then the respective regulatory evidences for the remaining columns.

### Running C++ pGENMi

The pgenmi program accepts three arguments: the path to the file with the aforementioned input data, a drug label, and a tf label
    
    ./pgenmi OPTIONS /path/to/input/file

The output is in the following format where , are replaced by tabs. Each model is a row in the file. The loglikelihood of the learned model and its initialization likelihood, along with parameters, are printed.

TF, DRUG, MODELNAME, NUM_FEATURES, MODEL BITMASK, INITAL_MODEL, TRAINED_MODEL

A model is printed as follows:

MODEL_ITER, MODEL_LOGLIK, MODEL_LOGLIK_PREV, MODEL_WEIGHTS, MODEL_ALPHA

Here MODEL is either trained or initial. Thus, each row has both initial and trained parameters and performance.

What distinguishes rows from one another are bitmasks, which are the length of the weight vectors. These bitmasks remove certain parameters from the model. The first bit in a bitmask, corresponding to whether or not to include the intercept term of the model, is always 1.

#### Options
```
  -a,  --all_disable      Disable both H_ALL and H_NULL model runs.
  -d,  --drug             Drug name (Default is DRUG).
  -f,  --full_disable     Disable full (H1) model.
  -h,  --help             Print this usage guide.
  -n,  --null_disable     Disable null (H0) model.
  -r,  --restarts=NUM     Restart model runs NUM times. 
  -s,  --singles          Run all models with only one feature
                            and intercept active.
  -t,  --tf               TF name (Default is TF).
  -w,  --without_singles  Run all models missing only one feature
                            excluding the intercept.

  -i,  --names="CSV"      A CSV of model names corresponding
                            to and in the order of --models.
                          If --models unset, this is ignored.
                          Default model names generated if there
                            more models than names, for the extra
                            models without names.
                          The CSV should be in double quotes.
                          Example: --names="H2,H3".

  -m,  --models="CSV"     Run models on CSV feature lists.
                          Features are column indices in FILe 
                            using 1 based indexing.
                          A model is a comma seperated value
                            (CSV) list of feature indices.
                          To specify multiple models, seperate
                            individual CSV models by '|'.
                          The entire INDICES list should be in
                            double quotes.
                          Example: --models="1,2|3"
                            This says run two models: 
                            The 1st on features 1 and 2.
                            The 2nd on features 1 and 3.
                            Note that the 2nd list omitted 1.
                            But, feature 1 (intercept) is
                              auto included when omitted.
```                              
## Running TensorFlow pGENMi

The TensorFlow directory contains a pgenmi.py file that behaves nearly identically to C++ pgenmi; however, it leverages built in optimizers that can be modified and customized. This of course presumes that your machine has python3 and TensorFlow installed.

The TensorFlow/example directory contains a NOTES file on how to run TensorFlow pGENMi properly. It follows a similar structure to the C++ version.

    python3 pgenmi.py --input_file /path/to/input/file

Getting the output in the same format as the C++ version will require some post-processing manipulation. Additionally, the null hypothesis, H0, is not tested within each run. Instead, a file for H0 must be created and run in pgenmi.py. The example directory contains example files on this.

