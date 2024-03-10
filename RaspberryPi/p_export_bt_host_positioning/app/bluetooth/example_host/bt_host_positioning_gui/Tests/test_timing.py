#!/usr/bin/env python3
from uuid import UUID
import time
import protocol_serial as ps

file_name = "timing_data.csv"

timings = []
startTimings = []
uuid_correct = UUID("AA2222222222222222222222222222FF")
#startTime = 0

def door_announcement(inDoorway: bool):
  print("Announcement inDoorway: " + str(inDoorway))

def init_announcement(flags: int):
  #ps.init_printout(flags)
  print("Sending")
  ptSerial.send_request_connect("0C4314F4627F", get_connect)
  time.sleep(4)
  #global startTime
  #startTime = time.time()
  startTimings.insert(len(timings), time.time())
  ptSerial.send_request_fare("0C4314F4627F", get_fare_id)

def get_connect():
  print("huh")

def get_fare_id(uuid: UUID):
  end = time.time()
  if uuid_correct != uuid:
    print("Failed: " + str(len(timings)))
    print("\tFare ID: " + str(uuid))
    return
  print("Done: " + str(len(timings)))
  time_elasped = end-startTimings[len(timings)]
  timings.append(time_elasped)
  with open(file_name, "a") as myfile:
    myfile.write(str(time_elasped) + "\n")
  #print("Fare ID: " + str(uuid))

global ptSerial
ptSerial = ps.ProtocolSerial("COM9", door_announcement, init_announcement)

while len(timings) != 100:
  time.sleep(1)

print("Start Timings:")
print(startTimings)
print("\n\n")
print("Timings:")
print(timings)
print("\n\n")
sum = 0
smallest = 999999
largest = 0
for i in timings:
    sum += i
    if i < smallest:
        smallest = i
    if i > largest:
       largest = i
print("Average:  " + str(sum/len(timings)))
print("Smallest: " + str(smallest))
print("Largest:  " + str(largest))
