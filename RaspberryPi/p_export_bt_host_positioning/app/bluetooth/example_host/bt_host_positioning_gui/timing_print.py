#!/usr/bin/env python3

file_name = "timing_data.csv"

file = open(file_name)

sum = 0
count = 0
median = 0
smallest = 999999
largest = 0
timings = []
for line in file:
    count += 1
    time = float(line)
    timings.append(time)
    sum += time
    if time < smallest:
        smallest = time
    if time > largest:
       largest = time

timings.sort()
if count % 2 == 0:
    index = int(count/2)
    median = (timings[index]+timings[index-1])/2
else:
    index = int((count-1)/2)
    median = timings[index]

print("Total number of entries: " + str(count))
print("Average:  " + str(sum/count))
print("Median:   " + str(median))
print("Smallest: " + str(smallest))
print("Largest:  " + str(largest))