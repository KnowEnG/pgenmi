## pGENMi Code

This github page contains code to associate TFs with Drugs.

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

### Running pgenmi



