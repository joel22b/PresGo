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
nodeA_x = 1.88
nodeA_y = 1.14 #2.14
nodeB_x = 0
nodeB_y = 1.86
nodeC_x = 0
nodeC_y = 0
calc_x = 0.5
calc_y = 0.5


rssi_arr = [[],[],[]]
distances = [[],[],[]]
weight_arr = [[],[],[]]
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

def trilateration(x1,y1,r1,x2,y2,r2,x3,y3,r3):
  A = 2*x2 - 2*x1
  B = 2*y2 - 2*y1
  C = r1**2 - r2**2 - x1**2 + x2**2 - y1**2 + y2**2
  D = 2*x3 - 2*x2
  E = 2*y3 - 2*y2
  F = r2**2 - r3**2 - x2**2 + x3**2 - y2**2 + y3**2
  x = (C*E - F*B) / (E*A - B*D)
  y = (C*D - A*F) / (B*D - A*E)
  return x,y

def wcl(weight,x,y):
    xiwi=np.multiply(x,weight)
    yiwi=np.multiply(y,weight)
    xw=np.sum(xiwi)/np.sum(weight)
    yw=np.sum(yiwi)/np.sum(weight)
    return xw,yw


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
                            dist = calc_dist(filtered_rssi, -53.42, 1.6)
                            distances[i].append(dist)
                            weight_arr[i].append(1/dist)

                            
                            #means[i] = np.nanmean(distances[i])
                            means[i] = dist
                            # global calc_x
                            # global calc_y
                            #calc_x,calc_y = trilateration(nodeA_x,nodeA_y,means[0], nodeB_x, nodeB_y, means[1], nodeC_x, nodeC_y, means[2])

                        #serial.read(max(0, size - len(data)))
                        break
                else:
                    time.sleep(1)
    #else:
        #print(ports[i] + ' not open')
    
    ax.clear()
    ax.scatter([nodeA_x], [nodeA_y], color="green")
    ax.scatter([nodeB_x], [nodeB_y], color="blue")
    ax.scatter([nodeC_x], [nodeC_y], color="red")
    #weights = [np.nanmean(weight_arr[0]),np.nanmean(weight_arr[1]),np.nanmean(weight_arr[2])]
    weights = [1/means[0], 1/means[1], 1/means[2]]
    calc_x,calc_y = wcl(weights, [nodeA_x,nodeB_x,nodeC_x],[nodeA_y,nodeB_y,nodeC_y])
    tri_x,tri_y = trilateration(nodeA_x,nodeA_y,means[0], nodeB_x, nodeB_y, means[1], nodeC_x, nodeC_y, means[2])
    ax.scatter([calc_x], [calc_y], color="purple")
    ax.scatter([tri_x], [tri_y], color="orange")
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