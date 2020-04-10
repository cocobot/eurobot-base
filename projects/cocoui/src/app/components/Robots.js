import React from 'react';
import Robot from './Robot';

class Robots extends React.Component {
  render() {
    return (
      <g>
        <Robot name="principal" />
        <Robot name="secondaire" />
      </g>
    );
  }
}

export default Robots;

