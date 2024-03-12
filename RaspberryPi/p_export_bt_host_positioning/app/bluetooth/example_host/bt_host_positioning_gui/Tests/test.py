#!/usr/bin/env python3
from uuid import UUID
import time
import protocol_serial as ps
import fare_system as fs

#address = "0C4314F4627F"
address2 = "4C5BB3CA9C47"
address1 = "4C5BB3CA9C43"
address4 = "4C5BB3C9F98C"

to_connect = []
to_fare = []

def door_announcement(inDoorway: bool):
  print("Announcement inDoorway: " + str(inDoorway))
  pass

def init_announcement(flags: int):
  ps.init_printout(flags)
  start_conn(address1)
  #start_conn(address2)
  #start_conn(address4)
  #print("Connect")
  #ptSerial.send_request_connect(address, get_connect)

def disconnect_announcement(address: str):
  print("Disconnected from: " + address)
  pass

def get_connect(addr: str, state: int):
  #print("Connected: state=" + str(state))
  if state != 0:
    print("Connection failed: " + str(state))
    #start_conn(addr)
    #global send
    #send = True
  else:
    print("Done " + addr)
    #if addr == address1:
    #ptSerial.send_request_fare(addr, get_fare_id)
    if addr == address1:
      ptSerial.send_request_fare(addr, get_fare_id)
      start_conn(address2)
    elif addr == address2:
      global send
      send = True
      #time.sleep(0.5)
      #ptSerial.send_request_fare(addr, get_fare_id)
    #  start_conn(address4)

def get_fare_id(uuid: UUID):
  #end = time.time()
  #print(end - start)
  print("Fare ID: " + str(uuid))
  (valid, balance) = fareSystem.validate_fare(uuid)
  if balance == fareSystem.fareError:
    print("Valid: [" + str(valid) + "] Error reading fob")
  elif balance == fareSystem.fareNotFound:
    print("Valid: [" + str(valid) + "] Fare not found")
  else:
    print("Valid: [" + str(valid) + "] Remaining balance: $" + str(balance))
  #if uuid == UUID(int=1):
  #  start_conn(address2)
  #elif uuid == UUID(int=2):
  #  start_conn(address4)
  #time.sleep(10)
  #
  #ptSerial.send_announcement_kill()
  #global send
  #send = True
  #ptSerial.send_request_fare("0C4314F4627F", get_fare_id)
    
def start_conn(addr: str):
  print("Connect " + addr)
  ptSerial.send_request_connect(addr, lambda state: get_connect(addr, state))

global ptSerial
ptSerial = ps.ProtocolSerial("COM9", door_announcement, init_announcement, disconnect_announcement, None)
#ptSerial.debug = True

global fareSystem
fareSystem = fs.FareSystem()

#time.sleep(1)
#ptSerial.send_announcement_kill()

#start = time.time()
#time.sleep(1)
#ptSerial.send_request_fare("0C4314F4627E", get_fare_id)
global send
send = False
while True:
  if send:
    send = False
    print("Starting")
    time.sleep(10)
    msg = ps.psm.Message()
    msg.raw = "Test"
    ptSerial.serial_write(msg)
    #ptSerial.send_request_fare(address2, get_fare_id)
    #start_conn(address4)
    #start_conn()
    #print("Connect")
    #address = "4C5BB3CA9C43"
    #ptSerial.send_request_connect(address, get_connect)
  pass
