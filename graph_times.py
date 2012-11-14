import numpy as np
import sys
import matplotlib.pyplot as plt
with open('time_results.csv') as f:
    rows = f.read().splitlines()

for i in range(len(rows)):
    rows[i] = [float(x) for x in rows[i].split(' ')]

T  = np.array(rows)
sizecol=0
nproccol=1
timecol=2
sizes = list(set(list(T[:,sizecol])))
nprocs = list(set(list(T[:,nproccol])))
print T
print sizes
print nprocs
first_index_per_size = {}
for size in sizes:
    first_index_per_size[size] = list(T[:, sizecol]).index(size)
print first_index_per_size
sys.exit()
fig1 = plt.figure(1, figsize=(6, 4), dpi=100)
ax1 = fig1.add_subplot(1, 1, 1)
ax1.imshow(T, cmap='hot', interpolation='nearest')
CS = ax1.contour(y, x, T.T)
plt.clabel(CS, inline=1, fontsize=10)
filename = 'heatmap_from_cpp.png'
plt.savefig(filename)
plt.show()


