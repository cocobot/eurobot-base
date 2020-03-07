const net = require('net');
const electron = require('electron');
const peg = require("pegjs");
const BrowserWindow = electron.BrowserWindow;
const ipcMain = electron.ipcMain;
const SerialPort = require('serialport');

const TCPCLIENT_DECODE_UART = true;
const TCPCLIENT_DECODE_CAN = false;
const COCOUI_ID = 0x11;

let CLIENT_ID = 0;
const MAGIC_START = 0xc0;
const DECODERS = {};
let AST = null;
DECODERS[0x8000] = "{position}F(x)F(y)F(angle)"
DECODERS[0x8001] = "{asserv_dist}F(target)F(distance)F(ramp_out)F(speed_target)F(speed)F(pid_out)F(pid_P)F(pid_I)F(pid_d)"
DECODERS[0x8002] = "{asserv_angle}F(target)F(angle)F(ramp_out)F(speed_target)F(speed)F(pid_out)F(pid_P)F(pid_I)F(pid_d)"
DECODERS[0x8003] = "{trajectory_orders}[B(type)F(time)F(a1)F(a2)F(a3)F(a4)F(start_x)F(start_y)F(start_angle)F(end_x)F(end_y)F(end_angle)F(estimated_distance_before_stop)](orders)"
DECODERS[0x8004] = "{pathfinder}H(length)H(width)[H(type)](nodes)"
DECODERS[0x8005] = "{printf}S(msg)"
DECODERS[0x8006] = "{game_state}B(robot_id)B(color)D(battery)D(time)D(score)"
DECODERS[0x8008] = "{action_scheduler}[S(name)F(x)F(y)F(score)](strategies)"
DECODERS[0x800a] = "{usirs}[H(us)H(ir)B(force_on)B(alert)B(alert_activated)](usir)"
DECODERS[0x800d] = "{asserv_params}F(d_ramp_speed)F(d_ramp_accel)F(d_pid_kp)F(d_pid_kd)F(d_pid_ki)F(d_pid_max_i)F(d_pid_max_e)F(a_ramp_speed)F(a_ramp_accel)F(a_pid_kp)F(a_pid_kd)F(a_pid_ki)F(a_pid_max_i)F(a_pid_max_e)";
DECODERS[0x800f] = "{ping}";

const GRAMMAR = `
{
    var pkt = this.__pkt;
}
start
  = obj:format { return () => { pkt.decoded = obj()} }

format
  = expr1:expr expr2:format { return () => { return Object.assign({}, expr1(), expr2());} }
  / expr1:expr { return () => { return expr1(); } }

expr
 = '{' id:identifier '}' { return () => {return {_name: id}; }}
 / '[' fmt:format ']' '(' id:identifier ')' { return () => {
   const len = pkt.buffer.readUInt16LE(pkt.offset);
   pkt.offset += 2;
   const ret = {};
   ret[id] = [];
   for(let i = 0; i < len; i += 1) {
     ret[id].push(fmt());
   }
   return ret;
 }}
 / value:reader '(' id:identifier ')'{ return () => { const obj = {}; obj[id] = value(); return obj }}

identifier
 = id:[a-zA-Z0-9_]+ { return id.join("") };

reader
 = Freader
 / Breader
 / Hreader
 / Dreader
 / Sreader

Breader
 = 'B' { return () => { const value = pkt.buffer.readUInt8(pkt.offset); pkt.offset += 1; return value; }};

Hreader
 = 'H' { return () => { const value = pkt.buffer.readUInt16LE(pkt.offset); pkt.offset += 2; return value; }};

Dreader
 = 'D' { return () => { const value = pkt.buffer.readUInt32LE(pkt.offset); pkt.offset += 4; return value; }};

Sreader
 = 'S' { return () => { 
   const len = pkt.buffer.readUInt16LE(pkt.offset); 
   pkt.offset += 2; 
   const svalue = pkt.buffer.toString('ascii', pkt.offset, pkt.offset + len); 
   pkt.offset += len + 1;
   return svalue;
}};

Freader
 = 'F' { return () => { const value = pkt.buffer.readFloatLE(pkt.offset); pkt.offset += 4; return value; }};

`;

class BootloaderClient {
  constructor(protocol, socket) {
    this._protocol = protocol;
    this._socket = socket;
    
    this._socket.on('close', () => this._onClose());
    this._socket.on('data', (data) => this._onData(data));

    this._protocol.setHijackCom(this);
  }

  recv(data) {
    this._socket.write(data);
  }

  _onClose() {
    this._protocol.setHijackCom(null);
  }

  _onData(data) {
    const asc = data.toString('ascii');
    console.log("ASC >" + asc);
    if(asc.includes('#RESET')) {
      this._protocol.formatAndSendtoAll({
        pid: 0x8007,
        fmt: "",
        args: [],
      });
    }
    this._protocol.sendToAll(data);
  }
}

class Client {
  constructor(protocol) {
    this._protocol = protocol;
    this._init = false;
    this._buffer = Buffer.alloc(0);
    this._peripheral = null;
    this._header = null;
    this._id = CLIENT_ID;
    this._sync = true;
    CLIENT_ID += 1;


    this._protocol._clients.push(this);
  }

  getID() {
    return this._id;
  }

  getName() {
    this._unimplemented();
  }

  _unimplemented() {
    console.warn("Unimplemented");
  }

  send() {
    this._unimplemented();
  }

  _crc_update(crc, a) {
    crc ^= a;

    for(let i = 0; i < 8; i += 1)
    {
      if (crc & 1)
      {
        crc = (crc >> 1) ^ 0xA001;
      }
      else
      {
        crc = (crc >> 1);
      }
    }

    return crc & 0xFFFF;
  }

  _parseData() {
    if(!this._init) {
      if(this._buffer.length > 11) {
        const b1 = this._buffer.slice(0, 10);
        this._peripheral = b1.toString('ascii');
        this._buffer = this._buffer.slice(11);
        this._init = true;
        console.log("Init done");
        setTimeout(() => this._parseData(), 0);
      }
    }
    else {
      if(this._header == null) {
        if(this._buffer.length > 0) {
          if(this._buffer.readUInt8(0) != MAGIC_START) {
            this._buffer = this._buffer.slice(1);
            if(this._sync) {
              console.log("Resync...");
              console.log(this._buffer);
            }
            this._sync = false;
            setTimeout(() => this._parseData(), 0);
          }
          else {
            if(this._buffer.length >= 8) {
              const header = {
                start: this._buffer.readUInt8(0),
                src: this._buffer.readUInt8(1),
                pid: this._buffer.readUInt16LE(2),
                len: this._buffer.readUInt16LE(4),
                crc: this._buffer.readUInt16LE(6),
                buffer: Buffer.alloc(0),
              }
              let crc = 0xFFFF;
              for(let i = 0; i < 6; i += 1) {
                crc = this._crc_update(crc, this._buffer.readUInt8(i));
              }
              if(crc == header.crc) {
                this._buffer = this._buffer.slice(8);
                this._header = header;
                setTimeout(() => this._parseData(), 0);
              }
              else {
                console.log("Bad CRC");
                this._buffer = this._buffer.slice(1);
                setTimeout(() => this._parseData(), 0);
              }
            }
          }
        }
      }
      else {
        if(this._header.buffer.length == (this._header.len + 2)) {
          if(!this._sync) {
            console.log("Synchronized !"); 
          }
          this._sync = true;
          this._parse(this._header);
          this._header = null;
          setTimeout(() => this._parseData(), 0);
        }
        else if(this._buffer.length > 0) {
          let pick = Math.min(this._header.len + 2 - this._header.buffer.length, this._buffer.length);
          this._header.buffer = Buffer.concat([this._header.buffer, this._buffer.slice(0, pick)]);
          this._buffer = this._buffer.slice(pick);
          setTimeout(() => this._parseData(), 0);
        }
      }
    }
  }

  _parse(pkt) {
    const decoder = DECODERS[pkt.pid];
    if(decoder == null) {
      console.log("Unable to decode:");
      console.log(pkt);
    }
    else {
      try {
        AST.__pkt = pkt;
        const run = AST.parse(decoder);
        pkt.decoded = {
        };
        pkt.offset = 0;
        run();

        //add meta data
        pkt.decoded._src = pkt.src;
        pkt.decoded._ith = pkt.ith;

        //decode source name
        switch(pkt.decoded._src) {
          case 1:
            pkt.decoded._src_name = "SMecanele";
            break;

          default:
            pkt.decoded._src_name = "?? (" + pkt.decoded._src + ")";
            break;
        }

        //printf in console for desktop without notification
        if(pkt.decoded._name == "printf") {
          console.log("["+pkt.decoded._src_name+"] " + pkt.decoded.msg);
        }
        
        //send to interface
        this._emit(pkt.decoded);
      }
      catch(e) {
        console.log("Unable to decode packet");
        console.log(decoder);
        console.log(e);
      }
    }
  }

  _emit(data) {
    BrowserWindow.getAllWindows().forEach((x) => {
      x.webContents.send('pkt', {
        client: this._id,
        clientName: this.getName(),
        data: data,
      });
    });
  }

  _onData(data) {
    if(this._protocol._hijackCom != null) {
      this._protocol._hijackCom.recv(data);
    }
    else {
      this._buffer = Buffer.concat([this._buffer, data]);
      this._parseData();
    }
  }

  formatAndSend(pkt) {
    const getLen = (fmt) => {
      let size = 0;
      for(let i = 0; i < fmt.length; i += 1) {
        switch(fmt[i]) {
           case 'B':
            size += 1;
            break;

          case 'D':
            size += 4;
            break;

          case 'F':
            size += 4;
            break;

          default:
            console.log("TODO !!! " + fmt[i]);
        }
      }

      return size;
    };

    pkt.data = Buffer.alloc(getLen(pkt.fmt));
    let offset = 0;

    for(let i = 0; i < pkt.fmt.length; i += 1) {
      switch(pkt.fmt[i]) {
         case 'B':
          pkt.data.writeUInt8(pkt.args.shift(), offset);
          offset += 1;
          break;

        case 'F':
          pkt.data.writeFloatLE(pkt.args.shift(), offset);
          offset += 4;
          break;

        case 'D':
          pkt.data.writeInt32LE(pkt.args.shift(), offset);
          offset += 4;
          break;



        default:
          console.log("TODO !!! " + pkt.fmt[i]);
      }
    }

    console.log(offset + "/" + pkt.data.length);
    console.log(pkt.data);
    this.sendPacket(pkt);
  }
}

class SerialClient extends Client {
  constructor(protocol, serial) {
    super(protocol);
    this._serial = serial;
    this._serial.on('data', (data) => this._onData(data));
  }

  getName() {
    return this._serial.path;
  }

  send(data) {
    this._serial.write(data);
  }

  sendPacket(pkt) {
    const headerBuffer = Buffer.alloc(8);
    headerBuffer.writeUInt8(0xC0, 0);
    headerBuffer.writeUInt8(COCOUI_ID, 1);
    headerBuffer.writeUInt16LE(pkt.pid, 2);
    headerBuffer.writeUInt16LE(pkt.data.length, 4);
    let crc = 0xFFFF;
    for(let i = 0; i < 6; i += 1) {
      crc = this._crc_update(crc, headerBuffer.readUInt8(i));
    }
    headerBuffer.writeUInt16LE(crc, 6);

    const crcDataBuffer = Buffer.alloc(2);
    for(let i = 0; i < pkt.data.length; i += 1) {
      crc = this._crc_update(crc, pkt.data.readUInt8(i));
    }
    crcDataBuffer.writeUInt16LE(crc, 0);

    this.send(headerBuffer);
    this.send(pkt.data);
    this.send(crcDataBuffer);
  }
}

class TCPClient extends Client {
  constructor(protocol, socket) {
    super(protocol);
    this._socket = socket;
    this._pid = null;
    this._queues = {};
    this._TCPbuffer = "";
    this._socket.on('close', () => this._onClose());
    this._socket.on('data', (data) => this._decodeTCPData(data));
  }

  getName() {
    const addr = this._socket.address();
    return addr.address + ":" + addr.port;
  }
  
  send(data) {
    try
    {
      this._socket.write(data);
    }
    catch(e) {
    }
  } 

  _decodeTCPData(data) {
    let asc = data.toString('ascii');
    if(this._pid == null) {
      //On first packet, we must decode the peripheral indentifier
      this._pid = asc.substring(0, 2);

      //Then we remove the first 4 octets "PID" followed by "<<" delimiter
      asc = asc.substring(4);
    }

    //we must add the data to the saved buffer
    this._TCPbuffer = this._TCPbuffer + asc;

    while(true) {
      //split the buffer using \n delimiter
      let idx = this._TCPbuffer.indexOf("\n");
      if(idx == -1) {
        break;
      }
      const pkt = this._TCPbuffer.substring(0, idx - 1);
      this._TCPbuffer = this._TCPbuffer.substring(idx + 1);

      //parse the incoming data
      switch(this._pid) {
        case "C0":
          //CAN packet found
          if(TCPCLIENT_DECODE_CAN) {
            this._decodeCAN(pkt);
          }
          break;

        case "U1":
          //UART packet found
          if(TCPCLIENT_DECODE_UART) {
            const buf = Buffer.alloc(1);
            buf.writeUInt8(parseInt(pkt, 16));
            this._onData(buf);
          }
          break;

        default:
          console.log("Unknown PID = " + this._pid);
          break;
      }
    }
  }

  _decodeCAN(pkt) {
    //transform raw string into parsed array of can data
    const can_data = pkt.split(":").map((x) => parseInt(x, 16));

    //extract source and packet_counter from can ID field
    const source = can_data[0] >> 7;
    const packet_counter = can_data[0] & 0x7F;

    let offset = 0;
    if(packet_counter == 0) {
      //first packet, we must decode the header from the 4 first octets
      const pid =  can_data[2] | (can_data[3] << 8);
      const len =  can_data[4] | (can_data[5] << 8);

      //create an object for the parsed data
      const obj_data = {
        src: source,
        pid: pid,
        len: len,
        buffer: Buffer.alloc(len + 2),
        recv_data: 0,
        ith: this._pid,
      }

      //put the data in the queue (erase previous data if not received properly)
      this._queues[source] = obj_data;

      //mark fisrt 4 octets as used
      offset = 4;
    }

    {
      //store recevied data into the allocated buffer
      const pkt = this._queues[source];
      if(pkt !== undefined) {
        for(offset; offset < can_data[1]; offset += 1, pkt.recv_data += 1) {
          //be sure to use packet_counter, we can receive data in any order
          pkt.buffer.writeUInt8(can_data[offset + 2], packet_counter * 8 + offset - 4);
        }
      }
    }

    //check if data is parsable
    for(let k in this._queues) {
      const pkt = this._queues[k];
      if(pkt.recv_data == (pkt.len + 2)) {
        //data received, time to check the CRC
        const pkt_crc = pkt.buffer.readUInt16LE(pkt.len);

        //compute crc from data
        let crc = 0xFFFF;
        for(let i = 0; i < pkt.len; i += 1) {
          crc = this._crc_update(crc, pkt.buffer.readUInt8(i));
        }

        //check crc
        if(crc == pkt_crc) {
          //Everything is ok. We can try to parse the packet
          this._parse(pkt);
        }
        else {
          console.log("CRC ERROR");
        }

        //Clear the queue
        this._queues[k] = undefined;
      }
    }
  }

  _onClose() {
    console.log("Closed");
  } 

  sendPacket(pkt) {
    switch(this._pid) {
      case "U1":
        this.sendUartPacket(pkt);
        break;

      default:
        console.log("ERROR SEND: " + this._pid);
        break;
    }
  }

  sendUartPacket(pkt) {
    const headerBuffer = Buffer.alloc(8);
    headerBuffer.writeUInt8(0xC0, 0);
    headerBuffer.writeUInt8(COCOUI_ID, 1);
    headerBuffer.writeUInt16LE(pkt.pid, 2);
    headerBuffer.writeUInt16LE(pkt.data.length, 4);
    let crc = 0xFFFF;
    for(let i = 0; i < 6; i += 1) {
      crc = this._crc_update(crc, headerBuffer.readUInt8(i));
    }
    headerBuffer.writeUInt16LE(crc, 6);

    const crcDataBuffer = Buffer.alloc(2);
    for(let i = 0; i < pkt.data.length; i += 1) {
      crc = this._crc_update(crc, pkt.data.readUInt8(i));
    }
    crcDataBuffer.writeUInt16LE(crc, 0);

    this.send(headerBuffer);
    this.send(pkt.data);
    this.send(crcDataBuffer);
  }
}

class Protocol {
  constructor() {
    this._clients = [];
    this._hijackCom = null;

    this._generateASTs();
    this._createTCPServer();
    this._checkSerial = setInterval(() => this._checkSerialPort(), 1000);

    ipcMain.on('pkt', (event, arg) => {
      this._clients.forEach((client) => {
        if(client.getID() == arg.client) {
          client.formatAndSend(arg);
        }
      });
    });
  }

  _checkSerialPort() {
    SerialPort.list().then((ports) => {
      for(let i = 0; i < ports.length; i += 1) {
        const name = ports[i].path;
        let found = false;
        for(let j = 0; (j < this._clients.length) && !found; j += 1) {
          if(this._clients[j].getName() == name) {
            found = true;
          }
        }
        if(!found) {
          const serial = new SerialPort(name, {
            baudRate: 115200,
            autoOpen: false
          });
          serial.open((err) => {
            if(err) {
            }
            else {
              this._newSerialClient(serial);
            }
          });
        }
      }
    }).catch((e) => {
      console.log(e);
    });
  }

  setHijackCom(obj) {
    this._hijackCom = obj;
  }

  _generateASTs() {
    AST = peg.generate(GRAMMAR);
  }

  _createTCPServer() {
    this._server = net.createServer((socket) => this._newTCPClient(socket));
    this._server.listen(10000, '127.0.0.1');

    this._bootloader = net.createServer((socket) => this._newTCPBootloaderClient(socket));
    this._bootloader.listen(10001, '127.0.0.1');
  }

  _newTCPClient(socket) {
    new TCPClient(this, socket);
  }

  _newSerialClient(serial) {
    new SerialClient(this, serial);
  }

  _newTCPBootloaderClient(socket) {
    new BootloaderClient(this, socket);
  }

  sendToAll(data) {
    for(let i = 0; i < this._clients.length; i += 1) {
      this._clients[i].send(data);
    }
  }

  formatAndSendtoAll(pkt) {
    this._clients.forEach((client) => {
      client.formatAndSend(pkt);
    });
  }
}

module.exports = Protocol;

