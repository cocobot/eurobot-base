import React from 'react';
import { connect } from 'react-redux';

class RobotComponent extends React.Component {
  render() {
    let shape = null;

    if(this.props.name == "Robot secondaire")
    {
      shape = (
        <g transform="rotate(90) translate(-100, -75)">
          <rect height="145" width="185" fill="#000000" />
        </g>
      );
    }
    else if(this.props.name == "Robot principal")
    {
      shape = (
        <g transform="rotate(90) translate(-150, -110)">
          <rect height="220" width="300" fill="#000000" />
        </g>
      );
    }
    else
    {
      shape = (
        <g>
          <circle cx="0" cy="0" r="100" strokeWidth="10" stroke="#FF0000"/>
          <text fontSize="60" y="20" fill="#FFFFFF">
            <tspan textAnchor="middle">?</tspan>
          </text>
        </g>
      );
    }

    const position = "translate(" + this.props.x + "," + this.props.y + ") rotate(" + this.props.angle + ")";
    return (
      <g>
        <g transform="translate(1500,1000) scale(1, -1)">
          <g opacity="0.8" transform={position}>
            {shape}
          </g>
        </g>
      </g>
    );
  }
}


const mapStateToProps = (state, ownProps) => {
  return {
    x: state.robots.getIn([ownProps.cid, 'position', 'x']),
    y: state.robots.getIn([ownProps.cid, 'position', 'y']),
    angle: state.robots.getIn([ownProps.cid, 'position', 'angle']),
  }
}

const Robot = connect(
  mapStateToProps,
  null,
)(RobotComponent);

export default Robot;

