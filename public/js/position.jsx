var cSkyBlue = "rgb(0, 124, 176)";
var cTrafficYellow = "rgb(247, 181, 0)";
var cSignalViolet = "rgb(132, 76, 130)";
var cEmeraldGreen = "rgb(54, 103, 53)";
var cTrafficWhite = "rgb(241, 240, 234)";
var cPebbleGrey = "rgb(181, 176, 161)";
var cDaffodilYellow = "rgb(232, 140, 0)";

var TrajectoryOrders = React.createClass({
  renderOrder: function(order, key) {
    var args = '';

    switch(order.type)
    {
      case 'D':
        args = 'distance = ' + order.args[2];
        if(order.args[1] != '-1.000') {
          args += ' - temps = ' + order.args[1];
        }
        break;

      case 'A':
        args = 'angle = ' + order.args[2];
        if(order.args[1] != '-1.000') {
          args += ' - temps = ' + order.args[1];
        }
        break;

      default:
        args = '? (' + order.args.join(' ') + ')';
        break;
    }

    return (
      <tr key={key}>
        <td>{order.type}</td>
        <td>{args}</td>
        <td></td>
      </tr>
    );
  },
  
  render: function() {
    return (
      <div className="panel panel-default">
        <div className="panel-heading">
          Gestion de la trajectoire
        </div>
        <div className="panel-body">
          <table className="table table-bordered table-condensed">
              <thead>
                <th>Type</th>
                <th>Arguments</th>
                <th>Flags</th>
              </thead>
              <tbody>
                {this.props.orders.map(this.renderOrder)}
              </tbody>
          </table>
        </div>
      </div>
    );
  },
});

var Robot2016Umbrella = React.createClass({
  UPDATE_PERIOD_MS: 1000,
  COLOR: "rgb(255, 8, 127)",

  getInitialState: function() {
    return {
      state: 'close',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("meca_umbrella", this.handleReceiveUmbrellaState);
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  update: function() {
    if(this.props.update) {
      utils.sendCommand({command: "meca_umbrella", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveUmbrellaState: function(data) {
    this.setState({state: data.answer.data[0]});
  },

  render: function() {
    var shape = null;

    if(this.state.state == 'close') {
      shape = (
       <g>
        <rect height="10" width="75" x="-5" y="" fill={this.COLOR} strokeWidth="4" stroke="#000000" />
       </g>
      );
    }
    else {
      var cone = [];

      for(var i = 0; i < 8; i += 1) {
        var rotate = "rotate(" + (45 * i) + ")";
        cone.push(<path key={cone.length} d="M0 0 L35 0 A35 35 0 0 1 20.506 20.506 Z" fill={this.COLOR} strokeWidth="4" stroke="#000000" transform={rotate}/>);
      }

      shape = (
       <g>
        {cone}
       </g>
      );
    }

    return (
      <g>
        {shape}
      </g>
    );
  },
});

var Robot = React.createClass({
  getInitialState: function() {
    return {
      name: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("info", this.handleReceiveInfo);
  },

  handleReceiveInfo: function(data) {
    if(data.answer.data.length >= 2) {
      this.setState({
        name: data.answer.data[0],
      });
    }
  },

  render: function() {
    var shape = null;
    var meca = [];

    if(this.state.name == "Robot secondaire")
    {
      shape = (
        <g transform="rotate(90) translate(-100, -75)">
          <rect height="150" width="200" fill="#000000" />
        </g>
      );

      meca.push(<Robot2016Umbrella key={meca.length} update={this.props.update} />);
    }
    else
    {
      shape = (
        <g>
          <circle cx="0" cy="0" r="100" strokeWidth="10" stroke="#FF0000"/>
          <text fontSize="60" y="20" fill="#FFFFFF">
            <tspan textAnchor="middle">?</tspan>
          </text>
        </g>
      );
    }

    var paths = [];
    var pathd = null;
    for(var i = 0; i < this.props.orders.length; i += 1) {
      var o = this.props.orders[i];
      if(pathd == null) {
        pathd = "M" + o.start.x + " " + o.start.y;
      }
      pathd += " L" + o.end.x + " " + o.end.y;

      var generateTurnPath = function(x, y, start, end, key) {
        x = parseFloat(x);
        y = parseFloat(y);
        start = parseFloat(start);
        end = parseFloat(end);

        var r = 50;
        var x0 = x + r * Math.cos(start * Math.PI / 180.0);
        var y0 = y + r * Math.sin(start * Math.PI / 180.0);
        var x1 = x + r * Math.cos(end * Math.PI / 180.0);
        var y1 = y + r * Math.sin(end * Math.PI / 180.0);

        var nend = end;
        var change = true;
        while(change) {
          change = false;
          if(Math.abs(nend + 360 - start) < Math.abs(end -start)) {
            nend = nend + 360;
            change = true;
          }
          if(Math.abs(nend - 360 - start) < Math.abs(end -start)) {
            nend = nend - 360;
            change = true;
          }
        }

        var sns = 0;
        if(start < nend) {
          sns = 1;
        }

        var pathc = "M" + x0 + " " + y0 + "A" + r + " " + r + " 0 0 " + sns + " " + x1 + " " + y1;
        //var pathc = "M" + x0 + " " + y0 + "L" + x1 + " " + y1;

        return <path key={key} d={pathc} strokeWidth="8" stroke="#FF0000" fill="rgba(0, 0, 0, 0)" />;
      };

      if((o.start.x == o.end.x) && (o.start.y == o.end.y)) {         
        if(o.start.a!= o.end.a) {
          paths.push(generateTurnPath(o.start.x, o.start.y, o.start.a, o.end.a, paths.length));
        }
      }
    }
    paths.push(<path key={paths.length} d={pathd} strokeWidth="8" stroke="#FF0000" fill="rgba(0, 0, 0, 0)" />);

    var position = "translate(" + this.props.x + "," + this.props.y + ") rotate(" + this.props.a + ")";
    return (
      <g>
        <g transform="translate(1500,1000) scale(1, -1)">
          <g opacity="0.8" transform={position}>
            {shape}
            {meca}
          </g>
          {paths}
        </g>
      </g>
    );
  },
});

var Eurobot2016Sand = React.createClass({
  render: function() {
    var scale = 0;

    switch(this.props.z) {
      case "0":
        scale = 1;
        break;

     case "1":
        scale = 0.9;
        break;
     
     case "2":
        scale = 0.75;
        break;

     case "3":
        scale = 0.60;
        break;

     case "4":
        scale = 0.45;
        break;
    }

    var translate = "translate(" + this.props.x + "," + this.props.y + ") scale(" + scale + ")";
    var opacity = 1;

    var r = [];
    if(this.props.type == "cube") {
      r = [<rect key="0" height="58" width="58" x="-29" y="-29" fill={cDaffodilYellow} strokeWidth="4" stroke="#000000" />];
    }
    if(this.props.type == "cylinder") {
      r = [<circle key="0" cx="0" xy="0" r="29" fill={cDaffodilYellow} strokeWidth="4" stroke="#000000" />];
    }
    if(this.props.type == "cone") {
      for(var i = 0; i < 8; i += 1) {
        var rotate = "rotate(" + (45 * i) + ")";
        r.push(<path key={r.length} d="M0 0 L29 0 A29 29 0 0 1 20.506 20.506 Z" fill={cDaffodilYellow} strokeWidth="4" stroke="#000000" transform={rotate}/>);
      }
    }

    return (
      <g>
        <g transform={translate} opacity={opacity}>
        {r}
        </g>
      </g>
    );
  }
});

var Eurobot2016Hut = React.createClass({
  render: function() {
    colors = [];

    var x = 0;
    var y = 0;
    var scale = '';

    switch(this.props.id) {
      case "0":
        x = 300;
        y = 0;
        break;

      case "1":
        x = 600;
        y = 0;
        break;

      case "2":
        x = 2400;
        y = 0;
        scale = 'scale(-1,1)';
        break;

      case "3":
        x = 2700;
        y = 0;
        scale = 'scale(-1,1)';
        break;
    }

    var translate = "translate(" + x + "," + y + ")";
    var opacity = 0.8;

    return (
      <g>
        <g transform={translate} opacity={opacity}>
          <g transform={scale}>
            <rect height="25" width="50" x="-50" y="-25" fill={cSkyBlue} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="0" y="-25" fill={cSkyBlue} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="-50" y="-75" fill={cSkyBlue} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="0" y="-75" fill={cSkyBlue} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="-50" y="-50" fill={cTrafficWhite} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="0" y="-50" fill={cTrafficWhite} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="-50" y="-100" fill={cTrafficWhite} strokeWidth="4" stroke="#000000" />
            <rect height="25" width="50" x="0" y="-100" fill={cTrafficWhite} strokeWidth="4" stroke="#000000" />
            <g transform="translate(-61,-11) rotate(45)">
              <rect height="10" width="25" x="10.5" y="11" fill={cTrafficWhite} strokeWidth="4" stroke="#000000" />
              <rect height="10" width="25" x="35.5" y="11" fill={cSkyBlue} strokeWidth="4" stroke="#000000" />
              <rect height="10" width="25" x="60.5" y="11" fill={cTrafficWhite} strokeWidth="4" stroke="#000000" />
              <rect height="10" width="25" x="85.5" y="11" fill={cSkyBlue} strokeWidth="4" stroke="#000000" />
            </g>
          </g>
          <text fontSize="60" y="50">
            <tspan textAnchor="middle">{this.props.id}</tspan>
          </text>
        </g>
      </g>
    );
  }
});

var Eurobot2016SeeShell = React.createClass({
  getColor: function(conf, id) {
    switch(conf) {
      case "1":
        switch(id) {
          case "0":
          case "3":
          case "6":
          case "8":
          case "10":
          case "16":
            return cTrafficWhite;

          case "4":
          case "12":
          case "13":
          case "18":
          case "19":
            return cSignalViolet;

          case "5":
          case "11":
          case "14":
          case "20":
          case "21":
            return cEmeraldGreen;

        }
        break;

      case "2":
        switch(id) {
          case "6":
          case "8":
          case "10":
          case "16":
          case "18":
          case "21":
            return cTrafficWhite;

          case "0":
          case "4":
          case "11":
          case "13":
          case "19":
            return cSignalViolet;

          case "3":
          case "5":
          case "12":
          case "14":
          case "20":
            return cEmeraldGreen;

        }
        break;

      case "3":
        switch(id) {
          case "6":
          case "7":
          case "9":
          case "10":
          case "18":
          case "21":
            return cTrafficWhite;

          case "0":
          case "1":
          case "11":
          case "13":
          case "19":
            return cSignalViolet;

          case "2":
          case "3":
          case "12":
          case "14":
          case "20":
            return cEmeraldGreen;

        }
        break;

      case "4":
        switch(id) {
          case "7":
          case "9":
          case "11":
          case "12":
          case "18":
          case "21":
            return cTrafficWhite;

          case "0":
          case "1":
          case "6":
          case "14":
          case "20":
            return cSignalViolet;

          case "2":
          case "3":
          case "10":
          case "13":
          case "19":
            return cEmeraldGreen;

        }
        break;

      case "5":
        switch(id) {
          case "15":
          case "17":
          case "18":
          case "19":
          case "20":
          case "21":
            return cTrafficWhite;

          case "0":
          case "1":
          case "6":
          case "9":
          case "13":
            return cSignalViolet;

          case "2":
          case "3":
          case "7":
          case "10":
          case "14":
            return cEmeraldGreen;

        }
        break;
    }

    return null;
  },

  render: function() {

    var x = 0;
    var y = 0;

    switch(this.props.id) {
      case "0":
        x = 200;
        y = 1250;
        break;

      case "1":
        x = 700;
        y = 1250;
        break;

      case "2":
        x = 2300;
        y = 1250;
        break;

      case "3":
        x = 2800;
        y = 1250;
        break;

      case "4":
        x = 900;
        y = 1450;
        break;

      case "5":
        x = 2100;
        y = 1450;
        break;

      case "6":
        x = 200;
        y = 1550;
        break;

      case "7":
        x = 700;
        y = 1550;
        break;

      case "8":
        x = 1500;
        y = 1550;
        break;

      case "9":
        x = 2300;
        y = 1550;
        break;

      case "10":
        x = 2800;
        y = 1550;
        break;

      case "11":
        x = 1200;
        y = 1650;
        break;

      case "12":
        x = 1800;
        y = 1650;
        break;

      case "13":
        x = 70;
        y = 1810;
        break;

      case "14":
        x = 2930;
        y = 1810;
        break;

      case "15":
        x = 700;
        y = 1850;
        break;

      case "16":
        x = 1500;
        y = 1850;
        break;

      case "17":
        x = 2300;
        y = 1850;
        break;

      case "18":
        x = 70;
        y = 1930;
        break;

      case "19":
        x = 190;
        y = 1930;
        break;

      case "20":
        x = 2810;
        y = 1930;
        break;

      case "21":
        x = 2930;
        y = 1930;
        break;
    }

    colors = [];
    var transparent = "rgba(255, 255, 255, 0)";
    for(var i = 0; i < this.props.conf.length; i += 1) {
      var color = this.getColor(this.props.conf[i], this.props.id);
      if(color == null) {
        color = transparent;
      }
      colors.push(color);
    }
    colors.sort();

    var translate = "translate(" + x + "," + y + ")";
    var render = [];
    var opacity = 0.8;
    if((colors.length == 0) || ((colors.length == 1) && (colors[0] == transparent))) {
      opacity = 0.3;
    }
    else if(colors.length == 1) {
      render.push(<circle key={render.length} r="36" strokeWidth="4" stroke="#000000" fill="rgba(0, 0, 0, 0)"/>);
      render.push(<circle key={render.length} r="34" fill={colors[0]}/>);
    }
    else {
      render.push(<circle key={render.length} r="36" strokeWidth="4" stroke="#000000" fill="rgba(0, 0, 0, 0)"/>);
      for(var i = 0; i < colors.length; i += 1) {
        var d = "M0 0 L34 0 A34 34 0 0 1 " + (34 * Math.cos(2 * Math.PI / colors.length)) + " " + (34 * Math.sin(2 * Math.PI / colors.length));
        var r = "rotate(" + (i * 360 / colors.length) + ")"
        render.push(<path key={render.length} d={d} fill={colors[i]} transform={r} strokeWidth="2"/>);
      }
    }

    return (
      <g>
        <g transform={translate} opacity={opacity}>
          {render}
          <text fontSize="60" y="21">
            <tspan textAnchor="middle">{this.props.id}</tspan>
          </text>
        </g>
      </g>
    );
  }
});

var Eurobot2016 = React.createClass({

  renderBorders: function() {
    var borders = [];

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="-22" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="1222" width="22" x="-22" y="-22" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="822" width="22" x="-22" y="1200" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="1222" width="22" x="3000" y="-22" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="822" width="22" x="3000" y="1200" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="2000" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);


    return borders;
  },

  renderFieldColor: function() {
    var fields = [];

    fields.push(<rect key={fields.length} height="2000" width="3000" x="0" y="00" fill={cTrafficYellow} />);
    fields.push(<path key={fields.length} d="M0 1200 L600 1200 L600 1275 L2400 1275 L2400 1200 L3000 1200 L3000 2000 L0 2000 Z" fill={cSkyBlue} />);
    fields.push(<circle key={fields.length} cx="600" cy="1550" r="350" fill={cSkyBlue}/>);
    fields.push(<circle key={fields.length} cx="2400" cy="1550" r="350" fill={cSkyBlue}/>);
    fields.push(<path key={fields.length} d="M350 350 A1150 1150 0 0 0 2650 350 Z" fill={cTrafficYellow}/>);

    fields.push(<path key={fields.length} d="M900 750 A600 600 0 0 0 1500 1350 L1500 1328 A578 578 0 0 1 922 750 Z" fill={cSignalViolet} />);
    fields.push(<path key={fields.length} d="M2100 750 A600 600 0 0 1 1500 1350 L1500 1328 A578 578 0 0 0 2078 750 Z" fill={cEmeraldGreen} />);

    return fields;
  },

  renderStartingAreas: function() {
    var start = [];

    start.push(<rect key={start.length} height="500" width="300" x="0" y="600" fill={cSignalViolet} />);
    start.push(<rect key={start.length} height="500" width="300" x="2700" y="600" fill={cEmeraldGreen} />);
    start.push(<rect key={start.length} height="50" width="300" x="0" y="650" fill={cTrafficWhite} />);
    start.push(<rect key={start.length} height="50" width="300" x="0" y="1000" fill={cTrafficWhite} />);
    start.push(<rect key={start.length} height="50" width="300" x="2700" y="1000" fill={cTrafficWhite} />);
    start.push(<rect key={start.length} height="50" width="300" x="2700" y="650" fill={cTrafficWhite} />);

    return start;
  },

  renderObstacles: function() {
    var obstacles = [];

    obstacles.push(<rect key={obstacles.length} height="200" width="22" x="800" y="0" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="200" width="22" x="2178" y="0" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);

    obstacles.push(<rect key={obstacles.length} height="22" width="1200" x="900" y="750" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="600" width="48" x="1476" y="750" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="48" x="1476" y="750" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1476" y="772" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1502" y="772" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="48" x="1476" y="1328" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1476" y="1306" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1502" y="1306" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="556" width="1" x="1500" y="772" fill={cDaffodilYellow} strokeWidth="2" stroke="#000000"/>);

    obstacles.push(<rect key={obstacles.length} height="254" width="22" x="928" y="1978" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="254" width="22" x="2050" y="1978" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);

    obstacles.push(<path key={obstacles.length} d="M0 2000 L250 2000 A250 250 0 0 0 0 1750 Z" fill={cPebbleGrey} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<path key={obstacles.length} d="M0 2000 L150 2000 A150 150 0 0 0 0 1850 Z" fill={cPebbleGrey} strokeWidth="2" stroke="#000000"/>);

    obstacles.push(<path key={obstacles.length} d="M3000 2000 L2750 2000 A250 250 0 0 1 3000 1750 Z" fill={cPebbleGrey} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<path key={obstacles.length} d="M3000 2000 L2850 2000 A150 150 0 0 1 3000 1850 Z" fill={cPebbleGrey} strokeWidth="2" stroke="#000000"/>);

    return obstacles;
  },


  render: function() {
    var seeShellConf = ["1", "2", "3", "4", "5"];

    return (
      <g>
        {this.renderFieldColor()}
        {this.renderStartingAreas()}
        {this.renderBorders()}
        {this.renderObstacles()}
        <Eurobot2016SeeShell id="0"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="1"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="2"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="3"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="4"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="5"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="6"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="7"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="8"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="9"  conf={seeShellConf}/>
        <Eurobot2016SeeShell id="10" conf={seeShellConf} />
        <Eurobot2016SeeShell id="11" conf={seeShellConf} />
        <Eurobot2016SeeShell id="12" conf={seeShellConf} />
        <Eurobot2016SeeShell id="13" conf={seeShellConf} />
        <Eurobot2016SeeShell id="14" conf={seeShellConf} />
        <Eurobot2016SeeShell id="15" conf={seeShellConf} />
        <Eurobot2016SeeShell id="16" conf={seeShellConf} />
        <Eurobot2016SeeShell id="17" conf={seeShellConf} />
        <Eurobot2016SeeShell id="18" conf={seeShellConf} />
        <Eurobot2016SeeShell id="19" conf={seeShellConf} />
        <Eurobot2016SeeShell id="20" conf={seeShellConf} />
        <Eurobot2016SeeShell id="21" conf={seeShellConf} />

        <Eurobot2016Hut id="0" />
        <Eurobot2016Hut id="1" />
        <Eurobot2016Hut id="2" />
        <Eurobot2016Hut id="3" />


        <Eurobot2016Sand type="cube" x="679" y="871" z="0"/>
        <Eurobot2016Sand type="cube" x="679" y="929" z="0"/>
        <Eurobot2016Sand type="cube" x="621" y="871" z="0"/>
        <Eurobot2016Sand type="cube" x="621" y="929" z="0"/>
        <Eurobot2016Sand type="cylinder" x="650" y="900" z="1"/>
        <Eurobot2016Sand type="cone" x="650" y="900" z="2"/>


        <Eurobot2016Sand type="cube" x="2379" y="871" z="0"/>
        <Eurobot2016Sand type="cube" x="2379" y="929" z="0"/>
        <Eurobot2016Sand type="cube" x="2321" y="871" z="0"/>
        <Eurobot2016Sand type="cube" x="2321" y="929" z="0"/>
        <Eurobot2016Sand type="cylinder" x="2350" y="900" z="1"/>
        <Eurobot2016Sand type="cone" x="2350" y="900" z="2"/>


        <Eurobot2016Sand type="cube" x="851" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="851" y="87" z="0"/>
        <Eurobot2016Sand type="cube" x="909" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="909" y="87" z="0"/>
        <Eurobot2016Sand type="cube" x="851" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="851" y="87" z="1"/>
        <Eurobot2016Sand type="cube" x="909" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="909" y="87" z="1"/>
        <Eurobot2016Sand type="cylinder" x="880" y="58" z="2"/>
        <Eurobot2016Sand type="cone" x="880" y="58" z="3"/>


        <Eurobot2016Sand type="cube" x="2149" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="2149" y="87" z="0"/>
        <Eurobot2016Sand type="cube" x="2091" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="2091" y="87" z="0"/>
        <Eurobot2016Sand type="cube" x="2149" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="2149" y="87" z="1"/>
        <Eurobot2016Sand type="cube" x="2091" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="2091" y="87" z="1"/>
        <Eurobot2016Sand type="cylinder" x="2120" y="58" z="2"/>
        <Eurobot2016Sand type="cone" x="2120" y="58" z="3"/>


        <Eurobot2016Sand type="cube" x="1268" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1326" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1384" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1442" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1500" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1558" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1616" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1674" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1732" y="29" z="0"/>
        <Eurobot2016Sand type="cube" x="1442" y="87" z="0"/>
        <Eurobot2016Sand type="cube" x="1500" y="87" z="0"/>
        <Eurobot2016Sand type="cube" x="1558" y="87" z="0"/>
        <Eurobot2016Sand type="cylinder" x="1500" y="145" z="0"/>

        <Eurobot2016Sand type="cylinder" x="1326" y="29" z="1"/>
        <Eurobot2016Sand type="cylinder" x="1384" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="1442" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="1500" y="29" z="1"/>
        <Eurobot2016Sand type="cube" x="1558" y="29" z="1"/>
        <Eurobot2016Sand type="cylinder" x="1616" y="29" z="1"/>
        <Eurobot2016Sand type="cylinder" x="1674" y="29" z="1"/>
        <Eurobot2016Sand type="cylinder" x="1442" y="87" z="1"/>
        <Eurobot2016Sand type="cube" x="1500" y="87" z="1"/>
        <Eurobot2016Sand type="cylinder" x="1558" y="87" z="1"/>

        <Eurobot2016Sand type="cylinder" x="1384" y="29" z="2"/>
        <Eurobot2016Sand type="cylinder" x="1442" y="29" z="2"/>
        <Eurobot2016Sand type="cylinder" x="1500" y="29" z="2"/>
        <Eurobot2016Sand type="cylinder" x="1558" y="29" z="2"/>
        <Eurobot2016Sand type="cylinder" x="1616" y="29" z="2"/>
        <Eurobot2016Sand type="cylinder" x="1500" y="87" z="2"/>

        <Eurobot2016Sand type="cone" x="1384" y="29" z="3"/>
        <Eurobot2016Sand type="cylinder" x="1442" y="29" z="3"/>
        <Eurobot2016Sand type="cylinder" x="1500" y="29" z="3"/>
        <Eurobot2016Sand type="cylinder" x="1558" y="29" z="3"/>
        <Eurobot2016Sand type="cone" x="1616" y="29" z="3"/>

        <Eurobot2016Sand type="cone" x="1442" y="29" z="4"/>
        <Eurobot2016Sand type="cone" x="1500" y="29" z="4"/>
        <Eurobot2016Sand type="cone" x="1558" y="29" z="4"/>

      </g>
    );
  }
});

var Field = React.createClass({
  render: function() {
    var xmin = -22;
    var ymin = -22;
    var xmax = 3022;
    var ymax = 2022;

    var margin = 200;

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

    var vBvalue = xmin + " " + ymin + " " + (xmax - xmin) + " " + (ymax - ymin);
    return (
      <svg viewBox={vBvalue} width="680px">
      {this.props.children}
      </svg>
    );
  }
});

var Position = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      x: 0,
      y: 0,
      a: 0,
      orders: [],
    }
  },

  onChange: function(event) {
    this.setState({command: event.target.value});
  },

  componentDidMount: function() {
    utils.onReceiveCommand("position", this.handleReceivePositionDebug);
    utils.onReceiveCommand("trajectory_list", this.handleReceiveTrajectoryList);
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  update: function() {
    if(this.props.show) {
      utils.sendCommand({command: "position_debug", argument: "1"});
      utils.sendCommand({command: "trajectory_list", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceivePositionDebug: function(data) {
    if(!this.props.show) {
      utils.sendCommand({command: "position_debug", argument: "0"});
    }
    var d = data.answer.data[0].split('=');
    var raw = d[1].split(",");
    this.setState({
      x: raw[0],
      y: raw[1],
      a: raw[2],
    });
  },

  handleReceiveTrajectoryList: function(data) {
    var orders = [];
    for(var i = 0; i < data.answer.data.length; i += 3) {
      var o = data.answer.data[i].split(',');
      var s = data.answer.data[i + 1].split(',');
      var e = data.answer.data[i + 2].split(',');
      var type = o[0];
      o.shift();
      orders.push({
        type: type,
        args: o,
        start: {
          x: s[0],
          y: s[1],
          a: s[2]
        },
        end: {
          x: e[0],
          y: e[1],
          a: e[2]
        },
      });
    }

    this.setState({orders: orders});
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
                Position
              </div>
              <div className="panel-body">
                <div className="row">
                  <div className="col-md-8">
                    <Field x={this.state.x} y={this.state.y}>
                      <Eurobot2016 />
                      <Robot update={this.props.show} x={this.state.x} y={this.state.y} a={this.state.a} orders={this.state.orders}/>
                    </Field>
                  </div>
                  <div className="col-md-4">
                    <TrajectoryOrders orders={this.state.orders} />
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
