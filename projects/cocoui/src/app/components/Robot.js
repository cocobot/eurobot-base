import React from 'react';
import { connect } from 'react-redux';

class RobotComponent extends React.Component {
  render() {
    if(this.props.x === undefined) {
       return <g />;
    }

    let shape = null;
    let paths = [];

    switch(this.props.name) {
      case "principal":
        shape = (
                 <g>
                 <path d="M125 180 L25 180 L-125 137 L-125 -137 L25 -180 L125 -180" fill="#000000" />
                 </g>
        );
        break;

      case "secondaire":
        shape = (
                 <g>
                 <g transform="translate(-90, -150)">
                 <rect height="300" width="180" fill="#000000" />
                 </g>
                 <g transform="scale(1, -1)">
                 <text fontSize="128" fontWeight="bolder" y="0" x="0" fill="#FF0000">
                 <tspan textAnchor="middle" dominantBaseline="middle">S</tspan>
                 </text>
                 </g>
                 </g>
        );
        break;
      default:
        shape = (
                 <g>
                 <circle cx="0" cy="0" r="100" strokeWidth="10" stroke="#FF0000"/>
                 <text fontSize="60" y="20" fill="#FFFFFF">
                 <tspan textAnchor="middle">?</tspan>
                 </text>
                 </g>
        );
    }


    /*
    //path
    let pathd = null;
    for(let i = 0; i < this.props.orders.length; i += 1) {
      let o = this.props.orders[i];
      if(o.end_x === Math.Nan)
      {
        continue;
      }
      if(pathd == null) {
        pathd = "M" + o.start_x + " " + o.start_y;
      }
      pathd += " L" + o.end_x + " " + o.end_y;

      const generateTurnPath = function(x, y, start, end, key) {
        x = parseFloat(x);
        y = parseFloat(y);
        start = parseFloat(start);
        end = parseFloat(end);

        const r = 50;
        const x0 = x + r * Math.cos(start * Math.PI / 180.0);
        const y0 = y + r * Math.sin(start * Math.PI / 180.0);
        const x1 = x + r * Math.cos(end * Math.PI / 180.0);
        const y1 = y + r * Math.sin(end * Math.PI / 180.0);

        let nend = end;
        let change = true;
        change = false; /// <--- TODO: understand this !
        while(change) {
          change = false;
          if(Math.abs(nend + 360 - start) < Math.abs(end -start)) {
            nend = nend + 360;
            change = true;
          }
          if(Math.abs(nend - 360 - start) < Math.abs(end -start)) {
            nend = nend - 360;
            change = true;
          }
        }

        let sns = 0;
        if(start < nend) {
          sns = 1;
        }

        const pathc = "M" + x0 + " " + y0 + "A" + r + " " + r + " 0 0 " + sns + " " + x1 + " " + y1;

        return <path key={key} d={pathc} strokeWidth="8" stroke="#FF0000" fill="rgba(0, 0, 0, 0)" />;
      };

      if((o.start_x === o.end_x) && (o.start_y === o.end_y)) {         
        if(o.start_angle !== o.end_angle) {
          paths.push(generateTurnPath(o.start_x, o.start_y, o.start_angle, o.end_angle, paths.length));
        }
      }
    }
    paths.push(<path key={paths.length} d={pathd} strokeWidth="8" stroke="#FF0000" fill="rgba(0, 0, 0, 0)" />);
    */


    const position = "translate(" + this.props.x * 1000 + "," + this.props.y * 1000 + ") rotate(" + this.props.a * 180.0 / Math.PI + ")";
    return (
      <g>
        <g transform="translate(1500,1000) scale(1, -1)">
          <g opacity="0.8" transform={position}>
            {shape}
          </g>
          <g>
            {paths}
          </g>
        </g>
      </g>
    );
  }
}


const mapStateToProps = (state, ownProps) => {
  return {
    x: state.physics.getIn(['robots', ownProps.name, 'position', 'x']),
    y: state.physics.getIn(['robots', ownProps.name, 'position', 'y']),
    a: state.physics.getIn(['robots', ownProps.name, 'position', 'a']),
  }
}

const Robot = connect(
  mapStateToProps,
  null,
)(RobotComponent);

export default Robot;


