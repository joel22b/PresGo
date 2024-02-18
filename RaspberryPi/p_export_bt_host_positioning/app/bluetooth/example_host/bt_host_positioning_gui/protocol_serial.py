#!/usr/bin/env python3

import serial
import threading
from uuid import UUID

import protocol_serial_messages as psm

class ProtocolSerial:
    running = True
    serialPort = None
    reqId = 0
    callbacks = []
    callbackAnnouncementDoor = None

    def __init__(self, callbackAnnouncementDoor):
        self.callbackAnnouncementDoor = callbackAnnouncementDoor
        # setup serial port for communicating with bluenrg board
        # /dev/ttyACM0 if plugged in before or without antenna array, else /dev/ttyACM1
        self.serialPort = serial.Serial(port='/dev/ttyACM1', baudrate=115200, timeout=10)

        # serial read on separate thread to not block main gui thread
        thread_ser_read = threading.Thread(target=self.serial_read_thread, daemon=True, args=())
        thread_ser_read.start()

        # serial write on separate thread to not block main gui thread
        #thread_ser_write = threading.Thread(target=self.write_to_serial_port, daemon=True, args=(ser,))
        #thread_ser_write.start()
    
    def serial_read_thread(self):
        while self.running:
            read_data = self.serialPort.readline()
            if read_data: 
                cmd = read_data.decode().rstrip().replace("\r", "")
                self.decode_message(cmd)
                print(f"Received: {cmd}")

    def serial_write(self, msg: psm.Message):
        #print("["+str(msg)+"]")
        self.serialPort.write(msg.__str__().encode())
    
    def decode_message(self, raw: str):
        msgIdentifierStr = raw[1:4]
        if psm.MsgIdentifier.Rsp.name == msgIdentifierStr:
            reqId = int(raw[5:7], 16)
            msgTypeStr = raw[8:12]
            if psm.MsgType.Fare.name == msgTypeStr:
                self.callbacks[reqId](UUID(raw[13:45]))
            elif psm.MsgType.Door.name == msgTypeStr:
                inDoorway = False
                if raw[13] == 'T':
                    inDoorway = True
                self.callbacks[reqId](inDoorway)
            elif psm.MsgType.Dist.name == msgTypeStr:
                self.callbacks[reqId](int(raw[13:]))
        elif psm.MsgIdentifier.Ann.name == msgIdentifierStr:
            msgTypeStr = raw[5:9]
            if psm.MsgType.Door.name == msgTypeStr:
                inDoorway = False
                if raw[10] == 'T':
                    inDoorway = True
                self.callbackAnnouncementDoor(inDoorway)
    
    def get_request_id(self) -> int:
        tmp = self.reqId
        self.reqId += 1
        if self.reqId == 100:
            self.reqId = 0
        return tmp


    def send_request_fare(self, address: str, callback):
        reqId = self.get_request_id()
        self.callbacks.insert(reqId, callback)
        msg = psm.RequestFare()
        msg.create(reqId, address)
        self.serial_write(msg)

    def send_request_dist(self, callback):
        reqId = self.get_request_id()
        self.callbacks.insert(reqId, callback)
        msg = psm.RequestDist()
        msg.create(reqId)
        self.serial_write(msg)

    def send_request_door(self, callback):
        reqId = self.get_request_id()
        self.callbacks.insert(reqId, callback)
        msg = psm.RequestDoor()
        msg.create(reqId)
        self.serial_write(msg)