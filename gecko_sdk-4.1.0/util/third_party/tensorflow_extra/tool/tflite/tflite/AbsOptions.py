# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class AbsOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsAbsOptions(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = AbsOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def AbsOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # AbsOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

def AbsOptionsStart(builder): builder.StartObject(0)
def AbsOptionsEnd(builder): return builder.EndObject()
