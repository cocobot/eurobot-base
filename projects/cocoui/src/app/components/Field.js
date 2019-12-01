import React from 'react';

class Field extends React.Component {
  render() {
    let xmin = 0 - 22 - 112;
    let ymin = 0 - 22 - 112;
    let xmax = 3000 + 22 + 112;
    let ymax = 2000 + 22 + 112;

    const margin = 200;

    var x = parseFloat(this.props.x) + 1500;
    if(xmin > x - margin) {
      xmin = x - margin;
    }
    if(xmax < x + margin) {
      xmax = x + margin;
    }

    var y = -parseFloat(this.props.y) + 1000;
    if(ymin > y - margin) {
      ymin = y - margin;
    }
    if(ymax < y + margin) {
      ymax = y + margin;
    }

    const vBvalue = xmin + " " + ymin + " " + (xmax - xmin) + " " + (ymax - ymin);
    return (
      <svg viewBox={vBvalue} ref="field">
            {this.props.children}
      </svg>
    );
  }
}

export default Field;



