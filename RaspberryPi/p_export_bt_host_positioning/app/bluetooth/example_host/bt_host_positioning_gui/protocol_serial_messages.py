from enum import Enum
from uuid import UUID

class MsgIdentifier(Enum):
    Unknown = 0
    Req = 1
    Rsp = 2
    Ann = 3

class MsgType(Enum):
    Unknown = 0
    Fare = 1
    Dist = 2
    Door = 3
    Init = 4
    Conn = 5
    Disc = 6

class InitFlags(Enum):
    Distance_Sensor = 1
    Bluetooth_Controller = 2

class Message:
    complete = False
    raw = ""

    def create(self):
        self.raw = "\t"

    def finish(self):
        self.raw += "\n"
        self.complete = True
     
    def __str__(self) -> str:
        return self.raw

class Request(Message):
    reqId = 255
    msgType = MsgType.Unknown

    def create(self, reqId: int, msgType: MsgType):
        super().create()
        self.reqId = reqId
        self.msgType = msgType
        self.raw += f"Req:{reqId:02}:" + self.msgType.name 

class Response(Message):
    reqId = 255
    msgType = MsgType.Unknown
    
    def create(self, reqId: int, msgType: MsgType):
        super().create()
        self.reqId = reqId
        self.msgType = msgType
        self.raw += f"Rsp:{reqId:02}:" + self.msgType.name

class Announcement(Message):
    msgType = MsgType.Unknown

    def create(self, msgType: MsgType):
        super().create()
        self.msgType = msgType
        self.raw += f"Ann:" + self.msgType.name

class RequestConnect(Request):
    address = ""

    def create(self, reqId: int, address: str):
        super().create(reqId, MsgType.Conn)
        self.address = address
        self.raw += ":" + address
        self.finish()

class RequestFare(Request):
    address = ""

    def create(self, reqId: int, address: str):
        super().create(reqId, MsgType.Fare)
        self.address = address
        self.raw += ":" + address
        self.finish()

class RequestDist(Request):
    def create(self, reqId: int):
        super().create(reqId, MsgType.Dist)
        self.finish()

class RequestDoor(Request):
    def create(self, reqId: int):
        super().create(reqId, MsgType.Door)
        self.finish()

class ResponseConnect(Response):
    state: int = 0

class ResponseFare(Response):
    uuid: UUID = None

class ResponseDist(Response):
    distance: int = 0

class ResponseDoor(Response):
    inDoorway: bool = False

class AnnouncementDoor(Announcement):
    inDoorway: bool = False