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
from uuid import UUID

BUS_NODE_WITH_CONNECTION_PORT = '/dev/ttyACM1' #outermost (triggered first upon entry)
# BUS_NODE_DISTANCE_SENSOR_ONLY_PORT = '/dev/ttyACM2' #innermost distance sensor (triggered first upon exit)

gui = None
fob_processing = None

class FobProcessing:
  def __init__(self):
    self.lock = Lock()
    self.reset()
    # setup gui and initialize global variables used by mqtt callbacks
    self.gui = tkinter_gui.TkinterGUI(self)
    # setup mqtt client and serial communication with stm board 
    self.mqtt = mqtt_client.MQTTClient(self)
    self.pt_serial = protocol_serial.ProtocolSerial(BUS_NODE_WITH_CONNECTION_PORT, self.door_announcement_1, self.stm_init_announcement, self.disconnect_announcement)
    # self.pt_serial_distance = protocol_serial.ProtocolSerial(BUS_NODE_DISTANCE_SENSOR_ONLY_PORT, self.door_announcement_2, self.stm_init_announcement) 

  def reset(self):
    self.fobs_in_cylinder_status = {}
    self.fobs_attempting_payment = []
    self.fobs_currently_connecting = []
    self.fobs_currently_connected = []
    self.someone_in_doorway1 = False
    # self.someone_in_doorway2 = False
    self.processed_count = 0

  def is_point_inside_cylinder(self, x, y, z, radius, height):
    in_circle = x ** 2 + y ** 2 <= radius ** 2
    in_height_range = 0 <= z <= height
    return in_circle and in_height_range

  def get_fare_id(self, address: str, uuid: UUID):
    print("Fare ID: " + str(uuid) + " Address: " + address)
    with self.lock:
      if address in self.fobs_attempting_payment:
        self.fobs_attempting_payment.remove(address)
      self.gui.enqueue_state(tkinter_gui.State.FAILURE if uuid == UUID(int=0) else tkinter_gui.State.SUCCESS)

  def process_message(self, x, y, z, fob_id):
    is_fob_in_cylinder = self.is_point_inside_cylinder(x, y, z, 1, 10)
    with self.lock:
      if not fob_id in self.fobs_currently_connecting and not fob_id in self.fobs_currently_connected:
        self.pt_serial.send_request_connect(fob_id, lambda state: self.connect_announcement(state, fob_id))
        self.fobs_currently_connecting.append(fob_id)
      self.fobs_in_cylinder_status[fob_id] = (is_fob_in_cylinder, datetime.now())
      #if not self.someone_in_doorway1 and not self.someone_in_doorway2:
      if not self.someone_in_doorway1:
        return
      most_recent_in_cyl_id = None
      most_recent_in_cyl_time = None
      for id, (is_fob_in_cylinder, time_last_seen) in self.fobs_in_cylinder_status.items():
        if is_fob_in_cylinder and (most_recent_in_cyl_time is None or time_last_seen > most_recent_in_cyl_time) and not id in self.fobs_attempting_payment:
          most_recent_in_cyl_id = id
          most_recent_in_cyl_time = time_last_seen
      if most_recent_in_cyl_id is None:
        return
      self.gui.enqueue_state(tkinter_gui.State.VALIDATING)
      self.fobs_attempting_payment.append(most_recent_in_cyl_id)
      self.pt_serial.send_request_fare(most_recent_in_cyl_id, lambda uuid: self.get_fare_id(most_recent_in_cyl_id, uuid))
      self.processed_count += 1

  def door_announcement_1(self, inDoorway: bool):
    print("Announcement inDoorway: " + str(inDoorway))
    with self.lock:
      self.someone_in_doorway1 = inDoorway
      if inDoorway:
        return
      if self.processed_count == 0:
        self.gui.enqueue_state(tkinter_gui.State.FAILURE, 'Valid payment fob not found.')
        self.gui.increment_person_counter() # Temporary, even with only 1 distance sensor there may be a better counting method
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
    # with self.lock:
    #   self.gui.reset()
    #   self.reset_variables()

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

def signal_handler(sig, frame):
  fob_processing.gui.running = False
  fob_processing.pt_serial.running = False
  global running
  running = False

def main():
  # setup signal handler for graceful shutdown
  signal.signal(signal.SIGINT, signal_handler)
  # setup gui, fob_processing, mqtt client, and serial communication with BlueNRG board
  global fob_processing
  fob_processing = FobProcessing()
  # start GUI loop on separate thread
  fob_processing.gui.start_main_loop()
  # loop on main thread until program termination
  global running
  while running:
    print('running') # not printing currently??
    time.sleep(0.5)

if __name__ == "__main__":
  main()
