"use strict";
var serial = require('serialport').SerialPort;

var serialPort = null;
var recvBuffer = "";
var callback = null;

var init = function(cb) {
  callback = cb;

  var serialPort = new serial("/dev/ttyUSB0", {
    baudrate: 115200
  }, function(error) {
    if ( error ) {
      console.log('failed to open: ' + error);
    } else {
      console.log('open');
      serialPort.on('data', receiveData);
    }
  });
};

var receiveData = function(data) {
  console.log(data);
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
};

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

  callback(obj);
}

exports.init = init;
