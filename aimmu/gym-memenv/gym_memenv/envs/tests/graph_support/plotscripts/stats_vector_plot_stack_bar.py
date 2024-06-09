from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
import csv
import numpy as np

'''
(newenv) pritam@pritam:~/aimmu_project/aimmu/gym-memenv/gym_memenv/envs/tests/graph_support/plotscripts$ python
stats_plot.py trial trial.pdf ../../../stats/rbm_baseline/stats.out ../../../stats/rbm_learning/ stats.out
../../../stats/spmv_baseline/stats.out ../../../stats/spmv_learning/stats.out ../../../stats/backprop_baseline/stats.out
../../../stats/backprop_learning/stats.out ../../../stats/pagerank_baseline/s tats.out
../../../stats/pagerank_learrning/stats.out
'''

def bar_plot(ax, data, benchlist, lg, ll, title, colors=None, total_width=0.8, single_width=1, legend=True):
  #if colors is None:
  #  colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
  colors = ['#e0f3db','#93db6c','#49bcc0', '#ff4500','#43a2ca',
            '#e9e5c2','#8eb9a6','#156e58','#ff3cf7','#f4cd64', '#aaaaaa',
            '#4ef692','#2ebaff','#c263f4','#a8ddb5','#ff2e2e']
  # Number of bars per group
  n_bars = len(data[0])

  # The width of a single bar
  bar_width = total_width / (n_bars/(len(lg)/2+1/len(lg)))

  # List containing handles for the drawn bars, used for the legend
  bars = []
  legend_t = lg
  x_ticks = []

  data = np.array(data)
  
  print(data)

  ind = np.arange(n_bars)

  # Iterate over all data
  for i, values in enumerate(data):
    if(i>0):
      bar = ax.bar(ind, values, width=bar_width*(1+1/len(lg)), bottom=np.sum(data[:i], axis = 0), color = colors[i % len(colors)])
      bars.append(bar[0])
    else:
      bar = ax.bar(ind, values, width=bar_width*(1+1/len(lg)))
      bars.append(bar[0])
    
    # Draw legend if we need
  if legend:
    ax.legend(bars, legend_t, bbox_to_anchor=(1.04,1), fancybox=True, shadow=True, ncol=1)

  for i in range(n_bars):
    x_offset = bar_width *2 * i + bar_width 
    x_ticks.append(x_offset+bar_width/2)  
  
  ax.xlabel(ll[0])
  ax.ylabel(ll[1])
  ax.title(title)
  ax.xticks(ind, benchlist)
  ax.xticks(rotation=90)
  ax.tight_layout()



if __name__=='__main__':
  from sys import argv
  #print(argv)
  target_folder = argv[1]
  plot_file_name = argv[2]
  title = argv[3]
  labels = argv[4]

  #lg = legends.split(',')
  #print("legends are: ", lg)
  #
  ll = labels.split(',')
  print("labels are: ", ll)

  x = []
  key = {}
  col_title_list = []
  plot = []
  

  for i, a in enumerate(argv[5:]):
    mypath = a
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
  
  stacked_keys = input("\n Enter the stacked key,number_of_occ: ")
  stacked_keys_and_occ = stacked_keys.split(',')
  print("plotting for: ", stacked_keys_and_occ[0], "for ",stacked_keys_and_occ[1], " size histogram")
  stacked_key_list = []

  for i in range(int(stacked_keys_and_occ[1])):
    l_key = stacked_keys_and_occ[0] + "[" + str(i) + "]" 
    stacked_key_list.append(l_key)

  lg = stacked_key_list
  print("legends are: ", lg)

  data = []
  for input_key in stacked_key_list:
    plot = key[input_key]
    print("plot before: ", plot)
    res = [float(ele) for ele in plot] 
    print("plot after: ", res)
    data.append(res)
  print(data)
  
  benchlist = []
  for n, bench in enumerate(col_title_list):
    b = bench.split('_')
    benchlist.append(bench)
  
  print(benchlist)
  n_bars = len(list(data)[0])

  bar_plot(plt, data, benchlist, lg, ll, title, total_width=1 + 2/(n_bars*10), single_width=.9)
  full_path = target_folder + "/" + plot_file_name 
  plt.savefig(full_path)
