#!/usr/bin/env python3
from uuid import UUID
import time
import protocol_serial as ps

#address = "0C4314F4627F"
address = "4C5BB3CA9C47"
#address = "4C5BB3CA9C43"

def door_announcement(inDoorway: bool):
  #print("Announcement inDoorway: " + str(inDoorway))
  pass

def init_announcement(flags: int):
  #ps.init_printout(flags)
  #print("Connect")
  ptSerial.send_request_connect(address, get_connect)

def disconnect_announcement(address: str):
  #print("Disconnected from: " + address)
  pass

def get_connect(state: int):
  #print("Connected: state=" + str(state))
  if state != 0:
    print("Fuck a duck")
  #print("Fare")
  ptSerial.send_request_fare(address, get_fare_id)

def get_fare_id(uuid: UUID):
  #end = time.time()
  #print(end - start)
  print("Fare ID: " + str(uuid))
  ptSerial.send_announcement_kill()
  #global send
  #send = True
  #ptSerial.send_request_fare("0C4314F4627F", get_fare_id)

global ptSerial
ptSerial = ps.ProtocolSerial("COM8", door_announcement, init_announcement, disconnect_announcement)
ptSerial.debug = True

#start = time.time()
#time.sleep(1)
#ptSerial.send_request_fare("0C4314F4627E", get_fare_id)
global send
send = False
while True:
  if send:
    send = False
    time.sleep(10)
    print("Connect")
    #address = "4C5BB3CA9C43"
    ptSerial.send_request_connect(address, get_connect)
  pass
