import electron from 'electron';
import {saveRobotPacket} from './actions';
import {robots, conns, win } from './reducers';
import {createStore, combineReducers} from 'redux';

class State {
  constructor() {
    this._store = createStore(
      combineReducers({
        robots: robots,
        win: win,
        conns: conns,
      })
    );

    electron.ipcRenderer.on("pkt", (event, pkt) => this._handlePkt(pkt)); 
  }

  _handlePkt(pkt) {
    this._store.dispatch(saveRobotPacket(pkt));
    //
  }

  getStore() {
    return this._store;
  }
};

const instance = new State();
export default instance;
