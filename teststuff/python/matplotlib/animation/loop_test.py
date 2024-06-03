#!/usr/bin/env python3

# importing libraries
import numpy as np
import time
import matplotlib.pyplot as plt

# creating initial data values
# of x and y
x = np.linspace(0, 4*np.pi, 180)
y = np.sin(x)
z = np.cos(x)

# to run GUI event loop
plt.ion()

# here we are creating sub plots
figure = plt.figure(figsize=(10, 8))
ax = figure.add_subplot(projection='3d')
line1, = ax.plot(x, y, z)

# setting title
plt.title("Geeks For Geeks", fontsize=20)

# setting x-axis label and y-axis label
plt.xlabel("X-axis")
plt.ylabel("Y-axis")

# Loop
for _ in range(50):
	# creating new Y values
    new_y = np.sin(x-0.5*_)
    new_z = np.cos(x-0.5*_)

	# updating data values
    line1.set_xdata(x)
    line1.set_ydata(new_y)
    line1.set_3d_properties(z) #type: ignore
	# drawing updated values
    figure.canvas.draw()

	# This will run the GUI event
	# loop until all UI events
    # currently waiting have been processed
    figure.canvas.flush_events()

    time.sleep(0.01)
