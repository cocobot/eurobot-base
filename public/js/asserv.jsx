var timeOffset = 0;
var clientTimestamp = (new Date()).valueOf();
$.getJSON('http://' + document.domain + ':' + location.port + '/api/time/' + clientTimestamp, function( data ) {
  var nowTimeStamp = (new Date()).valueOf();
  var serverClientRequestDiffTime = data.diff;
  var serverTimestamp = data.serverTimestamp;
  var serverClientResponseDiffTime = nowTimeStamp - serverTimestamp;
  var responseTime = (serverClientRequestDiffTime - nowTimeStamp + clientTimestamp - serverClientResponseDiffTime )/2
  
  var syncedServerTime = new Date((new Date()).valueOf() + (serverClientResponseDiffTime - responseTime));
  timeOffset = serverClientResponseDiffTime - responseTime;
});



var Parameter = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      value: "",
      modified_value: null,
    };
  },


  componentDidMount: function() {
    utils.onReceiveCommand(this.props.command, this.handleReceive);
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  update: function() {
    if(this.props.show) {
      utils.sendCommand({command: this.props.command, argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceive: function(data) {
    this.setState({value: data.answer.data[0]});
  },

  handleChange: function(event) {
    this.setState({modified_value: event.target.value});
  },

  send: function(event) {
    event.preventDefault();

    if(this.state.modified_value != null) {
      this.setState({value: this.state.modified_value, modified_value: null});
      utils.sendCommand({command: this.props.command, argument: this.state.modified_value});
    }
  },

  cancel: function(event) {
    event.preventDefault();

    this.setState({modified_value: null});
  },

  render: function() {
    var value = this.state.value;
    var cls = "form-group form-inline pull-right";
    var btns = "btn btn-success";
    var btnc = "btn btn-default";
    if(this.state.modified_value != null) {
      value = this.state.modified_value;
      cls += " has-warning";
    }
    else {
      btns += " disabled";
      btnc += " disabled";
    }

    return (
     <div className={cls}>
        <label className="small-margin-right">{this.props.name}</label>
        <input type="text" className="form-control" value={value} onChange={this.handleChange}/>
        <button type="submit" className={btns} onClick={this.send}>
          <span className="glyphicon glyphicon-chevron-right" aria-hidden="true"></span>&nbsp;
        </button>
        <button type="submit" className={btnc} onClick={this.cancel}>
          <span className="glyphicon glyphicon-repeat" aria-hidden="true"></span>&nbsp;
        </button>
      </div>
    );
  },
});

var ChartLine = React.createClass({
  getInitialState: function() {
    return {};
  },

  componentDidMount: function() {
  },

  render: function() {
    var style = {'backgroundColor': this.props.strokeStyle};
    return (
      <div>
         <span className="badge" style={style}>{this.props.name}</span>
      </div>
    );
  },
});

var Chart = React.createClass({
  getInitialState: function() {
    this.charts = {};
    return {
      smoothie: new SmoothieChart({
        maxValueScale: 1.1,
        minValueScale: 1.1,
      }),
    }
  },

  componentDidMount() {
    this.state.smoothie.streamTo(document.getElementById(this.props.id));

    var self = this;
    React.Children.forEach(this.props.children, function(child) {
      if(child.type.displayName == "ChartLine") {
        var ser = new TimeSeries();
        self.state.smoothie.addTimeSeries(ser,{
          lineWidth: child.props.lineWidth,
          strokeStyle: child.props.strokeStyle,
        });

        self.charts[child.ref] = {timeseries: ser, last: 0};
      }
    });
  },

  append: function(data) {
    if(data.key in this.charts) {
      var c = this.charts[data.key];
      if(data.date > c.last - 500) {
 //       c.timeseries.append(data.date, undefined);
      }
      
      var dt = data.date + timeOffset / 2;

      c.timeseries.append(dt, parseFloat(data.value));
      c.last = data.date;
    }
  },

  render: function() {
    var legends = [];
    var params = [];      
    if(this.props.children != undefined) {
      legends = this.props.children.filter(function (x) { return x.type.displayName == "ChartLine";});
    }

    return(
      <div className="row">
        <div className="col-md-11">
          <canvas width={this.props.width} height={this.props.height} id={this.props.id}></canvas>
        </div>
        <div className="col-md-1">
          {legends}
        </div>
      </div>
    );
  },
});


var Asserv = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    this.val = 9;

    return {
      width: 750,
      height: 130,
      data: [],
      debugRampDistance: false,
      debugPIDDistance: false,
      debugRampAngular: false,
      debugPIDAngular: false,
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("ramp_distance_debug", this.handleRampDistanceDebug);
    utils.onReceiveCommand("ramp_distance", this.handleReceiveRampDistance);

    utils.onReceiveCommand("pid_distance_debug", this.handlePIDDistanceDebug);
    utils.onReceiveCommand("pid_distance", this.handleReceivePIDDistance);

   utils.onReceiveCommand("ramp_angular_debug", this.handleRampAngularDebug);
    utils.onReceiveCommand("ramp_angular", this.handleReceiveRampAngular);

    utils.onReceiveCommand("pid_angular_debug", this.handlePIDAngularDebug);
    utils.onReceiveCommand("pid_angular", this.handleReceivePIDAngular);

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveRampDistance: function(data) {
    var d = data.answer.data[0].split('=');
    var raw = d[1].split(",");
    var date = data.answer.date;

    this.refs.chartdp.append({
      key: "dpt",
      date: date,
      value: raw[0],
    });

    this.refs.chartdp.append({
      key: "dpc",
      date: date,
      value: raw[1],
    });

    this.refs.chartdp.append({
      key: "dpf",
      date: date,
      value: raw[2],
    });

    this.refs.chartds.append({
      key: "dst",
      date: date,
      value: raw[3],
    });

    this.refs.chartds.append({
      key: "dsc",
      date: date,
      value: raw[4],
    });
  },

  handleReceivePIDDistance: function(data) {
    var d = data.answer.data[0].split('=');
    var raw = d[1].split(",");
    var date = data.answer.date;

    this.refs.chartdpid.append({
      key: "dpido",
      date: date,
      value: raw[0],
    });

    this.refs.chartdpid.append({
      key: "dpidp",
      date: date,
      value: raw[1],
    });

    this.refs.chartdpid.append({
      key: "dpidi",
      date: date,
      value: raw[2],
    });

    this.refs.chartdpid.append({
      key: "dpidd",
      date: date,
      value: raw[3],
    });

  },

  handleRampDistanceDebug: function(data) {
    if(data.answer.data[0] == "1") {
      this.setState({debugRampDistance: true});
    }
    else {
      this.setState({debugRampDistance: false});
    }
  },

  handlePIDDistanceDebug: function(data) {
    if(data.answer.data[0] == "1") {
      this.setState({debugPIDDistance: true});
    }
    else {
      this.setState({debugPIDDistance: false});
    }
  },

  onChangeRampDistance: function(event) {
    if(event.target.checked) {
      utils.sendCommand({command: "ramp_distance_debug", argument: "1"});
    }
    else {
      utils.sendCommand({command: "ramp_distance_debug", argument: "0"});
    }
  },

  onChangePIDDistance: function(event) {
    if(event.target.checked) {
      utils.sendCommand({command: "pid_distance_debug", argument: "1"});
    }
    else {
      utils.sendCommand({command: "pid_distance_debug", argument: "0"});
    }
  },

  handleReceiveRampAngular: function(data) {
    var d = data.answer.data[0].split('=');
    var raw = d[1].split(",");
    var date = data.answer.date;

    this.refs.chartap.append({
      key: "apt",
      date: date,
      value: raw[0],
    });

    this.refs.chartap.append({
      key: "apc",
      date: date,
      value: raw[1],
    });

    this.refs.chartap.append({
      key: "apf",
      date: date,
      value: raw[2],
    });

    this.refs.chartas.append({
      key: "ast",
      date: date,
      value: raw[3],
    });

    this.refs.chartas.append({
      key: "asc",
      date: date,
      value: raw[4],
    });
  },

  handleReceivePIDAngular: function(data) {
    var d = data.answer.data[0].split('=');
    var raw = d[1].split(",");
    var date = data.answer.date;

    this.refs.chartapid.append({
      key: "apido",
      date: date,
      value: raw[0],
    });

    this.refs.chartapid.append({
      key: "apidp",
      date: date,
      value: raw[1],
    });

    this.refs.chartapid.append({
      key: "apidi",
      date: date,
      value: raw[2],
    });

    this.refs.chartapid.append({
      key: "apidd",
      date: date,
      value: raw[3],
    });

  },

  handleRampAngularDebug: function(data) {
    if(data.answer.data[0] == "1") {
      this.setState({debugRampAngular: true});
    }
    else {
      this.setState({debugRampAngular: false});
    }
  },

  handlePIDAngularDebug: function(data) {
    if(data.answer.data[0] == "1") {
      this.setState({debugPIDAngular: true});
    }
    else {
      this.setState({debugPIDAngular: false});
    }
  },

  onChangeRampAngular: function(event) {
    if(event.target.checked) {
      utils.sendCommand({command: "ramp_angular_debug", argument: "1"});
    }
    else {
      utils.sendCommand({command: "ramp_angular_debug", argument: "0"});
    }
  },

  onChangePIDAngular: function(event) {
    if(event.target.checked) {
      utils.sendCommand({command: "pid_angular_debug", argument: "1"});
    }
    else {
      utils.sendCommand({command: "pid_angular_debug", argument: "0"});
    }
  },



  update: function() {
    if(this.props.show) {
      utils.sendCommand({command: "ramp_distance_debug", argument: null});
      utils.sendCommand({command: "ramp_angular_debug", argument: null});
      utils.sendCommand({command: "pid_distance_debug", argument: null});
      utils.sendCommand({command: "pid_angular_debug", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    return (
      <div className={divClasses}>
        <div className="row">
          <div className="col-md-12 main">
            <div className="panel panel-primary">
              <div className="panel-heading">
                <div className="pull-right">
                  <form className="form-inline">
                    <div className="checkbox small-margin-right">
                      <label>
                        <input type="checkbox" checked={this.state.debugRampDistance} onChange={this.onChangeRampDistance}/>Ramp distance 
                      </label>
                    </div>
                    <div className="checkbox small-margin-right">
                      <label>
                        <input type="checkbox" checked={this.state.debugPIDDistance} onChange={this.onChangePIDDistance}/>PID distance
                      </label>
                    </div>
                    <div className="checkbox small-margin-right">
                      <label>
                        <input type="checkbox" checked={this.state.debugRampAngular} onChange={this.onChangeRampAngular}/>Ramp angular 
                      </label>
                    </div>
                    <div className="checkbox small-margin-right">
                      <label>
                        <input type="checkbox" checked={this.state.debugPIDAngular} onChange={this.onChangePIDAngular}/>PID angular
                      </label>
                    </div>
                  </form>
                </div>
                Asserv
              </div>
              <div className="panel-body">
                <div className="row">
                  <div className="col-md-8">
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
                  </div>
                  <div className="col-md-4">
                    <Parameter name="Speed" command="ramp_distance_speed" show={this.props.show}/>
                    <Parameter name="Accel" command="ramp_distance_accel" show={this.props.show}/>
                    <Parameter name="Kp" command="pid_distance_kp" show={this.props.show}/>
                    <Parameter name="Ki" command="pid_distance_ki" show={this.props.show}/>
                    <Parameter name="Kd" command="pid_distance_kd" show={this.props.show}/>
                    <Parameter name="Max I" command="pid_distance_max_i" show={this.props.show}/>
                    <Parameter name="Max E" command="pid_distance_max_e" show={this.props.show}/>
                  </div>
                </div>
                <div className="row">
                  <div className="col-md-8">
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
                  </div>
                  <div className="col-md-4">
                    <Parameter name="Speed" command="ramp_angular_speed" show={this.props.show}/>
                    <Parameter name="Accel" command="ramp_angular_accel" show={this.props.show}/>
                    <Parameter name="Kp" command="pid_angular_kp" show={this.props.show}/>
                    <Parameter name="Ki" command="pid_angular_ki" show={this.props.show}/>
                    <Parameter name="Kd" command="pid_angular_kd" show={this.props.show}/>
                    <Parameter name="Max I" command="pid_angular_max_i" show={this.props.show}/>
                    <Parameter name="Max E" command="pid_angular_max_e" show={this.props.show}/>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
