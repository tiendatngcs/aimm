import numpy as np

path = "/home/grads/t/tiendat.ng.cs/github_repos/aimm/aimmu/gym-memenv/gym_memenv/envs/tests/pg_reuse_pred/input/"

benchfiles = [
    'backprop.pg',
    'lud.pg',
    'kmeans.pg',
    'naive_art_mac.pg',
    'pagerank.pg',
    'rbm.pg',
    'naive_art_reduce.pg',
    'streamcluster.pg',
    'sgemm.pg',
    'spmv.pg']

# path = "/home/grads/t/tiendat.ng.cs/github_repos/aimm/aimmu/gym-memenv/gym_memenv/envs/stats/pagerank_baseline/"
# benchfiles = ["page_access_count_59-51-15-12-07-2024.pg"]


category = {}

for file_name in benchfiles:
  file_path = path + file_name
  f = open(file_path, 'r')
  print("Working on file: ", f)
  for line in f:
    l = line.split(',')
    if file_name not in category:
      category[file_name] = np.zeros(5,int) #assigning an empty list
    
    if int(l[1]) < 10:
      category[file_name][0] += 1
    elif int(l[1]) < 100:
      category[file_name][1] += 1
    elif int(l[1]) < 1000:
      category[file_name][2] += 1
    elif int(l[1]) < 10000:
      category[file_name][3] += 1
    else:
      category[file_name][4] += 1

print(category)

