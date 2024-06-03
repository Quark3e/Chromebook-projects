#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

def f(t):
    return np.cos(2*np.pi*t) * np.exp(-t)


# Set up a figure twice as tall as it is wide
fig = plt.figure(figsize=plt.figaspect(0.5))
fig.suptitle('A tale of 2 subplots')
# First subplot
ax=[0,0]
ax[0] = fig.add_subplot(1, 2, 1)

t1 = np.arange(0.0, 5.0, 0.1)
t2 = np.arange(0.0, 5.0, 0.02)
t3 = np.arange(0.0, 2.0, 0.01)

ax[0].plot(t1, f(t1), 'bo',
        t2, f(t2), 'k--', markerfacecolor='green')
ax[0].grid(True)
ax[0].set_ylabel('Damped oscillation')

# Second subplot
ax[1] = fig.add_subplot(1, 2, 2, projection='3d')

X = np.arange(-5, 5, 0.25)
Y = np.arange(-5, 5, 0.25)
X, Y = np.meshgrid(X, Y)
R = np.sqrt(X**2 + Y**2)
Z = np.sin(R)

surf = ax[1].plot_surface(X, Y, Z, rstride=1, cstride=1,
                       linewidth=0, antialiased=False)
ax[1].set_zlim(-1, 1)
plt.legend(ax,["plot 1","plot 2"])
plt.show()