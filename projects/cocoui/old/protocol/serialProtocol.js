'use strict';
var serial = require('serialport').SerialPort;
var fs = require('fs');

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

    var addr = this.addr;
    if(addr == "serial") {
      var res = [];
      var files = fs.readdirSync("/dev");
      for(var i in files) {
        var name = files[i];
        if(!name.indexOf("ttyUSB")) {
          res.push('/dev/' + name);
        }
        if(!name.indexOf("ttyACM")) {
          res.push('/dev/' + name);
        }
      }

      if(res.length == 0)
      {
        console.log("No serial port available");
        setTimeout(function() {self.connect()}, 1000);
        return;
      }
      addr = res[this.try % res.length];
    }

    console.log("Try #" + this.try + " (" + addr + ")");
    this.serial = new serial(addr, {
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

    var self = this;
    this.serial.on('close', function(e) {
      self.serial = null;
      setTimeout(function() {self.connect()}, 1000);
    });
  }
};

SerialProtocol.prototype.send = function(data) {
  if(this.serial != null) {
    console.log("SEND: " + data);
    this.serial.write(data + "\n", function() {});
  }
};

SerialProtocol.prototype.sendRaw = function(data) {
  if(this.serial != null) {
    this.serial.write(data, function() {});
  }
};


SerialProtocol.prototype.receiveData = function(buffer) {
  if(this.handleReceiveData != null) {
    this.handleReceiveData(buffer);
  }
};

SerialProtocol.prototype.close = function() {
  if(this.serial != null) {
    this.closed = true;
    this.serial.close(function() {});
    this.serial = null;
  }
}

var create = function(addr, handleReceiveData) {
  return new SerialProtocol(addr, handleReceiveData);
};

exports.create = create;
