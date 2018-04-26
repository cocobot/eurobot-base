import electron from 'electron';
import {saveRobotPacket, removeRobot} from './actions';
import {robots, conns, win, options } from './reducers';
import {createStore, combineReducers} from 'redux';

class State {
  constructor() {
    this._store = createStore(
      combineReducers({
        robots: robots,
        win: win,
        conns: conns,
        options: options,
      })
    );

    electron.ipcRenderer.on("pkt", (event, pkt) => this._handlePkt(pkt)); 
    setInterval(() => this._purgeStore(), 500);
  }

  _purgeStore() {
    const toRemove = [];
    const now = Date.now();
    this._store.getState().conns.get('active').map((x, k) => {
      const stamp = x.get('timestamp');
      if(stamp + 2500 < now) {
        toRemove.push(k);
      }
    });
    toRemove.forEach((x) => {
      this._store.dispatch(removeRobot(x));
    });
  }

  _handlePkt(pkt) {
    this._store.dispatch(saveRobotPacket(pkt));
    switch(pkt.data._name) {
      case "printf":
        {
          const not = new Notification('CocoUI', {
            body: pkt.data.msg,
          });
        }
        break;
    }
  }

  getStore() {
    return this._store;
  }
};

const instance = new State();
export default instance;
