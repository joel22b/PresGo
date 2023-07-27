from matplotlib import pyplot as plot
from matplotlib import animation
import itertools
import random
import serial
import time
import math
import threading
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

# All units in meters
nodeA_x = 0
nodeA_y = 0
nodeB_x = 0.63
nodeB_y = 0.85
nodeC_x = 0
nodeC_y = 2.27
target_x = 0.5
target_y = 0.5

means = [0.5, 0.5, 0.5]

rssi_1_meter = -50
path_loss_exp = 1.6

fig2, ax = plot.subplots()
circles = [plot.Circle((0,0), 0, fill=False) for _ in range(3)]

def thread_serial(index):
    # Setup serial port connection
    port = ports[index]
    ser = serial.Serial(port=port, baudrate=baudrate)
    ser.timeout = 10  # set read timeout
    print(port + " is open: " + str(ser.is_open))  # True for opened

    # Setup Kalman filter
    filt = KalmanFilter(1,1)
    rssi_arr = []
    #dist_arr = []

    if ser.is_open:
        while not kill_sig:
            size = ser.inWaiting()
            if size:
                data = ser.readline()
                try:
                    data_str = data.decode()
                except:
                    print("Couldn't decode bytes to str")
                #print(data)
                if data_str.startswith("-"):
                    rssi = int(data_str)
                    rssi_arr.append(rssi) 
                    if(len(rssi_arr)>=2):
                        var = np.nanvar(rssi_arr)

                        if len(rssi_arr) > 20:
                            rssi_arr.pop(0)

                        filt.set_measurement_noise(var)
                        filtered_rssi = filt.filter(rssi)

                        #dist_arr.append(calc_dist(filtered_rssi, -53.42, 1.6))
                        means[index] = calc_dist(filtered_rssi, rssi_1_meter, path_loss_exp)#np.nanmean(dist_arr)
            else:
                time.sleep(1)
    else:
        print(port + ' not open')

# Setup serial port
baudrate = 115200
ports = ['COM5', 'COM6', 'COM8']#, 'COM7', 'COM8']  # set the correct port before run it

# Start serial threads
threads = []
kill_sig = False
for i in range(len(ports)):
    t = threading.Thread(target=thread_serial, args=(i,))
    threads.append(t)

for t in threads:
    t.start()

def init():  # Initialization function
    for circle in circles:
        circle.center = (0, 0)
        circle.radius = 0
        ax.add_patch(circle)
    return circles

def update(frame):  # Update function
    target_x,target_y = trilateration(nodeA_x,nodeA_y,means[0], nodeB_x, nodeB_y, means[1], nodeC_x, nodeC_y, means[2])

    ax.clear()
    ax.scatter([nodeA_x], [nodeA_y], color="green")
    ax.scatter([nodeB_x], [nodeB_y], color="blue")
    ax.scatter([nodeC_x], [nodeC_y], color="red")
    ax.scatter([target_x], [target_y], color="purple")

    #global target_x
    #global target_y
    
    
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

kill_sig = True
for t in threads:
    t.join()