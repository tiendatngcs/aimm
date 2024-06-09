
from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
import csv
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
  avg_comp_dist = []

  for i, a in enumerate(argv[6:]):
    mypath = a
    comp_dest = 0
    count = 0
    print("the series file: ", mypath)
    x = []
    with open(mypath, 'r') as csvfile:
      plots=csv.reader(csvfile, delimiter=',')
      count = 0
      for row in plots:
        #print(row)
        for e in row:
          if(e != ''):
            x.append(float(e))
            count += 1
            comp_dest += float(e)
    #y = list(range(0, len(x)))
    avg_comp_dist.append(comp_dest/count)
    violin.append(x)

#print("plotting --> ", lg[i]) 
#plt.plot(x, label=lg[i])
##plt.scatter(y, x, label=lg[i])
#plt.title(title)
#plt.xlabel(ll[0])
#plt.ylabel(ll[1])
#plt.legend(loc='best')
#full_path = target_folder + "/" + plot_file_name 
#plt.savefig(full_path)\

print("average comp dist: ", avg_comp_dist)

# Create a figure instance
fig = plt.figure()
sns.set(font_scale=1.3)
# Create an axes instance
#ax = fig.gca()
# Create the violinplot
#violinplot = ax.violinplot(violin)
ax = sns.violinplot(data=violin, widths=0.8)
ax.set_xlabel("Techniques", fontsize=16)
ax.set_ylabel("Utilization", fontsize=16)
#ax.set_xticks([1,3,5])
ax.set_xticklabels(lg)

full_path = target_folder + "/" + plot_file_name 
plt.savefig(full_path, bbox_inches='tight', pad_inches = .1)
