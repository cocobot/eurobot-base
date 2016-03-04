from flask import Flask, render_template
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app)
app.config['DEBUG'] = True
app.config['SECRET_KEY'] = 'secret!'
ui = None


@app.route('/')
@app.route('/')
@app.route('/page/<page>/')
@app.route('/page/<page>/<path:args>/')
def index(page="information", args=[]):
    if not isinstance(args, list):
        args = args.split('/')
    strargs = '['
    for arg in args:
        if strargs != '[':
            strargs += ',' + arg
        else:
            strargs += arg
    strargs += ']'
    return render_template('index.html', page=page, args=strargs)


class UI:
    def __init__(self, protocol, port=5000):
        global ui
        self.app = app
        self.socketio = socketio
        self.port = port
        self.protocol = protocol
        ui = self
        self.i = 0


    def run(self):
        self.protocol.received_event = lambda evt: self.sendReceivedData(evt)
        self.socketio.run(self.app, host="0.0.0.0", port=self.port, use_reloader=False)

    def sendReceivedData(self, evt):
        print("loutre")
        self.i = self.i + 1
        self.socketio.emit('receive', {'data': self.i}, namespace='/cocoui', broadcast=True)
