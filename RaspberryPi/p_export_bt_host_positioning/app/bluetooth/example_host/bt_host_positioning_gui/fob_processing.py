#!/usr/bin/env python3

""" Positioning visualizer tool """
from datetime import datetime
from decimal import Decimal
import mqtt_client
import protocol_serial
import random
import signal
from threading import Lock
import time
import tkinter_gui
from tkinter_gui import SystemStatus
from uuid import UUID
import fare_system

BUS_NODE_WITH_CONNECTION_PORT = '/dev/ttyACM1' # outermost (triggered first upon entry)
# BUS_NODE_DISTANCE_SENSOR_ONLY_PORT = '/dev/ttyACM2' # innermost distance sensor (triggered first upon exit)
PAY_ZONE_X_MAX = 2
PAY_ZONE_Y_MAX = 2
PAY_ZONE_Z_MAX = 50 # don't care about this dimension if it's facing upwards
VALIDATION_ATTEMPT_LOCKOUT_S = 5 # only attempt a validation again after x seconds for a particular fare fob
TIME_BETWEEN_NO_FOB_RESULTS_S = 0.4

fob_processing = None

class FobProcessing:
  def __init__(self):
    self.lock = Lock()
    self.reset(True)
    # setup gui and initialize global variables used by mqtt callbacks
    self.gui = tkinter_gui.TkinterGUI(self)
    # setup mqtt client and serial communication with stm board 
    self.mqtt = mqtt_client.MQTTClient(self, self.gui)
    self.pt_serial = protocol_serial.ProtocolSerial(BUS_NODE_WITH_CONNECTION_PORT, self.door_announcement_1, self.stm_init_announcement, self.disconnect_announcement, self.gui)
    self.pt_serial.send_announcement_kill() 
    # self.pt_serial_distance = protocol_serial.ProtocolSerial(BUS_NODE_DISTANCE_SENSOR_ONLY_PORT, self.door_announcement_2, self.stm_init_announcement) 
    self.fare_sys = fare_system.FareSystem()

  def reset(self, init = False):
    self.fobs_last_in_pay_zone = {}
    self.fobs_validating = []
    self.fobs_most_recent_validation_attempt = {}
    self.fobs_currently_connecting = []
    self.fobs_currently_connected = []
    self.someone_in_doorway1 = False
    # self.someone_in_doorway2 = False
    self.processed_count = 0
    self.time_of_last_no_fob = None
    if not init:
      self.pt_serial.send_announcement_kill() 

  def is_point_inside_pay_zone(self, x, y, z):
    # pay zone is a recangular prism
    return x < PAY_ZONE_X_MAX and y < PAY_ZONE_Y_MAX and z < PAY_ZONE_Z_MAX

  def get_fare_id(self, address: str, uuid: UUID):
    self.fobs_most_recent_validation_attempt[address] = datetime.now()
    print("Fare ID: " + str(uuid) + " Address: " + address)
    self.pt_serial.send_announcement_kill()
    (valid, balance) = self.fare_sys.validate_fare(uuid)
    with self.lock:
      if address in self.fobs_validating:
        self.fobs_validating.remove(address)
      text = "Remaining Balance: $"+str(balance)
      state = tkinter_gui.State.SUCCESS
      if balance == self.fare_sys.fareError:
        text = "Error reading Fare Fob"
        state = tkinter_gui.State.FAILURE
      elif balance == self.fare_sys.fareNotFound:
        text = "Invalid Fare Fob"
        state = tkinter_gui.State.FAILURE
      self.gui.enqueue_state(state, text)

  def process_message(self, x, y, z, fob_id):
    is_fob_in_pay_zone = self.is_point_inside_pay_zone(x, y, z)
    with self.lock:
      if not fob_id in self.fobs_currently_connecting and not fob_id in self.fobs_currently_connected:
        self.pt_serial.send_request_connect(fob_id, lambda state: self.connect_announcement(state, fob_id))
        self.fobs_currently_connecting.append(fob_id)
      self.fobs_last_in_pay_zone[fob_id] = (is_fob_in_pay_zone, datetime.now())
      if not self.someone_in_doorway1: # and not self.someone_in_doorway2:
        return
      most_recent_in_pay_zone_id = None
      most_recent_in_pay_zone_time = None
      for id, (is_fob_in_pay_zone, time_last_seen) in self.fobs_last_in_pay_zone.items():
        if is_fob_in_pay_zone and (most_recent_in_pay_zone_time is None or time_last_seen > most_recent_in_pay_zone_time) and not id in self.fobs_validating and (not id in self.fobs_most_recent_validation_attempt or (datetime.now() - self.fobs_most_recent_validation_attempt[id]).total_seconds() > VALIDATION_ATTEMPT_LOCKOUT_S):
          most_recent_in_pay_zone_id = id
          most_recent_in_pay_zone_time = time_last_seen
      if most_recent_in_pay_zone_id is None:
        return
      self.gui.enqueue_state(tkinter_gui.State.VALIDATING)
      self.fobs_validating.append(most_recent_in_pay_zone_id)
      self.pt_serial.send_request_fare(most_recent_in_pay_zone_id, lambda uuid: self.get_fare_id(most_recent_in_pay_zone_id, uuid))
      self.processed_count += 1

  def door_announcement_1(self, inDoorway: bool):
    print("Announcement inDoorway: " + str(inDoorway))
    with self.lock:
      self.someone_in_doorway1 = inDoorway
      if inDoorway:
        return
      if self.processed_count == 0:
        if not self.time_of_last_no_fob or (datetime.now() - self.time_of_last_no_fob).total_seconds() > TIME_BETWEEN_NO_FOB_RESULTS_S:
          self.gui.enqueue_state(tkinter_gui.State.FAILURE, 'Valid payment fob not found.')
          self.gui.increment_person_counter() # Change this if we don't like it always triggering
        self.time_of_last_no_fob = datetime.now()
      else:
        self.processed_count = 0
      # if self.someone_in_doorway2:
      #   self.gui.increment_person_counter()
 
  # def door_announcement_2(self, inDoorway: bool):
  #   print("Announcement inDoorway2: " + str(inDoorway))
  #   with self.lock:
  #     if not self.gui:
  #       return
  #     self.someone_in_doorway2 = inDoorway
  #     if inDoorway:
  #       return
  #     if self.someone_in_doorway1:
  #       self.gui.decrement_person_counter()

  def stm_init_announcement(self, flags: int):
    protocol_serial.init_printout(flags)
    self.pt_serial.check_and_set_error_status(flags)

  def connect_announcement(self, state: int, address: str):
    print("Connection state for " + address + ": " + str(state))
    with self.lock:
      if address in self.fobs_currently_connecting:
        self.fobs_currently_connecting.remove(address)
      if state == 0:
        self.fobs_currently_connected.append(address)

  def disconnect_announcement(self, address: str):
    print("Disconnected from: " + address)
    with self.lock:
      if address in self.fobs_currently_connected:
       self.fobs_currently_connected.remove(address)
      if address in self.fobs_currently_connecting:
        self.fobs_currently_connecting.remove(address)

def signal_handler(sig, frame):
  fob_processing.gui.running = False
  fob_processing.pt_serial.running = False

def main():
  try:
    # setup signal handler for graceful shutdown
    signal.signal(signal.SIGINT, signal_handler)
    # setup gui, fob_processing, mqtt client, and serial communication with BlueNRG board
    global fob_processing
    fob_processing = FobProcessing()
    # start GUI loop on separate thread
    fob_processing.gui.start_main_loop()
  except Exception as e:
    print('Error on main thread:', str(e))
    if fob_processing.gui:
      fob_processing.gui.set_system_status(SystemStatus.ERROR)

if __name__ == "__main__":
  main()
