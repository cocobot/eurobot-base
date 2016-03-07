'use strict';
var serial = require('serialport').SerialPort;

function SerialProtocol(addr, handleReceiveData) {
  this.addr = addr;
  this.handleReceiveData = handleReceiveData;
  this.serial = null;
  this.try = 0;
  this.closed = false;

  this.connect();
};

SerialProtocol.prototype.connect = function() {
  if(!this.closed) {
    var self = this;

    this.try += 1;

    console.log("Try #" + this.try);
    this.serial = new serial(this.addr, {
      baudrate: 115200
    }, function(error) {
      if ( error ) {
        console.log('failed to open: ' + error);
        setTimeout(function() {self.connect()}, 1000);
      } else {
        console.log('open');
        self.try = 0;
        self.serial.on('data', function (buffer) { self.receiveData(buffer); });
      }
    });
  }
};

SerialProtocol.prototype.send = function(data) {
  console.log("SEND: " + data);
  this.serial.write(data + "\n", function() {});
};

SerialProtocol.prototype.receiveData = function(buffer) {
  if(this.handleReceiveData != null) {
    this.handleReceiveData(buffer);
  }
};

SerialProtocol.prototype.close = function() {
  if(this.serial != null) {
    this.serial.close(function() {});
    this.serial = null;
    this.closed = true;
  }
}

var create = function(addr, handleReceiveData) {
  return new SerialProtocol(addr, handleReceiveData);
};

exports.create = create;
