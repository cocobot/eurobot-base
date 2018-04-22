const net = require('net');
const electron = require('electron');
const peg = require("pegjs");
const BrowserWindow = electron.BrowserWindow;

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
 / Sreader

Breader
 = 'B' { return () => { const value = pkt.buffer.readUInt8(pkt.offset); pkt.offset += 1; return value; }};

Hreader
 = 'H' { return () => { const value = pkt.buffer.readUInt16LE(pkt.offset); pkt.offset += 2; return value; }};

Sreader
 = 'S' { return () => { 
   const len = pkt.buffer.readUInt16LE(pkt.offset); 
   pkt.offset += 2; 
   const svalue = pkt.buffer.toString('ascii', pkt.offset, pkt.offset + len); 
   pkt.offset += len;
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
  }

  _onClose() {
    console.log("Closed");
  }

  _onData(data) {
    console.log(data);
  }
}

class Client {
  constructor(protocol, socket) {
    this._protocol = protocol;
    this._socket = socket;
    this._init = false;
    this._buffer = Buffer.alloc(0);
    this._peripheral = null;
    this._header = null;
    this._id = CLIENT_ID;
    this._sync = true;
    CLIENT_ID += 1;

    this._socket.on('close', () => this._onClose());
    this._socket.on('data', (data) => this._onData(data));
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
            if(this._buffer.length >= 7) {
              const header = {
                start: this._buffer.readUInt8(0),
                pid: this._buffer.readUInt16LE(1),
                len: this._buffer.readUInt16LE(3),
                crc: this._buffer.readUInt16LE(5),
                buffer: Buffer.alloc(0),
              }
              let crc = 0xFFFF;
              for(let i = 0; i < 5; i += 1) {
                crc = this._crc_update(crc, this._buffer.readUInt8(i));
              }
              if(crc == header.crc) {
                this._buffer = this._buffer.slice(7);
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

  _onData(data) {
    this._buffer = Buffer.concat([this._buffer, data]);
    this._parseData();
  }

  _onClose() {
    console.log("Closed");
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
        pkt.decoded = {};
        pkt.offset = 0;
        run();
        
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
        data: data,
      });
    });
  }
}

class Protocol {
  constructor() {
    this._clients = {
    };

    this._generateASTs();
    this._createTCPServer();
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
    new Client(this, socket);
  }

  _newTCPBootloaderClient(socket) {
    new BootloaderClient(this, socket);
  }
}

module.exports = Protocol;