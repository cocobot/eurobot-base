import React from 'react';
import { Nav, Navbar, NavbarBrand, NavItem,
         UncontrolledDropdown, DropdownToggle, DropdownItem, DropdownMenu,
         Badge,
} from 'reactstrap';
import { connect } from 'react-redux';

const electron = window.require("electron");

const ipcRenderer = electron.ipcRenderer;

class TopMenuComponent extends React.Component {
  _renderRobot(data, name) {

    let color = "light";
    const colorVal = data.getIn(['game_state', 'color']);
    if(colorVal === 0) {
      color = "blue";
    }
    else if(colorVal === 1) {
      color = "warning";
    }

    let robot = <Badge color={color}>Robot {name}</Badge>;


    //battery
    const batteryVal = (data.getIn(['game_state', 'battery']) / 1000.0).toFixed(2);
    let batteryColor = "light";
    switch(name) {
      case "principal":
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

      case "secondaire":
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
    const timeVal = data.getIn(['game_state', 'time']);
    let timeColor = "danger";
    if(timeVal < 50) {
      timeColor = "success";
    }
    else if(timeVal < 80) {
       timeColor = "warning";
    }
    time = <Badge color={timeColor}>{timeVal} s</Badge>;
    

    return (
      <NavItem key={name}>
        <UncontrolledDropdown nav inNavbar>
          <DropdownToggle nav caret>
            <small><b>{robot}</b><br />
              {battery}
              {time}
              <Badge color="info">{data.getIn(['game_state', 'score'])}</Badge>
            </small>
          </DropdownToggle>
          <DropdownMenu >
            <DropdownItem onClick={() => this._openUSIRWindow(name)}>
              USIR
            </DropdownItem>
            <DropdownItem onClick={() => this._openAsservWindow(name)}>
              Asserv
            </DropdownItem>
            <DropdownItem onClick={() => this._openMecaWindow(name)}>
              Meca
            </DropdownItem>
            <DropdownItem divider />
            <DropdownItem onClick={() => this._openCanonWindow(name)}>
              Canon
            </DropdownItem>
          </DropdownMenu>
        </UncontrolledDropdown>
      </NavItem>
    );
  }

  _openCanonWindow(robot) {
    ipcRenderer.send('window', {
      id: 'canon',
      robot: robot, 
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
    return (
      <div>
        <Navbar color="dark" dark expand="md">
        <NavbarBrand href="#">CocoUI</NavbarBrand>
          <Nav className="ml-auto" navbar>
            {this.props.robots.entrySeq().map(([key, x]) => {
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
    robots: state.robots,
  }
}

const TopMenu = connect(
  mapStateToProps,
  null,
)(TopMenuComponent);

export default TopMenu;

