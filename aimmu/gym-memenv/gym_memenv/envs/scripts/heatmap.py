#!/usr/bin/env python
from matplotlib import colors
import matplotlib.pyplot as plt
import numpy as np
import random

arr = []
for i in range(0, 512):
    a = []
    for j in range(0, 512):
        a.append(random.randrange(0,3))
    arr.append(np.array(a))

print 'done'
cmap = colors.ListedColormap(['green','red','blue'])
print 'done1'
bounds=[0,1,2,3]
norm = colors.BoundaryNorm(bounds, cmap.N)
print 'done2'
heatmap = plt.pcolor(np.array(arr), cmap=cmap, norm=norm)
print 'done3'
plt.colorbar(heatmap, ticks=[0,1,2,3])
print 'done4'

#plt.imshow(np.array(a), cmap='hot', interpolation='nearest')
plt.show()
