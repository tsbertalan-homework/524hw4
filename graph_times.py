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
#print T

size_indices = []
for size in sizes:
    size_indices.append(list(T[:, sizecol]).index(size))
fig1 = plt.figure(1, figsize=(6, 4), dpi=100)
ax1 = fig1.add_subplot(1, 1, 1)
colors = ['red', 'blue', 'green', 'yellow', 'orange', 'cyan', 'magenta', 'black']
for i in range(len(size_indices)):
    if i == len(size_indices)-1:
        final = T.shape[0]
    else:
        final = size_indices[i+1]
    procs =  T[size_indices[i]:final,nproccol]
    times =  T[size_indices[i]:final,timecol]
    try:
        procs, times = zip(*sorted(zip(procs, times)))
    except:
        pass
    ax1.plot(procs, times, color=colors[i])
filename = 'times.png'
plt.legend(["NX = %i" % x for x in sizes])
ax1.set_xlabel('nprocs')
ax1.set_ylabel('times [s]')

#locs, labels = plt.xticks()
#print locs
#print list(labels)
#labels = ['{0} {0}','{0} {0}', '{0} {0}']
#new_labels = [x.format(locs[i]) for i,x  in enumerate(labels)]
ax1.set_xlim([0,20])
plt.xticks([1, 2, 4, 8], ['1', '2', '4', '8'])

#plt.savefig(filename)
plt.show()


