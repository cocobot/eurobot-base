import electron from 'electron';
import React from 'react';
import { 
  Container,
  Row, Col,
  Form, FormGroup, Label, Input,
  Card, CardHeader, CardBody, CardTitle, CardText,
  Badge,
} from 'reactstrap';
import { SmoothieChart, TimeSeries } from 'smoothie';

const timeOffset = 0;


class Parameter extends React.Component {

  constructor(props) {
     super(props);
     this.UPDATE_PERIOD_MS = 1000;
     this.state = {
       value: "",
       modified_value: null,
     };
  }


  //componentDidMount: function() {
  //  utils.onReceiveCommand(this.props.command, this.handleReceive);
  //  setTimeout(this.update, this.UPDATE_PERIOD_MS);
  //},

  //update: function() {
  //  if(this.props.show) {
  //    utils.sendCommand({command: this.props.command, argument: null});
  //  }

  //  setTimeout(this.update, this.UPDATE_PERIOD_MS);
  //},

  //handleReceive: function(data) {
  //  this.setState({value: data.answer.data[0]});
  //},

  //handleChange: function(event) {
  //  this.setState({modified_value: event.target.value});
  //},

  //send: function(event) {
  //  event.preventDefault();

  //  if(this.state.modified_value != null) {
  //    this.setState({value: this.state.modified_value, modified_value: null});
  //    utils.sendCommand({command: this.props.command, argument: this.state.modified_value});
  //  }
  //},

  //cancel: function(event) {
  //  event.preventDefault();

  //  this.setState({modified_value: null});
  //},

  render() {
  //  var value = this.state.value;
  //  var cls = "form-group form-inline pull-right";
  //  var btns = "btn btn-success";
  //  var btnc = "btn btn-default";
  //  if(this.state.modified_value != null) {
  //    value = this.state.modified_value;
  //    cls += " has-warning";
  //  }
  //  else {
  //    btns += " disabled";
  //    btnc += " disabled";
  //  }

    return (
     <div />
     //<div className={cls}>
     //   <label className="small-margin-right">{this.props.name}</label>
     //   <input type="text" className="form-control" value={value} onChange={this.handleChange}/>
     //   <button type="submit" className={btns} onClick={this.send}>
     //     <span className="glyphicon glyphicon-chevron-right" aria-hidden="true"></span>&nbsp;
     //   </button>
     //   <button type="submit" className={btnc} onClick={this.cancel}>
     //     <span className="glyphicon glyphicon-repeat" aria-hidden="true"></span>&nbsp;
     //   </button>
     // </div>
    );
  }
}

class Chart extends React.Component {
  constructor(props) {
    super(props);

    this.charts = {};
    this.state = {
      smoothie: new SmoothieChart({
        maxValueScale: 1.1,
        minValueScale: 1.1,
        scaleSmoothing:1,
        //yRangeFunction: (range) => {
        //  return {min: range.min, max: range.max};
        //}
      }),
    };
  }

  componentDidMount() {
    this.state.smoothie.streamTo(document.getElementById(this.props.id));

    var self = this;
    React.Children.forEach(this.props.children, function(child) {
      try {
        var ser = new TimeSeries();
        self.state.smoothie.addTimeSeries(ser,{
          lineWidth: child.props.lineWidth,
          strokeStyle: child.props.strokeStyle,
        });

        self.charts[child.ref] = {timeseries: ser, last: 0};
      }
      catch(e) {
      }
    });
  }

  append(data) {
    if(data.key in this.charts) {
      var c = this.charts[data.key];

      var dt = data.date + timeOffset / 2;
      c.timeseries.append(dt, parseFloat(data.value));
      c.last = data.date;
    }
  }

  render() {
    var legends = [];
    var params = [];      
    //if(this.props.children != undefined) {
    //  legends = this.props.children.filter(function (x) { return x.type.displayName == "ChartLine";});
    //}

    return(
      <Row>
        <Col md="11">
          <canvas width={this.props.width} height={this.props.height} id={this.props.id}></canvas>
        </Col>
        <Col md="1">
          {this.props.children}
        </Col>
      </Row>
    );
  }
}

class ChartLine extends React.Component {
  render() {
    var style = {'backgroundColor': this.props.strokeStyle};
    return (
      <div>
         <span className="badge" style={style}>{this.props.name}</span>
      </div>
    );
  }
};

class Asserv extends React.Component {
  constructor(props) {
    super(props);

    electron.ipcRenderer.on("pkt", (event, pkt) => this._handlePkt(pkt)); 

    this.state = {
      width: 600,
      height: 130,
      data: [],
      debugRampDistance: false,
      debugPIDDistance: false,
      debugRampAngular: false,
      debugPIDAngular: false,
    };
  }

  _handlePkt(pkt) {
    const now = Date.now();
    switch(pkt.data._name) {
      case "asserv_dist":
        try {
          this.refs.chartdp.append({
                                   key: "dpt",
                                   date: now,
                                   value: pkt.data.target,
          });
          this.refs.chartdp.append({
                                   key: "dpf",
                                   date: now,
                                   value: pkt.data.ramp_out,
          });
          this.refs.chartdp.append({
                                   key: "dpc",
                                   date: now,
                                   value: pkt.data.distance,
          });
          this.refs.chartds.append({
                                   key: "dst",
                                   date: now,
                                   value: pkt.data.speed_target,
          });
          this.refs.chartds.append({
                                   key: "dst",
                                   date: now,
                                   value: pkt.data.speed,
          });
          this.refs.chartdpid.append({
                                     key: "dpido",
                                     date: now,
                                     value: pkt.data.pid_out,
          });
          this.refs.chartdpid.append({
                                     key: "dpidp",
                                     date: now,
                                     value: pkt.data.pid_P,
          });
          this.refs.chartdpid.append({
                                     key: "dpidpi",
                                     date: now,
                                     value: pkt.data.pid_I,
          });
          this.refs.chartdpid.append({
                                     key: "dpidpd",
                                     date: now,
                                     value: pkt.data.pid_D,
          });
        }
    catch(e) {
    }
        break;
    }
  }

  render() {
    return (
      <Container style={{ marginTop: '15px' }}>
        <Row>
          <Col>
            <Card>
              <CardHeader>
                <Form inline className="float-right" style={{marginBottom: 0}}>
                  <FormGroup inline className="mb-2 mr-sm-2 mb-sm-0">
                    <Label check size="sm">
                      <Input type="checkbox" onChange={(e, checked) => this._onChangeDebugPathfinder(e, checked)} checked={this.props.debugPathfinder} />Debug pathfinder
                    </Label>
                  <FormGroup inline className="mb-2 mr-sm-2 mb-sm-0">
                  </FormGroup>
                    <Label check size="sm">
                      <Input type="checkbox" onChange={(e, checked) => this._onChangeDebugActionScheduler(e, checked)} checked={this.props.debugActionScheduler} />Debug strat
                    </Label>
                  </FormGroup>
                </Form>
                Asserv
              </CardHeader>
              <CardBody>
                <Row>
                  <Col md="8">
                    <Chart ref="chartdp" width={this.state.width} height={this.state.height} id="asserv_position_dist">
                      <ChartLine ref="dpt" name="Distance - Position target" strokeStyle="rgba(66, 139, 202, 1)" lineWidth="2" />
                      <ChartLine ref="dpf" name="Distance - Position filtered" strokeStyle="rgba(240, 173, 78, 1)" lineWidth="2" />
                      <ChartLine ref="dpc" name="Distance - Current position" strokeStyle="rgba(217, 83, 79, 1)" lineWidth="2" />
                    </Chart>
                    <Chart ref="chartds" width={this.state.width} height={this.state.height} id="asserv_speed_dist">
                      <ChartLine ref="dst" name="Distance - Speed target" strokeStyle="rgba(66, 139, 202, 1)" lineWidth="2" />
                      <ChartLine ref="dsc" name="Distance - Current speed" strokeStyle="rgba(240, 173, 78, 1)" lineWidth="2" />
                    </Chart>
                    <Chart ref="chartdpid" width={this.state.width} height={this.state.height} id="asserv_pid_distance">
                      <ChartLine ref="dpido" name="Distance - PID output" strokeStyle="rgba(66, 139, 202, 1)" lineWidth="2" />
                      <ChartLine ref="dpidp" name="Distance - PID p" strokeStyle="rgba(240, 173, 78, 1)" lineWidth="2" />
                      <ChartLine ref="dpidi" name="Distance - PID i" strokeStyle="rgba(217, 83, 79, 1)" lineWidth="2" />
                      <ChartLine ref="dpidd" name="Distance - PID d" strokeStyle="rgba(60, 118, 61, 1)" lineWidth="2" />
                    </Chart>
                  </Col>
                  <Col md="4">
                    <Parameter name="Speed" command="ramp_distance_speed" show={this.props.show}/>
                    <Parameter name="Accel" command="ramp_distance_accel" show={this.props.show}/>
                    <Parameter name="Kp" command="pid_distance_kp" show={this.props.show}/>
                    <Parameter name="Ki" command="pid_distance_ki" show={this.props.show}/>
                    <Parameter name="Kd" command="pid_distance_kd" show={this.props.show}/>
                    <Parameter name="Max I" command="pid_distance_max_i" show={this.props.show}/>
                    <Parameter name="Max E" command="pid_distance_max_e" show={this.props.show}/>
                  </Col>
                </Row>
                <Row>
                  <Col md="8">
                    <Chart ref="chartap" width={this.state.width} height={this.state.height} id="asserv_position_angular">
                      <ChartLine ref="apt" name="Angle - Position target" strokeStyle="rgba(66, 139, 202, 1)" lineWidth="2" />
                      <ChartLine ref="apf" name="Angle - Position filtered" strokeStyle="rgba(240, 173, 78, 1)" lineWidth="2" />
                      <ChartLine ref="apc" name="Angle - Current position" strokeStyle="rgba(217, 83, 79, 1)" lineWidth="2" />
                    </Chart>
                    <Chart ref="chartas" width={this.state.width} height={this.state.height} id="asserv_speed_angular">
                      <ChartLine ref="ast" name="Angle - Speed target" strokeStyle="rgba(66, 139, 202, 1)" lineWidth="2" />
                      <ChartLine ref="asc" name="Angle - Current speed" strokeStyle="rgba(240, 173, 78, 1)" lineWidth="2" />
                    </Chart>
                    <Chart ref="chartapid" width={this.state.width} height={this.state.height} id="asserv_pid_angular">
                      <ChartLine ref="apido" name="Angle - PID output" strokeStyle="rgba(66, 139, 202, 1)" lineWidth="2" />
                      <ChartLine ref="apidp" name="Angle - PID p" strokeStyle="rgba(240, 173, 78, 1)" lineWidth="2" />
                      <ChartLine ref="apidi" name="Angle - PID i" strokeStyle="rgba(217, 83, 79, 1)" lineWidth="2" />
                      <ChartLine ref="apidd" name="Angle - PID d" strokeStyle="rgba(60, 118, 61, 1)" lineWidth="2" />
                    </Chart>
                  </Col>
                  <Col md="4">
                    <Parameter name="Speed" command="ramp_angular_speed" show={this.props.show}/>
                    <Parameter name="Accel" command="ramp_angular_accel" show={this.props.show}/>
                    <Parameter name="Kp" command="pid_angular_kp" show={this.props.show}/>
                    <Parameter name="Ki" command="pid_angular_ki" show={this.props.show}/>
                    <Parameter name="Kd" command="pid_angular_kd" show={this.props.show}/>
                    <Parameter name="Max I" command="pid_angular_max_i" show={this.props.show}/>
                    <Parameter name="Max E" command="pid_angular_max_e" show={this.props.show}/>
                  </Col>
                </Row>
              </CardBody>
            </Card>
          </Col>
        </Row>
      </Container>
    );
  }
}

export default Asserv;

