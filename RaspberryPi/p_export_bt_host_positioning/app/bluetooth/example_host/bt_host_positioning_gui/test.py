#!/usr/bin/env python3
from uuid import UUID
import time
import protocol_serial as ps

def door_announcement(inDoorway: bool):
  print("Announcement inDoorway: " + str(inDoorway))

def get_fare_id(uuid: UUID):
  print("Fare ID: " + str(uuid))
  #time.sleep(10)
  #ptSerial.send_request_fare("0C4314F4627F", get_fare_id)

global ptSerial
ptSerial = ps.ProtocolSerial(door_announcement)

time.sleep(2)
print("Sending")
ptSerial.send_request_fare("0C4314F4627F", get_fare_id)
#time.sleep(1)
#ptSerial.send_request_fare("0C4314F4627E", get_fare_id)

while True:
  pass
