import socket
import queue
import threading

DEBUG = False


class Protocol:
    def __init__(self):
        self.recv_queue = queue.Queue()
        self.broadcast_queue = queue.Queue()
        self.broadcast_map = dict()

    def register(self, name, func):
        if name in self.broadcast_map:
            self.broadcast_map[name].append(func)
        else:
            self.broadcast_map[name] = [func]
    

    def send(self, data, argument=None):
        if DEBUG:
            print(">>>: %s %s" % (data, str(argument)))
        self.recv_queue.put([data, argument])

    def broadcast(self, cmd, answer):
        self.broadcast_queue.put([cmd, answer])

    def process_async(self,data):
        splited = data.split("=")
        self.broadcast_queue.put([splited[0][1:], splited[1]])

    def process_broadcast(self):
        while not self.broadcast_queue.empty():
            evt = self.broadcast_queue.get()
            if DEBUG:
               print("<<< %s" % str(evt[1]))
            if evt[0] in self.broadcast_map:
                lst = self.broadcast_map[evt[0]]
                for l in lst:
                    l(evt)



class DummyProtocol(Protocol):
    def __init__(self):
        Protocol.__init__(self)

    def close(self):
        pass

    def send(self, data, argument = None):
        Protocol.send(self, data, argument)
        self.recv_queue.get()


class TCPProtocol(Protocol):
    def __init__(self):
        Protocol.__init__(self)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("127.0.0.1", 10000))
        self.socket.send("USART1".encode("utf-8"))
        self.thread = threading.Thread(target=self.worker)
        self.thread.daemon = True
        self.thread.start()

    def close(self):
        self.socket.close()

    def worker(self):
        while True:
            try:
                packet = self.recv_queue.get(True, 0.1)
                cmd = packet[0]
                arg = packet[1]
                if arg is not None:
                    self.socket.send((cmd + ' ' + str(arg) + '\n').encode("utf-8"))
                else:
                    self.socket.send((cmd + '\n').encode("utf-8"))
                self.socket.settimeout(2)
                resp = []
                buf = ''
                while True:
                    try:
                        data = str(chr(self.socket.recv(1)[0]))
                        if(data == "\n"):
                            if data[0] == '#':
                                self.process_async(buf)
                            else:
                                resp.append(buf)
                            buf = ''
                        else:
                            buf += data
                        if buf == "> ":
                            self.broadcast(cmd, resp)
                            break
                    except socket.timeout:
                        break
            except queue.Empty:
                pass

            self.socket.settimeout(0.1)
            buf = ''
            while True:
                try:
                    data = str(chr(self.socket.recv(1)[0]))
                    if(data == "\n"):
                        self.process_async(buf)
                        buf = ''
                        if not self.recv_queue.empty():
                            break
                    else:
                        buf += data
                except socket.timeout:
                    if len(buf) == 0:
                        break


