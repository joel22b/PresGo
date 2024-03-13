#!/usr/bin/env python3

""" Positioning visualizer tool """
from collections import deque, namedtuple
from datetime import datetime
import mqtt_client
import protocol_serial
from threading import Lock
import tkinter_gui
from uuid import UUID
import fare_system

STM_BLUETOOTH_AND_DISTANCE_PORT = '/dev/ttyACM1' # outermost (triggered first upon entry)
STM_DISTANCE_ONLY_PORT = '/dev/ttyACM2'          # innermost distance sensor (triggered first upon exit)
PAY_ZONE_X_MAX = 4
PAY_ZONE_Y_MAX = 6
PAY_ZONE_Z_MAX = 50                              # don't care about this dimension if it's facing upwards
LOCKOUT_VALIDATION_ATTEMPT_S = 2                # only attempt a validation again after x seconds for a particular fare fob
LOCKOUT_NO_FOB_STATE_S = 0.4                     # only trigger no fare fob state (passing distance sensor with no fob) every x seconds
LOCKOUT_PERSON_COUNTER_UPDATE_S = 0.75           # don't update the person counter again until x seconds have elapsed
STALE_LOCATION_DATA_THRESHOLD_S = 1.5            # ignore saved position measurements from a fob if they are older than x seconds

IdWithTimeLastSeen = namedtuple('IdWithTimeLastSeen', ['id', 'time_last_seen'])

class FobProcessing:
  def __init__(self):
    self.lock = Lock()
    self.on_reset()
    self.fare_sys = fare_system.FareSystem()
    self.gui = tkinter_gui.TkinterGUI(self)
    self.pt_serial = protocol_serial.ProtocolSerial(STM_BLUETOOTH_AND_DISTANCE_PORT, self.gui, self.on_door_announcement_front, self.on_stm_init_announcement, self.on_disconnect_announcement)
    self.pt_serial_distance = protocol_serial.ProtocolSerial(STM_DISTANCE_ONLY_PORT, self.gui, self.on_door_announcement_back, self.on_distance_stm_init_announcement)
    self.mqtt = mqtt_client.MQTTClient(self, self.gui)
    self.pt_serial.send_announcement_kill()
    self.pt_serial_distance.send_announcement_kill()

  def process_message(self, x, y, z, fob_id):
    with self.lock:
      unconnected_fob = fob_id not in self.connecting and fob_id not in self.connected
      if unconnected_fob:
        self.connecting.append(fob_id)
    if unconnected_fob:
      self.pt_serial.send_request_connect(fob_id, lambda state: self.on_connect_announcement(state, fob_id))
      #print("Initiating connection with:", fob_id)
      #self.print_connect_lists()
      return
    #print('fob_id:',fob_id, 'x', x, "y", y, self.is_point_inside_pay_zone(x, y, z))
    in_pay_zone = self.is_point_inside_pay_zone(x, y, z)
    with self.lock:
      for fob in self.pay_zone_queue:
        if fob.id == fob_id:
          if not in_pay_zone:
            self.pay_zone_queue.remove(fob)
          return
      if in_pay_zone:
        self.pay_zone_queue.append(IdWithTimeLastSeen(id=fob_id, time_last_seen=datetime.now()))

  def process_pay_zone_queue_entry(self):
    no_fob_found = False
    with self.lock:
      # print(len(self.pay_zone_queue))
      if len(self.pay_zone_queue) == 0:
        no_fob_found = not self.last_no_fob_time or (datetime.now() - self.last_no_fob_time).total_seconds() > LOCKOUT_NO_FOB_STATE_S
        # print(no_fob_found)
    if no_fob_found:
      self.gui.enqueue_state(tkinter_gui.State.FAILURE, 'Valid Fare Fob not found.')
      with self.lock:
        self.last_no_fob_time = datetime.now()
        return
    same_fob_too_quick = False
    with self.lock:
      (id, time_last_seen) = self.pay_zone_queue.pop()
      if id in self.validating:
        return
      same_fob_too_quick = id in self.last_validation_attempt and \
        (datetime.now() - self.last_validation_attempt[id]).total_seconds() < LOCKOUT_VALIDATION_ATTEMPT_S
      if not same_fob_too_quick:
        self.validating.append(id)
    if same_fob_too_quick:
      self.gui.enqueue_state(tkinter_gui.State.SUCCESS, "Fare has already been paid.")
      return
    self.gui.enqueue_state(tkinter_gui.State.VALIDATING)      
    self.pt_serial.send_request_fare(id, lambda uuid: self.on_fare_response(id, uuid))

  def on_fare_response(self, address: str, uuid: UUID):
    with self.lock:
      self.last_validation_attempt[address] = datetime.now()
    print("Fare Response - Fare ID: " + str(uuid) + " Address: " + address)
    if uuid == UUID(int=0):
      if address == "4C5BB3CA9C43":
        uuid = UUID(int=1)
      elif address == "4C5BB3CA9C47":
        uuid = UUID(int=2)
      elif address == "4C5BB3CA9C49":
        uuid = UUID(int=3)
      elif address == "4C5BB3C9F98C":
        uuid = UUID(int=4)
    (valid, balance) = self.fare_sys.validate_fare(uuid)
    text = f"Remaining Balance: {'' if balance >= 0 else '-'}${str(abs(balance))}."
    state = tkinter_gui.State.SUCCESS if valid else tkinter_gui.State.FAILURE
    if balance == self.fare_sys.fareError:
      text = "Error reading Fare Fob."
    elif balance == self.fare_sys.fareNotFound:
      text = "Invalid Fare Fob."
    self.gui.enqueue_state(state, text)
    with self.lock:
      if address in self.validating:
        self.validating.remove(address)

  def on_door_announcement_front(self, inDoorway: bool):
    print("STM Announcement - Front sensor inDoorway: " + str(inDoorway))
    with self.lock:
      self.someone_in_doorway_front = inDoorway
    if inDoorway:
      return
    with self.lock:
      person_entering_bus = self.someone_in_doorway_back and \
        (self.last_person_counter_update_time is None or (datetime.now() - self.last_person_counter_update_time).total_seconds() > LOCKOUT_PERSON_COUNTER_UPDATE_S)
    if person_entering_bus:
      print('person entering bus')
      self.process_pay_zone_queue_entry()
      self.gui.increment_person_counter()
      with self.lock:
        self.last_person_counter_update_time = datetime.now()
 
  def on_door_announcement_back(self, inDoorway: bool):
    print("STM Announcement - Back sensor inDoorway: " + str(inDoorway))
    with self.lock:
      self.someone_in_doorway_back = inDoorway
    if inDoorway:
      return
    with self.lock:
      person_exiting_bus = self.someone_in_doorway_front and \
        (self.last_person_counter_update_time is None or(datetime.now() - self.last_person_counter_update_time).total_seconds() > LOCKOUT_PERSON_COUNTER_UPDATE_S)
    if person_exiting_bus:
      print('person exiting bus')
      self.gui.decrement_person_counter()
      with self.lock:
        self.last_person_counter_update_time = datetime.now()

  def on_connect_announcement(self, state: int, address: str):
    with self.lock:
      if address in self.connecting:
        self.connecting.remove(address)
      if state == 0:
        self.connected.append(address)
    #print("STM Announcement - Connection state for " + address + ": " + str(state))
    #self.print_connect_lists()

  def on_disconnect_announcement(self, address: str):
    with self.lock:
      if address in self.connecting:
        self.connecting.remove(address)
      if address in self.connected:
        self.connected.remove(address)
    #print("STM Announcement - Disconnected from: " + address)
    #self.print_connect_lists()

  def on_stm_init_announcement(self, flags: int):
    protocol_serial.init_printout(flags)
    self.pt_serial.check_and_set_error_status(flags)
    self.on_reset()
  
  def on_distance_stm_init_announcement(self, flags: int):
    protocol_serial.init_printout(flags)
    self.pt_serial_distance.check_and_set_error_status(flags)
    self.on_reset()

  def initiate_reset(self):
    self.fare_sys.__init__()
    self.pt_serial.send_announcement_kill()
    self.pt_serial_distance.send_announcement_kill()

  def on_reset(self):
    with self.lock:
      self.pay_zone_queue = deque()
      self.last_validation_attempt = {}
      self.validating = []
      self.connecting = []
      self.connected = []
      self.someone_in_doorway_front = False
      self.someone_in_doorway_back = False
      self.last_no_fob_time = None
      self.last_person_counter_update_time = None

  def print_connect_lists(self):
    with self.lock:
      print('Fobs Connecting:', self.connecting)
      print('Fobs Connected: ', self.connected)

  # pay zone is a rectangular prism
  def is_point_inside_pay_zone(self, x, y, z):
    return abs(x) < PAY_ZONE_X_MAX and abs(y) < PAY_ZONE_Y_MAX and abs(z) < PAY_ZONE_Z_MAX
