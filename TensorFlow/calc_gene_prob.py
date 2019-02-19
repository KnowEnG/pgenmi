#from scipy.stats import chi2
import argparse
import sys
import numpy as np
import pandas as pd
import itertools

#ARGS = None
pnames = ["PRIOR-0", "PRIOR-1", "LIK-0", "LIK-1", "LIK", "POST-0", "POST-1"]


def sigmoid(x, derivative=False):
  return x*(1-x) if derivative else 1/(1+np.exp(-x))
  
  
def main(args):
  param_file = args.param_file
  input_file = args.input_file
  output_file = args.output_file
  
  # Read pgenmi input and make GENE rownames and remove column
  din = pd.read_csv(input_file, sep="\t")
  din.index = din["GENE"]
  din = din.iloc[:,1:] 
  dout = din
  genes = din.index
  
  # Read pgenmi output and extract w,a and make name (H_ALL,H_KD,etc..) rownames
  dpfull = pd.read_csv(param_file, sep="\t") 
  dp = dpfull.iloc[:,list(range(dpfull.columns.get_loc("TRAINED_LOGLIK_PREV") + 1, len(dpfull.columns)))]
  dp.index = dpfull["NAME"]

  # Subset data frames in w,a for parameters and x,y for input
  dw = dp.iloc[:,0:-1]
  da = dp.iloc[:,-1]
  dx = din.iloc[:,1:]
  dy = din.iloc[:,0]
  models = dw.index
  
  # Make output
  #dout = pd.concat([dout, pd.DataFrame(1- sigmoid(np.matmul(dx, dw.transpose())), index=genes,columns=[x + '_' + model_prob[0] for x in list(models)])], axis=1) 
  model_prob = pd.DataFrame([[x + '_' + i for x in list(models)] for i in pnames], columns=models, index=pnames).transpose()
  dout[model_prob["PRIOR-0"]] = pd.DataFrame(1- sigmoid(np.matmul(dx.values, dw.transpose().values)), index=genes)
  dout[model_prob["PRIOR-1"]] = 1 - dout[model_prob["PRIOR-0"]]
  dout[model_prob["LIK-0"]] = dout[model_prob["PRIOR-0"]]
  dout[model_prob["LIK-1"]] = pd.DataFrame(da.apply(lambda x: np.power(dy,x-1)).apply(lambda x: x * da).transpose().values * dout[model_prob["PRIOR-1"]].values, index=genes)
  dout[model_prob["LIK"]]  = pd.DataFrame(dout[model_prob["LIK-0"]].values + dout[model_prob["LIK-1"]].values, index=genes)
  dout[model_prob["POST-0"]] = pd.DataFrame(dout[model_prob["LIK-0"]].values / dout[model_prob["LIK"]].values,index=genes)
  dout[model_prob["POST-1"]]= pd.DataFrame(dout[model_prob["LIK-1"]].values / dout[model_prob["LIK"]].values,index=genes)
  ratio_pairs = [i for j in [list(itertools.combinations(k,2)) for k in [list(l) for l in model_prob.transpose().values]] for i in j]
  ratio_names = ['|'.join(i) for i in ratio_pairs]
  dout[ratio_names] = pd.DataFrame([dout[pair[0]].values / dout[pair[1]].values for pair in ratio_pairs], index=ratio_names, columns=genes).transpose()
  dout.to_csv(output_file,sep="\t") 
  
  
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--param_file", type=str, default="param_file.tsv",
                        help='File to run pGENMi on')
    parser.add_argument("--input_file", type=str, default="input_file.tsv",
                        help='File to run pGENMi on')
    parser.add_argument("--output_file", type=str, default="output_file.tsv",
                        help='File to run pGENMi on')
    ARGS, unparsed = parser.parse_known_args()
    main(ARGS)
    #tf.app.run(main=main, argv=[sys.argv[0]] + unparsed)
