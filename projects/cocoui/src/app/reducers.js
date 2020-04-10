import { Map } from 'immutable';

const defaultConnsState = new Map({
  active: Map(),
});

const defaultWinState = new Map({
  id: null,
});


const defaultRobotsState = new Map({
  'secondaire': new Map(),
});

const defaultPhysicsState = new Map({
});

let defaultOptionsState = new Map({
  debugPathfinder: true,
  debugActionScheduler: true,
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
        state = state.setIn([action.pkt.data._robot, action.pkt.data._src_name, action.pkt.data._name, k], action.pkt.data[k]);
      }
      break; 
    default:
      break;
  }
  return state;
}

export const conns = (state = defaultConnsState, action) => {
  switch (action.type) {
    case 'SAVE_ROBOT_PACKET':
      state = state.setIn(['active', action.pkt.client], Map({
        timestamp: Date.now(),
        name: action.pkt.clientName,
      }));
      break;

    case 'REMOVE_ROBOT':
      state = state.deleteIn(['active', action.client]);
      break;

    default:
      break;
  }
  return state;
}

export const win = (state = defaultWinState, action) => {
  switch (action.type) {
    case 'SAVE_ROBOT_PACKET':
      if(state.get('id') !== action.pkt.client) {
        state = state.set('id', action.pkt.client);
      }
      break;

    default:
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

    case 'UPDATE_DEBUG_ACTION_SCHEDULER':
      state = state.set('debugActionScheduler', action.value);
      break;

    default:
      break;
  }

  if(bState !== state) {
    localStorage.setItem('options', JSON.stringify(state.toJS()));
  }

  return state;
}

export const physics = (state = defaultPhysicsState, action) => {
  //get all keys/values from an JS object as array
  const getAllKeys = (map) => {
    const keys = [];

    //iterate all object fields
    for(let k in map) {
      //only real fields
      if(!map.hasOwnProperty(k)) {
        continue;
      }

      if(typeof(map[k]) == "object") {
        //if object, get all fields recursivly
        const subkeys = getAllKeys(map[k]);  
        for(let i = 0; i < subkeys.length; i += 1) {
          //prefix field name
          const subkey = subkeys[i];
          subkey.unshift(k);
          keys.push(subkey);
        }
      }
      else {
        //value, insert directly in the array
        keys.push([k, map[k]]);
      }
    }

    return keys;
  }

  switch (action.type) {
    case 'SAVE_PHYSICS_DATA':
      //convert JS object to Immutable map
      const keys = getAllKeys(action.physics);
      for(let i = 0; i < keys.length; i += 1) {
        const key = keys[i];
        const value = key.pop();
        state = state.setIn(key, value);
      }
      break; 

    default:
      break;
  }
  return state;
}
