import React from 'react';

const cSkyBlue = "rgb(0, 124, 176)";
const cTrafficYellow = "rgb(247, 181, 0)";
const cSignalViolet = "rgb(132, 76, 130)";
const cEmeraldGreen = "rgb(54, 103, 53)";
const cTrafficWhite = "rgb(241, 240, 234)";
const cDaffodilYellow = "rgb(232, 140, 0)";
const cPebbleGrey = "rgb(181, 176, 161)";


class Eurobot2016Sand extends React.Component {
  render() {
    let scale = 0;

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

    const translate = "translate(" + this.props.x + "," + this.props.y + ") scale(" + scale + ")";
    const opacity = 1;

    let r = [];
    if(this.props.type == "cube") {
      r = [<rect key="0" height="58" width="58" x="-29" y="-29" fill={cDaffodilYellow} strokeWidth="4" stroke="#000000" />];
    }
    if(this.props.type == "cylinder") {
      r = [<circle key="0" cx="0" xy="0" r="29" fill={cDaffodilYellow} strokeWidth="4" stroke="#000000" />];
    }
    if(this.props.type == "cone") {
      for(let i = 0; i < 8; i += 1) {
        const rotate = "rotate(" + (45 * i) + ")";
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
}

class Eurobot2016Hut extends React.Component {
  render() {
    const colors = [];

    let x = 0;
    let y = 0;
    let scale = '';

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

    const translate = "translate(" + x + "," + y + ")";
    const opacity = 0.8;

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
}

class Eurobot2016SeeShell extends React.Component {
  getColor(conf, id) {
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
  }

  render() {

    let x = 0;
    let y = 0;

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

    const colors = [];
    const transparent = "rgba(255, 255, 255, 0)";
    for(let i = 0; i < this.props.conf.length; i += 1) {
      let color = this.getColor(this.props.conf[i], this.props.id);
      if(color == null) {
        color = transparent;
      }
      colors.push(color);
    }
    colors.sort();

    const translate = "translate(" + x + "," + y + ")";
    const render = [];
    let opacity = 0.8;
    if((colors.length == 0) || ((colors.length == 1) && (colors[0] == transparent))) {
      opacity = 0.3;
    }
    else if(colors.length == 1) {
      render.push(<circle key={render.length} r="36" strokeWidth="4" stroke="#000000" fill="rgba(0, 0, 0, 0)"/>);
      render.push(<circle key={render.length} r="34" fill={colors[0]}/>);
    }
    else {
      render.push(<circle key={render.length} r="36" strokeWidth="4" stroke="#000000" fill="rgba(0, 0, 0, 0)"/>);
      for(let i = 0; i < colors.length; i += 1) {
        let d = "M0 0 L34 0 A34 34 0 0 1 " + (34 * Math.cos(2 * Math.PI / colors.length)) + " " + (34 * Math.sin(2 * Math.PI / colors.length));
        let r = "rotate(" + (i * 360 / colors.length) + ")"
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
}


class Eurobot2016 extends React.Component {

  renderFieldColor() {
    const fields = [];

    fields.push(<rect key={fields.length} height="2000" width="3000" x="0" y="00" fill={cTrafficYellow} />);
    fields.push(<path key={fields.length} d="M0 1200 L600 1200 L600 1275 L2400 1275 L2400 1200 L3000 1200 L3000 2000 L0 2000 Z" fill={cSkyBlue} />);
    fields.push(<circle key={fields.length} cx="600" cy="1550" r="350" fill={cSkyBlue}/>);
    fields.push(<circle key={fields.length} cx="2400" cy="1550" r="350" fill={cSkyBlue}/>);
    fields.push(<path key={fields.length} d="M350 350 A1150 1150 0 0 0 2650 350 Z" fill={cTrafficYellow}/>);

    fields.push(<path key={fields.length} d="M900 750 A600 600 0 0 0 1500 1350 L1500 1328 A578 578 0 0 1 922 750 Z" fill={cSignalViolet} />);
    fields.push(<path key={fields.length} d="M2100 750 A600 600 0 0 1 1500 1350 L1500 1328 A578 578 0 0 0 2078 750 Z" fill={cEmeraldGreen} />);

    return fields;
  }

  renderStartingAreas() {
    const start = [];

    start.push(<rect key={start.length} height="500" width="300" x="0" y="600" fill={cSignalViolet} />);
    start.push(<rect key={start.length} height="500" width="300" x="2700" y="600" fill={cEmeraldGreen} />);
    start.push(<rect key={start.length} height="50" width="300" x="0" y="650" fill={cTrafficWhite} />);
    start.push(<rect key={start.length} height="50" width="300" x="0" y="1000" fill={cTrafficWhite} />);
    start.push(<rect key={start.length} height="50" width="300" x="2700" y="1000" fill={cTrafficWhite} />);
    start.push(<rect key={start.length} height="50" width="300" x="2700" y="650" fill={cTrafficWhite} />);

    return start;
  }

  renderBorders() {
    const borders = [];

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="-22" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="1222" width="22" x="-22" y="-22" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="822" width="22" x="-22" y="1200" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="1222" width="22" x="3000" y="-22" fill={cTrafficYellow} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="822" width="22" x="3000" y="1200" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="2000" fill={cSkyBlue} strokeWidth="2" stroke="#000000"/>);


    return borders;
  }

  renderObstacles() {
    const obstacles = [];

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
  }

  render() {
    let seeShellConf = [];

    let conf = "0";
    
    if(conf == "0") {
      seeShellConf = ["1", "2", "3", "4", "5"];
    }
    else {
      seeShellConf = [this.state.conf];
    }

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
}

export default Eurobot2016;
