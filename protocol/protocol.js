'use strict';

//package
var fs = require('fs');
var serialProtocol = require('./serialProtocol.js');
var TCPProtocol = require('./TCPProtocol.js');

//consts
var DEFAULT_COOL_TIME = 1000;

//internal storage
var emit = function() {};
var lowLevel = null;
var recvBuffer = "";
var recvBufferLines = [];
var lastCommand = [];
var cooltime = 0;
var bootloader = false;
var bootloader_state = {};

var configure = function(emit_func) {
  emit = emit_func;
}

var getAvailable = function() {
  var res = [];

  var files = fs.readdirSync("/dev");
  for(var i in files) {
    var name = files[i];
    //if(!name.indexOf("ttyS")) {
    //  res.push({type: 'Serial', addr: '/dev/' + name});
    //}
    if(!name.indexOf("ttyUSB")) {
      res.push({type: 'Serial', addr: '/dev/' + name});
    }
    if(!name.indexOf("ttyACM")) {
      res.push({type: 'Serial', addr: '/dev/' + name});
    }
  }

  res.push({type: 'TCP', addr: '127.0.0.1:10000'});
  res.push({type: 'TCP', addr: '127.0.0.1:10001'});

  return res;
};

var getState = function() {
  return {
    connected: lowLevel != null,
    available: getAvailable(),
    bootloader: bootloader,
  };
}

var connect = function(addr) {
  if(!addr.indexOf("/dev")) {
    lowLevel = serialProtocol.create(addr, receiveData);
  }
  else {
    lowLevel = TCPProtocol.create(addr, receiveData);
  }
}

var disconnect = function() {
  if(lowLevel != null) {
    lowLevel.close();
    lowLevel = null;
  }
}

var receiveData = function(buffer) {
  for(var i = 0, _l = buffer.length; i < _l; i += 1) {
    var d = String.fromCharCode(buffer[i]);
    if(d == '>') {
      handleReceivedData(recvBufferLines, false);
      recvBuffer = "";
      recvBufferLines = []
    }
    else if(d == '\n') {
      console.log("RECV: " + recvBuffer);
      if(bootloader) {
        handleReceivedData([recvBuffer], true);
        recvBufferLines = []
      }
      else {
        if(recvBuffer[0] == "#") {
          recvBuffer = recvBuffer.substring(1);
          handleReceivedData([recvBuffer], true);
        }
        else {
          recvBufferLines.push(recvBuffer);
        }
      }
      recvBuffer = "";
    }
    else {
      recvBuffer += d;
    }
  }
}

var handleReceivedData = function(data, async) {
  var now = Date.now();

  if(!bootloader) {
    if(now - cooltime > DEFAULT_COOL_TIME) {
      var obj = {};

      if(!async) {
        var request = null;

        while(lastCommand.length > 0) {
          var r = lastCommand.shift();
          if(r.date > now - 500) {
            request = r;
            break;
          }
        }

        obj.request = request;
      }
      obj.answer = {async: async, data: data, date: Date.now()};

      if((obj.request != undefined) && (obj.request.fake_cmd_value != undefined)) {
        if(obj.answer.data[0] != 'invalid command: \'' + obj.request.command +  '\'') {
          console.log("*************************** OUT of SYNC *********************");
          console.log(obj.answer.data[0]);
          console.log('invalid command \'' + obj.request.command +  '\'');
          console.log('---');
          cooltime = now;
        }
      }
      else {
        emit("receive", obj);
      }
    }
  }
  else {
    switch(bootloader_state.state) {
      case 'try_sync':
        if(data[0] == 'SYNC ' + bootloader_state.try_number + ' OK') {
          bootloader_state.state = 'check_crc';
          bootloader_state.pid_idx = 0;
          bootloader_state.try_number = 0;
          bootloader_state.checked_crc = [];
        }
        break;

      case 'check_crc':
        var answer = data[0].split(' ');
        if(answer[0] == 'CRC') {
          var crc = parseInt(answer[1], 16);
          crc = crc & 0xFFFFFFFF;
          bootloader_state.checked_crc.push(crc);
          bootloader_state.pid_idx += 1;
          bootloader_state.try_number = 0;
        }

      case 'erase':
        if(data[0] == "IN PROGRESS") {
          bootloader_state.try_number = 1;
          bootloader_state.pid_idx += 1;
        }
        else if(data[0] == "ERASE OK") {
          bootloader_state.state = 'flash';
          bootloader_state.pid_idx = 0;
          bootloader_state.try_number = 0;
        }
        break;

      case 'flash':
        if(data[0] == 'FLASH OK') {
          bootloader_state.pid_idx += 1;
          bootloader_state.try_number = 0;
        }
        break;
    }
  }
}

var send = function(data) {
  var now = Date.now();
  if(now - cooltime > DEFAULT_COOL_TIME) {
    if(!bootloader) {
      if(lowLevel != null) {
        data.date = Date.now();
        lastCommand.push(data);
        if(data.argument != undefined) {
          lowLevel.send(data.command + " " + data.argument);
        }
        else {
          lowLevel.send(data.command);
        }
      }
    }
  }
};

var programSend = function(data, buffer) {
  if(bootloader) {
    if(lowLevel != null) {
      lowLevel.send(data);
      if(buffer !== undefined) {
        lowLevel.sendRaw(buffer);
      }
    }
  }
};

var handleProgramState = function() {
  if(bootloader) {
    switch(bootloader_state.state) {
      case 'try_sync':
        bootloader_state.try_number += 1;
        bootloader_state.sendMsg("Trying to synchronize with MCUAL loader - #" + bootloader_state.try_number);
        if((bootloader_state.try_number % 10) == 0)
        {
          programSend("system_reboot");
        }
        programSend("SYNC " + bootloader_state.try_number);
        break;

      case 'check_crc':
        var crcs = Object.keys(bootloader_state.param.pages);

        if(bootloader_state.try_number == 0) {
          if(bootloader_state.pid_idx > 0) {
            var tc = bootloader_state.pid_idx - 1;
            if(bootloader_state.checked_crc[tc] != bootloader_state.param.crc[crcs[tc]]) {
              bootloader_state.state = 'erase';
              bootloader_state.try_number = 0;
              bootloader_state.pid_idx = 0;
              return;
            }
          }
          if(bootloader_state.pid_idx >= crcs.length) {
            bootloader_state.state = 'done';
            bootloader_state.sendMsg("Program already loaded. No need to flash");
            bootloader_state.sendMsg("Boot program");
            bootloader_state.end(true);
            programSend("BOOT");
            bootloader = false;
          }
          else {
            bootloader_state.sendMsg("Verifying page #" + bootloader_state.pid_idx);
            bootloader_state.try_number = 1;
            programSend("CRC " + crcs[bootloader_state.pid_idx]);
          }
        }
        else {
          bootloader_state.state = 'try_sync';
          bootloader_state.try_number = 0;
        }
        break;

      case 'erase':
        bootloader_state.sendMsg("Erasing flash (" + bootloader_state.pid_idx + "." + bootloader_state.try_number + ")");
        if(bootloader_state.try_number == 0) {
          bootloader_state.try_number = 1;
          programSend("ERASE");
        }
        else {
          if(bootloader_state.try_number > 10) {
            bootloader_state.state = 'try_sync';
            bootloader_state.try_number = 0;
          }
          bootloader_state.try_number += 1;
        }
        break;

      case 'flash':
        var pages = Object.keys(bootloader_state.param.pages);

        if(bootloader_state.try_number == 0) {
          if(bootloader_state.pid_idx >= pages.length) {
            bootloader_state.state = 'check_crc';
            bootloader_state.pid_idx = 0;
            bootloader_state.try_number = 0;
            bootloader_state.checked_crc = [];
          }
          else {
            bootloader_state.sendMsg("Flashing page #" + bootloader_state.pid_idx);
            bootloader_state.try_number = 1;
            programSend("FLASH " + pages[bootloader_state.pid_idx], bootloader_state.param.pages[pages[bootloader_state.pid_idx]]);
          }
        }
        else {
          if(bootloader_state.try_number > 10) {
            bootloader_state.state = 'try_sync';
            bootloader_state.try_number = 0;
          }
          bootloader_state.try_number += 1;
        }
        break;
    }
  }
};

var program = function(param, sendMsg, callback) {
  if(bootloader) {
    sendMsg("Bootloader already running");
    callback(false);
  }
  bootloader = true;
  bootloader_state = {state: 'try_sync', try_number: 0, sendMsg: sendMsg, end: callback, param:param};
  handleProgramState();
};

var detectOutOfSync = function() {
  var rnd = Math.floor(Math.random() * 1000);
  send({command: 'fake_cmd_' + rnd, fake_cmd_value: rnd});
};

setInterval(detectOutOfSync, 1000);
setInterval(handleProgramState, 250);

exports.getState = getState;
exports.connect = connect;
exports.disconnect = disconnect;
exports.configure = configure
exports.send = send;
exports.program = program;
