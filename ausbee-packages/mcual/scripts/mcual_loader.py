#!/bin/python2

# import serial
import random
import time
import sys
import binascii
import zlib
import socket

class HexFile():
    def __init__(self, path, pagesize):
        self.path = path
        self.pagesize = pagesize
        self.pages = dict()
        self.crc = dict()
        self.parse()
        for pid in self.pages:
            self.computeCRC(pid)

    def crc32(self, crc, value):
        polynom = 0x04C11DB7
        crc = crc ^ value
        for i in range(0, 32):
            if crc & 0x80000000:
                crc = (crc << 1) ^ polynom
            else:
                crc = (crc << 1)

        return crc & 0xFFFFFFFF


    def computeCRC(self, pid):
        buf = ''
        data = self.pages[pid]
        crc = 0xffffffff
        for i in range(0, len(data), 4):
            value = data[i] & 0xFF 
            value |= (data[i + 1] << 8) & 0xFF00
            value |= (data[i + 2] << 16) & 0xFF0000
            value |= (data[i + 3] << 24) & 0xFF000000
            buf += '%c%c%c%c' % (data[i], data[i + 1], data[i + 2], data[i + 3])
            crc = self.crc32(crc, value)
        self.crc[pid] = crc


    def setData(self, addr, data):
        pid = addr / self.pagesize
        if pid in self.pages:
            page = self.pages[pid]
            page[addr % self.pagesize] = data
        else:
            page = [0xff] * self.pagesize
            page[addr % self.pagesize] = data
            self.pages[pid] = page


    def parse(self):
        offset = 0

        with open(self.path) as f:
            while True:
                line = f.readline()
                if not line:
                    break
                if line[0] != ':':
                    print("Corrupted hex file (1)")
                    break
                line = line[1:-2]
                byte = []
                for i in range(0, len(line), 2):
                    hexdata = int(line[i:i+2], 16)
                    byte.append(hexdata)

                if byte[3] == 0x01:
                    break
                elif byte[3] == 0x04:
                    offset = byte[4] << 24 | byte[5] << 16
                elif byte[3] == 0x00:
                    addr =  byte[1] << 8 | byte[2]
                    for i in range(4, len(byte) - 1):
                        self.setData(offset + addr, byte[i])
                        addr += 1
                elif byte[3] == 0x05:
                    pass
                else:
                    print("Corrupted hex file (1)")
                    print(line)
                    break
                    

class Loader():
    def __init__(self, port, comid, hexf):
        if type(port) is socket._socketobject:
            self.socket = port
            self.socket.settimeout(0.200)
            self.socket.sendall("BT<<")
        else:
            self.socket = None
            self.serial = serial.Serial(port, '115200', timeout=0.5)
        self.hexf = hexf
        self.sync = random.randint(1, 99)
        self.synchronized = False
        self.debug = False
        self.comid = int(comid)

    def send(self, data):
        if self.debug:
            print(" >> %s" % data)
        if self.socket == None:
            self.serial.write(data + '\n')
        else:
            self.socket.sendall(data + '\r\n')

    def recv(self):
        data = ''
        if self.socket == None:
            data = self.serial.readline()
        else:
            while True:
                try:
                    char = self.socket.recv(1)
                    data += char
                    if char == '\n':
                        break
                    if len(data) > 128:
                        data = ''
                        break
                except socket.error, e:
                    data = ''
                    break
        data = data[:-1]
        if self.debug:
            print(" << %s" % data)
        return data


    def erase(self):
        sys.stdout.write("Erasing")
        sys.stdout.flush()
        self.send("ERASE")
        while(True):
            line = self.recv()
            if line == "ERASE OK":
                print(". Done")
                break
            else:
                sys.stdout.write(".")
                sys.stdout.flush()


    def synchronize(self):
        sys.stdout.write("Trying to sychronize")
        sys.stdout.flush()
        while(True):
            self.send("RESET:" + str(self.comid))
            line = self.recv()
            if line == "BOOTLOADER %d OK" % self.comid:
                print(".")
                print("Synchronization OK")
                self.synchronized = True
                break
            else:
                sys.stdout.write(".")
                sys.stdout.flush()
                self.recv()
                self.synchronized = False

    def verify(self):
        pids = self.hexf.pages.keys()
        pids.sort()
        for pid in pids:
            self.send("CRC:%d" % pid)
            line = None
            while True:
                line = self.recv().split(' ')
                if line[0] == 'CRC' and int(line[1], 16) == pid:
                    break
                else:
                    self.send("CRC:%d" % pid)
            crc = int(line[2], 16) & 0xFFFFFFFF
            if crc != self.hexf.crc[pid]:
                print("CRC pid %d mismatch(0x%X, 0x%X)" % (pid, crc, self.hexf.crc[pid]))
                print("Flash needed")
                return False
        return True


    def program(self):
        pids = self.hexf.pages.keys()
        pids.sort()
        for pid in pids:
            data = "FLASH:%d:" % pid
            first = True
            for byte in self.hexf.pages[pid]:
                if not first:
                    data += "/"
                first = False
                data +=  hex(byte)
            self.send(data)

            while(True):
                line = self.recv()
                if line == "FLASH OK":
                    print(". Done")
                    break
                else:
                    sys.stdout.write(".")
                    sys.stdout.flush()



    def run(self):
        loaded = False
        while not loaded:
            self.synchronize()
            loaded = self.verify()      
            if not loaded:
                self.erase()
                self.program()

        
        self.send("BOOT")
        print("Loaded !")


def bootloader_cocoui():
    print("*** Trying to bootload with CocoUI ***")
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('127.0.0.1', 10000))
        s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        hexf = HexFile(sys.argv[2], 16 * 1024)
        loader = Loader(s, sys.argv[1], hexf)
        loader.run()
        return True
    except socket.error, e:
        print(" - Unable to connect to CocoUI")
        print(e)
        return False


#def bootoader_default_implementation():
#    print("*** Use default python implementation as failback ***") 
#    hexf = HexFile(sys.argv[2], 16 * 1024)
#    loader = Loader(sys.argv[1], hexf)
#    loader.run()
#

if len(sys.argv) < 3:
    print("Usage: %s board_id hexfile" % sys.argv[0])
else:
    if not bootloader_cocoui():
        #bootoader_default_implementation()
        pass
