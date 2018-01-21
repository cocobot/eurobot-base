import { Map, Set } from 'immutable';

const defaultConnsState = new Map({
  active: Set(),
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
        state = state.setIn([action.pkt.client, k], action.pkt.data[k]);
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
