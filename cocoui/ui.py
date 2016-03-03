from flask import Flask, render_template
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app)
app.config['DEBUG'] = True
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
    def __init__(self, port=5000):
        global ui
        self.app = app
        self.socketio = socketio
        self.port = port
        ui = self

    def run(self):
        self.socketio.run(self.app, host="0.0.0.0", port=self.port)
