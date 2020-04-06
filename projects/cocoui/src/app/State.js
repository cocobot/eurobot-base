import {saveRobotPacket, removeRobot} from './actions';
import {createStore, combineReducers} from 'redux';
import {robots, conns, win, options } from './reducers';

const electron = window.require("electron");

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
      return null;
    });
    toRemove.forEach((x) => {
      this._store.dispatch(removeRobot(x));
    });
  }

  _handlePkt(pkt) {
    this._store.dispatch(saveRobotPacket(pkt));
    switch(pkt.data._name) {
      case "ping":
        console.log(pkt);
        break;

   case "printf":
        new Notification('CocoUI', {
          body: pkt.data.msg,
        });
        break;

      default:
        break;
    }
  }

  getStore() {
    return this._store;
  }
};

const instance = new State();
export default instance;
