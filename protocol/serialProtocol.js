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
        serialPort.on('data', self.receiveData);
      }
    });
  }
};

SerialProtocol.prototype.receiveData = function(buffer) {
  this.handleReceiveData(buffer);
};

SerialProtocol.prototype.close = function() {
  if(this.serial != null) {
    this.serial.close(function() {});
    this.serial = null;
    this.closed = true;
  }
}

var create = function(addr) {
  return new SerialProtocol(addr);
};

exports.create = create;
