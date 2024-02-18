#!/usr/bin/env python3

""" Positioning visualizer tool """

import argparse
from datetime import datetime
import json
import math
import os
import paho.mqtt.client as mqtt
import protocol_serial as ps
import re
import signal
import threading
from threading import Event, Lock
import tkinter
from uuid import UUID
import visualizer

DEFAULT_CONFIG = os.path.join(os.path.dirname(__file__), "../bt_host_positioning/config/positioning_config.json")
DEFAULT_CONNECTION = {"host": "localhost", "port": 1883}

# GUI Constants
GUI_WIDTH = 800 # in px
GUI_HEIGHT = 800 # in px
GUI_TIME_BETWEEN_UPDATES = 4000 # in ms
GUI_FONT = 'Arial'
GUI_COLOR_SUCCESS = 'green'
GUI_COLOR_FAILURE = 'red'
GUI_COLOR_PROCESSING = 'yellow'
GUI_COLOR_NEUTRAL = 'grey'
in_cylinder = {}
tags_being_fare_checked = []
close_event = Event()
lock = Lock()

someone_in_doorway = False
processed_count = 0
running = True
ptSerial = None

def signal_handler(sig, frame):
  global running
  running = False
  ptSerial.running = False
  close_event.set()

def check_close_event():
  if close_event.is_set():
    root.destroy()
  else:
    root.after(100, check_close_event)

def is_point_inside_cylinder( x, y, z, radius, height):
  in_circle = x ** 2 + y ** 2 <= radius ** 2
  in_height_range = 0 <= z <= height
  return in_circle and in_height_range

def reset_gui_main_text():
  canvas.itemconfig(gui_main_text, text='Waiting for passengers...')
  canvas.config(bg=GUI_COLOR_NEUTRAL)

def setup_gui():
  global root, canvas, gui_main_text, gui_status_text
  root = tkinter.Tk()
  root.title('PresGo GUI')
  canvas = tkinter.Canvas(root, width=GUI_WIDTH, height=GUI_HEIGHT, bg=GUI_COLOR_NEUTRAL)
  canvas.pack(fill='both', expand=True)
  gui_main_text = canvas.create_text(math.ceil(GUI_WIDTH/2), math.ceil(GUI_HEIGHT/2), text='Waiting for passengers...', font=(GUI_FONT, math.ceil(GUI_WIDTH/40)))
  gui_status_text = canvas.create_text(GUI_WIDTH-math.ceil(GUI_WIDTH/40), math.ceil(GUI_HEIGHT/40), text='System Status: Running', font=(GUI_FONT, math.ceil(GUI_WIDTH/60)), fill=GUI_COLOR_SUCCESS)
  root.after(100, check_close_event)

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

def on_message(client, userdata, msg):
  ''' Called when a PUBLISH message is received from the server. '''
  m = re.match(r"silabs/aoa/position/.+/(?P<tag_id>.+)", msg.topic)
  if m is not None:
    entry = json.loads(msg.payload)
    is_tag_in_cylinder = is_point_inside_cylinder(entry["x"], entry["y"], entry["z"], 1, 10)
    tag_id = m.group("tag_id").replace("ble-pd-", "")
    with lock:
      in_cylinder[tag_id] = (is_tag_in_cylinder, datetime.now())
      if someone_in_doorway:
        most_recent_in_cyl_id = None
        most_recent_in_cyl_time = None
        for id, (is_tag_in_cylinder, time_last_seen) in in_cylinder.items():
          if is_tag_in_cylinder and (most_recent_in_cyl_time is None or time_last_seen > most_recent_in_cyl_time) and not id in tags_being_fare_checked:
            most_recent_in_cyl_id = id
            most_recent_in_cyl_time = time_last_seen
        if most_recent_in_cyl_id is not None:
          canvas.itemconfig(gui_main_text, text=f'Attempting to validate {most_recent_in_cyl_id}') #update this message
          canvas.config(bg=GUI_COLOR_PROCESSING)
          tags_being_fare_checked.append(most_recent_in_cyl_id)
          cb = lambda uuid: get_fare_id(most_recent_in_cyl_id, uuid)
          ptSerial.send_request_fare(most_recent_in_cyl_id, cb)
          global processed_count
          processed_count += 1

def get_fare_id(address: str, uuid: UUID):
  print("Fare ID: " + str(uuid) + " Address: " + address)
  with lock:
    if address in tags_being_fare_checked:
      tags_being_fare_checked.remove(address)
  if str(uuid) == '00000000-0000-0000-0000-000000000000':
    canvas.itemconfig(gui_main_text, text=f'Failed to validate {address}') #update this error msg
    canvas.config(bg=GUI_COLOR_FAILURE)
  else:
    canvas.itemconfig(gui_main_text, text=f'Validated {address}') #update this success message
    canvas.config(bg=GUI_COLOR_SUCCESS)
  root.after(GUI_TIME_BETWEEN_UPDATES, reset_gui_main_text)

def door_announcement(inDoorway: bool):
  print("Announcement inDoorway: " + str(inDoorway))
  with lock:
    global someone_in_doorway
    someone_in_doorway = inDoorway
    if not inDoorway:
      global processed_count
      if processed_count == 0:
        canvas.itemconfig(gui_main_text, text=f'Valid payment fob not found.')
        canvas.config(bg=GUI_COLOR_FAILURE)
        root.after(GUI_TIME_BETWEEN_UPDATES, reset_gui_main_text)
      else:
        processed_count = 0

def get_distance(distance: int):
  print("Distance: " + str(distance))

def get_door(inDoorway: bool):
  print("inDoorway: " + str(inDoorway))

def main():
  # setup signal handler for graceful shutdown
  signal.signal(signal.SIGINT, signal_handler)

  # setup gui and initialize global variables used by mqtt callbacks
  setup_gui()

  # mqtt operations on separate thread to not block main gui thread
  thread_mqtt = threading.Thread(target=setup_mqtt, daemon=True)
  thread_mqtt.start()

  global ptSerial
  ptSerial = ps.ProtocolSerial(door_announcement)

  # GUI blocking loop
  root.mainloop()


if __name__ == "__main__":
  main()
