import React from 'react';
import { connect } from 'react-redux';
import Robot from './Robot';

class RobotsComponent extends React.Component {
  render() {
    return (
      <g>
        {this.props.active.map((x, key) => {
          return <Robot key={key} cid={key} />
        })}
      </g>
    );
  }
}


const mapStateToProps = (state, ownProps) => {
  return {
    active: state.conns.get('active'),
  }
}

const Robots = connect(
  mapStateToProps,
  null,
)(RobotsComponent);

export default Robots;
