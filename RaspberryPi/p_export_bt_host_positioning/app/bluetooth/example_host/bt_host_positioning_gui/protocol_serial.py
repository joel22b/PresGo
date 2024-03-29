#!/usr/bin/env python3

import protocol_serial_messages as psm
import serial
import threading
from tkinter_gui import SystemStatus
from uuid import UUID

class ProtocolSerial:
    running = True
    debug = False
    serialPort = None
    reqId = 1
    callbacks = []
    callbackAnnouncementDoor = None
    callbackAnnouncementInit = None
    callbackAnnouncementDisconnect = None
    gui = None

    def __init__(self, port: str, gui, callbackAnnouncementDoor, callbackAnnouncementInit, callbackAnnouncementDisconnect = None):
        self.callbackAnnouncementDoor = callbackAnnouncementDoor
        self.callbackAnnouncementInit = callbackAnnouncementInit
        self.callbackAnnouncementDisconnect = callbackAnnouncementDisconnect
        self.gui = gui
        portname = port.split('/')[-1]
        filename = "Logs/output_"+portname+".log"
        print("Opening log file: " + filename)
        self.outFile = open(filename, 'w')
        self.serialPort = serial.Serial(port=port, baudrate=115200, timeout=10)
        thread_ser_read = threading.Thread(target=self.serial_read_thread, daemon=True, args=())
        thread_ser_read.start()
        for _ in range(self.reqId):
            self.callbacks.append(None)

    def serial_read_thread(self):
        while self.running:
            try:
                read_data = self.serialPort.readline()
                if read_data:
                    cmd = read_data.decode().rstrip().replace("\r", "")
                    if self.debug:
                        print(f"Received: {cmd}")
                    self.outFile.write(f"Received: {cmd}\n")
                    self.outFile.flush()
                    self.decode_message(cmd)
            except Exception as e:
                print('Error in protocol_serial serial_read_thread:', str(e))
                if str(e).startswith("'utf-8' codec can't decode byte"):
                    continue
                if self.gui != None:
                    self.gui.set_system_status(SystemStatus.ERROR)

    def serial_write(self, msg: psm.Message):
        if self.debug:
            print("["+str(msg)+"]")
        self.outFile.write("["+str(msg)+"]\n")
        self.outFile.flush()
        self.serialPort.write(msg.__str__().encode())

    def decode_message(self, raw: str):
        msgIdentifierStr = raw[1:4]
        if psm.MsgIdentifier.Rsp.name == msgIdentifierStr:
            reqId = int(raw[5:7], 16)
            if reqId >= len(self.callbacks):
                print("ReqId out of range, ignoring: reqId="+str(reqId)+" callbacks len="+str(len(self.callbacks)))
                return
            msgTypeStr = raw[8:12]
            if psm.MsgType.Conn.name == msgTypeStr:
                state = int(raw[13:21], 16)
                self.callbacks[reqId](state)
            elif psm.MsgType.Fare.name == msgTypeStr:
                uuid = UUID(raw[13:45])
                self.callbacks[reqId](uuid)
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
            elif psm.MsgType.Init.name == msgTypeStr:
                flags = int(raw[10:12], 16)
                self.outFile.write(init_str(flags))
                self.outFile.flush()
                self.callbackAnnouncementInit(flags)
            elif psm.MsgType.Disc.name == msgTypeStr:
                self.callbackAnnouncementDisconnect(raw[10:22])

    def get_request_id(self) -> int:
        tmp = self.reqId
        self.reqId += 1
        if self.reqId == 100:
            self.reqId = 0
        return tmp

    def send_request_connect(self, address: str, callback):
        reqId = self.get_request_id()
        self.callbacks.insert(reqId, callback)
        msg = psm.RequestConnect()
        msg.create(reqId, address)
        self.serial_write(msg)

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

    def send_announcement_kill(self):
        msg = psm.AnnouncementKill()
        msg.create()
        self.serial_write(msg)

    def check_and_set_error_status(self, flags:int):
        for flag in psm.InitFlags:
            if not (flag.value&flags):
                print("Error in ", flag.name)
                if self.gui != None:
                    self.gui.set_system_status(SystemStatus.ERROR)

def init_str(flags: int) -> str:
    s = "Initialization complete:\n"
    for flag in psm.InitFlags:
        s += "\t" + flag.name.replace("_", " ") + ": " + str(bool(flag.value & flags)) + "\n"
    return s

def init_printout(flags: int):
    print(init_str(flags))
