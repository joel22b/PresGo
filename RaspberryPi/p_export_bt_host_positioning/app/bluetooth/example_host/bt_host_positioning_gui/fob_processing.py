#!/usr/bin/env python3

""" Positioning visualizer tool """
from collections import deque, namedtuple
from datetime import datetime
import mqtt_client
import protocol_serial
import threading
from threading import Lock
import time
import tkinter_gui
from uuid import UUID
import fare_system

BUS_NODE_WITH_CONNECTION_PORT = '/dev/ttyACM1' # outermost (triggered first upon entry)
# BUS_NODE_DISTANCE_SENSOR_ONLY_PORT = '/dev/ttyACM2' # innermost distance sensor (triggered first upon exit)
PAY_ZONE_X_MAX = 2
PAY_ZONE_Y_MAX = 2
PAY_ZONE_Z_MAX = 50 # don't care about this dimension if it's facing upwards
LOCKOUT_VALIDATION_ATTEMPT_S = 15 # only attempt a validation again after x seconds for a particular fare fob
LOCKOUT_NO_FOB_STATE_S = 0.4 # only trigger no fare fob state (passing distance sensor with no fob) every x seconds
STALE_LOCATION_DATA_THRESHOLD_S = 1.5 # ignore saved position measurements from a fob if they are older than x seconds
PROCESS_PAY_ZONE_QUEUE_INTERVAL_S = 0.1 # check for fobs in pay zone every x seconds

IdWithTimeLastSeen = namedtuple('IdWithTimeLastSeen', ['id', 'time_last_seen'])

class FobProcessing:
  def __init__(self):
    self.lock = Lock()
    self.reset(True)
    self.fare_sys = fare_system.FareSystem()
    self.gui = tkinter_gui.TkinterGUI(self)
    self.mqtt = mqtt_client.MQTTClient(self, self.gui)
    self.pt_serial = protocol_serial.ProtocolSerial(BUS_NODE_WITH_CONNECTION_PORT, self.door_announcement_1, self.stm_init_announcement, self.disconnect_announcement, self.gui)
    self.pt_serial.send_announcement_kill()
    # self.pt_serial_distance = protocol_serial.ProtocolSerial(BUS_NODE_DISTANCE_SENSOR_ONLY_PORT, self.door_announcement_2, self.stm_init_announcement)
    threading.Thread(target=self.process_pay_zone_queue, daemon=True).start()

  def reset(self, init = False):
    self.pay_zone_queue = deque()
    self.last_validation_attempt = {}
    self.validating = []
    self.connecting = []
    self.connected = []
    self.someone_in_doorway1 = False
    # self.someone_in_doorway2 = False
    self.processed_count = 0
    self.last_no_fob_time = None
    if not init: # maybe change???
      self.pt_serial.send_announcement_kill() 

  # pay zone is a rectangular prism
  def is_point_inside_pay_zone(self, x, y, z):
    return x < PAY_ZONE_X_MAX and y < PAY_ZONE_Y_MAX and z < PAY_ZONE_Z_MAX

  def get_fare_id(self, address: str, uuid: UUID):
    self.last_validation_attempt[address] = datetime.now()
    print("Fare ID: " + str(uuid) + " Address: " + address)
    self.pt_serial.send_announcement_kill()
    if uuid == UUID(int=0):
      if address == "4C5BB3CA9C43":
        uuid = UUID(int=1)
      elif address == "4C5BB3CA9C47":
        uuid = UUID(int=2)
      elif address == "4C5BB3CA9C49":
        uuid = UUID(int=3)
    (valid, balance) = self.fare_sys.validate_fare(uuid)
    text = "Remaining Balance: $"+str(balance)
    state = tkinter_gui.State.SUCCESS if valid else tkinter_gui.State.FAILURE
    if balance == self.fare_sys.fareError:
      text = "Error reading Fare Fob"
    elif balance == self.fare_sys.fareNotFound:
      text = "Invalid Fare Fob"
    self.gui.enqueue_state(state, text)
    with self.lock:
      if address in self.validating:
        self.validating.remove(address)

  def process_message(self, x, y, z, fob_id):
    with self.lock:
      if fob_id not in self.connecting and fob_id not in self.connected:
        self.pt_serial.send_request_connect(fob_id, lambda state: self.connect_announcement(state, fob_id))
        self.connecting.append(fob_id)
      if not self.is_point_inside_pay_zone(x, y, z):
        return
      for id_with_time_last_seen in self.pay_zone_queue:
        if id_with_time_last_seen.id == fob_id:
          return
      self.pay_zone_queue.append(IdWithTimeLastSeen(id=fob_id, time_last_seen=datetime.now()))

  def process_pay_zone_queue(self):
    while True: 
      time.sleep(PROCESS_PAY_ZONE_QUEUE_INTERVAL_S)
      with self.lock:
        if not self.someone_in_doorway1: # and not self.someone_in_doorway2:
          continue
        if len(self.pay_zone_queue) == 0:
          continue
        (id, time_last_seen) = self.pay_zone_queue.pop()
        if id in self.validating or \
          id in self.last_validation_attempt and \
          (datetime.now() - self.last_validation_attempt[id]).total_seconds() < LOCKOUT_VALIDATION_ATTEMPT_S or \
          (datetime.now() - time_last_seen).total_seconds() > STALE_LOCATION_DATA_THRESHOLD_S:
            continue
        self.gui.enqueue_state(tkinter_gui.State.VALIDATING)
        self.validating.append(id)
        self.pt_serial.send_request_fare(id, lambda uuid: self.get_fare_id(id, uuid))
        self.processed_count += 1

  def door_announcement_1(self, inDoorway: bool):
    print("Announcement inDoorway: " + str(inDoorway))
    with self.lock:
      self.someone_in_doorway1 = inDoorway
      if inDoorway:
        return
      if self.processed_count == 0:
        if not self.last_no_fob_time or (datetime.now() - self.last_no_fob_time).total_seconds() > LOCKOUT_NO_FOB_STATE_S:
          self.gui.enqueue_state(tkinter_gui.State.FAILURE, 'Valid payment fob not found.')
          self.gui.increment_person_counter() # Change this if we don't like it always triggering
        self.last_no_fob_time = datetime.now()
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
    self.connecting = []
    self.connected = []
    #self.fobs_last_in_pay_zone = {}
    #self.fobs_validating = []    
    #self.someone_in_doorway1 = False
    protocol_serial.init_printout(flags)
    self.pt_serial.check_and_set_error_status(flags)

  def connect_announcement(self, state: int, address: str):
    with self.lock:
      if address in self.connecting:
        self.connecting.remove(address)
      if state == 0:
        self.connected.append(address)
    print("Connection state for " + address + ": " + str(state))
    print('Connecting:', self.connecting)
    print('Connected:', self.connected)

  def disconnect_announcement(self, address: str):
    with self.lock:
      if address in self.connecting:
        self.connecting.remove(address)
      if address in self.connected:
       self.connected.remove(address)
    print("Disconnected from: " + address)
    print('Connecting:', self.connecting)
    print('Connected:', self.connected)
