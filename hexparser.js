'use strict';

var crc32 = function(crc, value) {
  var polynom = 0x04C11DB7;
  crc = crc ^ value;
  for(var i = 0; i < 32; i += 1) {
    if(crc & 0x80000000) {
      crc = (crc << 1) ^ polynom;
    }
    else {
      crc = (crc << 1);
    }
  }

  return crc & 0xFFFFFFFF;
}

var computeCRC = function(state, pid)  {
  var data = state.pages[pid];
  var crc = 0xffffffff
  for(var i = 0; i < data.length; i += 4) {
    var value = data[i] & 0xFF;
    value |= (data[i + 1] << 8) & 0xFF00;
    value |= (data[i + 2] << 16) & 0xFF0000;
    value |= (data[i + 3] << 24) & 0xFF000000;
    crc = crc32(crc, value);
  }
  state.crc[pid] = crc;
}

var setData = function(state, addr, data) {
  var pid = parseInt(addr / state.pagesize);
  if(pid in state.pages) {
    var page = state.pages[pid];
    page[addr % state.pagesize] = data;
  }
  else {
    var page = [];
    for(var i = 0; i < state.pagesize; i += 1) {
      page.push(0xff);
    }
    page[addr % state.pagesize] = data;
    state.pages[pid] = page;
  }
}

var parseLine = function(state, line) {
  if(line[0] != ':') {
    throw "Corrupted hex file (1)";
  }

  var bytes = [];
  for(var i = 1; i < line.length; i += 2) {
    var hexdata = parseInt(line[i] + "" + line[i + 1], 16);
    bytes.push(hexdata);
  }

  if(bytes[3] == 0x01) {
    return;
  }
  else if(bytes[3] == 0x04) {
    state.offset = bytes[4] << 24 | bytes[5] << 16;
  }
  else if(bytes[3] == 0x00) {
    state.addr =  bytes[1] << 8 | bytes[2];
    for(var i = 4; i < bytes.length - 1; i += 1) {
      setData(state, state.offset + state.addr, bytes[i]);
      state.addr += 1;
    }
  }
  else if(bytes[3] == 0x05) {
    return;
  }
  else {
    throw "Corrupted hex file (2)";
  }
};

var parse = function(file, callback) {
  var buf = file.buffer;

  var state = {
    pages: {},
    crc: {},
    addr: 0,
    offset: 0,
    pagesize: 16 * 1024,
  };

  var line = '';
  for(var i = 0; i < buf.length; i += 1) {
    var c = String.fromCharCode(buf[i]);
    if(c == '\r') {
      //
    }
    else if(c == '\n') {
      try {
        parseLine(state, line);
      }
      catch(e) {
        callback(false, e);
        return;
      }
      line = '';
    }
    else {
      line += c;
    }
  }

  var pids = Object.keys(state.pages);
  for(var i = 0; i < pids.length; i += 1) {
    computeCRC(state, pids[i]);
  }

  callback(true, null, state);
};

exports.parse = parse;
