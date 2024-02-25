import json
import queue
import random
import re

MAX_NUM_TAGS = 300

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