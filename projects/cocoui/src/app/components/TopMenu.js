import React from 'react';
import { Nav, Navbar, NavbarBrand, NavItem,
         UncontrolledDropdown, DropdownToggle, DropdownItem, DropdownMenu,
         Badge,
} from 'reactstrap';
import { connect } from 'react-redux';

const electron = window.require("electron");

const ipcRenderer = electron.ipcRenderer;

class TopMenuComponent extends React.Component {
  _renderRobot(active, key) {
    const name = active.getIn(['name']);

    //robot
    let robot = <Badge color="danger">Robot ?</Badge>;
    const robotVal = this.props.robots.getIn([key, 'game_state', 'robot_id']);

    let color = "light";
    const colorVal = this.props.robots.getIn([key, 'game_state', 'color']);
    if(colorVal === 0) {
      color = "success";
    }
    else if(colorVal === 1) {
      color = "warning";
    }

    if(robotVal === 0) {
      robot = <Badge color={color}>Robot principal</Badge>;
    }
    else if(robotVal === 1) {
      robot = <Badge color={color}>Robot secondaire</Badge>;
    }

    //battery
    const batteryVal = (this.props.robots.getIn([key, 'game_state', 'battery']) / 1000.0).toFixed(2);
    let batteryColor = "light";
    switch(robotVal) {
      case 0:
        if(batteryVal < 18) {
           batteryColor = "danger";
        }
        else if(batteryVal < 19.5) {
           batteryColor = "warning";
        }
        else {
           batteryColor = "success";
        }
        break;

      case 1:
        if(batteryVal < 11.3) {
           batteryColor = "danger";
        }
        else if(batteryVal < 11.6) {
           batteryColor = "warning";
        }
        else {
           batteryColor = "success";
        }
        break

      default:
        break;
    }
    let battery = <Badge color={batteryColor}>{batteryVal} V</Badge>;

    //time
    let time = <Badge color="danger">? s</Badge>;
    const timeVal = this.props.robots.getIn([key, 'game_state', 'time']);
    let timeColor = "danger";
    if(timeVal < 50) {
      timeColor = "success";
    }
    else if(timeVal < 80) {
       timeColor = "warning";
    }
    time = <Badge color={timeColor}>{timeVal} s</Badge>;
    

    return (
      <NavItem key={key}>
        <UncontrolledDropdown nav inNavbar>
          <DropdownToggle nav caret>
            <small><b>{name}</b><br />
              {robot}
              {battery}
              {time}
              <Badge color="info">{this.props.robots.getIn([key, 'game_state', 'score'])}</Badge>
            </small>
          </DropdownToggle>
          <DropdownMenu >
            <DropdownItem>
              Default view
            </DropdownItem>
            <DropdownItem divider />
            <DropdownItem onClick={() => this._openUSIRWindow(key)}>
              USIR
            </DropdownItem>
            <DropdownItem onClick={() => this._openAsservWindow(key)}>
              Asserv
            </DropdownItem>
            <DropdownItem onClick={() => this._openMecaWindow(key)}>
              Meca
            </DropdownItem>
            <DropdownItem divider />
            <DropdownItem onClick={() => this._reset(key)}>
              Reset
            </DropdownItem>
          </DropdownMenu>
        </UncontrolledDropdown>
      </NavItem>
    );
  }

  _reset(cid) {
    ipcRenderer.send('pkt', {
      pid: 0x8007,
      fmt: "",
      args: [],
      client: cid, 
    });
  }

  _openUSIRWindow(cid) {
    ipcRenderer.send('window', {
      id: 'USIR',
      client: cid, 
    });
  }

  _openAsservWindow(cid) {
    ipcRenderer.send('window', {
      id: 'Asserv',
      client: cid, 
    });
  }

  _openMecaWindow(cid) {
    ipcRenderer.send('window', {
      id: 'Meca',
      client: cid, 
    });
  }

  render() {

    //const conn_list = [];
    //this.props.active.entrySeq().forEach(([key, x]) => {
    //  console.log(key);
    //  console.log(x);
    //});
    //console.log(conn_list);

    return (
      <div>
        <Navbar color="dark" dark expand="md">
        <NavbarBrand href="#">CocoUI</NavbarBrand>
          <Nav className="ml-auto" navbar>
            {this.props.active.entrySeq().map(([key, x]) => {
              return this._renderRobot(x, key);
            })}
          </Nav>
        </Navbar>
      </div>
    );
  }
}

const mapStateToProps = (state, ownProps) => {
  return {
    active: state.conns.get('active'),
    robots: state.robots,
  }
}

const TopMenu = connect(
  mapStateToProps,
  null,
)(TopMenuComponent);

export default TopMenu;

