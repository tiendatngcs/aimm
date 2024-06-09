import sys
import networkx as nx
from gensim.models import Word2Vec, KeyedVectors
import numpy as np
import random

sys.path.append("src/node2vec/src")

import node2vec as n2v

#from mem_env import FeatureExtract as fext

A = None

class find_cube:
  def __init__(self, agent):
    global A
    A = agent
  
  def get_agent():
    return A

  def get_cube(input_for_n2v):#input needs to go through the node2vec embedding layer
    
    #directed = True
    #p = 1
    #q = 1
    #num_walks = 2
    #walk_length = 10
    #dimensions = 32
    #window_size = 4
    #workers = 8
    #iterations = 1
    #
    #if(len(input_for_n2v)<=0):
    #  return random.randint(0,15)

    #G = nx.DiGraph()
    #G.add_weighted_edges_from(input_for_n2v)
    #G = n2v.Graph(G, directed, p, q)
    #G.preprocess_transition_probs()
    #walks = G.simulate_walks(num_walks, walk_length)
    ##learn_embeddings(walks)
    #walks = [map(str, walk) for walk in walks]
    #model = Word2Vec(walks, size=dimensions, window=window_size, 
    #    min_count=0, sg=1, workers=workers, iter=iterations)
    ##model.wv.save_word2vec_format(output)
    #embedding_matrix = np.zeros((len(model.wv.vocab), 32))
    #for i in range(len(model.wv.vocab)):
    #  embedding_vector = model.wv[model.wv.index2word[i]]
    #  if embedding_vector is not None:
    #     embedding_matrix[i] = embedding_vector
    #
    #rows = embedding_matrix.shape[0]
    #if(rows < 1024):
    #  ele = np.zeros(((1024-rows), 32))
    #  embedding_matrix = np.append(embedding_matrix, ele)
    #  embedding_matrix = embedding_matrix.reshape(1024,32)
   
    flat_list = [item for sublist in input_for_n2v for item in sublist]
    #print(flat_list)
    if(len(flat_list) < 128):
      ele = np.zeros(((128-len(flat_list)), 1))
      flat_list = np.append(flat_list, ele)
    flat_list = np.array(flat_list)
    flat_list = flat_list.reshape(8,16,1)

    action = A.forward(flat_list)
      
    ''' PROBLEM :: Using this action we can not allocate new frame allocation, 
                   because the actions are trined for migrations and not for 
                   allocations !!!!!
    ''' 
    #print("***PROBLEM:: We can not use the agent for allocatiion which is trained for migration ***")
    #assert(0)

    return action 
