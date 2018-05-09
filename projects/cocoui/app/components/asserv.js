import electron from 'electron';
import React from 'react';
import { 
  Container,
  Row, Col,
  Form, FormGroup, Label,
  Card, CardHeader, CardBody, CardTitle, CardText,
  Badge,
  InputGroup, InputGroupAddon, InputGroupText, Input,
} from 'reactstrap';
import { SmoothieChart, TimeSeries } from 'smoothie';
import { connect } from 'react-redux';

const ipcRenderer = electron.ipcRenderer;

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


  handleChange(event) {
    event.preventDefault();
    this.setState({modified_value: event.target.value});
  }

  handleKeyPress(event) {
    if (event.key === 'Enter') {
      this.props.active.map((x, key) => {
        ipcRenderer.send('pkt', {
          pid: 0x800E,
          fmt: "BF",
          args: [this.props.id, parseFloat(this.state.modified_value)],
          client: key, 
        });
      });
      this.setState({modified_value: null});
    }
  }

  render() {
    var value = '';
    var uartValue = '';

    if(this.props.data != null) {
      if(this.props.data.length > 0) {
        const data = this.props.data[0];
        value = data.get(this.props.command);
        uartValue = value;
      }
    }


  //  var cls = "form-group form-inline pull-right";
  //  var btns = "btn btn-success";
  //  var btnc = "btn btn-default";
    if(this.state.modified_value != null) {
      value = this.state.modified_value;
  //    cls += " has-warning";
    }
  //  else {
  //    btns += " disabled";
  //    btnc += " disabled";
  //  }

    return (
     <InputGroup size="sm" >
      <InputGroupAddon addonType="prepend">{this.props.name}</InputGroupAddon>
      <Input placeholder={"current value: " + uartValue} value={value} onChange={(e) => this.handleChange(e)} onKeyPress={(e) => this.handleKeyPress(e)} />
     </InputGroup>
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
      <Container style={{ marginTop: '5px' }}>
        <Row>
           {this.props.children}
        </Row>

        <Row>
          <Col md="11">
            <canvas width={this.props.width} height={this.props.height} id={this.props.id}></canvas>
          </Col>
        </Row>
      </Container>
    );
  }
}

class ChartLine extends React.Component {
  render() {
    var style = {'backgroundColor': this.props.strokeStyle};
    return (
      <Col md="3">
        <span className="badge" style={style}>{this.props.name}</span>
      </Col>
    );
  }
};

class AsservComponent extends React.Component {
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

    setInterval(() => {

      this.props.active.map((x, key) => {
        ipcRenderer.send('pkt', {
          pid: 0x800C,
          fmt: "",
          args: [],
          client: key, 
        });
      });
    }, 2000);
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

      case "asserv_angle":
        try {
          this.refs.chartap.append({
                                   key: "apt",
                                   date: now,
                                   value: pkt.data.target,
          });
          this.refs.chartap.append({
                                   key: "apf",
                                   date: now,
                                   value: pkt.data.ramp_out,
          });
          this.refs.chartap.append({
                                   key: "apc",
                                   date: now,
                                   value: pkt.data.distance,
          });
          this.refs.chartas.append({
                                   key: "ast",
                                   date: now,
                                   value: pkt.data.speed_target,
          });
          this.refs.chartas.append({
                                   key: "ast",
                                   date: now,
                                   value: pkt.data.speed,
          });
          this.refs.chartapid.append({
                                     key: "apido",
                                     date: now,
                                     value: pkt.data.pid_out,
          });
          this.refs.chartapid.append({
                                     key: "apidp",
                                     date: now,
                                     value: pkt.data.pid_P,
          });
          this.refs.chartapid.append({
                                     key: "apidpi",
                                     date: now,
                                     value: pkt.data.pid_I,
          });
          this.refs.chartapid.append({
                                     key: "apidpd",
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
                Asserv
              </CardHeader>
              <CardBody>
                <Row>
                  <Col md="9">
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
                  <Col md="3">
                    <Parameter active={this.props.active} id={0} name="Speed" data={this.props.asservs} command="d_ramp_speed" show={this.props.show}/>
                    <Parameter active={this.props.active} id={1} name="Accel" data={this.props.asservs} command="d_ramp_accel" show={this.props.show}/>
                    <Parameter active={this.props.active} id={2} name="Kp" data={this.props.asservs} command="d_pid_kp" show={this.props.show}/>
                    <Parameter active={this.props.active} id={3} name="Kd" data={this.props.asservs} command="d_pid_kd" show={this.props.show}/>
                    <Parameter active={this.props.active} id={4} name="Ki" data={this.props.asservs} command="d_pid_ki" show={this.props.show}/>
                    <Parameter active={this.props.active} id={5} name="Max I" data={this.props.asservs} command="d_pid_max_i" show={this.props.show}/>
                    <Parameter active={this.props.active} id={6} name="Max E" data={this.props.asservs} command="d_pid_max_e" show={this.props.show}/>
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
                    <Parameter active={this.props.active} id={7} name="Speed" data={this.props.asservs} command="a_ramp_speed" show={this.props.show}/>
                    <Parameter active={this.props.active} id={8} name="Accel" data={this.props.asservs} command="a_ramp_accel" show={this.props.show}/>
                    <Parameter active={this.props.active} id={9} name="Kp" data={this.props.asservs} command="a_pid_kp" show={this.props.show}/>
                    <Parameter active={this.props.active} id={10} name="Kd" data={this.props.asservs} command="a_pid_kd" show={this.props.show}/>
                    <Parameter active={this.props.active} id={11} name="Ki" data={this.props.asservs} command="a_pid_ki" show={this.props.show}/>
                    <Parameter active={this.props.active} id={12} name="Max I" data={this.props.asservs} command="a_pid_max_i" show={this.props.show}/>
                    <Parameter active={this.props.active} id={13} name="Max E" data={this.props.asservs} command="a_pid_max_e" show={this.props.show}/>
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

const mapStateToProps = (state, ownProps) => {
  const asservs = [];
  state.conns.get('active').map((x, key) => {
    const asserv = state.robots.getIn([key, 'asserv_params'], new Map());
    if(asserv != null) {
      asservs.push(asserv);
    }
  });
  return {
    active: state.conns.get('active'),
    asservs: asservs,
  }
}

const Asserv = connect(
  mapStateToProps,
  null,
)(AsservComponent);



export default Asserv;

