export const saveRobotPacket = (pkt) => {
  return {
    type: 'SAVE_ROBOT_PACKET',
    pkt
  }
}

export const updateDebugPathfinder = (value) => {
  return {
    type: 'UPDATE_DEBUG_PATHFINDER',
    value
  }
}
