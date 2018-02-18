import React from 'react';
import { connect } from 'react-redux';

class RobotComponent extends React.Component {
  render() {
    let shape = null;
    let paths = [];

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


    //path
    let pathd = null;
    for(let i = 0; i < this.props.orders.length; i += 1) {
      let o = this.props.orders[i];
      if(o.end_x == Math.Nan)
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

      if((o.start_x == o.end_x) && (o.start_y == o.end_y)) {         
        if(o.start_angle != o.end_angle) {
          paths.push(generateTurnPath(o.start_x, o.start_y, o.start_angle, o.end_angle, paths.length));
        }
      }
    }
    paths.push(<path key={paths.length} d={pathd} strokeWidth="8" stroke="#FF0000" fill="rgba(0, 0, 0, 0)" />);


    const position = "translate(" + this.props.x + "," + this.props.y + ") rotate(" + this.props.angle + ")";
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
    x: state.robots.getIn([ownProps.cid, 'position', 'x']),
    y: state.robots.getIn([ownProps.cid, 'position', 'y']),
    angle: state.robots.getIn([ownProps.cid, 'position', 'angle']),
    orders: state.robots.getIn([ownProps.cid, 'trajectory_orders', 'orders'], []),
  }
}

const Robot = connect(
  mapStateToProps,
  null,
)(RobotComponent);

export default Robot;

