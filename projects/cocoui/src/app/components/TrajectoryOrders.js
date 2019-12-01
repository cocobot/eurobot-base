import React from 'react';
import { connect } from 'react-redux';

class TrajectoryOrdersComponent extends React.Component {
  renderOrder(order, key) {
    let type = order.type;
    let args = '';

    const flags = [];

    switch(order.type)
    {
      case 0:
        type = 'D';
        args = 'distance\u00A0=\u00A0' + order.a1.toFixed(0);
        if(order.time !== '-1.000') {
          args += ' t\u00A0=\u00A0' + order.time;
        }
        break;

      case 1:
        type = 'A';
        args = 'angle\u00A0=\u00A0' + order.a1.toFixed(0);
        if(order.time !== '-1.000') {
          args += ' t\u00A0=\u00A0' + order.time;
        }
        break;

      case 2:
      case 3:
        if(order.type === 2) {
          type = 'XY';
        }
        else {
          type = 'XY BACKWARD';
        }
        args = 'x\u00A0=\u00A0' + order.a1.toFixed(0);
        args += ' y\u00A0=\u00A0' + order.a2.toFixed(0);
        if(order.time !== '-1.000') {
          args += ' t\u00A0=\u00A0' + order.time;
        }
        break;


      default:
        args = '? (' + JSON.stringify(order) + ')';
        break;
    }

    const end_str = 'fin: x\u00A0=\u00A0' + order.end_x.toFixed(0) + ' y\u00A0=\u00A0' + order.end_y.toFixed(0) + ' a\u00A0=\u00A0' + order.end_angle.toFixed(0);

    const below_args = <small><i>{end_str}</i></small>;

    if(order.estimated_distance_before_stop > 0.01)
    {
      flags.push(<span key={flags.length} className="badge badge-warning">Sans ralentissement</span>);
    }

    if(flags.length > 0) {
      flags.unshift(<hr key="hr" />);
      flags.unshift(<br key="br" />);
    }

    return (
      <tr key={key} className="d-flex">
        <td className="col-sm-4"><small>{type}</small></td>
        <td className="col-sm-8"><small>{args}</small><br /><hr />{below_args}{flags}</td>
      </tr>
    );
  }
 
  render() {

    return (
      <div>
        <table className="table table-bordered table-condensed">
            <thead>
              <tr className="d-flex">
                <th className="col-sm-4">Type</th>
                <th className="col-sm-8">Arguments</th>
              </tr>
            </thead>
            <tbody>
              {this.props.orders.map(this.renderOrder)}
            </tbody>
        </table>
      </div>
    );
  }
}

const mapStateToProps = (state, ownProps) => {
  const id = state.win.get('id');
  return {
    orders: state.robots.getIn([id, 'trajectory_orders', 'orders'], []),
  }
}

const TrajectoryOrders = connect(
  mapStateToProps,
  null,
)(TrajectoryOrdersComponent);

export default TrajectoryOrders;

