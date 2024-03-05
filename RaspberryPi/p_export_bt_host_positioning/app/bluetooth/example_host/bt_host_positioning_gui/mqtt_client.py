import argparse
import fob_processing as fp
import json
import os
import paho.mqtt.client as mqtt
import re
import threading
import visualizer

DEFAULT_CONFIG = os.path.join(os.path.dirname(__file__), "../bt_host_positioning/config/positioning_config.json")
DEFAULT_CONNECTION = {"host": "localhost", "port": 1883}

class MQTTClient:
<<<<<<< HEAD
  def __init__(self, fob_processing):
    self.fob_processing = fob_processing
    # mqtt operations on separate thread to not block main thread
=======
  def __init__(self, fob_processing,gui):
    # self.running = True # TODO probably restart this on reset signal after you stop the gui
    self.fob_processing = fob_processing
    self.gui = gui
    # mqtt operations on separate thread to not block main gui thread
>>>>>>> d3ba859400eceb9dfd0758efe603042d4a62b597
    self.thread_mqtt = threading.Thread(target=self.setup_mqtt, daemon=True)
    self.thread_mqtt.start()

  def setup_mqtt(self):
<<<<<<< HEAD
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-c", metavar="CONFIG_FILE", help="Configuration file path", default=DEFAULT_CONFIG)
    parser.add_argument("-m", metavar="HOST[:PORT]", help="MQTT broker connection parameters", default=DEFAULT_CONNECTION, type=self.mqtt_conn_type)
    args = parser.parse_args()
    v = visualizer.Visualizer()
    v.parse_config(args.c)
    client = mqtt.Client(userdata=v)
    client.on_connect = self.on_connect
    client.on_message = self.on_message
    client.connect(host=args.m["host"], port=args.m["port"])
    client.loop_forever()
=======
    try:
      parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
      parser.add_argument("-c", metavar="CONFIG_FILE", help="Configuration file path", default=DEFAULT_CONFIG)
      parser.add_argument("-m", metavar="HOST[:PORT]", help="MQTT broker connection parameters", default=DEFAULT_CONNECTION, type=self.mqtt_conn_type)
      args = parser.parse_args()
      v = visualizer.Visualizer()
      v.parse_config(args.c)
      client = mqtt.Client(userdata=v)
      client.on_connect = self.on_connect
      client.on_message = self.on_message
      client.connect(host=args.m["host"], port=args.m["port"])
      client.loop_forever() # TODO replace with loop_start if can get it to work

    except Exception as e:
        self.gui.set_system_error_status()
>>>>>>> d3ba859400eceb9dfd0758efe603042d4a62b597

  def mqtt_conn_type(self, arg):
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

  def on_connect(self, client, userdata, flags, rc):
    ''' Called when a CONNACK response is received from the server. '''
    print("Connected with result code " + str(rc))
    topic = "silabs/aoa/position/{}/#".format(userdata.positioning_id)
    print("Subscribe for ", topic)
    client.subscribe(topic)
    for loc_id in userdata.locators:
      topic = "silabs/aoa/angle/{}/#".format(loc_id)
      print("Subscribe for ", topic)
      client.subscribe(topic)

  def on_message(self, client, userdata, msg):
    ''' Called when a PUBLISH message is received from the server. '''
    m = re.match(r"silabs/aoa/position/.+/(?P<tag_id>.+)", msg.topic)
    if m is None:
      return
    entry = json.loads(msg.payload)
    tag_id = m.group('tag_id').replace("ble-pd-", "")
    x, y, z = entry["x"], entry["y"], entry["z"]
    self.fob_processing.process_message(x, y, z, tag_id)
