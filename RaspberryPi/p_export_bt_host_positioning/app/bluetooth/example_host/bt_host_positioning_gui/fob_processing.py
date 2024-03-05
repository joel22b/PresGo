#!/usr/bin/env python3

""" Positioning visualizer tool """
from datetime import datetime
import mqtt_client
import protocol_serial
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
  def __init__(self, gui):
    self.lock = Lock()
    self.reset_variables()
    self.gui = gui
    self.mqtt = mqtt_client.MQTTClient(self,gui)
    self.pt_serial = protocol_serial.ProtocolSerial(BUS_NODE_WITH_CONNECTION_PORT, self.door_announcement_1, self.init_announcement,gui) 
    # self.pt_serial_distance = protocol_serial.ProtocolSerial(BUS_NODE_DISTANCE_SENSOR_ONLY_PORT, self.door_announcement_2, self.init_announcement) 

  def reset_variables(self):
    self.in_cylinder = {}
    self.tags_being_fare_checked = []
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
      if address in self.tags_being_fare_checked:
        self.tags_being_fare_checked.remove(address)
      self.gui.enqueue_state(tkinter_gui.State.FAILURE if uuid == UUID(int=0) else tkinter_gui.State.SUCCESS)

  def process_message(self, x, y, z, tag_id):
    #if not tag_id in currently_connected
    # this.pt_serial.send_request_connect(tag_id, get_connect)
    is_tag_in_cylinder = self.is_point_inside_cylinder(x, y, z, 10, 10)
    with self.lock:
      self.in_cylinder[tag_id] = (is_tag_in_cylinder, datetime.now())
      #if not self.someone_in_doorway1 and not self.someone_in_doorway2:
      if not self.someone_in_doorway1:
        return
      most_recent_in_cyl_id = None
      most_recent_in_cyl_time = None
      for id, (is_tag_in_cylinder, time_last_seen) in self.in_cylinder.items():
        if is_tag_in_cylinder and (most_recent_in_cyl_time is None or time_last_seen > most_recent_in_cyl_time) and not id in self.tags_being_fare_checked:
          most_recent_in_cyl_id = id
          most_recent_in_cyl_time = time_last_seen
      if most_recent_in_cyl_id is None:
        return
      self.gui.enqueue_state(tkinter_gui.State.VALIDATING)
      self.tags_being_fare_checked.append(most_recent_in_cyl_id)
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
        self.gui.increment_person_counter() #TEMP
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

  def init_announcement(self, flags: int):
    protocol_serial.init_printout(flags)
    self.pt_serial.send_request_connect('0C4314F4627F', self.get_connect) # TODO put this in on_message instead
    self.pt_serial.send_request_connect('4C5BB3CA9C47', self.get_connect) # TODO put this in on_message instead
    self.pt_serial.send_request_connect('4C5BB3CA9C43', self.get_connect) # TODO put this in on_message instead
    with self.lock:
      self.gui.reset()
      self.reset_variables()

  def get_connect(self, state: int):
    print("huh") # TODO add to a list of current connections

def signal_handler(sig, frame):
  gui.running = False
  #fob_processing.mqtt.running = False
  fob_processing.pt_serial.running = False
  global running
  running = False

def main():
  # setup signal handler for graceful shutdown
  signal.signal(signal.SIGINT, signal_handler)
  # setup gui and initialize global variables used by mqtt callbacks
  global gui
  gui = tkinter_gui.TkinterGUI()
  # setup fob_processing, mqtt client, and serial communication with BlueNRG board
  global fob_processing
  fob_processing = FobProcessing(gui)
  # start GUI loop on separate thread
  gui.start_main_loop()
  # loop on main thread until program termination
  global running
  while running:
    print('running') # not printing currently??
    time.sleep(0.5)

if __name__ == "__main__":
  main()
