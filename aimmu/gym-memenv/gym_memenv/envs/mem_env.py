import gym
from gym import error, spaces, utils
from gym.utils import seeding
import numpy as np
import math
import random
import sys
import matplotlib.pyplot as plt
import copy
import os
import networkx as nx
from gensim.models import Word2Vec, KeyedVectors
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import write_dot, graphviz_layout

sys.path.append("bin")
sys.path.append("src/node2vec/src")

#from aimmuagent.infer_cube import find_cube

import node2vec as n2v

try:
  import rlmmu_pybind
except ImportError as e:
  raise error.DependencyNotInstalled("{}. (Please make sure that the pybind library is there in the right place.)'".format(e))


''' 
FeatureExtract is an interface with the node2vec embedding 
logic for graph 
'''
class FeatureExtract():
  def __init__(self, args):
    self.args = args
    
  '''
  Learn embeddings by optimizing the Skipgram objective using SGD.
  '''
  def learn_embeddings(self, input_n2v, sys_state):
    G = nx.DiGraph()
    #print(input_n2v)
    #nx_G = self.read_graph(input_n2v)
    #self.read_graph(input_n2v)
    #nx.draw(nx_G, pos=nx.spring_layout(nx_G))
    #plt.savefig("graphs/input/representation/random.png")
    G.add_weighted_edges_from(input_n2v)
    G = n2v.Graph(G, self.args.directed, self.args.p, self.args.q)
    G.preprocess_transition_probs()
    walks = G.simulate_walks(self.args.num_walks, self.args.walk_length)
    #learn_embeddings(walks)
    #print(walks)
    walks = [map(str, walk) for walk in walks]
    model = Word2Vec(walks, size=self.args.dimensions, window=self.args.window_size, 
        min_count=0, sg=1, workers=self.args.workers, iter=self.args.iter)
    #model.wv.save_word2vec_format(self.args.output)
    embedding_matrix = np.zeros((len(model.wv.vocab), 8))
    for i in range(len(model.wv.vocab)):
      embedding_vector = model.wv[model.wv.index2word[i]]
      if embedding_vector is not None:
         embedding_matrix[i] = embedding_vector
    
    #print("0 ----- embedding_matrix shape: ", embedding_matrix.shape, "sys: ", sys_state.shape)
    embedding_matrix = np.append(embedding_matrix,sys_state)
    
    flat_list = [item for sublist in embedding_matrix for item in sublist]
    
    #print(embedding_matrix)
    #rows = embedding_matrix.shape[0]
    #print("1 --- embedding matrix shape: ", embedding_matrix.shape, " rows: ", rows)
    ##self.state = np.full(((3,32)),0)
    #if(rows < 128):
    #  ele = np.zeros(((128-rows), 1))
    #  embedding_matrix = np.append(embedding_matrix, ele)

    #print("2 --- embedding matrix shape: ", embedding_matrix.shape, " rows: ", rows)
    #embedding_matrix = embedding_matrix.reshape(6,6,1)
    #print(embedding_matrix)
    #return embedding_matrix
    embedding_matrix = flat_list.reshape(6,6,1)
    return embedding_matrix


'''
MenEnv class processes the raw input for the agent and also
acts as the mediator by mimicing some part of the memory network 
'''
class MemEnv(gym.Env):
  
  def __init__(self):
    initial_state = np.zeros((8, 16, 1))
    #initial_state = np.zeros((16, 32)) #n2v embedding
    #0-no-migr, 1-near-to-src/dest, 2-far-from-src/dest, 3-computation-migr-nei, 4-computation-migr-far, 5-training samplingrate increase
    #(tsri), 6-tsrd (tsr decrease), 7-flip dest
    self.action_space = spaces.Discrete(8) #4-SPMV,  
    
    #self.agent = find_cube.get_agent()
    self.observation_space = spaces.Box(-initial_state, initial_state, dtype=np.float32)
    self.state = None
    self.training = True
    self.hop_monitor = []
    self.action_monitor = []
    self.reward_monitor = []
    self.num_steps = 0
    self.epoch = 1
    self.reward = 0
    self.num_times_updated = 0
    self.reward_bin = []
    self.tot_episode_reward = 0
    self.avg_reward_monitor = []
    self.opc_monitor = []
    self.apps_page_info_list = []
    self.action_list = []
    self.state_common_part = [] #num_cube = 16, #hist_len = 16
    self.apps_page_info = [] 
    self.proc = 0

  def get_simpptr(self, args):
    self.obj = rlmmu_pybind.wrapper_mmu(args.aimmu_config)
    self.FeatureExt = FeatureExtract(args)
    self.graph_folder = self.obj.stats_folder()
    print("graph folder: ", self.graph_folder)
    self.best_migcand = args.best_migcand

    try:
      os.mkdir(self.graph_folder)
    except OSError as error:
      print(error)

    frame_per_cube = self.obj.get_frames_per_cube()
    self.num_of_processes = self.obj.get_num_proc()
    # there are 16 cubes, 4KB pages, 65536 frames per cube!
    
    print("============= configurations on agent side: START ===============")
    print("Number of frames per cube: ", frame_per_cube)
    print("Number of processes: ", self.num_of_processes)
    print("============= configurations on agent side: END ===============")
    
    self.opc = self.obj.run()
    self.opc = [0]*self.num_of_processes #operations per cycle!!!!
    self.last_opc = [0]*self.num_of_processes
    self.epoch_opc = [0]*self.num_of_processes
    self.best_opc = [0]*self.num_of_processes #best operations per cycle!!!!
    self.tot_episode_opc = [0]*self.num_of_processes #best operations per cycle!!!!
    
  def step(self, action):
    
    if(self.num_steps % self.num_of_processes == 0):
      self.proc = 0
      self.obj.send_action(self.action_list)
      self.action_list.clear()
      self.opc = self.obj.run()
      #self.input_n2v = self.obj.get_input_node2vec()
      self.sys_state = np.array(self.obj.get_system_state(), dtype=object)
      #self.state = np.array(self.FeatureExt.learn_embeddings(self.input_n2v, self.sys_state))
      ################################################################################
      ### state: {#cube x 3} + {#hist_len x 1} + {#num_proc x (3 + #hist_len x 4)} ###
      ################################################################################
      flat_list = [item for sublist in self.sys_state for item in sublist]
      self.state_common_part = flat_list[0:48] #num_cube = 16, #hist_len = 16
      self.apps_page_info = flat_list[64:] 
      self.apps_page_info_list.clear()

    self.num_steps += 1
    self.action_list.append(action)
    
    start = 67*self.proc
    end = 67 + 67*self.proc
    self.apps_page_info_list.append(self.apps_page_info[start:end]);
    
    state = self.state_common_part + self.apps_page_info_list[self.proc]
    if(len(state) < 128):
      ele = np.zeros(((128-len(state)), 1))
      state = np.append(state, ele)
    state = np.array(state)
    state = state.reshape(8,16,1)
    self.state = state
    
    g_reward = self.opc[self.proc] - self.best_opc[self.proc]
    l_reward = self.opc[self.proc] - self.last_opc[self.proc]
    
    if(self.opc[self.proc] > self.last_opc[self.proc]):
      self.reward = 1
    elif(self.opc[self.proc] == self.last_opc[self.proc]):
      self.reward = 0
    else:
      self.reward = -1
    
    self.last_opc[self.proc] = self.opc[self.proc]
    
    if(self.opc[self.proc] > self.best_opc[self.proc]):
      self.best_opc[self.proc] = self.opc[self.proc]

    #self.action_monitor.append(action*self.epoch)
    #self.reward_monitor.append(self.reward)

    self.tot_episode_reward += self.reward
    self.tot_episode_opc[self.proc] += self.opc[self.proc]
    if(self.num_steps % 100 == 0):
      #self.avg_reward_monitor.append(self.tot_episode_reward/100)
      #self.opc_monitor.append(self.tot_episode_opc/100)
      self.tot_episode_reward = 0
      self.tot_episode_opc = [0]*self.num_of_processes #best operations per cycle!!!!

    self.proc += 1
    
    if(self.num_steps % 1000 == 0):
      #HP = self.obj.hop_profile()
      #if(len(HP) > 0):
      #  self.hop_monitor = HP[0] 
      #self.plot_hop_monitor()
      self.epoch += 1
      #self.plot_action_reward()
      #self.tot_episode_reward = 0

    done = self.obj.finished()

    return self.state, self.reward, done, {} #done will be true when new graph is taken up
  
  def run_epoch(self, args, print_each=1):
    print("--------->>>> Custom run of the simulation without RL agent")
    self.obj = rlmmu_pybind.wrapper_mmu(args.aimmu_config)
    self.FeatureExt = FeatureExtract(args)
    epoch_count = 0
    while (True):
      # run 256 trace step
      self.obj.run()
      if (epoch_count % print_each == 0):
        print("--------->>>> Printing stats")
        self.print_stats()
      epoch_count+=1

  def reset(self):
    print("xxxxxxxxxxxxxxxxxxxxxxxxxx RESETING ALL THE STATES XXXXXXXXXXXXXXXXXXXXXXXX")
    self.state = np.zeros((8,16,1)) 
    self.obj.reset_finished()
    

    return self.state
  
  def plot_hop_monitor(self):
    a = [item[0] for item in self.hop_monitor]
    plt.plot(a, 'b--o')
    X = range(0,len(self.hop_monitor))
    b = [item[2] for item in self.hop_monitor]
    plt.scatter(X,b, c = "green", marker = "x", edgecolor="black", s = 5)
    plt.ylabel('Average hops')
    plt.xlabel('Epochs')
    filepath = self.graph_folder + "/" + 'avg_hop_graph.png'
    plt.savefig(filepath)
  
  def plot_action_reward(self):
    print("plotting action-reward-scatter")
    #plt.scatter(self.action_monitor, self.reward_monitor, c = "green", marker = "x", edgecolor="black", s = 5)
    plt.plot(self.avg_reward_monitor)
    plt.plot(self.opc_monitor)
    #plt.ylabel('Reward')
    #plt.xlabel('Action')
    plt.ylabel('Reward')
    plt.xlabel('Epoch')
    filepath = self.graph_folder + "/" + 'avg_action_reward.png'
    plt.savefig(filepath)
  
  def print_stats(self):
    self.obj.print_stats_sim()
