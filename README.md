## pGENMi Code

This github page contains pGENMi code to associate TFs with Drugs. The paper is posted on [bioArxiv](http://www.biorxiv.org/content/early/2017/06/02/145607) and is in submission to journals.

### Making
Modify Makefile with your platforms specific needs. This code uses C++11.
The given Makefile assumes a .obj and bin directory. Simply make all as below:

    make all
  
This should produce pgmllr which is the main program that produces output on the likelihoods for the full model and null model, as well as parameters and other information.

### Input Data

The format of the data to pgmllr is TSV. It includes a header like below:

PVAL  A0  A1  A2

ENSG00000000001 0.01 1 1 0

ENSG00000000002 0.30 1 0 1

ENSG00000000003 0.04 1 1 1

...

ENSG00000000500 1.00  1 0 0

The header should include PVAL in the first column and then A#, where the # indexes the regulatory evidence for the column.
All subsequent datalines should begin with a symbol in the first column, p-value in the second column, a 1 for the third column, and then the respective regulatory evidences for the remaining columns.

### Running C++ pGENMi

The pgenmi program accepts three arguments: the path to the file with the aforementioned input data, a drug label, and a tf label
    
    ./pgenmi /path/to/input/file drug tf

The output is in the following format where , are replaced by tabs and H1 indicates the model with regulatory evidences and H0 the null model:

drug, tf, 2 * LLR, H1 LogLikelihood (H1LL), H1 LogLikelihood Before Convergence (H1LLL), H1LL - H1LLL, H1 Initial Log Likelihood Before Learning, Number of EM iterations to train H1, Tab Separated List of H1's initial parameters, H1 initial alpha, Tab Separated List of H1's final parameters, H1 final alpha, all of the previous metrics reported for H1 instead reported for H0.

## Running TensorFlow pGENMi

The TensorFlow directory contains a pgenmi.py file that behaves nearly identically to C++ pgenmi; however, it leverages built in optimizers that can be modified and customized. This of course presumes that your machine has python3 and TensorFlow installed.

The TensorFlow/example directory contains a NOTES file on how to run TensorFlow pGENMi properly. It follows a similar structure to the C++ version.

    python3 pgenmi.py --input_file /path/to/input/file

Getting the output in the same format as the C++ version will require some post-processing manipulation. Additionally, the null hypothesis, H0, is not tested within each run. Instead, a file for H0 must be created and run in pgenmi.py. The example directory contains example files on this.

