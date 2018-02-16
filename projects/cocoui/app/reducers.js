import { Map, Set } from 'immutable';

const defaultConnsState = new Map({
  active: Set(),
});

const defaultWinState = new Map({
  id: null,
});


const defaultRobotsState = new Map({
});


export const robots = (state = defaultRobotsState, action) => {
  switch (action.type) {
    case 'SAVE_ROBOT_PACKET':
      for(let k in action.pkt.data) {
        if(!action.pkt.data.hasOwnProperty(k)) {
          continue;
        }
        state = state.setIn([action.pkt.client, action.pkt.data._name, k], action.pkt.data[k]);
      }
      break; 
  }
  return state;
}

export const conns = (state = defaultConnsState, action) => {
  switch (action.type) {
    case 'SAVE_ROBOT_PACKET':
      state = state.set('active', state.get('active').add(action.pkt.client));
      break;
  }
  return state;
}

export const win = (state = defaultWinState, action) => {
  switch (action.type) {
    case 'SAVE_ROBOT_PACKET':
      if(state.get('id') != action.pkt.client) {
        state = state.set('id', action.pkt.client);
      }
      break;
  }
  return state;
}
