#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
def ydot(t, y):
    g = 9.81
    l = 1
    z1 = y[1]
    z2 = -g/l*np.sin(y[0])
    return np.array([z1, z2])

def eulerstep(t, x, h):
    return ([x[j]+h*ydot(t,x)[j] for j in range(len(x))])

def eulersmethod(Theta0, T, n):
    z = Theta0
    h = T/n
    t = [i*h for i in range(n)]
    theta = [[],[]]
    for i in range(n):
        z = eulerstep(t[i], z, h)
        theta[0].append(z[0])
        theta[1].append(z[1])
    return(t, theta[0], theta[1])

def animate_pendulum(x, y, h):
    fig = plt.figure(figsize=(8,8))
    ax = fig.add_subplot(autoscale_on = False, xlim=(-2.2, 2.2), ylim = (-2.2, 2.2))
    ax.grid()
    line, = ax.plot([],[], 'o', c='blue', lw=1)
    time_text = ax.text(0.05, 0.9, '', transform = ax.transAxes)

    def animate(i):
        xline = [0, x[1]]
        yline = [0, y[1]]

        line.set_data(xline, yline)
        time_text.set_text(f"time = {i*h:1f}s")
        return line, time_text
    ani = FuncAnimation(
        fig, animate, len(x), interval = h*1000, blit = True, repeat = False
    )
    plt.show()

def min():
    L=2
    T=20
    n=500
    h=T/n
    y_0 = [np.pi/12, 0]

    t, angle, velocity = eulersmethod(y_0, T, n)
    x, y = L*np.sin(angle[:]), -L*np.cos(angle[:])
    animate_pendulum(x, y, h)

min()