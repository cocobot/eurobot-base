import { Map, Set, fromJS } from 'immutable';

const defaultConnsState = new Map({
  active: Set(),
});

const defaultWinState = new Map({
  id: null,
});


const defaultRobotsState = new Map({
});

let defaultOptionsState = new Map({
  debugPathfinder: true,
});

let saved = localStorage.getItem('options');
if(saved != null) {
  try
  {
    saved = JSON.parse(saved);
    for(let i in saved) {
      if(saved.hasOwnProperty(i)) {
        defaultOptionsState = defaultOptionsState.set(i, saved[i]);
      }
    }
  }
  catch(e) {
    console.log(e);
  }
}

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

export const options = (state = defaultOptionsState, action) => {
  const bState = state;
  switch (action.type) {
    case 'UPDATE_DEBUG_PATHFINDER':
      state = state.set('debugPathfinder', action.value);
      break;
  }

  if(bState != state) {
    localStorage.setItem('options', JSON.stringify(state.toJS()));
  }

  return state;
}
