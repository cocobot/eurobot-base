'use strict';

var express = require('express');
var http = require('http');

var app = express()
app.use(express.static('public'));

app.get('/', function(req, res){
  res.redirect('/index.html');
});

var io = require('socket.io').listen(app.listen(3000));

io.sockets.on('connection', function (socket) {
      console.log('Un client est connecté !');
});

io.sockets.on('connection', function (socket) {
  console.log("loutre");
  socket.emit('message', { message: 'welcome to the chat' });
  socket.on('send', function (data) {
    io.sockets.emit('message', data);
  });
});

var protocol = require('./serial_protocol');
protocol.init(function(data) {
  io.sockets.emit('receive', data);
});


