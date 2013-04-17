#!/usr/bin/python
#
# Write BLogic EEPROM
# (c) 2013 kongo <kongo@z80.se>

import usb
import struct
from binascii import unhexlify

class IHex:

    def __init__(self):
        self.data = []

    def load(self, filename):
        f = file(filename)

        while True:
            l = f.readline().strip()
            if len(l) < 1:
                break

            if l[0] != ':':
                raise Exception('Unexpected start of line, "%c"' % l[0])

            (length, address, type) = struct.unpack('>BHB', unhexlify(l[1:9]))
            data = unhexlify(l[9:9+length*2])

            self.data.append({'type': type, 'addr': address, 'data': data})
 
class EZUSB:

    def __init__(self, vid, pid):
        self.device = self.findDevice(vid, pid).open()
 
    def findDevice(self, vid, pid):
        for bus in usb.busses():
            for device in bus.devices:
                if (device.idVendor == vid) and (device.idProduct == pid):
                    return device
        raise Exception, 'No device found'
 
    def writeRAM(self, addr, buffer):
        self.device.controlMsg(0x40, 0xa0, buffer, addr, 0x00, 1000)
 
    def reset(self, halt):
        if halt == True:
            self.writeRAM(0xe600, [1])
        else:
            self.writeRAM(0xe600, [0])
 
    def downloadFW(self, filename):
        ihx = IHex()
        ihx.load(filename)

        self.reset(1)
        for m in ihx.data:
            if m['type'] == 0:
                self.writeRAM(m['addr'], m['data'])
        self.reset(0)

if __name__ == '__main__':
    print 'Connecting to blogic...'
    
    e = EZUSB(0x04b4, 0x8613)
    
    print 'Okay, downloading firmware...'

    e.downloadFW('blogic.hex')

    print 'Done.'
