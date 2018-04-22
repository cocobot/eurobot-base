import electron from 'electron';
import {saveRobotPacket} from './actions';
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
  }

  _handlePkt(pkt) {
    this._store.dispatch(saveRobotPacket(pkt));
    switch(pkt.data._name) {
      case "printf":
        {
          console.warn("log ");
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
