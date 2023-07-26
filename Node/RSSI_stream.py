from matplotlib import pyplot as plot
from matplotlib import animation
import itertools
import random
import serial
import time
import math
import numpy as np

class KalmanFilter:

    cov = float('nan')
    x = float('nan')

    def __init__(self, R, Q):
        """
        Constructor
        :param R: Process Noise
        :param Q: Measurement Noise
        """
        self.A = 1
        self.B = 0
        self.C = 1

        self.R = R
        self.Q = Q

    def filter(self, measurement):
        """
        Filters a measurement
        :param measurement: The measurement value to be filtered
        :return: The filtered value
        """
        u = 0
        if math.isnan(self.x):
            self.x = (1 / self.C) * measurement
            self.cov = (1 / self.C) * self.Q * (1 / self.C)
        else:
            predX = (self.A * self.x) + (self.B * u)
            predCov = ((self.A * self.cov) * self.A) + self.R

            # Kalman Gain
            K = predCov * self.C * (1 / ((self.C * predCov * self.C) + self.Q));

            # Correction
            self.x = predX + K * (measurement - (self.C * predX));
            self.cov = predCov - (K * self.C * predCov);

        return self.x

    def last_measurement(self):
        """
        Returns the last measurement fed into the filter
        :return: The last measurement fed into the filter
        """
        return self.x

    def set_measurement_noise(self, noise):
        """
        Sets measurement noise
        :param noise: The new measurement noise
        """
        self.Q = noise

    def set_process_noise(self, noise):
        """
        Sets process noise
        :param noise: The new process noise
        """
        self.R = noise

def calc_dist(rss,a,n):
    cal_d= pow(10,((a-rss)/(10*n)))
    return cal_d



# All units in meters
nodeA_x = 0
nodeA_y = 0
nodeB_x = 1
nodeB_y = 0
nodeC_x = 1
nodeC_y = 1
x = 0.5
y = 0.5

rssi_arr = [[],[],[]]
distances = [[],[],[]]
filters = [KalmanFilter(1,1), KalmanFilter(1,1), KalmanFilter(1,1)]
means = [0.5, 0.5, 0.5]

fig2, ax = plot.subplots()
circles = [plot.Circle((0,0), 0, fill=False) for _ in range(3)]

# Setup serial port
baudrate = 115200
ports = ['COM4', 'COM5', 'COM6']#, 'COM7', 'COM8']  # set the correct port before run it

serials = []
for p in ports:
    ser = serial.Serial(port=p, baudrate=baudrate)
    ser.timeout = 2  # set read timeout
    print(p + " is open: " + str(ser.is_open))  # True for opened
    serials.append(ser)

def init():  # Initialization function
    for circle in circles:
        circle.center = (0, 0)
        circle.radius = 0
        ax.add_patch(circle)
    return circles

def update(frame):  # Update function
    for i, serial in enumerate(serials):
        if serial.is_open:
            while True:
                size = serial.inWaiting()
                if size:
                    data = serial.readline()
                    try:
                        data_str = data.decode()
                    except:
                        print("Couldn't decode bytes to str")
                    #print(data)
                    if data_str.startswith("-"):
                        rssi = int(data_str)
                        rssi_arr[i].append(rssi) 
                        if(len(rssi_arr[i])>=2):
                            var = np.nanvar(rssi_arr[i])
                            filters[i].set_measurement_noise(var)
                            filtered_rssi = filters[i].filter(rssi)

                            distances[i].append(calc_dist(filtered_rssi, -53.42, 1.6))
                            means[i] = np.nanmean(distances[i])


                        
                        
                        

                        break
                else:
                    time.sleep(1)
    #else:
        #print(ports[i] + ' not open')

    ax.clear()
    ax.scatter([nodeA_x], [nodeA_y], color="green")
    ax.scatter([nodeB_x], [nodeB_y], color="blue")
    ax.scatter([nodeC_x], [nodeC_y], color="red")
    ax.scatter([x], [y], color="yellow")
    
    circles[0].center = (nodeA_x, nodeA_y)
    circles[0].radius = means[0]
    circles[1].center = (nodeB_x, nodeB_y)
    circles[1].radius = means[1]
    circles[2].center = (nodeC_x, nodeC_y)
    circles[2].radius = means[2]
    
    for circle in circles:
        ax.add_patch(circle)
    
    return circles

ani = animation.FuncAnimation(  # Create animation object
    fig2, update, init_func=init, frames=itertools.count(), blit=False
)

plot.show()