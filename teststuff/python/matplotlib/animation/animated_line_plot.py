#!/usr/bin/env python3

from random import randint

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# create empty lists for the x and y data
x = [10, 0]
y = [0, 5]

# create the figure and axes objects
fig, ax = plt.subplots()

# function that draws each frame of the animation
def animate(i):
    global x, y
    ax.clear()
    pt = randint(1,9) # grab a random integer to be the next y-value in the animation
    #x.append(i)
    #y.append(pt)
    y[0] = pt
    print(x,y)
    ax.plot(x, y, linestyle='solid')
    ax.set_xlim([0,20])
    ax.set_ylim([0,10])

# run the animation
ani = FuncAnimation(fig, animate, frames=20, interval=50, repeat=True)

plt.show()
