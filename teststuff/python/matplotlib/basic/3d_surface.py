#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

from matplotlib import cm
from matplotlib.ticker import LinearLocator

fig, ax = plt.subplots(subplot_kw={"projection": "3d"})

# Make data.
X = np.arange(-5, 5, 0.25)
Y = np.arange(-5, 5, 0.25)
X, Y = np.meshgrid(X, Y)
R = np.sqrt(X**2 + Y**2)
#print(X)
Z = np.sin(R)

#for i in Z: print(i)

#x2 = np.arange(-5, 5, 0.25)
#y2 = np.arange(-5, 5, 0.25)
#x2, y2 = np.meshgrid(x2, y2)
#r2 = np.sqrt(x2**2+y2**2)
#z2 = np.array([0]*len(x2*y2))


x2,y2,z2 = [],[],[]
for x in range(-500, 525, 25):
    for y in range(-500, 525, 25):
        x2.append(x/100)
        y2.append(y/100)
#        z2.append(0)

z2 = [0]*len(x2)

# Plot the surface.
surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)


surf2 = ax.scatter(x2, y2, z2)

## Customize the z axis.
#ax.set_zlim(-1.01, 1.01)
#ax.zaxis.set_major_locator(LinearLocator(10))
## A StrMethodFormatter is used automatically
#ax.zaxis.set_major_formatter('{x:.02f}')

## Add a color bar which maps values to colors.
#fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
