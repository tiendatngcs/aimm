from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
from collections import defaultdict, OrderedDict
import csv

'''
python consolidate.py ../../../stats/spmv_learning pagewise_migr.pdf "Page-wise Migration"
"Page Access,Number of Pages" ../../../stats/spmv_learning/migr_count_50-50-03-10-01-2021.csv
'''

if __name__=='__main__':
  from sys import argv
  #print(argv)
  target_folder = argv[1]
  plot_file_name = argv[2]
  title = argv[3]
  labels = argv[4]

  #lg = legends.split(',')
  #print("legends are: ", lg)
  
  ll = labels.split(',')
  print("labels are: ", ll)

  for i, a in enumerate(argv[5:]):
    mypath = a
    print("the series file: ", mypath)
    x = defaultdict(list) 
    with open(mypath, 'r') as csvfile:
      plots=csv.reader(csvfile, delimiter=',')
      for row in plots:
        #print(row)
        for e in row:
          if(e != ''):
            key = e.split('|')
            x[key[1]].append(key[0])
            #x.append(float(e))
        #    x.append(e)
    y = list(range(0, len(x)))
    
    for k in sorted(x):
      print("migr: ", k, " num pages: ", len(x[k]))
    #print("plotting --> ", lg[i]) 
    #plt.plot(x, label=lg[i])
    ##plt.scatter(y, x, label=lg[i])
    #plt.title(title)
    #plt.xlabel(ll[0])
    #plt.ylabel(ll[1])
    #plt.legend(loc='best')
    #full_path = target_folder + "/" + plot_file_name 
    #plt.savefig(full_path)
