import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import random

# Create the figure and polar plot
fig = plt.figure()
ax = fig.add_subplot(111, polar=True)
line, = ax.plot([], [], 'o')

def update(frame):
    # Generate a random angle array to simulate new data
    # Replace this with actual logic to get new AoA data
    AoA_degrees = [random.uniform(-180, 180) for _ in range(10)]

    # Clear the plot and draw new plot
    ax.clear()
    ax.plot(np.radians(AoA_degrees), [1]*len(AoA_degrees), 'o')
    return line,

# Create the animation
ani = FuncAnimation(fig, update, frames=range(100))

plt.show()