#!/usr/bin/env python3

""" Positioning visualizer tool """

import argparse
import json
import os
import paho.mqtt.client as mqtt
import queue
import re
import random
import serial
import signal
import sys
import threading
from threading import Event
import time
import tkinter

MAX_NUM_TAGS = 300

DEFAULT_CONFIG = os.path.join(os.path.dirname(__file__), "../bt_host_positioning/config/positioning_config.json")
DEFAULT_CONNECTION = {"host": "localhost", "port": 1883}

close_event = Event()
running = True


class Visualizer(object):
  def __init__(self):
    self.q_pos = queue.Queue()
    self.q_ang = queue.Queue()
    self.tags = {}
    self.locators = {}
    self.positioning_id = None


  def update(self):
    # Process position messages
    while not self.q_pos.empty():
      entry = self.q_pos.get()
      tag_id = entry.pop("tag_id")
      if tag_id not in self.tags:
        if len(self.tags) == MAX_NUM_TAGS:
          continue
        self.add_tag(tag_id)
      self.tags[tag_id]["position"].update(entry)

    # Process angle messages
    while not self.q_ang.empty():
      entry = self.q_ang.get()
      tag_id = entry.pop("tag_id")
      loc_id = entry.pop("loc_id")
      if tag_id not in self.tags:
        if len(self.tags) == MAX_NUM_TAGS:
          continue
        self.add_tag(tag_id)
      if loc_id not in self.tags[tag_id]["angle"]:
        self.tags[tag_id]["angle"][loc_id] = {}
      self.tags[tag_id]["angle"][loc_id].update(entry)


  def parse_config(self, conf_file):
    locators = []
    with open(conf_file, "rb") as conf:
      config = json.load(conf)
      locators = config["locators"]
      self.positioning_id = config["id"]

    for loc in locators:
      loc_id = loc.pop("id")
      # MQTT topics are case sensitive
      m = re.match(r"^ble-(?P<address_type>[A-Za-z]+)-(?P<address>[0-9A-Fa-f]+)$", loc_id)
      if m is None:
        raise Exception("Invalid locator id format: {}".format(loc_id))
      loc_id = "ble-{}-{}".format(m.group("address_type").lower(), m.group("address").upper())
      self.locators[loc_id] = loc
      self.locators[loc_id]["sequence_nr"] = len(self.locators) - 1


  def add_tag(self, tag_id):
    print("Add tag {}".format(tag_id))
    self.tags[tag_id] = {}
    self.tags[tag_id]["sequence_nr"] = len(self.tags) - 1
    self.tags[tag_id]["color"] = [random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1), 1.0]
    self.tags[tag_id]["position"] = {}
    self.tags[tag_id]["angle"] = {}


  def is_point_inside_cylinder(self, x, y, z, radius, height):
    in_circle = x ** 2 + y ** 2 <= radius ** 2
    in_height_range = 0 <= z <= height
    return in_circle and in_height_range


def signal_handler(sig, frame):
  global running
  running = False
  close_event.set()


def check_close_event():
  if close_event.is_set():
    root.destroy()
  else:
    root.after(100, check_close_event)


def read_from_serial_port(ser):
  while running:
    read_data = ser.readline().decode().rstrip().replace("\r", "")
    if read_data: 
      print(f"Received: {read_data}")


def write_to_serial_port(ser):
  while running:
    ser.write("Hello from Raspberry Pi\n".encode())
    time.sleep(2)


def setup_gui():
  global root, canvas, gui_text
  root = tkinter.Tk()
  root.title('PresGo GUI')
  canvas = tkinter.Canvas(root, width=400, height=300)
  canvas.pack()
  gui_text = canvas.create_text(200, 200, text='Not in Cylinder', font=('Arial', 20))
  root.after(100, check_close_event)


def setup_mqtt():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument("-c", metavar="CONFIG_FILE", help="Configuration file path", default=DEFAULT_CONFIG)
  parser.add_argument("-m", metavar="HOST[:PORT]", help="MQTT broker connection parameters", default=DEFAULT_CONNECTION, type=mqtt_conn_type)
  args = parser.parse_args()

  v = Visualizer()
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
  #print("msg topic", msg.topic, "match", m, "msg payload", msg.payload.decode('utf-8'))

  if m is not None:
    entry = json.loads(msg.payload)
    #entry["tag_id"] = m.group("tag_id")
    #userdata.q_pos.put(entry)
    #print(entry)
    is_in_cylinder = userdata.is_point_inside_cylinder(entry["x"], entry["y"], entry["z"], 1, 10)
    gui_str = "tag in cylinder: {}".format(is_in_cylinder)
    canvas.itemconfig(gui_text, text=gui_str)
  else:
    return
    print("msg payload", msg.payload.decode('utf-8'))
    m = re.match(r"silabs/aoa/angle/(?P<loc_id>.+)/(?P<tag_id>.+)", msg.topic)
    if m is not None:
      entry = json.loads(msg.payload)
      print(entry)
      entry["loc_id"] = m.group("loc_id")
      entry["tag_id"] = m.group("tag_id")
      userdata.q_ang.put(entry)


def main():
  # setup signal handler for graceful shutdown
  signal.signal(signal.SIGINT, signal_handler)

  # setup gui and initialize global variables used by mqtt callbacks
  setup_gui()

  # mqtt operations on separate thread to not block main gui thread
  #thread_mqtt = threading.Thread(target=setup_mqtt, daemon=True)
  #thread_mqtt.start()

  # setup serial port for communicating with bluenrg board
  # /dev/ttyACM0 if plugged in before or without antenna array, else /dev/ttyACM1
  ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=10)

  # serial read on separate thread to not block main gui thread
  thread_ser_read = threading.Thread(target=read_from_serial_port, daemon=True, args=(ser,))
  thread_ser_read.start()

  # serial write on separate thread to not block main gui thread
  thread_ser_write = threading.Thread(target=write_to_serial_port, daemon=True, args=(ser,))
  thread_ser_write.start()

  # GUI blocking loop
  root.mainloop()


if __name__ == "__main__":
  main()
