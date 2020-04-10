import {saveRobotPacket, removeRobot, savePhysicsData} from './actions';
import {createStore, combineReducers} from 'redux';
import {robots, conns, win, options, physics } from './reducers';

const electron = window.require("electron");

class State {
  constructor() {
    this._store = createStore(
      combineReducers({
        robots: robots,
        win: win,
        conns: conns,
        options: options,
        physics: physics,
      })
    );

    electron.ipcRenderer.on("pkt", (event, pkt) => this._handlePkt(pkt)); 
    electron.ipcRenderer.on("physics", (event, physics) => this._handlePhysics(physics)); 
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
      case "printf":
        new Notification('CocoUI', {
          body: pkt.data.msg,
        });
        break;

      default:
        break;
    }
  }

  /**
   * @brief Physics simulation results update
   */
  _handlePhysics(physics) {
    this._store.dispatch(savePhysicsData(physics));
  }

  getStore() {
    return this._store;
  }
};

const instance = new State();
export default instance;
