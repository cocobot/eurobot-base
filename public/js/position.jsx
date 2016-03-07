var cSkyBlue = "rgb(0, 124, 176)";
var cTrafficYellow = "rgb(247, 181, 0)";
var cSignalViolet = "rgb(132, 76, 130)";
var cEmeraldGreen = "rgb(54, 103, 53)";
var cTrafficWhite = "rgb(241, 240, 234)";

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
    colors = [];

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

    if(this.props.conf == "0") {
      for(var i = 1; i < 6; i += 1) {
        var color = this.getColor(i + "", this.props.id);
        if(color != null) {
          if(colors.indexOf(color) < 0) {
            colors.push(color);
          }
        }
      }
    }
    else {
      var color = this.getColor(this.props.conf, this.props.id);
      if(color != null) {
        colors.push(color);
      }
    }
    
    var translate = "translate(" + x + "," + y + ")";
    var render = [];
    var opacity = 0.8;
    if(colors.length == 3) {
      render = [
        <circle key="0" r="38" />,
        <path key="1" d="M0 0 L34 0 A34 34 0 0 1 -17 29.445" fill={colors[0]} transform="rotate(0)" strokeWidth="2"/>,
        <path key="2" d="M0 0 L34 0 A34 34 0 0 1 -17 29.445" fill={colors[1]} transform="rotate(120)" strokeWidth="2"/>,
        <path key="3" d="M0 0 L34 0 A34 34 0 0 1 -17 29.445" fill={colors[2]} transform="rotate(240)" strokeWidth="2"/>

        ];
    }
    else if(colors.length == 2) {
      render = [
        <circle key="0" r="38" />,
        <path key="1" d="M0 0 L34 0 A34 34 0 0 1 -34 0" fill={colors[0]} transform="rotate(0)" strokeWidth="2"/>,
        <path key="2" d="M0 0 L34 0 A34 34 0 0 1 -34 0" fill={colors[1]} transform="rotate(180)" strokeWidth="2"/>
        ];
    }
    else if(colors.length == 1) {
      render = [
        <circle key="0" r="38" />,
        <circle key="1" r="34" fill={colors[0]}/>
      ];
    }
    else {
      opacity = 0.3;
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
    fields.push(<circle key={fields.length} cx="1500" cy="350" r="1150" fill={cTrafficYellow}/>);

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

    obstacles.push(<rect key={obstacles.length} height="200" width="22" x="800" y="0" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="200" width="22" x="2178" y="0" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);

    obstacles.push(<rect key={obstacles.length} height="22" width="1200" x="900" y="750" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="600" width="48" x="1476" y="750" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="48" x="1476" y="750" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1476" y="772" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1502" y="772" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="48" x="1476" y="1328" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1476" y="1306" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="22" width="22" x="1502" y="1306" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="556" width="1" x="1500" y="772" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);

    obstacles.push(<rect key={obstacles.length} height="254" width="22" x="928" y="1978" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);
    obstacles.push(<rect key={obstacles.length} height="254" width="22" x="2050" y="1978" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);

    return obstacles;
  },


  render: function() {
    var seeShellConf = "0";

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
      </g>
    );
  }
});

var Field = React.createClass({
  render: function() {
    return (
      <svg viewBox="-22 -22 3044 2044" width="680px">
      {this.props.children}
      </svg>
    );
  }
});

var Position = React.createClass({
  getInitialState: function() {
    return {
    }
  },

  onChange: function(event) {
    this.setState({command: event.target.value});
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
                <Field>
                  <Eurobot2016 />
                </Field>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
