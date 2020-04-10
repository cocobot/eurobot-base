import React from 'react';
import { connect } from 'react-redux';

class PathfinderComponent extends React.Component {
  render() {
    if(!this.props.debugPathfinder) {
      return <g />;
    }
    const nodes = [];
    let gridSize = 15;

    if(this.props.width !== 0) {
      gridSize = 2000 / this.props.width;
    }

    for(let i = 0; i < this.props.length; i += 1) {
      for(let j = 0; j < this.props.width; j += 1) {
        const node = this.props.nodes[i * this.props.width + j];
        if(node != null) {
          const type  = node.type;
          //Default = jaune
          let r = 237;
          let g = 235;
          let b = 26;
          let af = 0.5;
          let as = 1;

          if(type & 4){//OPEN_LIST = Turquoise foncé
            r = 0;
            g = 100;
            b = 100;
          }
          else if(type & 8){ //TEMPORARY_ALLOWED No color
            r = 255;
            g = 255;
            b = 255;
            af = 0;
          }
          else if(type & 1){ //CLOSED LIST = Turquoise clair
            r = 0;
            g = 255;
            b = 165;
          }
          else if(type & 2){ //FINAL_TRAJ = Fushia
            r = 255;
            g = 0;
            b = 255;
          }
          else if(type & 0x0010){ //START_POINT = Bleu
            r = 0;
            g = 0;
            b = 199;
          }
          else if(type & 0x0020){ //TARGET_POINT = Violet
            r = 66;
            g = 0;
            b = 117;
          }
          else if(type & 0x0040){ //ROBOT Orange foncé
            r = 243;
            g = 84;
            b = 39;
          }
          else if(type & 0x200){ //GAME_ELEMENT Orange clair
            r = 243;
            g = 192;
            b = 39;
          }
          else if(type & 0x2000){ //OBSTACLE Rouge
            r = 255;
            g = 0;
            b = 0;
          }
          else if(type & 0x4000){ //FORBIDDEN Gris foncé
            r = 0;
            g = 0;
            b = 0;
            af = 0.7;
          }
          else if(type & 0x8000){ //SOFT_OBSTACLE Gris clair
            r = 0;
            g = 0;
            b = 0;
          }
          else if(type & 0x1000){ //NEW NODE (No color)
            r = 255;
            g = 255;
            b = 255;
            af = 0;
          }
          else{ //Used as debug, When a node reach an inconsistant state
            r = type & 0xff;
            g = 127;
            b = type & 0xff00;
            af = 1;
          }

          nodes.push(
              <rect key={i + "." + j} height={gridSize} strokeWidth={1}  width={gridSize} x={i * gridSize} y={j * gridSize} stroke={"rgba("+r+","+g+","+b+","+as+")"} fill={"rgba("+r+","+g+","+b+","+af+")"}/>
          );
        }
      }
    } 
    const res = <g>{nodes}</g>;
    return res;
  }
}


const mapStateToProps = (state, ownProps) => {
  const robot = "secondaire";
  const brain = "SBrain";
  return {
    nodes: state.robots.getIn([robot, brain, 'pathfinder', 'nodes'], []),
    length: state.robots.getIn([robot, brain, 'pathfinder', 'length'], 0),
    width: state.robots.getIn([robot, brain, 'pathfinder', 'width'], 0),
    debugPathfinder: state.options.getIn(['debugPathfinder']), 
  }
}

const Pathfinder = connect(
  mapStateToProps,
  null,
)(PathfinderComponent);

export default Pathfinder;


