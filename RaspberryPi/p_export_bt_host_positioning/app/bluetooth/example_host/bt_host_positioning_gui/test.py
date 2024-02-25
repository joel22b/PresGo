#!/usr/bin/env python3
from uuid import UUID
import time
import protocol_serial as ps

def door_announcement(inDoorway: bool):
  print("Announcement inDoorway: " + str(inDoorway))

def init_announcement(flags: int):
  ps.init_printout(flags)

def get_connect():
  print("huh")

def get_fare_id(uuid: UUID):
  end = time.time()
  print(end - start)
  print("Fare ID: " + str(uuid))
  #time.sleep(10)
  #ptSerial.send_request_fare("0C4314F4627F", get_fare_id)

global ptSerial
ptSerial = ps.ProtocolSerial("COM9", door_announcement, init_announcement)

time.sleep(2)
print("Connect")
ptSerial.send_request_connect("0C4314F4627F", get_connect)

time.sleep(10)
print("Fare")
start = time.time()
ptSerial.send_request_fare("0C4314F4627F", get_fare_id)
#time.sleep(1)
#ptSerial.send_request_fare("0C4314F4627E", get_fare_id)

while True:
  pass
