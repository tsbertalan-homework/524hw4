import numpy as np
Nx = Ny = 16
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
T = np.empty((Nx, Ny, Nsteps))
print T.shape

#ICs:
T[:, :, 0] = 0
for i in xrange(Nx):
    for j in xrange(Ny):
        T[i, 0, 0] = np.sin(i * dx) ** 2
        T[i, -1, 0] = np.cos(i * dx) ** 2

for k in xrange(Nsteps-1):
    for i in xrange(0, Nx - 1):
        for j in xrange(1, Ny - 1):
            T[i, j, k + 1] = T[i, j, k] + dt * kappa / dx / dy * (\
                                    + T[i - 1, j, k] + T[i + 1, j, k] \
                                    + T[i, j - 1, k] + T[i, j + 1, k] \
                                    - 4 * T[i, j, k] \
                                )
    for j in xrange(1, Ny - 1):
        T[-1, j, k + 1] = T[0, j, k + 1]


import matplotlib.pyplot as plt
import sys
fig1 = plt.figure(1, figsize=(3, 2), dpi=100)
ax1 = fig1.add_subplot(1,1,1)
for k in xrange(80-1):
    ax1.imshow(T[:,:,k])
    filename = 'heatmaps/heatmap_t%s.png' % str(k).zfill(4)
    sys.stdout.write('saving: ' + filename + ' of ' + '%i' % Nsteps + '\r')
    sys.stdout.flush()
    plt.savefig(filename)
#plt.show()

#from mayavi.mlab import contour_surf
#x, y = np.mgrid[x0:dx:xmax, y0:dy:ymax]
#s = contour_surf(x, y, T[:, :, -1])
#s.scene.save('test.png')
#
