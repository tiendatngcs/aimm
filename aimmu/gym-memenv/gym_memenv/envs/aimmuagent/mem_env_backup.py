import gym
from gym import error, spaces, utils
from gym.utils import seeding
import numpy as np
import math
import random
import sys
import matplotlib.pyplot as plt
import copy

sys.path.append("bin")

try:
  import rlmmu_pybind
except ImportError as e:
  raise error.DependencyNotInstalled("{}. (Please make sure that the pybind library is there in the right place.)'".format(e))


class MemEnv(gym.Env):
  
  def __init__(self):
    self.obj = rlmmu_pybind.wrapper_mmu()
    initial_state = np.zeros(16)
    self.accuracy_monitor = [] 
    self.action_space = spaces.Discrete(16)
    self.observation_space = spaces.Box(-initial_state, initial_state, dtype=np.float32)
    self.seed()
    self.viewer = None
    self.state = None
    self.example_over = True
    self.example = []
    self.iterations = 0
    self.example_state = []
    self.state_dict = {}
    self.epoch_num = 0
    self.pg_cube_map = {}
    self.org_pg_cube_map = {}
    self.improvement_param = 0
    self.num_actions_taken = 0
    self.tot_num_actions_taken = 0
    self.page_miss = 0
    self.tot_page_miss = 0
    self.page_access = 0
    self.episode = 0
    self.example_empty = 0
    self.miss_rate = 0 
    self.accuracy = 0
    self.trace_done_perc = 0
    self.total_reward = 0
    self.episode_reward = 0
    self.reward_monitor = []
    self.prev_key = -1
    self.training = True
    self.prev_action = -1
    #frame_per_cube = 65536 
    frame_per_cube = self.obj.get_frames_per_cube()
    self.num_of_processes = self.obj.get_num_proc()
    # there are 16 cubes, 4KB pages, 65536 frames per cube!
    #self.cube_capacity = (4 * 1024 * 1024)/(4 * 16) #in terms of number of frames
    #self.get_file_list()
    self.cube_list = [frame_per_cube]*16 #initialized with the size!
    #self.cube_list = [256, 512, 64, 65536, 128, 256, 65536, 512, 256, 512, 64, 65536, 128, 256, 65536, 512]
    self.replacement = False
    self.num_repl_penalty = 0
    self.tot_num_repl_penalty = 0
    self.repl_monitor = []
    self.repl_avg_epoch = 0
    self.migration_list = []
    self.hop_monitor = []
    self.migration_cnt = []
    self.missrate_monitor = []
    self.pgmiss_per_process = [0]*(self.num_of_processes)
    self.pgaccess_per_process = [0]*(self.num_of_processes)
    
    print("============= configurations on agent side: START ===============")
    print("Number of frames per cube: ", frame_per_cube)
    print("Number of processes: ", self.num_of_processes)
    print("============= configurations on agent side: END ===============")

  def seed(self, seed=None):
    self.np_random, seed = seeding.np_random(seed)
    return [seed]


  def hop_diff(self, c1, c2):
    dist = int(abs(c1/4 - c2/4) + abs(c1%4 - c2%4))
    return dist 

  def get_state_from_example(self):#here we need to recalculate the state!!!
    temp_state = self.example_state.pop()
    self.prev_key = temp_state[17]
    assert(len(temp_state)==18)
    return temp_state 


  def gen_state_from_dict(self):
    sum_value = 0
    for key in self.state_dict.keys():
      tot_acc = self.state_dict[key][0]
      if(tot_acc!=0):
        temp_state = []
        c1 = self.state_dict[key][18]
        for i in range(1, 17):
          c2 = i-1
          acc_frac = self.state_dict[key][i]/tot_acc
          temp_state.append(acc_frac)
          hops = self.hop_diff(c1,c2)
          if(hops!=0):
            sum_value += acc_frac 
        temp_state.append(sum_value)
        self.state_dict[key][17] = sum_value
        temp_state.append(key)
        self.example_state.append(temp_state)

  def recalculate_new_state(self, key, action):
    temp_state = []
    sum_frac = 0
    tot_acc = self.state_dict[key][0] #total access
    c1 = action
    key_value = key #int(key.split('-')[0])
    if key_value not in self.pg_cube_map:
      assert(False) 
    ###########################################################  
    self.pg_cube_map[key_value] = action #### updating cube !!!
    ########### this updated value will be used next iteration (1000)
    ########### because the dictionary will be updated only on the next epoch
    ###########################################################
    old_cube_k = self.state_dict[key][18]
    self.state_dict[key][18] = c1 #updating the dictionary here
    ###### instead of updating here itself, this infor should be updated on episode end in reset
    ########### but it is okay to update here because the dictionary will be used only on the next epoch
    for i in range(1,17):
      norm_val = 0
      c2 = i-1
      hops = self.hop_diff(c1,c2)
      #WE ARE ADDING ONLY FOR NON-ZERO DISTANT NODES
      # We do this way as the allocation site of the page may change after each action
      if(hops!=0):
        if(tot_acc != 0):
          norm_val =  (self.state_dict[key][i]/tot_acc) * (hops/6) 
        sum_frac +=  norm_val
      
      temp_state.append(norm_val) 
    
    temp_state[c1] = -1 #this one is replcting the new location of the marker righaway and that is okay
                        #as the new value of the sum will remain localized in this state --- with respect to other
                        # states the location for the current page does change in this epoch of (1000). It will be 
                        # reflected only on next 1000 epoch!!!!
    #assert(sum_frac!=0)
    temp_state.append(sum_frac)
    temp_state.append(key)
    
    if(self.cube_list[action] <= 0):
      self.replacement = True
    return temp_state

  

  def get_reward(self, reward_info, prev_sum):
    new_sum = 0
    reward = -1
    for i in reward_info:
      if(i!=-1):
        new_sum += i
    ######################################
    #reward = ((prev_sum - new_sum)/max(new_sum,prev_sum))*10
    ######################################
    #if(new_sum == 0):
    #  reward = 20
    #else:
    #  if(self.replacement==True):
    #    self.replacement = False
    #    self.num_repl_penalty += 1
    #    self.tot_num_repl_penalty += 1
    #    reward = -7
    #  else:
    #    reward = 7

    #    if(prev_sum > new_sum):
    #      reward +=  max(4,((prev_sum - new_sum)/(new_sum + prev_sum))*4)  
    #    else:
    #      reward += -max(18.5, (1 - (prev_sum/new_sum))*15)
    #    if(new_sum == 0):
    #      reward += 13
    ############################################
    if(prev_sum > new_sum):
      reward = 2
    elif(prev_sum == new_sum):
      reward = 0
    else:
      reward = -7.5

    return reward, new_sum


  def gen_state_from_trace(self, action_for_prev_state):
    sum_value = 1
    new_sum = 1
    reward_for_prev_state_action = 0
    new_state = []
    
    if(self.example_over==False and (self.prev_key in self.state_dict)):
      sum_value = self.state_dict[self.prev_key][17]
      recalculated_state = self.recalculate_new_state(self.prev_key, action_for_prev_state)
      assert(len(recalculated_state)==18)
      reward_for_prev_state_action, new_sum = self.get_reward(recalculated_state[:-2], sum_value)
      self.iterations += 1  
      
      if(self.iterations>=1):
        self.iterations = 0 
    else:
      self.prev_action = action_for_prev_state
      self.iterations = 1
      self.example_over = False
    
    new_state = self.get_state_from_example() #pops from front
    assert(len(new_state)==18)
    return new_state[:-2], reward_for_prev_state_action, new_sum 


  def get_state(self, action):
    state, reward, new_sum = self.gen_state_from_trace(action) 
    return state, reward, new_sum
  
  def update_state_dic_trbuff(self, pid, key_r, cube_k, value_r, cube_v):
    key_addr = str(key_r) + "-" + str(pid)
    value_addr = str(value_r) + "-" + str(pid)
    self.page_access += 1
    self.pgaccess_per_process[pid] += 1
    if key_addr in self.state_dict:
      if value_addr not in self.pg_cube_map:
        self.pg_cube_map[value_addr] = cube_v 
        self.org_pg_cube_map[value_addr] = cube_v
      self.state_dict[key_addr][0] += 1
      self.state_dict[key_addr][cube_v+1] += 1
    else: #creating new key entry ==> on a page fault !!!
      self.pg_cube_map[key_addr] = cube_k
      self.org_pg_cube_map[key_addr] = cube_k
      
      self.page_miss += 1
      self.tot_page_miss += 1
      self.pgmiss_per_process[pid] += 1
      self.state_dict[key_addr] = [0]*19
      self.state_dict[key_addr][0] = 1 #number of accessess
      self.state_dict[key_addr][17] = -1 #sum value, will be updated after each evaluation!
      self.state_dict[key_addr][18] = cube_k
      self.cube_list[cube_k] -= 1 #keeping track of cube capacity
      if value_addr in self.pg_cube_map:
        cube_v = self.pg_cube_map[value_addr]
      else:
        self.pg_cube_map[value_addr] = cube_v
        self.org_pg_cube_map[value_addr] = cube_v
      self.cube_list[cube_v] -= 1 #keeping track of cube capacity
      self.state_dict[key_addr][cube_v+1] = 1 #for the first time
      

  def trace_buff_dict(self, trace_buffer):
    for k in self.state_dict: 
      cube_assigned = self.state_dict[k][18]
      self.state_dict[k] = [0]*19
      self.state_dict[k][18] = cube_assigned
    for trace in trace_buffer:
      pid = trace[0]
      if(trace[5]!=0):
        self.addr = trace[3:6]
        self.dest_pg = int(self.addr[0])>>11
        self.src1_pg = int(self.addr[1])>>11
        self.src2_pg = int(self.addr[2])>>11
        self.dest_node = trace[6]
        self.src1_node = trace[7]
        self.src2_node = trace[8]
        
        self.update_state_dic_trbuff(pid, self.dest_pg, self.dest_node, self.src1_pg, self.src1_node) #it may need to create a new entry in case of fault 
        self.update_state_dic_trbuff(pid, self.dest_pg, self.dest_node, self.src2_pg, self.src2_node) 
        self.update_state_dic_trbuff(pid, self.src1_pg, self.src1_node, self.dest_pg, self.dest_node)    
        self.update_state_dic_trbuff(pid, self.src2_pg, self.src2_node, self.dest_pg, self.dest_node)  
      else:
        self.addr = trace[3:5]
        self.dest_pg = int(self.addr[0])>>11
        self.src1_pg = int(self.addr[1])>>11
        self.dest_node = trace[6]
        self.src1_node = trace[7]
        self.update_state_dic_trbuff(pid, self.dest_pg, self.dest_node, self.src1_pg, self.src1_node) #it may need to create a new entry in case of fault 
        self.update_state_dic_trbuff(pid, self.src1_pg, self.src1_node, self.dest_pg, self.dest_node)    


  def step(self, action):
      if(len(self.example_state)==0):
        self.obj.run()
        self.sys_op_trace = self.obj.get_system_state()
        assert(len(self.sys_op_trace)!=0)
        self.obj.clear_trace_buffer()
        self.trace_buff_dict(self.sys_op_trace)
        assert(len(list(self.state_dict))>0)
        self.gen_state_from_dict()
        assert(len(self.example_state)!=0)
      self.state, self.reward, new_sum = self.get_state(action)
      self.improvement_param += (1-new_sum)
      self.num_actions_taken += 1
      self.tot_num_actions_taken += 1
      self.episode += 1
      done = False
      if(self.episode%1000 == 0):
        try:
          self.miss_rate = self.page_miss / self.page_access
          temp = []
          for proc in range(self.num_of_processes):
            if(self.pgaccess_per_process[proc]!=0):
              temp.append(float(self.pgmiss_per_process[proc]/self.pgaccess_per_process[proc]))
              self.pgmiss_per_process[proc]=0
              self.pgaccess_per_process[proc]=0
            else:
              temp.append(0)
          self.missrate_monitor.append(temp)

        except: 
          print("number of page miss: ",self.page_miss, " page access:", self.page_access)
          for proc in range(self.num_of_processes):
            if(self.pgaccess_per_process[proc]!=0):
              print(float(self.pgmiss_per_process[proc]/self.pgaccess_per_process[proc]))
        
        self.accuracy = self.improvement_param / self.num_actions_taken
        self.accuracy_monitor.append(self.accuracy)
        
        print("Acc: %.3f" % self.accuracy, "PgMissR: %0.3f" % self.miss_rate, " avgRwd: %0.3f" 
            % (self.total_reward/self.num_actions_taken),
            "ex_st size: %0.3f" % len(self.example_state), " NumReplPen: %0.3f" % self.tot_num_repl_penalty, 
            "rate: %0.3f" % (self.num_repl_penalty/self.num_actions_taken)," tot_miss: ", self.tot_page_miss)
        
        self.repl_avg_epoch += (self.num_repl_penalty/self.num_actions_taken) 
        self.episode_reward += self.total_reward
        self.plot_accuracy()
        self.improvement_param = 0
        self.num_actions_taken = 0
        self.page_miss = 0
        self.page_access = 0
        self.miss_rate = 0
        self.accuracy = 0
        self.total_reward = 0
        self.num_repl_penalty = 0

      if(self.episode%10000==0):
        done = True
        print("Episode average reward: ", self.episode_reward/10000)
        self.reward_monitor.append(self.episode_reward/10000)
        self.repl_monitor.append(self.repl_avg_epoch/10)
        self.repl_avg_epoch = 0
        self.episode_reward = 0
        self.plot_reward()
        self.migration_list.clear()
        self.num_proc = [0]*6
        for vaddr in self.org_pg_cube_map:
          if(self.org_pg_cube_map[vaddr]!=self.pg_cube_map[vaddr]):
            addr = vaddr.split('-')
            self.migration_list.append([int(addr[1]), int(addr[0]), int(self.org_pg_cube_map[vaddr]),int(self.pg_cube_map[vaddr])])
            #print("pid: ", int(addr[1]))
            self.num_proc[int(addr[1])] += 1
        #self.hop_monitor.append(self.obj.get_migration_list(self.migration_list))
        self.hop_monitor = self.obj.get_migration_list(self.migration_list)
        #print(self.hop_monitor)
        self.migration_cnt.append(self.num_proc)
        self.print_stats()
        self.plot_hop_monitor()
        self.plot_migration_cnt()
        self.plot_page_missrate()
      self.total_reward += self.reward/10
      
      return self.state, self.reward/10, done, {}

  
  def reset(self):
    print("xxxxxxxxxxxxxxxxxxxxxxxxxx RESETING ALL THE STATES XXXXXXXXXXXXXXXXXXXXXXXX")
    self.state = np.full(16,0)
    self.state_dict = {}
    self.pg_cube_map = {}
    self.org_pg_cube_map = {}
    self.examples = []
    self.example_state = []
    return self.state
  
  def plot_testing_accuracy(self):
    #print("printing accuracy ...")
    #plt.ion()
    plt.clf()
    plt.plot(self.accuracy_monitor)
    plt.ylabel('accuracy')
    plt.xlabel('epoch')
    #plt.draw()
    #plt.pause(0.00001)
    plt.savefig('graphs/acc_testing_graph.png')

  def plot_training_accuracy(self):
    #print("printing accuracy ...")
    #plt.ion()
    plt.clf()
    plt.plot(self.accuracy_monitor)
    plt.ylabel('accuracy')
    plt.xlabel('epoch')
    #plt.draw()
    #plt.pause(0.00001)
    plt.savefig('graphs/acc_training_graph.png')

  def plot_accuracy(self):
    #print("printing accuracy ...")
    #plt.ion()
    plt.clf()
    plt.plot(self.accuracy_monitor)
    plt.ylabel('accuracy')
    plt.xlabel('epoch')
    #plt.draw()
    #plt.pause(0.00001)
    plt.savefig('graphs/acc_graph.png')

  def plot_reward(self):  
    plt.clf()
    plt.plot(self.reward_monitor)
    plt.ylabel('Average reward per episode')
    plt.xlabel('episode')
    plt.savefig('graphs/reward_graph.png')
  
  def plot_loss(self, loss_monitor):
    plt.clf()
    plt.plot(loss_monitor)
    plt.ylabel('Average loss')
    plt.xlabel('episode')
    plt.savefig('graphs/loss_graph.png')

  def plot_meanq(self, meanq_monitor):
    plt.clf()
    plt.plot(meanq_monitor)
    plt.ylabel('Average meanq')
    plt.xlabel('episode')
    plt.savefig('graphs/meanq_graph.png')

  def plot_repl(self):
    plt.clf()
    plt.plot(self.repl_monitor)
    plt.ylabel('Average replacement')
    plt.xlabel('episode')
    plt.savefig('graphs/repl_graph.png')
  
  def plot_hop_monitor(self):
    plt.clf()
    plt.plot(self.hop_monitor[0])
    plt.ylabel('Average hops')
    plt.xlabel('episode')
    plt.savefig('graphs/avg_hop_graph.png')
  
  def plot_migration_cnt(self):
    plt.clf()
    plt.plot(self.migration_cnt)
    plt.ylabel('migration')
    plt.xlabel('episode')
    plt.savefig('graphs/cnt_migration_graph.png')
  
  def plot_page_missrate(self):
    plt.clf()
    plt.plot(self.missrate_monitor)
    plt.ylabel('page_miss_rate')
    plt.xlabel('episode')
    plt.savefig('graphs/pgmissrate_graph.png')

  def print_stats(self):
    self.obj.print_stats_sim()
