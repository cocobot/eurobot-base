'use strict';

//package
var fs = require('fs');
var serialProtocol = require('./serialProtocol.js');
var TCPProtocol = require('./TCPProtocol.js');

//internal storage
var emit = function() {};
var lowLevel = null;
var recvBuffer = "";
var recvBufferLines = [];
var lastCommand = [];


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
  var obj = {};

  if(!async) {
    var request = null;

    var now = Date.now();
    while(lastCommand.length > 0) {
      var r = lastCommand.shift();
      if(r.date > now - 2000) {
        request = r;
        break;
      }
    }

    obj.request = request;
  }
  obj.answer = {async: async, data: data, date: Date.now()};

  emit("receive", obj);
}

var send = function(data) {
  if(lowLevel != null) {
    data.date = Date.now();
    lastCommand.push(data);
    lowLevel.send(data.command);
  }
};


exports.getState = getState;
exports.connect = connect;
exports.disconnect = disconnect;
exports.configure = configure
exports.send = send;
