from matplotlib import pyplot as plot
from matplotlib import animation
import itertools
import random

# All units in meters
nodeA_x = 0
nodeA_y = 0
nodeB_x = 1
nodeB_y = 0
nodeC_x = 1
nodeC_y = 1
x = 0.5
y = 0.5

mean_1 = random.uniform(0, 1)
mean_2 = random.uniform(0, 1)
mean_3 = random.uniform(0, 1)

fig2, ax = plot.subplots()
circles = [plot.Circle((0,0), 0, fill=False) for _ in range(3)]

def init():  # Initialization function
    for circle in circles:
        circle.center = (0, 0)
        circle.radius = 0
        ax.add_patch(circle)
    return circles

def update(i):  # Update function
    mean_1 = random.uniform(0, 1)
    mean_2 = random.uniform(0, 1)
    mean_3 = random.uniform(0, 1)

    ax.clear()
    ax.scatter([nodeA_x], [nodeA_y], color="green")
    ax.scatter([nodeB_x], [nodeB_y], color="blue")
    ax.scatter([nodeC_x], [nodeC_y], color="red")
    ax.scatter([x], [y], color="yellow")
    
    circles[0].center = (nodeA_x, nodeA_y)
    circles[0].radius = mean_1
    circles[1].center = (nodeB_x, nodeB_y)
    circles[1].radius = mean_2
    circles[2].center = (nodeC_x, nodeC_y)
    circles[2].radius = mean_3
    
    for circle in circles:
        ax.add_patch(circle)
    
    return circles

ani = animation.FuncAnimation(  # Create animation object
    fig2, update, init_func=init, frames=100, interval=200, blit=False
)

plot.show()