import numpy as np
import matplotlib.pyplot as plt
from sys import exit
with open('output.csv') as f:
    rows = f.read().splitlines()

for i in range(len(rows)):
    rows[i] = [float(x) for x in rows[i].split(',')]
#print rows[0][1]

T  = np.array(rows)
Nx = T.shape[0]
Ny = T.shape[1]
#for t in T[:, 8]:
#    print t
#exit()

fig1 = plt.figure(1, figsize=(6, 4), dpi=100)
ax1 = fig1.add_subplot(1, 1, 1)
x, y = np.mgrid[0:Nx, 0:Ny]
ax1.imshow(T.T, cmap='hot', interpolation='none')
#CS = ax1.contour(y, x, T.T)
#plt.clabel(CS, inline=1, fontsize=10)
filename = 'heatmap_from_cpp.png'
plt.savefig(filename)
#ax1.cla()
plt.show()

