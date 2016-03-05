'use strict';

//pacakges
var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');
var argv = require('optimist').argv;
var protocol = require('./protocol/protocol.js');

//create express app
var app = express()
app.use(express.static('public'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

//generate routes
app.get('/', function(req, res){
  res.redirect('/index.html');
});
app.get('/api/protocol', function(req, res) {
  res.json(protocol.getState());
});
app.post('/api/protocol', function(req, res) {
  protocol.connect(req.body.addr);
  res.json(protocol.getState());
});
app.delete('/api/protocol', function(req, res) {
  protocol.disconnect();
  res.json(protocol.getState());
});

//start serveur
var io = require('socket.io').listen(app.listen(argv.http));
protocol.configure(io.sockets.emit);


//start serial protocol
//var protocol = require('./serial_protocol');
//protocol.init(function(data) {
//  io.sockets.emit('receive', data);
//});

console.log("HTTP server started: http://localhost:" + argv.http);
