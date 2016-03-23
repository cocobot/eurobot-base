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
      if(recvBuffer[0] == "#") {
        recvBuffer = recvBuffer.substring(1);
        handleReceivedData([recvBuffer], true);
      }
      else {
        recvBufferLines.push(recvBuffer);
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

var send = function(data) {
  var now = Date.now();
  if(now - cooltime > DEFAULT_COOL_TIME) {
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
};

var detectOutOfSync = function() {
  var rnd = Math.floor(Math.random() * 1000);
  send({command: 'fake_cmd_' + rnd, fake_cmd_value: rnd});
};

setInterval(detectOutOfSync, 1000);

exports.getState = getState;
exports.connect = connect;
exports.disconnect = disconnect;
exports.configure = configure
exports.send = send;
