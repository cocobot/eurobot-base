'use strict';

//package
var fs = require('fs');
var serialProtocol = require('./serialProtocol.js');

//internal storage
var emit = function() {};
var lowLevel = null;
var recvBuffer = "";


var configure = function(emit_func) {
  emit = emit_func;
}

var getAvailable = function() {
  var res = [];

  var files = fs.readdirSync("/dev");
  for(var i in files) {
    var name = files[i];
    if(!name.indexOf("ttyS")) {
      res.push({type: 'Serial', addr: '/dev/' + name});
    }
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
}

var disconnect = function() {
  if(lowLevel != null) {
    lowLevel.close();
    lowLevel = null;
  }
}

var receiveData = function(buffer) {
  for(var i = 0, _l = data.length; i < _l; i += 1) {
    var d = String.fromCharCode(data[i]);
    if(d == '\n') {
      handleReceivedData(recvBuffer);
      recvBuffer = "";
    }
    else {
      recvBuffer += d;
    }
  }
}

var handleReceivedData = function(data) {
  var obj = {}
  if(data[0] == "#") {
    obj.async = true;
    data = data.slice(1);
  }
  else {
    obj.async = false;
  }

  var splited = data.split('=');
  obj.command = splited[0];
  obj.data = splited[1];
  obj.date = new Date();

  emit("received", obj);
}


exports.getState = getState;
exports.connect = connect;
exports.disconnect = disconnect;
exports.configure = configure
