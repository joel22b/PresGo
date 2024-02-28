#!/usr/bin/env python3

""" Positioning visualizer tool """

import argparse
from datetime import datetime
import json
import os
import paho.mqtt.client as mqtt
import protocol_serial as ps
import re
import signal
import threading
from threading import Event, Lock
import tkinter_gui
from uuid import UUID
import visualizer

DEFAULT_CONFIG = os.path.join(os.path.dirname(__file__), "../bt_host_positioning/config/positioning_config.json")
DEFAULT_CONNECTION = {"host": "localhost", "port": 1883}
ANTENNA_ARRAY_PORT = '/dev/ttyACM1'

gui = None
in_cylinder = {}
tags_being_fare_checked = []
close_event = Event()
lock = Lock()
someone_in_doorway = False
processed_count = 0
ptSerial = None

def signal_handler(sig, frame):
  ptSerial.running = False
  close_event.set()

def is_point_inside_cylinder( x, y, z, radius, height):
  in_circle = x ** 2 + y ** 2 <= radius ** 2
  in_height_range = 0 <= z <= height
  return in_circle and in_height_range

def setup_mqtt():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument("-c", metavar="CONFIG_FILE", help="Configuration file path", default=DEFAULT_CONFIG)
  parser.add_argument("-m", metavar="HOST[:PORT]", help="MQTT broker connection parameters", default=DEFAULT_CONNECTION, type=mqtt_conn_type)
  args = parser.parse_args()
  v = visualizer.Visualizer()
  v.parse_config(args.c)
  client = mqtt.Client(userdata=v)
  client.on_connect = on_connect
  client.on_message = on_message
  client.connect(host=args.m["host"], port=args.m["port"])
  client.loop_forever()

def mqtt_conn_type(arg):
  """ Argument parser for MQTT server connection parameters. """
  retval = DEFAULT_CONNECTION
  arglist = arg.split(":", 1)
  if len(arglist[0]) == 0:
    raise argparse.ArgumentTypeError("Host name is empty")
  retval["host"] = arglist[0]
  if len(arglist) > 1:
    try:
      retval["port"] = int(arglist[1])
    except ValueError as val:
      raise argparse.ArgumentTypeError("Invalid port number: " + arglist[1]) from val
  return retval

def on_connect(client, userdata, flags, rc):
  ''' Called when a CONNACK response is received from the server. '''
  print("Connected with result code " + str(rc))
  topic = "silabs/aoa/position/{}/#".format(userdata.positioning_id)
  print("Subscribe for ", topic)
  client.subscribe(topic)
  for loc_id in userdata.locators:
    topic = "silabs/aoa/angle/{}/#".format(loc_id)
    print("Subscribe for ", topic)
    client.subscribe(topic)
    #ptSerial.send_request_connect('0C4314F4627F', get_connect)

def on_message(client, userdata, msg):
  ''' Called when a PUBLISH message is received from the server. '''
  m = re.match(r"silabs/aoa/position/.+/(?P<tag_id>.+)", msg.topic)
  if m is None:
    return
  entry = json.loads(msg.payload)
  is_tag_in_cylinder = is_point_inside_cylinder(entry["x"], entry["y"], entry["z"], 1, 10)
  with lock:
    in_cylinder[get_trimmed_tag_id(m.group("tag_id"))] = (is_tag_in_cylinder, datetime.now())
    if not someone_in_doorway:
      return
    most_recent_in_cyl_id = None
    most_recent_in_cyl_time = None
    for id, (is_tag_in_cylinder, time_last_seen) in in_cylinder.items():
      if is_tag_in_cylinder and (most_recent_in_cyl_time is None or time_last_seen > most_recent_in_cyl_time) and not id in tags_being_fare_checked:
        most_recent_in_cyl_id = id
        most_recent_in_cyl_time = time_last_seen
    if most_recent_in_cyl_id is None:
      return
    gui.enqueue_state(tkinter_gui.State.VALIDATING)
    tags_being_fare_checked.append(most_recent_in_cyl_id)
    ptSerial.send_request_fare(most_recent_in_cyl_id, lambda uuid: get_fare_id(most_recent_in_cyl_id, uuid))
    global processed_count
    processed_count += 1

def get_trimmed_tag_id(tag_id):
  return tag_id.replace("ble-pd-", "")

def get_fare_id(address: str, uuid: UUID):
  print("Fare ID: " + str(uuid) + " Address: " + address)
  with lock:
    if address in tags_being_fare_checked:
      tags_being_fare_checked.remove(address)
  gui.enqueue_state(tkinter_gui.State.FAILURE if uuid == UUID(int=0) else tkinter_gui.State.SUCCESS)

def door_announcement(inDoorway: bool):
  print("Announcement inDoorway: " + str(inDoorway))
  with lock:
    global someone_in_doorway
    someone_in_doorway = inDoorway
    if inDoorway:
      return
    global processed_count
    if processed_count == 0:
      gui.enqueue_state(tkinter_gui.State.FAILURE, 'Valid payment fob not found.')
    else:
      processed_count = 0

def init_announcement(flags: int):
  ps.init_printout(flags)
  ptSerial.send_request_connect('0C4314F4627F', get_connect)

def get_connect():
  print("huh")

def main():
  # setup signal handler for graceful shutdown
  signal.signal(signal.SIGINT, signal_handler)
  # setup gui and initialize global variables used by mqtt callbacks
  global gui
  gui = tkinter_gui.TkinterGUI(close_event)
  # mqtt operations on separate thread to not block main gui thread
  thread_mqtt = threading.Thread(target=setup_mqtt, daemon=True)
  thread_mqtt.start()
  # setup communication with BlueNRG board
  global ptSerial
  ptSerial = ps.ProtocolSerial(ANTENNA_ARRAY_PORT, door_announcement, init_announcement)
  # GUI blocking loop
  gui.start_main_loop()

if __name__ == "__main__":
  main()
