'use strict';

//pacakges
var express = require('express');
var http = require('http');
var bodyParser = require('body-parser');
var optimist = require('optimist');
var path = require('path');
var jsxCompile = require('express-jsx');
var multer  = require('multer');

var protocol = require('./protocol/protocol.js');
var hexParser = require('./hexparser.js');


//set defaults
optimist = optimist.default('http', 3000);
var argv = optimist.argv;

//create express app
var app = express();
var storage = multer.memoryStorage();
var upload = multer({storage: storage});
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
app.post('/api/program', upload.single('file'), function(req, res) {
  var sendMsg = function(msg) {
    res.write(msg + "\n");
    io.sockets.emit("bootloader", {msg: msg});
  };

  var sendEnd = function(success) {
    if(success) {
      res.write("Done\n");
      io.sockets.emit("bootloader", {msg: "Done"});
    }
    else {
      res.write("Fail\n");
      io.sockets.emit("bootloader", {msg: "Fail"});
    }
    res.end();
  };
  
  sendMsg("Parsing hex file");
  var parser = hexParser.parse(req.file, function(success, msg, param) {
    if(!success) {
      sendMsg(msg);
    }
    else {
      protocol.program(param, sendMsg, function(success, msg) {
        if(!success) {
          sendMsg(msg);
          sendEnd(false);
        }
        else {
          sendEnd(true);
        }
      });
    }
  });


  //var f = function(res, i, cb) {
  //  console.log("pouet");
  //  if(i < 5) {
  //    res.write("plop " + i + "\n")
  //    setTimeout(function() { cb(res, i + 1, cb);} , 1000);
  //  }
  //  else {
  //    res.write("plop " + i + "\n")
  //  }
  //}

  //setTimeout(function() { f(res, 0, f);} , 1000);
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
