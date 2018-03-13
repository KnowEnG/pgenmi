import argparse
import csv
import math
import os
import sys
import tensorflow as tf
from scipy.stats import chi2
import numpy as np
import tensorflow.contrib.slim as slim
import tensorflow.contrib.metrics as metrics

ARGS = None

def get_file_dims(input_file, header=True):
  num_columns = 0
  num_genes = 0
  with open(input_file) as fp:
    line_no = 0
    for line in fp:
      line_no += 1
      if (line_no == 1 and header == True):
        next;
      else:
        new_num_columns = len(line.split("\t"))
        if (num_columns != 0 and num_columns != new_num_columns):
          print("Error the number of columns isn't consistent")
          print(line);
          print("Num genes " + str(num_genes))
          print("New columns " + str(new_num_columns))
          print("Old columns " + str(num_columns))
          exit(1)
        num_columns = new_num_columns
        num_genes += 1 
  return num_genes, num_columns
  
  
def load_data(input_file, num_features):
  evidence = np.loadtxt(input_file, dtype="float64", skiprows = 1, delimiter="\t", usecols=tuple([i for i in range(2, 2 + num_features)]))
  data = np.loadtxt(input_file, dtype="float64", skiprows = 1, delimiter="\t", usecols=1)
  genes = np.loadtxt(input_file, dtype="str", skiprows = 1, delimiter="\t", usecols=0)
  return genes, data, evidence
  
  
def mstep(new_a, loglik, scope, reuse=True):
  with tf.variable_scope(scope, reuse=reuse):
    w = tf.get_variable("w", dtype=tf.float64)
    a = tf.get_variable("a", dtype=tf.float64)
  a_update = tf.assign(a, tf.maximum(tf.convert_to_tensor(0.000001, tf.float64), tf.minimum(new_a, 1)))
  w_update = tf.train.AdamOptimizer(0.01).minimize(-1 * loglik);
  return a_update, w_update, w
  
  
def estep(x, y, n, m, scope, reuse=False):
  # ESTEP: Compute Posteriors
  with tf.variable_scope(scope, reuse=reuse):
    w = tf.get_variable("w", shape=[m, 1], dtype=tf.float64);
    a = tf.get_variable("a", dtype=tf.float64, trainable=False, initializer=tf.random_uniform([], minval=0, maxval=1, dtype=tf.float64));
    #tf.add_to_collection("NON_GRADIENT", a)
    
  # Compute Pr(zg=1| xg1..xgm, w1...wm)
  if m == 1:
    prob_zg1 = tf.sigmoid(tf.squeeze(tf.matmul(tf.ones([1,1], dtype=tf.float64), w)));
  else:
    prob_zg1 = tf.sigmoid(tf.squeeze(tf.matmul(x, w)));
  # Compute p(yg, zg=1 | xg1..xgm, w1...wm)
  prob_pg1 = a * tf.pow(y, a - 1) * prob_zg1
  # Compute p(yg, zg=0 | xg1..xgm, w1...wm)
  prob_pg0 = 1 - prob_zg1
  # Compute p(yg| xg1..xgm, w1...wm)
  prob_pg = prob_pg1 + prob_pg0
  # Compute p(zg=1|yg, xg1..xgm, w1...wm)
  post_zg1 = tf.div(prob_pg1, prob_pg)
  # Compute total loglik
  loglik = tf.reduce_sum(tf.log(prob_pg));
  # update a
  new_a = tf.div(tf.reduce_sum(-1 * post_zg1), tf.reduce_sum(post_zg1 * tf.log(y)));
  return loglik, new_a, post_zg1
  
  
def inference(x, y, n, m, scope, reuse=False):
  loglik, new_a, post_zg1 = estep(x, y, n, m, scope, reuse)
  a_update, w_update, new_w = mstep(new_a, loglik, scope)
  with tf.control_dependencies([a_update, w_update]):
    new_loglik, blah2, new_post_zg1 = estep(x, y, n, m, scope, True);
  return new_loglik, a_update, w_update, new_w
  
  
def next_step(sess, ops, scope, i, feed_dict=None):
  output = sess.run(ops, feed_dict=feed_dict)
  model = {'name': scope, 'i': i, 'loglik': output[0], 'a': output[1], 'w': output[3].tolist()}
  return model
  
  
def get_model(sess, loglik_op, scope, i, feed_dict=None):
  loglik = sess.run(loglik_op, feed_dict=feed_dict);
  a = sess.run(get_var(scope, 'a'))
  w = sess.run(get_var(scope, 'w'))
  model = {'name': scope, 'i': i, 'loglik': loglik, 'a': a, 'w': w.tolist()}
  return model
  

def print_models(models):
  for model in models:
    print_model(model)
    

def print_model(model):
 print (str_model(model) + "\n") ;
 
 
def str_model(model):
  w_str = str(np.squeeze(model["w"])).replace('[', '').replace(']','').replace(',','\t');
  return str(model["name"]) + "\t" + str(model["i"]) + "\t" + str(model['loglik']) + "\t" + w_str + "\t" + str(model["a"])
  
  
def get_loglik(x, y, n, m, scope, reuse=True):
  loglik, new_a, post_zg1 = estep(x, y, n, m, scope, reuse)
  return loglik
  
  
def get_var(scope, var):
  with tf.variable_scope(scope, reuse=True): 
    return tf.get_variable(var, dtype=tf.float64)
  

def run_graph(input_file, evidence, data, num_genes, num_features, min_steps=5000, max_steps=10000, delta=0.001, last_n=1000):
  global_step = tf.Variable(0, trainable=False, name='global_step')
  x = tf.convert_to_tensor(evidence)
  y = tf.convert_to_tensor(data)
  h1 = inference(x, y, num_genes, num_features, input_file)
  
  with tf.Session() as sess:
    sess.run(tf.global_variables_initializer());
    h1_prev = get_model(sess, h1[0], input_file, 'INITIAL_0', feed_dict = None)
    h1_init = h1_prev
    i = 1
    delta_loglik = delta + 1
    h1_last_n_loglik = h1_prev["loglik"]
    while((i <= min_steps) or (delta_loglik > delta and i <= max_steps)):
      h1_next = next_step(sess, h1, input_file, 'FINAL_' + str(i), feed_dict=None)
      if (i % last_n == 0):
        delta_loglik = h1_next["loglik"] - h1_last_n_loglik
        h1_last_n_loglik = h1_next["loglik"]
      i+=1
      
    h1_init_str = str_model(h1_init)
    h1_next_str = str_model(h1_next) 
    print(h1_init_str + "\t" + h1_next_str);
    
  
def main(_):
  os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
  seed = ARGS.seed;
  input_file = ARGS.input_file
  tf.set_random_seed(seed)
  num_genes, num_columns = get_file_dims(input_file)
  num_features = num_columns - 2;
  genes, data, evidence = load_data(input_file, num_features);
  run_graph(input_file, evidence, data, num_genes, num_features)
  
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_file", type=str, default="input.tsv",
                        help='File to run pGENMi on')
    parser.add_argument('--seed', type=int, default='10902',
                        help='Set seed for reproducibility')
    ARGS, unparsed = parser.parse_known_args()
    tf.app.run(main=main, argv=[sys.argv[0]] + unparsed)