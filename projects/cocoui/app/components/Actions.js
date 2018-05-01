import React from 'react';
import { connect } from 'react-redux';

class Action extends React.Component {
  render() {
    let color = "#FFFFFF";
    let score = this.props.data.score;

    if(score == -1)
    {
      color = "#EE0515";
      score = "No time";
    }
    else if(score == -2)
    {
      color = "#FFAA00";
      score = "Locked";
    }
    else if(score == -3)
    {
      color = "#10EE10";
      score = "Done";
    }
    else
    {
      score = score.toFixed(3);
    }

    const position = "translate(" + this.props.data.x + "," + this.props.data.y + ")";
    return (
      <g transform="translate(1500,1000) scale(1, -1)">
        <defs>
          <filter x="0" y="0" width="1" height="1" id="solid">
            <feFlood flood-color="#000000"/>
            <feComposite in="SourceGraphic"/>
          </filter>
        </defs>
        <g opacity="0.8" transform={position}>
          <circle cx="0" cy="0" r="100" strokeWidth="10" stroke={color}/>
          <g transform="scale(1, -1)">
            <text fontSize="60" y="20" fill="#FFFFFF">
              <tspan textAnchor="middle">{score}</tspan>
            </text>
            <text filter="url(#solid)" fontSize="40" y="-80" fill="#FFFFFF">
              <tspan textAnchor="middle">{this.props.data.name}</tspan>
            </text>
          </g>
        </g>
      </g>
    );
  }
}

class ActionsComponent extends React.Component {
  render() {
    if(!this.props.debugActionScheduler) {
      return <g />;
    }
    return (
      <g>
        {this.props.strategies.map((x, key) => {
          return <Action key={key} data={x} />;
        })}
      </g>
    );
  }
}


const mapStateToProps = (state, ownProps) => {
  const id = state.win.get('id');
  return {
    strategies: state.robots.getIn([id, 'action_scheduler', 'strategies'], []),
    debugActionScheduler: state.options.getIn(['debugActionScheduler']), 
  }
}

const Actions = connect(
  mapStateToProps,
  null,
)(ActionsComponent);

export default Actions;
