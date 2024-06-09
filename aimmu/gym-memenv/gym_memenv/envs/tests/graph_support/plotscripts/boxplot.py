
from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
import csv
from numpy import *
import seaborn as sns

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

  violin = []
  
  for i, a in enumerate(argv[6:]):
    mypath = a
    
    print("the series file: ", mypath)
    x = []
    with open(mypath, 'r') as csvfile:
      plots=csv.reader(csvfile, delimiter=',')
      for row in plots:
        #print(row)
        for e in row:
          if(e != ''):
            x.append(float(e))
    #y = list(range(0, len(x)))
    violin.append(ma.log10(x))

#print("plotting --> ", lg[i]) 
#plt.plot(x, label=lg[i])
##plt.scatter(y, x, label=lg[i])
#sns.set(font_scale=1.3)
plt.title(title)
#plt.xlabel(ll[0])
#plt.ylabel(ll[1])
#plt.legend(loc='best')
#full_path = target_folder + "/" + plot_file_name 
#plt.savefig(full_path)\

# Create a figure instance
#fig = plt.figure()
# Create an axes instance
#ax = fig.gca()

#plt.xticks(range(1, len(lg)+1),lg)
# Create the violinplot
plt.figure(figsize=(6, 3))
boxplot = plt.boxplot(violin,  labels=lg)
plt.xlabel("Benchmarks", fontsize=12)
plt.ylabel("Number of Active Pages (log10)", fontsize=12)
full_path = target_folder + "/" + plot_file_name 
plt.savefig(full_path,bbox_inches='tight', pad_inches = .1)
