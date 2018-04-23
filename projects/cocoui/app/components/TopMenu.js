import React from 'react';
import { Nav, Navbar, NavbarBrand, NavItem, NavLink,
         UncontrolledDropdown, DropdownToggle, DropdownItem, DropdownMenu,
         Badge,
} from 'reactstrap';
import { connect } from 'react-redux';

class TopMenuComponent extends React.Component {
  _renderRobot(x, key) {
    //robot
    let robot = <Badge color="danger">Robot ?</Badge>;
    const robotVal = this.props.robots.getIn([x, 'game_state', 'robot_id']);

    let color = "light";
    const colorVal = this.props.robots.getIn([x, 'game_state', 'color']);
    if(colorVal == 0) {
      color = "success";
    }
    else if(colorVal == 1) {
      color = "warning";
    }

    if(robotVal == 0) {
      robot = <Badge color={color}>Robot principal</Badge>;
    }
    else if(robotVal == 0) {
      robot = <Badge color={color}>Robot secondaire</Badge>;
    }

    //time
    let time = <Badge color="danger">? s</Badge>;
    const timeVal = this.props.robots.getIn([x, 'game_state', 'time']);
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
            <small><b>??</b><br />
              {robot}
              <Badge color="danger">? V</Badge>
              {time}
            </small>
          </DropdownToggle>
          <DropdownMenu >
            <DropdownItem>
              Option 1
            </DropdownItem>
            <DropdownItem>
              Option 2
            </DropdownItem>
            <DropdownItem divider />
            <DropdownItem>
              Reset
            </DropdownItem>
          </DropdownMenu>
        </UncontrolledDropdown>
      </NavItem>
    );
  }

  render() {
    return (
      <div>
        <Navbar color="dark" dark expand="md">
        <NavbarBrand href="#">CocoUI</NavbarBrand>
          <Nav className="ml-auto" navbar>
            {this.props.active.map((x, key) => {
              return this._renderRobot(x, key);
            })}
            <NavItem>
              <UncontrolledDropdown nav inNavbar>
                <DropdownToggle nav caret>
                  <small><b>/dev/ttyUSB0</b><br />
                    <Badge color="light">Robot principal</Badge>
                    <Badge color="danger">12 V</Badge>
                    <Badge color="info">100 s</Badge>
                  </small>
                </DropdownToggle>
                <DropdownMenu >
                  <DropdownItem>
                    Option 1
                  </DropdownItem>
                  <DropdownItem>
                    Option 2
                  </DropdownItem>
                  <DropdownItem divider />
                  <DropdownItem>
                    Reset
                  </DropdownItem>
                </DropdownMenu>
              </UncontrolledDropdown>
            </NavItem>
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
