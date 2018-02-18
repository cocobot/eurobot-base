import React from 'react';
import { connect } from 'react-redux';

class PathfinderComponent extends React.Component {
  render() {
    const nodes = [];
    let gridSize = 15;

    if(this.props.width != 0) {
      gridSize = 2000 / this.props.width;
    }

    for(let i = 0; i < this.props.length; i += 1) {
      for(let j = 0; j < this.props.width; j += 1) {
        const node = this.props.nodes[i * this.props.width + j];
        if(node != null) {
          const type  = node.type;
          let r = 0;
          let g = 0;
          let b = 0;
          let af = 0.5;
          let as = 0.7;

          switch(type) {
            case 0x8000: //SOFT OBSTACLE
              r = 255;
              g = 255;
              break;

            case 0x4000: //FORBIDDEN
              r = 255;
              g = 165;
              break;

            case 0x2000: //OBSTACLE
              r = 255;
              break;

            case 0x1000: //NEW NODE
              g = 165;
              break;

            case 0x1004: //NEW NODE + OPEN_LIST
              g = 100;
              b = 100;
              break;

            case 2: //FINAL TRAJ
              r = 255;
              b = 255;
              break;

            case 0x1: //CLOSED LIST
              g = 255;
              b = 165;
              break;

            case 0x0: //?
              b = 255;
              break;

            default:
              console.log("type? ");
              console.log(type);
              r = 255;
              g = 255;
              b = 255;
              as = 1;
              af = 1;
              break;
          }

          nodes.push(
            <g key={i + "." + j} transform={"translate(" + (i * gridSize) + ", " + (j * gridSize) + ")"}>
              <rect height={gridSize} strokeWidth={1}  width={gridSize} stroke={"rgba("+r+","+g+","+b+","+as+")"} fill={"rgba("+r+","+g+","+b+","+af+")"}/>
            </g>
          );
        }
      }
    }
    return (
      <g>
        { nodes }
      </g>
    );
  }
}


const mapStateToProps = (state, ownProps) => {
  const id = state.win.get('id');
  return {
    nodes: state.robots.getIn([id, 'pathfinder', 'nodes'], []),
    length: state.robots.getIn([id, 'pathfinder', 'length'], 0),
    width: state.robots.getIn([id, 'pathfinder', 'width'], 0),
  }
}

const Pathfinder = connect(
  mapStateToProps,
  null,
)(PathfinderComponent);

export default Pathfinder;

