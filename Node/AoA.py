import numpy as np
import matplotlib.pyplot as plt

IQ_samples = [(-104,2),(0,-104),(106,-5),(8,104),(-106,8),(-12,-103),(103,-21),(19,101),(-26,-100),(27,101),(-42,-96),(44,95),(-49,-92),(54,88),(-56,-91),(60,85),(-69,-78),(69,79),(-73,-76),
              (73,78),(-80,-69),(82,63),(-90,-54),(88,56),(-98,-40),(94,46),(-95,-38),(100,24),(-102,-22),(102,27),(-104,-9),(104,14),(-105,-3),(106,-1),(-106,-8),(105,-5),(-103,18),(101,-20),
              (-101,35),(101,-26),(-97,37),(90,-44),(-92,48),(95,-46),(-90,56),(83,-63),(-77,71),(80,-69),(-70,77),(67,-80),(-69,79),(57,-89),(-51,92),(50,-92),(-47,95),(45,-96),(-31,101),
              (30,-103),(-26,102),(29,-102),(-19,103),(6,-104),(0,106),(5,-105),(13,105),(-11,-106),(29,100),(-25,-102),(34,98),(-36,-99),(50,93),(-50,-94),(54,88),(-54,-92),(63,81),(-64,-86),
              (79,71),(-82,-67),(82,64),(-73,-74),(91,53),(-86,-60)]

phases = [np.arctan2(Q, I) for I, Q in IQ_samples]

phase_diffs = np.diff(phases)

##         GHzMHzKHz Hz 
#frequency = 2400000000
#wavelength = 1/frequency
wavelength = 0.125
distance = 0.0625

AoA_radians = -wavelength / (2*np.pi*distance) * phase_diffs
AoA_degrees = np.degrees(AoA_radians)

print(AoA_degrees)

# Create a new figure for the polar plot
plt.figure()

# Create a polar subplot
plt.subplot(111, polar=True)

# Each angle has equal radius in this case (1)
radius = [1]*len(AoA_radians)

# Make the plot
plt.plot(AoA_radians, radius, 'o')

# Show the plot
plt.show()