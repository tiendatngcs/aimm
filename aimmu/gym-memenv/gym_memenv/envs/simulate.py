import argparse
import numpy as np
import gym
import gym_memenv

import keras
from keras import layers
from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten, Bidirectional, LSTM, Embedding, Dropout, Conv1D, Conv2D, Reshape
from keras.layers import TimeDistributed, GlobalAveragePooling2D, Lambda, ConvLSTM2D
from keras.constraints import maxnorm
from keras.layers.convolutional import MaxPooling2D
from keras.utils import np_utils
from keras.optimizers import Adam
from keras import initializers 
from aimmuagent.aimmu_agent import AIMMUAgent
import tensorflow as tf

from rl.policy import EpsGreedyQPolicy
from rl.memory import SequentialMemory
#from temp_memory import TempSequentialMemory  

#from gprof import GProfiler
#import marshal

from aimmuagent.infer_cube import find_cube

class Sim:

  def __init__(self, args):
    ENV_NAME = 'memenv-v0'
    # Get the environment and extract the number of actions.
    env = gym.make(ENV_NAME)
    nb_actions = env.action_space.n
    env.get_simpptr(args) 
    
    model = Sequential()
    #model.add(ConvLSTM2D(32,(3,3), input_shape=(1,8,16,1), activation='relu', return_sequences=False))
    #model.add(Conv2D(32,(3,3), padding='same', activation='relu', kernel_constraint=maxnorm(3)))
    #model.add(Conv2D(32,(3,3), padding='same', activation='relu', kernel_constraint=maxnorm(3)))
    #model.add(Conv2D(32,(3,3), padding='same', activation='relu', kernel_constraint=maxnorm(3)))
    #model.add(MaxPooling2D(pool_size=(2,2)))
    
    #model.add(Flatten())
    model.add(Flatten(input_shape=(1,) + env.observation_space.shape))
    model.add(Dense(256, activation = 'relu'))
    model.add(Dense(128, activation = 'relu'))
    model.add(Dense(64, activation = 'relu'))
    model.add(Dense(32, activation = 'relu'))
    model.add(Dense(16, activation = 'relu'))
    model.add(Dense(16, activation = 'relu'))
    #model.add(Dense(nb_actions, activation='softmax'))
    model.add(Dense(nb_actions, activation='linear'))
   

    print(model.summary())
    # Finally, we configure and compile our agent. You can use every built-in Keras optimizer and
    # even the metrics!
    mem_size = int(args.replay_buff_sz)
    memory = SequentialMemory(limit=mem_size, window_length=1)#window is related with the number of actions
    #TempSeqMemory = TempSequentialMemory(limit=500000, window_length=1)
    policy = EpsGreedyQPolicy()
    # you can specify the dueling_type to one of {'avg','max','naive'}
    dqn = AIMMUAgent(env, model=model, nb_actions=nb_actions, memory=memory, 
        nb_steps_warmup=100, enable_dueling_network=True, dueling_type='max', 
        target_model_update=1e-2, policy=policy)
    #dqn.compile(Adam(lr=0.07), metrics=['mae', 'accuracy'])
    dqn.compile(Adam(lr=0.005), metrics=['mae', 'accuracy'])#,'sparse_categorical_crossentropy'])
    
    #dqn.load_weights("saved_weights/ai_mmu_memenv-v0_weights.h5f")
    
    fc = find_cube(dqn)
    dqn.fit(env, nb_steps=100000000, action_repetition=1, visualize=False, verbose=2)
    # After training is done, we save the final weights.
    dqn.save_weights("saved_weights/ai_mmu_{}_weights.h5f".format(ENV_NAME), overwrite=True)

'''
Add configuration parameters as per the requirement
'''

def configure():
  
  parser = argparse.ArgumentParser()
  
  parser.add_argument('--aimmu-config', default='setup/default.cfg', 
      help='Please provide the aimmu-sim config file, default = setup/default.cfg')
  
  parser.add_argument('--graph-folder', default='stats', 
      help='Please the path to the graph folder, default = stats')
  
  parser.add_argument('--replay-buff-sz', default=500000, 
      help='Size of the Agent\'s replay buffer, default = stats')
  
  parser.add_argument('--best-migcand', default=False, 
      help='It is a simulation methodology, which finds the best mig candidate and trains them')
  
  parser.add_argument('--adaptive-epoch', default=False, 
      help='Flag to enable the neural circuit based adaptive epoch engine')
  
  '''arguments related to node2vec'''
	
  parser.add_argument('--input', nargs='?', default='graph/karate.edgelist',
	                    help='Input graph path')

  parser.add_argument('--output', nargs='?', default='graphs/emb/random.emb',
                      help='Embeddings path')
  
  parser.add_argument('--dimensions', type=int, default=1,
                      help='Number of dimensions. Default is 1.')
  
  parser.add_argument('--walk-length', type=int, default=16,
                      help='Length of walk per source. Default is 16.')
  
  parser.add_argument('--num-walks', type=int, default=1,
                      help='Number of walks per source. Default is 1.')
  
  parser.add_argument('--window-size', type=int, default=16,
                    	help='Context size for optimization. Default is 16.')
  
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
  print("aimmu-config >>>>   >>>>  >>>>> ", args.aimmu_config)
  return args


def main():
  args = configure()
  S = Sim(args)

if __name__ == '__main__':
  main()
