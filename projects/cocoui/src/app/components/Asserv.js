import React from 'react';
import State from '../State.js';
import { 
  Container,
  Row, Col,
  Card, CardHeader, CardBody,
  InputGroup, InputGroupAddon, Input,
  Navbar, NavbarBrand,
  Alert,
  Button,
} from 'reactstrap';
import { connect } from 'react-redux';
import {ReloadIcon} from './Icons.js';
import $ from 'jquery';
const electron = window.require("electron");
const ipcRenderer = electron.ipcRenderer;
window.jQuery = $;
window.require('flot');

const PLOT_MAX_TIME_ms = 10000;

class ParameterComponent extends React.Component {
  constructor(props) {
     super(props);
     this.state = {
       value: "",
     };
  }

  handleChange(event) {
    event.preventDefault();
    this.setState({value: event.target.value});
  }

  handleKeyPress(event) {
    if (event.key === 'Enter') {
      let value = this.state.value;

      if(value === "") {
        value = this.props.value;
      }
      else {
        value = parseFloat(value);
      }

      ipcRenderer.send('pkt', {
        pid: 0x800E,
        fmt: "BBF",
        args: [this.props.board_id, this.props.id, value],
      });
      this.setState({value: ""});
    }
  }

  render() {
    return (
     <InputGroup size="sm" >
      <InputGroupAddon addonType="prepend">{this.props.name}</InputGroupAddon>
      <Input placeholder={this.props.value} value={this.state.value} onChange={(e) => this.handleChange(e)} onKeyPress={(e) => this.handleKeyPress(e)} />
     </InputGroup>
    );
  }
}

const mapParameterStateToProps = (state, ownProps) => {

  let board_name = "";
  let board_id = 0x02;
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

  board_name += "Brain";

  return {
    value: state.robots.getIn([ownProps.robot, board_name, "asserv_params", ownProps.command], ""),
    board_id: board_id,
  }
}

const Parameter = connect(
  mapParameterStateToProps,
  null,
)(ParameterComponent);

class Asserv extends React.Component {
  constructor(props) {
    super(props);

    //default state
    this.state = {
      chartWidth: 800,
      chartHeight: 200,
    };

    //creates all chart refs
    this._chartdp = React.createRef();
    this._chartds = React.createRef();
    this._chartdpid = React.createRef();
    this._chartap = React.createRef();
    this._chartas = React.createRef();
    this._chartapid = React.createRef();

    //reload parameters at window loading
    this.reload();
  }

  reload() {
    ipcRenderer.send('pkt', {
      pid: 0x800C,
      fmt: "",
      args: [],
    });
  }

  render() {
    return (
      <div>
        <div>
          <Navbar color="dark" dark expand="md">
            <NavbarBrand href="#">CocoUI: Asserv: {this.props.robot}
            </NavbarBrand>
          </Navbar>
        </div>

        <Container style={{ marginTop: '15px' }}>
          <Row>
            <Col>
              <Card>
                <CardHeader>
                  Asserv
                  <Button size="sm" className="float-right" onClick={() => this.reload()} color="success"><ReloadIcon /></Button>
                </CardHeader>
                <CardBody>
                  <Row>
                    <Col md="9">
                      <div ref={this._chartdp} style={{height: this.state.chartHeight + "px", width: this.state.chartWidth + "px"}}/>
                      <div ref={this._chartds} style={{height: this.state.chartHeight + "px", width: this.state.chartWidth + "px"}}/>
                      <div ref={this._chartdpid} style={{height: this.state.chartHeight + "px", width: this.state.chartWidth + "px"}}/>
                    </Col>
                    <Col md="3">
                      <Alert>Distance</Alert>
                      <Parameter robot={this.props.robot} id={0} name="Speed" data={this.props.asservs} command="d_ramp_speed" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={1} name="Accel" data={this.props.asservs} command="d_ramp_accel" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={2} name="Kp" data={this.props.asservs} command="d_pid_kp" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={3} name="Kd" data={this.props.asservs} command="d_pid_kd" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={4} name="Ki" data={this.props.asservs} command="d_pid_ki" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={5} name="Max I" data={this.props.asservs} command="d_pid_max_i" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={6} name="Max E" data={this.props.asservs} command="d_pid_max_e" show={this.props.show}/>
                    </Col>
                  </Row>
                  <Row>
                    <Col md="9">
                      <div ref={this._chartap} style={{height: this.state.chartHeight + "px", width: this.state.chartWidth + "px"}}/>
                      <div ref={this._chartas} style={{height: this.state.chartHeight + "px", width: this.state.chartWidth + "px"}}/>
                      <div ref={this._chartapid} style={{height: this.state.chartHeight + "px", width: this.state.chartWidth + "px"}}/>
                    </Col>
                    <Col md="3">
                      <Alert>Angle</Alert>
                      <Parameter robot={this.props.robot} id={7} name="Speed" data={this.props.asservs} command="a_ramp_speed" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={8} name="Accel" data={this.props.asservs} command="a_ramp_accel" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={9} name="Kp" data={this.props.asservs} command="a_pid_kp" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={10} name="Kd" data={this.props.asservs} command="a_pid_kd" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={11} name="Ki" data={this.props.asservs} command="a_pid_ki" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={12} name="Max I" data={this.props.asservs} command="a_pid_max_i" show={this.props.show}/>
                      <Parameter robot={this.props.robot} id={13} name="Max E" data={this.props.asservs} command="a_pid_max_e" show={this.props.show}/>
                    </Col>
                  </Row>
                </CardBody>
              </Card>
            </Col>
          </Row>
        </Container>
      </div>
    );
  }

  addPlotData() { 
    const now = Date.now();
    const robots = State.getStore().getState().robots;
    const brain = this.props.robot === "principal" ? "Pbrain" : "SBrain";

    for(let key in this._plotData) {
      const pdata = this._plotData[key];

      let ymin = [Infinity];
      let ymax = [-Infinity];
      for(let i in pdata.data) {
        const cl = pdata.data[i]; 
        cl.data.push([now, robots.getIn([this.props.robot, brain, cl.category, cl.command], 0)]);

        //remove old data
        while(cl.data.length > 0) {
          const time = cl.data[0][0];
          if(time < now - PLOT_MAX_TIME_ms) {
            cl.data.shift();
          }
          else {
            break;
          }
        }

        //find min max
        for(let j = 0; j < cl.data.length; j += 1) {
          const data = cl.data[j][1];
          if(data < ymin[0]) {
            ymin[0] = data - 1;
          }
          if(data > ymax[0]) {
            ymax[0] = data + 1;
          }
        }
      }

      pdata.plot.getAxes().yaxis.options.min = ymin[0];
      pdata.plot.getAxes().yaxis.options.max = ymax[0];
     
      pdata.plot.getAxes().xaxis.options.min = now - PLOT_MAX_TIME_ms;
      pdata.plot.getAxes().xaxis.options.max = now;

      pdata.plot.setData(pdata.data);
      pdata.plot.setupGrid();
      pdata.plot.draw();
    }
  }

  componentDidMount() {
    const now = Date.now();

    const opt = {
      xaxis: {
					mode: "time",
					timeBase: "milliseconds",
          min: now - PLOT_MAX_TIME_ms,
          max: now,
      },
      yaxes: [ 
        { 
          axisLabel: '?',
        }, 
      ],
      legend: {
        show: true,
        position: "nw",
      },
    }

    this._plotData = {
      dp: {
        opt: Object.assign(Object.assign({}, opt), {yaxes: [{axisLabel: 'mm'}]}),
        data: [
        {
          label: 'Position target',
          data: [],
          category: 'asserv_angle',
          command: 'target',
        },
        {
          label: 'Position filtered',
          data: [],
          category: 'asserv_angle',
          command: 'ramp_out',
        },
        {
          label: 'Current position',
          data: [],
          category: 'asserv_angle',
          command: 'angleance',
        },
      ]},

      ds: {
        opt: Object.assign(Object.assign({}, opt), {yaxes: [{axisLabel: 'mm/s'}]}),
        data: [
        {
          label: 'Speed target',
          data: [],
          category: 'asserv_angle',
          command: 'speed_target',
        },
        {
          label: 'Current speed',
          data: [],
          category: 'asserv_angle',
          command: 'speed',
        },
      ]},

      dpid: {
        opt: Object.assign(Object.assign({}, opt), {yaxes: [{axisLabel: 'rpm'}]}),
        data: [
        {
          label: 'PID output',
          data: [],
          category: 'asserv_angle',
          command: 'pid_out',
        },
        {
          label: 'PID - P',
          data: [],
          category: 'asserv_angle',
          command: 'pid_P',
        },
        {
          label: 'PID - I',
          data: [],
          category: 'asserv_angle',
          command: 'pid_I',
        },
        {
          label: 'PID - D',
          data: [],
          category: 'asserv_angle',
          command: 'pid_D',
        },
      ]},

      ap: {
        opt: Object.assign(Object.assign({}, opt), {yaxes: [{axisLabel: 'deg'}]}),
        data: [
        {
          label: 'Position target',
          data: [],
          category: 'asserv_angle',
          command: 'target',
        },
        {
          label: 'Position filtered',
          data: [],
          category: 'asserv_angle',
          command: 'ramp_out',
        },
        {
          label: 'Current position',
          data: [],
          category: 'asserv_angle',
          command: 'angle',
        },
      ]},

      as: {
        opt: Object.assign(Object.assign({}, opt), {yaxes: [{axisLabel: 'deg/s'}]}),
        data: [
        {
          label: 'Speed target',
          data: [],
          category: 'asserv_angle',
          command: 'speed_target',
        },
        {
          label: 'Current speed',
          data: [],
          category: 'asserv_angle',
          command: 'speed',
        },
      ]},

      apid: {
        opt: Object.assign(Object.assign({}, opt), {yaxes: [{axisLabel: 'rpm'}]}),
        data: [
        {
          label: 'PID output',
          data: [],
          category: 'asserv_angle',
          command: 'pid_out',
        },
        {
          label: 'PID - P',
          data: [],
          category: 'asserv_angle',
          command: 'pid_P',
        },
        {
          label: 'PID - I',
          data: [],
          category: 'asserv_angle',
          command: 'pid_I',
        },
        {
          label: 'PID - D',
          data: [],
          category: 'asserv_angle',
          command: 'pid_D',
        },
      ]},

    };

    for(let key in this._plotData) {
      this._plotData[key].plot = $.plot(this["_chart" + key].current, this._plotData[key].data, this._plotData[key].opt);
    }
    setInterval(() => this.addPlotData(), 100);
  }
}

export default Asserv;
