'use strict';

//pacakges
var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');
var optimist = require('optimist');
var path = require('path');
var protocol = require('./protocol/protocol.js');
var jsxCompile = require('express-jsx');

//set defaults
optimist = optimist.default('http', 3000);
var argv = optimist.argv;

//create express app
var app = express()
app.use(jsxCompile(path.join(__dirname, 'public'), {
    dest: path.join(__dirname, 'compiled-js')
}));
//  app.use(jsxCompile(path.join(__dirname, 'public')));
app.use(express.static(path.join(__dirname, 'compiled-js')));
app.use(express.static('public'));
app.engine('html', require('ejs').renderFile);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));


//generate routes
app.get('/', function(req, res){
  res.render('index.html');
});
app.get('/page/*', function(req, res){
  res.render('index.html');
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
app.get('/api/time/:ct', function(req, res) {
  var ct = parseInt(req.params.ct);
  var serverTimestamp = Date.now();
  var diff = serverTimestamp - ct;
  res.json({
    'diff': diff,
    'serverTimestamp': serverTimestamp,
  });
});

//start serveur
var io = require('socket.io').listen(app.listen(argv.http));
protocol.configure(function(chan, data) { io.sockets.emit(chan, data); });
io.sockets.on('connection', function (socket) {
  socket.on('send', function(data) { protocol.send(data); });
});

console.log("HTTP server started: http://localhost:" + argv.http);

if(argv.autoconnect != undefined) {
  protocol.connect(argv.autoconnect);
}
