export const saveRobotPacket = (pkt) => {
  return {
    type: 'SAVE_ROBOT_PACKET',
    pkt
  }
}

export const removeRobot = (client) => {
  return {
    type: 'REMOVE_ROBOT',
    client
  }
}

export const updateDebugPathfinder = (value) => {
  return {
    type: 'UPDATE_DEBUG_PATHFINDER',
    value
  }
}

export const updateDebugActionScheduler = (value) => {
  return {
    type: 'UPDATE_DEBUG_ACTION_SCHEDULER',
    value
  }
}

