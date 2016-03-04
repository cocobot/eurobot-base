import sys
import Queue as queue
import socket
import threading
import serial
import time

class Protocol:
    def __init__(self):
        self.thread = threading.Thread(target=self.worker)
        self.thread.daemon = True
        self.connected = False
        self.received_event = lambda x: x

    def run(self):
        self.thread.start()

    def worker(self):
        print("Start")
        recv_buffer = ''
        while True:
            byte = self.read()
            for b in byte:
                if b == '\n':
                    print("<< " + recv_buffer)

                    sync = True
                    if recv_buffer[0] == '#':
                        sync = False
                        recv_buffer = recv_buffer[1:]
                    splited = recv_buffer.split('=')
                    data = {'sync': sync, 'cmd': recv_buffer[0], 'data': recv_buffer[1]}
                    self.received_event(data)

                    recv_buffer = ''
                else:
                    recv_buffer += b



class SerialProtocol(Protocol):
    def __init__(self, port):
        Protocol.__init__(self)
        self.port = port
        self.serial = None

    
    def read(self):
        while True:
            if not self.connected:
                self.connect()
            if self.connected:
                try:
                    byte = self.serial.read(1000)
                    if(len(byte) > 0):
                        return byte
                except serial.SerialException as e:
                    self.connected = False

    
    def connect(self):
        try:
            if self.serial is not None:
                self.serial.close()
        except serial.SerialException:
            pass
        try:
            self.serial = serial.Serial(self.port, 115200)
            self.serial.timeout = 0.05
            self.connected = True
            print("Connected")
        except serial.SerialException:
            time.sleep(1.5)

#DEBUG = False
#
#
#class Protocol:
#    def __init__(self):
#        self.recv_queue = queue.Queue()
#        self.broadcast_queue = queue.Queue()
#        self.broadcast_map = dict()
#
#    def register(self, name, func):
#        if name in self.broadcast_map:
#            self.broadcast_map[name].append(func)
#        else:
#            self.broadcast_map[name] = [func]
#    
#
#    def send(self, data, argument=None):
#        if DEBUG:
#            print(">>>: %s %s" % (data, str(argument)))
#        self.recv_queue.put([data, argument])
#
#    def broadcast(self, cmd, answer):
#        self.broadcast_queue.put([cmd, answer])
#
#    def process_async(self,data):
#        splited = data.split("=")
#        self.broadcast_queue.put([splited[0][1:], splited[1]])
#
#    def process_broadcast(self):
#        while not self.broadcast_queue.empty():
#            evt = self.broadcast_queue.get()
#            if DEBUG:
#               print("<<< %s" % str(evt[1]))
#            if evt[0] in self.broadcast_map:
#                lst = self.broadcast_map[evt[0]]
#                for l in lst:
#                    l(evt)
#
#
#
#class DummyProtocol(Protocol):
#    def __init__(self):
#        Protocol.__init__(self)
#
#    def close(self):
#        pass
#
#    def send(self, data, argument = None):
#        Protocol.send(self, data, argument)
#        self.recv_queue.get()
#
#
#class TCPProtocol(Protocol):
#    def __init__(self):
#        Protocol.__init__(self)
#        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#        self.socket.connect(("127.0.0.1", 10000))
#        self.socket.send("USART1".encode("utf-8"))
#        self.thread = threading.Thread(target=self.worker)
#        self.thread.daemon = True
#        self.thread.start()
#
#    def close(self):
#        self.socket.close()
#
#    def worker(self):
#        while True:
#            try:
#                packet = self.recv_queue.get(True, 0.1)
#                cmd = packet[0]
#                arg = packet[1]
#                if arg is not None:
#                    self.socket.send((cmd + ' ' + str(arg) + '\n').encode("utf-8"))
#                else:
#                    self.socket.send((cmd + '\n').encode("utf-8"))
#                self.socket.settimeout(2)
#                resp = []
#                buf = ''
#                while True:
#                    try:
#                        if sys.version_info[0] < 3:
#                            data = self.socket.recv(1)[0]
#                        else:
#                            data = str(chr(self.socket.recv(1)[0]))
#                        if(data == "\n"):
#                            if data[0] == '#':
#                                self.process_async(buf)
#                            else:
#                                resp.append(buf)
#                            buf = ''
#                        else:
#                            buf += data
#                        if buf == "> ":
#                            self.broadcast(cmd, resp)
#                            break
#                    except socket.timeout:
#                        break
#            except queue.Empty:
#                pass
#
#            self.socket.settimeout(0.1)
#            buf = ''
#            while True:
#                try:
#                    if sys.version_info[0] < 3:
#                        data = self.socket.recv(1)[0]
#                    else:
#                        data = str(chr(self.socket.recv(1)[0]))
#
#                    if(data == "\n"):
#                        self.process_async(buf)
#                        buf = ''
#                        if not self.recv_queue.empty():
#                            break
#                    else:
#                        buf += data
#                except socket.timeout:
#                    if len(buf) == 0:
#                        break
#
#
