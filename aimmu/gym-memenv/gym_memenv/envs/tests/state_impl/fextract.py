import sys
import argparse
import numpy as np
import networkx as nx
from gensim.models import Word2Vec
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import write_dot, graphviz_layout

sys.path.append("bin")
sys.path.append("node2vec/src")

import node2vec as n2v

try:
  import graph_pybind
except ImportError as e:
  raise error.DependencyNotInstalled("{}. (Please make sure that the pybind library is there in the right place.)'".format(e))

class FeatureExtract:

  def __init__(self, args):
    self.fex = graph_pybind.graph_wrapper(args.graph_config)
  
  def run_sim(self, args):
    self.fex.run_ops(int(args.num_ops))

  def display_raw(self):
    self.fex.display_raw_data()

  def get_n2v_inp(self):
    self.inp = self.fex.get_n2v_inp()
    print("\n\n Printing input for Node2Vec \n\n")
    print(self.inp)
    return self.inp


'''Reading configurations'''
def configure():
  
  parser = argparse.ArgumentParser()
  
  '''arguments related to state creation testing '''

  parser.add_argument('--graph-config', default='utils/conf.cfg', 
      help='Please provide the graph config file, default = utils/conf.cfg')
  
  parser.add_argument('--num-ops', default=100000, 
      help='Please provide the number of operations for testing')
  
  parser.add_argument('--disp-raw', default=False, 
      help='[Optional] display for the raw values collected')
  
  
  '''arguments related to node2vec'''
	
  parser.add_argument('--input', nargs='?', default='graph/karate.edgelist',
	                    help='Input graph path')

  parser.add_argument('--output', nargs='?', default='emb/karate.emb',
                      help='Embeddings path')
  
  parser.add_argument('--dimensions', type=int, default=128,
                      help='Number of dimensions. Default is 128.')
  
  parser.add_argument('--walk-length', type=int, default=80,
                      help='Length of walk per source. Default is 80.')
  
  parser.add_argument('--num-walks', type=int, default=10,
                      help='Number of walks per source. Default is 10.')
  
  parser.add_argument('--window-size', type=int, default=10,
                    	help='Context size for optimization. Default is 10.')
  
  parser.add_argument('--iter', default=1, type=int,
                      help='Number of epochs in SGD')
  
  parser.add_argument('--workers', type=int, default=8,
                      help='Number of parallel workers. Default is 8.')
  
  parser.add_argument('--p', type=float, default=1,
                      help='Return hyperparameter. Default is 1.')
  
  parser.add_argument('--q', type=float, default=1,
                      help='Inout hyperparameter. Default is 1.')
  
  parser.add_argument('--weighted', dest='weighted', action='store_true',
                      help='Boolean specifying (un)weighted. Default is unweighted.')
  parser.add_argument('--unweighted', dest='unweighted', action='store_false')
  parser.set_defaults(weighted=False)
  
  parser.add_argument('--directed', dest='directed', action='store_true',
                      help='Graph is (un)directed. Default is undirected.')
  parser.add_argument('--undirected', dest='undirected', action='store_false')
  parser.set_defaults(directed=False)


  args = parser.parse_args()
  print("graph-config >>>> --- >>>> --- >>>> ", args.graph_config)
  return args


'''
Reads the input network in networkx.
'''
def read_graph(input_n2v):
  G = nx.DiGraph()
  G.add_weighted_edges_from(input_n2v)
  return G


'''
Learn embeddings by optimizing the Skipgram objective using SGD.
'''
def learn_embeddings(walks):
  walks = [map(str, walk) for walk in walks]
  model = Word2Vec(walks, size=args.dimensions, window=args.window_size, min_count=0, sg=1, workers=args.workers, iter=args.iter)
  model.wv.save_word2vec_format(args.output)
  #model.save_word2vec_format(args.output)
  return


def main(args):
  S = FeatureExtract(args)
  S.run_sim(args)
  if(args.disp_raw):
    S.display_raw()
  input_n2v = S.get_n2v_inp()
  '''
  Pipeline for representational learning for all nodes in a graph.
  '''
  nx_G = read_graph(input_n2v)
  nx.draw(nx_G, pos=nx.spring_layout(nx_G))
  plt.savefig("input/representation/random.png")
  G = n2v.Graph(nx_G, args.directed, args.p, args.q)
  G.preprocess_transition_probs()
  walks = G.simulate_walks(args.num_walks, args.walk_length)
  learn_embeddings(walks)


if __name__ == '__main__':
  args = configure()
  main(args)
