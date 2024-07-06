import argparse
import gym
import gym_memenv
import numpy as np


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
  # obj = rlmmu_pybind.wrapper_mmu(args.aimmu_config)
  # obj.run()
  ENV_NAME = 'memenv-v0'
  # Get the environment and extract the number of actions.
  env = gym.make(ENV_NAME)
  nb_actions = env.action_space.n
  env.get_simpptr(args)
  env.reset()
  for i in range(10000000):
    print("Epoch", i)
    env.step(1)
  

if __name__ == '__main__':
  main()