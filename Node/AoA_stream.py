import serial
import time
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import random
import itertools
import ast

# Create the figure and polar plot
fig = plt.figure()
ax = fig.add_subplot(111, polar=True)
line, = ax.plot([], [], 'o')

# Setup serial port
z1baudrate = 115200
z1port = 'COM6'  # set the correct port before run it

z1serial = serial.Serial(port=z1port, baudrate=z1baudrate)
z1serial.timeout = 2  # set read timeout
# print z1serial  # debug serial.
print(z1serial.is_open)  # True for opened
#if z1serial.is_open:
#    while True:
#        size = z1serial.inWaiting()
#        if size:
#            data = z1serial.readline()
#            print(data)
#        else:
#            time.sleep(1)
#else:
#    print('z1serial not open')

def update(frame):
    # Generate a random angle array to simulate new data
    # Replace this with actual logic to get new AoA data
    #AoA_degrees = [random.uniform(-180, 180) for _ in range(10)]

    # Read in data from serial
    IQ_samples = []
    if z1serial.is_open:
        while True:
            size = z1serial.inWaiting()
            if size:
                data = z1serial.readline()
                try:
                    data_str = data.decode()
                except:
                    print("Couldn't decode bytes to str")
                #print(data)
                if data_str.startswith("IQ samples (AoA): "):
                    samples = data_str[18:]
                    #print(samples)
                    IQ_samples = ast.literal_eval(samples)
                    break
            else:
                time.sleep(1)
    else:
        print('z1serial not open')
    
    phases = [np.arctan2(Q, I) for I, Q in IQ_samples]

    phase_diffs = np.diff(phases)

    wavelength = 0.125
    distance = 0.0625

    AoA_radians = -wavelength / (2*np.pi*distance) * phase_diffs

    # Clear the plot and draw new plot
    ax.clear()
    ax.plot(AoA_radians, [1]*len(AoA_radians), 'o')
    return line,

# Create the animation
ani = FuncAnimation(fig, update, frames=itertools.count())

plt.show()
