import numpy as np
import sys
import matplotlib.pyplot as plt
Nx = Ny = 32
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
Teven = np.empty((Nx, Ny))
Todd = np.empty((Nx, Ny))

#ICs:
Todd[:, :] = 0
Teven[:, :] = 0
for i in xrange(Nx):
    for j in xrange(Ny):

        Todd[i, 0] = np.sin(i * dx) ** 2
        Todd[i, -1] = np.cos(i * dx) ** 2
        Teven[i, 0] = np.sin(i * dx) ** 2
        Teven[i, -1] = np.cos(i * dx) ** 2

for k in xrange(Nsteps - 1):
    if k%2:  # if k is odd, this is True.
        T = Todd
        Tnext = Teven
    else:
        T = Teven
        Tnext = Todd
    for i in xrange(0, Nx - 1):
        for j in xrange(1, Ny - 1):
            Tnext[i, 0] = np.sin(i * dx) ** 2  # forcing
            Tnext[i, -1] = np.cos(i * dx) ** 2
            Tnext[i, j] = T[i, j] + dt * kappa / dx / dy * (\
                                    + T[i - 1, j] + T[i + 1, j] \
                                    + T[i, j - 1] + T[i, j + 1] \
                                    - 4 * T[i, j] \
                                )
    for j in xrange(1, Ny - 1):
        Tnext[-1, j] = Tnext[0, j]  # Periodic left/right boundaries
    sys.stdout.write('\r step ' + '%i'%k + ' of ' + '%i'%Nsteps + ' (%.3f%% done)'%(float(k)/Nsteps*100))


fig1 = plt.figure(1, figsize=(6, 4), dpi=100)
ax1 = fig1.add_subplot(1, 1, 1)
x, y = np.mgrid[0:Nx, 0:Ny]
ax1.imshow(T[:, :].T, cmap='hot')
CS = ax1.contour(y, x, T[:, :].T)
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
