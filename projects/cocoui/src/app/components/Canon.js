import React from 'react';
import {ReloadIcon} from './Icons.js';
import { 
  Container,
  Row, Col,
  Card, CardHeader, CardBody,
  Navbar, NavbarBrand,
  InputGroup, InputGroupAddon, Input, InputGroupText,
  Button,
} from 'reactstrap';
import { connect } from 'react-redux';
import { Map } from 'immutable';
const electron = window.require("electron");
const ipcRenderer = electron.ipcRenderer;

class CanonConfigComponent extends React.Component {
  constructor(props) {
     super(props);
     this.state = {
       imax: "",
       maxrpm: "",
     };

    this.reload();
  }

  handleChange(key, event) {
    event.preventDefault();
    const obj = {}
    obj[key] = event.target.value;
    this.setState(obj);
  }

  handleKeyPress(key, event) {
    let imax = this.state.imax;
    let maxrpm = this.state.maxrpm;

    if(imax === "") {
      imax = this.props.imax;
    }
    else {
      imax = parseFloat(imax);
    }
    if(maxrpm === "") {
      maxrpm = this.props.max_speed_rpm;
    }
    else {
      maxrpm = parseFloat(maxrpm);
    }

    if (event.key === 'Enter') {
      ipcRenderer.send('pkt', {
        pid: 0x2001, //SET_MOTOR_CFG
        fmt: "BFF",
        args: [this.props.board_id, imax, maxrpm],
      });

      this.setState({imax: "", maxrpm: ""});
    }
  }

  /**
   * @brief Reload canon configuartion
   */
  reload() {
    //send packet to all canon
    ipcRenderer.send('pkt', {
      pid: 0x2002, //REQ_MOTOR_CFG
      fmt: "",
      args: [],
    });
  }


  render() {
    return (
    <Card>
      <CardHeader>
        Configuration: {this.props.side}
        <Button size="sm" className="float-right" onClick={() => this.reload()} color="success"><ReloadIcon /></Button>
      </CardHeader>
      <CardBody>
        <InputGroup>
          <InputGroupAddon addonType="prepend">
            <InputGroupText>Imax</InputGroupText>
          </InputGroupAddon>
          <Input value={this.state.imax} onChange={(e) => this.handleChange('imax', e)} onKeyPress={(e) => this.handleKeyPress('imax', e)} placeholder={this.props.imax}/>
        </InputGroup>
        <InputGroup>
          <InputGroupAddon addonType="prepend">
            <InputGroupText>Max rpm</InputGroupText>
          </InputGroupAddon>
          <Input value={this.state.maxrpm} onChange={(e) => this.handleChange('maxrpm',e)} onKeyPress={(e) => this.handleKeyPress('maxrpm', e)} placeholder={this.props.max_speed_rpm}/>
        </InputGroup>

      </CardBody>
    </Card>
    );
  }
}


const mapCanonConfigStateToProps = (state, ownProps) => {

  let board_name = "";
  let board_id = 0;
  switch(ownProps.robot) {
    case "principal":
      board_name = "P";
      board_id += 0x08;
      break;
    case "secondaire":
      board_name = "S";
      break;
    default:
      break;
  }

  switch(ownProps.side) {
    case "left":
      board_name += "L";
      board_id += 0x04;
      break;
    case "right":
      board_name += "R";
      board_id += 0x05;
      break;
    default:
      break;
  }

  board_name += "Canon";

  return {
    imax: state.robots.getIn([ownProps.robot, board_name, "set_cfg_motor", "imax"], ""),
    max_speed_rpm: state.robots.getIn([ownProps.robot, board_name, "set_cfg_motor", "max_speed_rpm"], ""),
    board_id: board_id,
  }
}

const CanonConfig = connect(
  mapCanonConfigStateToProps,
  null,
)(CanonConfigComponent);

class CanonSetpointComponent extends React.Component {
  constructor(props) {
     super(props);
     this.state = {
       left: "",
       right: "",
     };
  }

  handleChange(key, event) {
    event.preventDefault();
    const obj = {}
    obj[key] = event.target.value;
    this.setState(obj);
  }

  handleKeyPress(key, event) {
    let left = this.state.left;
    if(left === "") {
      left = this.props.left.getIn(['setpoint_rpm'], 0);
    }
    else {
      left = parseFloat(left);
    }
    let right = this.state.right;
    if(right === "") {
      right = this.props.right.getIn(['setpoint_rpm'], 0);
    }
    else {
      right = parseFloat(right);
    }


    if (event.key === 'Enter') {
      ipcRenderer.send('pkt', {
        pid: 0x2000,
        fmt: "BFF",
        args: [1,left, right],
      });

      this.setState({left: "", right: ""});
    }
  }

  _setOveride(value) {
    console.log(value);
    if(this.props.robot === "secondaire") {
      let master = 0x02;
    
      if(value) {
        master = 0x11;
      }

      ipcRenderer.send('pkt', {
        pid: 0x2005,
        fmt: "BBB",
        args: [0x04, 0x05, master],
      });
    }
    else {
      let master = 0x0A;
    
      if(value) {
        master = 0x11;
      }

      ipcRenderer.send('pkt', {
        pid: 0x2005,
        fmt: "BBB",
        args: [0x0C, 0x0D, master],
      });
    }

    this.setState({left: "", right: ""});
  }

  render() {

    let override = false;
    let override_left = false;
    let override_right = false;
    const left_board_id = this.props.left.getIn(['master_id'], 2) & 0xF7;
    const right_board_id = this.props.right.getIn(['master_id'], 2) & 0xF7;
    if(left_board_id != 2)
    {
      override_left = true;
    }
    if(right_board_id != 2)
    {
      override_right = true;
    }
    if(override_left && override_right)
    {
      override = true;
    }

    const left = override_left ? this.state.left : "";
    const left_default = this.props.left.getIn(['setpoint_rpm'], "");
    const right = override_right ? this.state.right : "";
    const right_default = this.props.right.getIn(['setpoint_rpm'], "");

    return (
    <Card>
      <CardHeader>
        Consignes
      </CardHeader>
      <CardBody>

      <InputGroup>
        <InputGroupAddon addonType="prepend">
          <InputGroupText>
            <Input addon type="checkbox" checked={override} onChange={() => this._setOveride(!override)}/>
          </InputGroupText>
        </InputGroupAddon>
        <Input value="Override brain" />
      </InputGroup>

        <InputGroup>
          <InputGroupAddon addonType="prepend">
            <InputGroupText>Gauche</InputGroupText>
          </InputGroupAddon>
          <Input value={left} disabled={!override_left} onChange={(e) => this.handleChange('left', e)} onKeyPress={(e) => this.handleKeyPress('left', e)} placeholder={left_default}/>
        </InputGroup>

        <InputGroup>
          <InputGroupAddon addonType="prepend">
            <InputGroupText>Droit</InputGroupText>
          </InputGroupAddon>
          <Input value={right} disabled={!override_right} onChange={(e) => this.handleChange('right', e)} onKeyPress={(e) => this.handleKeyPress('right', e)} placeholder={right_default}/>
        </InputGroup>



      </CardBody>
    </Card>
    );
  }
}


const mapCanonSetpointStateToProps = (state, ownProps) => {

  let left_board_name = "";
  let right_board_name = "";
  switch(ownProps.robot) {
    case "principal":
      left_board_name = "P";
      right_board_name = "P";
      break;
    case "secondaire":
      left_board_name = "S";
      right_board_name = "S";
      break;
    default:
      break;
  }

  left_board_name += "LCanon";
  right_board_name += "RCanon";

  return {
    left: state.robots.getIn([ownProps.robot, left_board_name, "motor_dbg"], new Map()),
    right: state.robots.getIn([ownProps.robot, right_board_name, "motor_dbg"], new Map()),
  }
}

const CanonSetpoint = connect(
  mapCanonSetpointStateToProps,
  null,
)(CanonSetpointComponent);


class CanonComponent extends React.Component {
  constructor(props) {
    super(props);
    this.UPDATE_PERIOD_MS = 500;
    this._requestDebug();
  }

  _requestDebug() {
    if(this.props.robot === "secondaire") {
      ipcRenderer.send('pkt', {
        pid: 0x2003,
        fmt: "BB",
        args: [0x04, 0x05],
      });
    }
    else {
      ipcRenderer.send('pkt', {
        pid: 0x2003,
        fmt: "BB",
        args: [0x0C, 0x0D],
      });
    }
    setTimeout(() => this._requestDebug(), this.UPDATE_PERIOD_MS);
  }

  render() {
    return (
      <div>
        <div>
          <Navbar color="dark" dark expand="md">
            <NavbarBrand href="#">CocoUI: Canon: {this.props.robot}
            </NavbarBrand>
          </Navbar>
        </div>
        <Container style={{ marginTop: '15px' }}>
          <Row>
            <Col md="4">
              <CanonConfig side="left" robot={this.props.robot}/>
            </Col>
            <Col md="4">
              <CanonSetpoint robot={this.props.robot}/>
            </Col>
            <Col md="4">
              <CanonConfig side="right" robot={this.props.robot}/>
            </Col>
          </Row>
        </Container>
      </div>
    );
  }
}

const mapStateToProps = (state, ownProps) => {
  return {
    data: state.robots.getIn([ownProps.robot]),
  }
}

const Canon = connect(
  mapStateToProps,
  null,
)(CanonComponent);


export default Canon;
