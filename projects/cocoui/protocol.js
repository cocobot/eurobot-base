const net = require('net');
const electron = require('electron');
const BrowserWindow = electron.BrowserWindow;

let CLIENT_ID = 0;
const MAGIC_START = 0xc0;
const DECODERS = {};
const AST = {};
DECODERS[0x8000] = "{position}F(x)F(y)F(angle)"
DECODERS[0x8001] = "{asserv_dist}F(target)F(distance)F(ramp_out)F(speed_target)F(speed)F(pid_out)F(pid_P)F(pid_I)F(pid_d)"
DECODERS[0x8002] = "{asserv_angle}F(target)F(angle)F(ramp_out)F(speed_target)F(speed)F(pid_out)F(pid_P)F(pid_I)F(pid_d)"
DECODERS[0x8003] = "{trajecotry}[B(type)]"

class Client {
  constructor(protocol, socket) {
    this._protocol = protocol;
    this._socket = socket;
    this._init = false;
    this._buffer = Buffer.alloc(0);
    this._peripheral = null;
    this._header = null;
    this._id = CLIENT_ID;
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
            console.log("Resync...");
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
    const decoder = AST[pkt.pid];
    if(decoder == null) {
      console.log("Unable to decode:");
      console.log(pkt);
    }
    else {
      try {
        pkt.decoded = {};
        pkt.offset = 0;
        for(let i = 0; i < decoder.length; i += 1) {
          decoder[i](pkt);
        }
        this._emit(pkt.decoded);
      }
      catch(e) {
        console.log("Unable to decode packet");
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

  _decode(fmt) {
    let actions = [];
    const reName = /\{(.*)\}(.*)/;

    let match = fmt.match(reName);
    if(match != null) {
      actions.push((pkt) => {
        pkt.decoded._name = match[1];
      });
      actions = actions.concat(this._decode(match[2]));
    }
    else {
      const reGroups = /(F\(\w*\))/g;
      let match = fmt.match(reGroups);
      if(match.length == 1) {
        const reGroup = /(F)\((\w*)\)/;
        let match = fmt.match(reGroup);
        actions.push((pkt) => {
          let value = null;
          switch(match[1]) {
            case 'F':
              value = pkt.buffer.readFloatLE(pkt.offset);
              pkt.offset += 4;
              break;

            default:
              console.log("Unable to read : " + match[1]);
              break;
          }
          pkt.decoded[match[2]] = value;
        });
      }
      else {
        for(let i = 0; i < match.length; i += 1) {
          actions = actions.concat(this._decode(match[i]));
        }
      }
    }

    return actions;
  }

  _generateASTs() {
    console.log(DECODERS);
    for(let pid in DECODERS) {
      if(!DECODERS.hasOwnProperty(pid)) {
        continue;
      }
      AST[pid] = this._decode(DECODERS[pid]);
    }
    console.log(AST);
  }

  _createTCPServer() {
    this._server = net.createServer((socket) => this._newTCPClient(socket));
    this._server.listen(10000, '127.0.0.1');
  }

  _newTCPClient(socket) {
    new Client(this, socket);
  }
}

module.exports = Protocol;
