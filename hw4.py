'''This is a prototype implementation.'''
import numpy as np
import sys
import matplotlib.pyplot as plt
Nx = Ny = 8
pi = np.pi
t0 = 0
kappa = 0.4
tmax = 0.5 * pi ** 2 / kappa
x0 = y0 = 0
xmax = ymax = pi
dx = (xmax - x0) / Nx
print 'dx is', dx
dy = (ymax - y0) / Ny
dt = dx * dy / 4 / kappa
print 'dt is', dt
print 'tmax is', tmax
Nsteps = int(np.ceil((tmax - t0) / dt))
print 'Nsteps is', Nsteps
T = np.empty((Nx, Ny, 2))

#ICs:
T[:, :, :] = 0
for i in xrange(Nx):
    for j in xrange(Ny):

        T[i,  0, :] = np.sin(i * dx) ** 2
        T[i, -1, :] = np.cos(i * dx) ** 2

for k in xrange(Nsteps - 1):
    next_k = k%2
    this_k = (k+1)%2
    for i in xrange(0, Nx - 1):
        for j in xrange(1, Ny - 1):
            T[i, 0, next_k] = np.sin(i * dx) ** 2  # forcing
            T[i, -1, next_k] = np.cos(i * dx) ** 2
            T[i, j, next_k] = T[i, j, this_k] + dt * kappa / dx / dy * (\
                                    + T[i - 1, j, this_k] + T[i + 1, j, this_k]\
                                    + T[i, j - 1, this_k] + T[i, j + 1, this_k]\
                                    - 4 * T[i, j, this_k]\
                                )
    for j in xrange(1, Ny - 1):
        T[-1, j, next_k] = T[0, j, next_k]  # Periodic left/right boundaries
#    sys.stdout.write('\r step ' + '%i'%k + ' of ' + '%i'%Nsteps + ' (%.3f%% done)'%(float(k)/Nsteps*100))


fig1 = plt.figure(1, figsize=(6, 4), dpi=100)
ax1 = fig1.add_subplot(1, 1, 1)
x, y = np.mgrid[0:Nx, 0:Ny]
ax1.imshow(T[:, :, 1].T, cmap='hot')
CS = ax1.contour(y, x, T[:, :, 1].T)
plt.clabel(CS, inline=1, fontsize=10)
filename = 'heatmaps/heatmap_t%s.png' % str(k).zfill(4)
plt.savefig(filename)
#ax1.cla()
plt.show()

#from mayavi.mlab import contour_surf
#x, y = np.mgrid[x0:dx:xmax, y0:dy:ymax]
#s = contour_surf(x, y, T[:, :, -1])
#s.scene.save('test.png')
#
