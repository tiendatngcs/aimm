from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
import csv

'''
(newenv) pritam@pritam:~/aimmu_project/aimmu/gym-memenv/gym_memenv/envs/tests/graph_support/plotscripts$ python
stats_plot.py trial trial.pdf "baseline,aimmu" "Percent of Migration" "Benchmarks,% migration" False 
../../../stats/rbm_learning/ stats.out ../../../stats/spmv_learning/stats.out 
../../../stats/backprop_learning/stats.out ../../../stats/pagerank_learrning/stats.out
'''

def bar_plot(ax, data, lg, ll, title, colors=None, total_width=0.8, single_width=1, legend=True):
  #if colors is None:
  #  colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
  #colors = ['#e0f3db','#a8ddb5','#43a2ca']
  colors = ['#93db6c','#49bcc0','#43a2ca']
  # Number of bars per group
  n_bars = len(list(data)[0])

  # The width of a single bar
  bar_width = total_width / (len(data)*n_bars)

  # List containing handles for the drawn bars, used for the legend
  bars = []
  legend_t = [lg[0] + "/" + lg[1]]
  print("legend ----->", legend_t)
  x_ticks = []

  # Iterate over all data
  for i, (name, values) in enumerate(data.items()):
    # The offset in x direction of that bar
    #x_offset = (i - n_bars / 2) * bar_width + bar_width / 2
    x_offset = bar_width *2 * i + bar_width 
    x_ticks.append(x_offset+bar_width/10)  
    # Draw a bar for every value of that type
    for x, y in enumerate(values):
      bar = ax.bar(x*(0.05) + x_offset, y, width=bar_width * single_width, color=colors[x])

      # Add a handle to the last drawn bar, which we'll need for the legend
      if(i==0):
        bars.append(bar[0])
    
    # Draw legend if we need
  ax.legend(bars, legend_t)
  ax.xlabel(ll[0])
  ax.ylabel(ll[1])
  ax.title(title)
  ax.xticks(x_ticks, data.keys())

if __name__=='__main__':
  from sys import argv
  #print(argv)
  target_folder = argv[1]
  plot_file_name = argv[2]
  title = argv[3]
  labels = argv[4]
  normalize = argv[5]

  if(normalize!="True"):
    print("\n---- No Normalization -----\n")
  
  ll = labels.split(',')
  print("labels are: ", ll)

  x = []
  key = {}
  col_title_list = []
  plot = []

  for i, a in enumerate(argv[6:]):
    mypath = a
    '''
    example:
    the series file:  ../../../stats/spmv_baseline/stats.out
    the series file:  ../../../stats/spmv_learning/stats.out
    '''
    print("the series file: ", mypath)
    col_title = mypath.split('/')[-2]
    print("column title: ", col_title)
    col_title_list.append(col_title)

    lines = open(mypath, 'r')
    for n, line in enumerate(lines):
      if(n>3):
        k_and_v = line.split('|')[0]
        kv = k_and_v.split(':' and '\t')[0]
        kvsp = kv.split(' ')  
        #print(n, kvsp[0])
        k = kvsp[0]
        v = kvsp[2]
        if(k not in key):
          key[k] = []
        key[k].append(v)
  k = key.keys()
  print(k)
  input_key = input("\n\n Which key you want to plot: ")
  #for k, v in key.items():
  #  print(k)
  print("plotting for: ", input_key)
 
  if(normalize=="True"):
    norm_key = input("\n\n Which key you want to normalize with: ")
    print("normalized by key: ", norm_key)
    

  data = {}
  plot = key[input_key]
  
  if(normalize=="True"):
    norm = key[norm_key]
    lg = [input_key, norm_key]
    print("norm: ", norm)
  else:
    lg = [input_key, " "]

  print("plot: ", plot)

  for n, bench in enumerate(col_title_list):
    #need to take it from argument to make it more flexible
    b = bench.split('_')[0]
    print("n: ", n, "b: ",b)
    data[b] = []
    if(normalize=="True"):
      pl_e = float(plot[n])/float(norm[n]) #normalizing factor
    else:
      pl_e = float(plot[n]) #no normalization 
    data[b].append(pl_e)
  
  print(data)

  #fig, ax = plt.subplots()
  bar_plot(plt, data, lg, ll, title, total_width=.8, single_width=.9)
  full_path = target_folder + "/" + plot_file_name 
  plt.savefig(full_path)
