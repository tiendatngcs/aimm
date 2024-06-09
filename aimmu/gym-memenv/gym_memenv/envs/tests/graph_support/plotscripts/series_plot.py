from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
import csv
import random
import numpy as np

'''
command line: 
python series_plot.py ../../../stats/spmv_learning hop_timeline.pdf "baseline,aimmu" "hop-average timeline"
"Epoch,hop-average" ../../../stats/spmv_baseline/hop_avg_50-44-12-15-11-2020.csv ../../../stats/spmv_learning/hop_avg_29-17-04-15-11-2020.csv
'''

if __name__=='__main__':
  from sys import argv
  #print(argv)
  target_folder = argv[1]
  plot_file_name = argv[2]
  legends = argv[3]
  title = argv[4]
  labels = argv[5]

  lg = legends.split(',')
  print("legends are: ", lg)
  
  ll = labels.split(',')
  print("labels are: ", ll)

  markers = ['^', 'v', 'o', '+', 'x', 'X', "1", "s", "p"]

  for i, a in enumerate(argv[6:]):
    mypath = a
    '''
    example:
    the series file:  ../../../stats/spmv_baseline/hop_avg_50-44-12-15-11-2020.csv
    the series file:  ../../../stats/spmv_learning/hop_avg_29-17-04-15-11-2020.csv
    '''
    print("the series file: ", mypath)
    x = []
    with open(mypath, 'r') as csvfile:
      plots=csv.reader(csvfile, delimiter=',')
      for row in plots:
        #print(row)
        for e in row:
          if(e != ''):
            x.append(float(e))
    y = list(range(0, len(x)))
    
    sorted_sample = [x[i] for i in sorted(random.sample(range(len(x)), 2500))]
    
    random_number = random.sample(range(0, 2500), 20)
    #equi_sp = np.linspace(0,2500,20,endpoint=False)
    equi_sp = list(np.arange(0,2500,250,dtype=int))
    
    #print(equi_sp)
    #print(random_number)

    print("plotting --> ", lg[i]) 
    plt.plot(sorted_sample, label=lg[i], marker=markers[i], markevery=equi_sp)
    #plt.scatter(y, x, label=lg[i])
    plt.title(title)
    plt.xlabel(ll[0])
    plt.ylabel(ll[1])
    plt.legend(loc='best')
    full_path = target_folder + "/" + plot_file_name 
    plt.savefig(full_path,bbox_inches='tight', pad_inches = .1)
