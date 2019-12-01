import React from 'react';
import Pathfinder from '../Pathfinder';
import Actions from '../Actions';

const cSilexGray = "rgb(181, 176, 161)";
const cSignalYellow = "rgb(247, 181, 0)";
const cSignalBlue = "rgb(0, 124, 176)";
const cGreen = "rgb(0, 111, 61)";
const cRed = "rgb(187, 30, 16)";

class Eurobot2020LightHouse extends React.Component {
  render() {
    return (
     <g transform={"translate("+this.props.x+", "+this.props.y+")"}>
      <rect height="200" width="450" x="0" y="-200" fill={this.props.color} strokeWidth="2" stroke="#000000"/>);
      <rect height="22" width="450" x="0" y="0" fill={this.props.color} strokeWidth="2" stroke="#000000"/>);
     </g>
    );
  }
}

class Eurobot2020BigTower extends React.Component {
  render() {
    return (
     <g transform={"translate("+this.props.x+", "+this.props.y+")"}>
      <rect height="200" width="200" x="-200" y="-200" fill={cSignalBlue} strokeWidth="2" stroke="#000000"/>);
      <rect height="200" width="200" x="0" y="-200" fill={cSignalYellow} strokeWidth="2" stroke="#000000"/>);
      <rect height="22"  width="200" x="-100" y="-222" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
     </g>
    );
  }
}

class Eurobot2020SmallTower extends React.Component {
  render() {
    return (
     <g transform={"translate("+this.props.x+", "+this.props.y+") scale("+this.props.orientation+", 1)"}>
      <rect height="122" width="22" x="0" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
      <rect height="22" width="70" x="-70" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
      <rect height="100" width="70" x="-70" y="0" fill={this.props.color} strokeWidth="2" stroke="#000000"/>);
     </g>
    );
  }
}

class Eurobot2020Cup extends React.Component {
  render() {
    let x, y, color;
    let id = parseInt(this.props.id);

    switch(id)
    {
      case 0:
      case 2:
      case 4:
      case 7:
      case 8:
      case 10:
      case 14:
      case 15:
      case 17:
      case 19:
      case 20:
      case 22:
        color = cRed;
        break;

      case 1:
      case 3:
      case 5:
      case 6:
      case 9:
      case 11:
      case 12:
      case 13:
      case 16:
      case 18:
      case 21:
      case 23:
        color = cGreen;
        break;
    }
    
    switch(id)
    {
      case 0:
        x = 670;
        break;
      case 1:
        x = 2330;
        break;
      case 2:
      case 12:
        x = 300;
        break;
      case 3:
        x = 950;
        break;
      case 4:
        x = 2050;
        break;
      case 5:
      case 15:
        x = 2700;
        break;
      case 6:
      case 10:
        x = 450;
        break;
      case 7:
      case 11:
        x = 2550;
        break;
      case 8:
        x = 1100;
        break;
      case 9:
        x = 1900;
        break;
      case 13:
        x = 1270;
        break;
      case 14:
        x = 1730;
        break;
      case 16:
        x = 1065;
        break;
      case 17:
        x = 1335;
        break;
      case 18:
        x = 1665;
        break;
      case 19:
        x = 1935;
        break;
      case 20:
        x = 1005;
        break;
      case 21:
        x = 1395;
        break;
      case 22:
        x = 1605;
        break;
      case 23:
        x = 1995;
        break;
    }

    switch(id)
    {
      case 0:
      case 1:
        y = 100;
        break;
      case 2:
      case 3:
      case 4:
      case 5:
        y = 400;
        break;
      case 6:
      case 7:
        y = 510;
        break;
      case 8:
      case 9:
        y = 800;
        break;
      case 10:
      case 11:
        y = 1080;
        break;
      case 12:
      case 13:
      case 14:
      case 15:
        y = 1200;
        break;
      case 16:
      case 17:
      case 18:
      case 19:
        y = 1650;
        break;
      case 20:
      case 21:
      case 22:
      case 23:
        y = 1955;
        break;
    }
    

    return (
     <g transform={"translate("+x+", "+y+")"}>
        <circle r="36" fill={color} strokeWidth="10" stroke="#000000" />
        <circle r="27" fill={color} strokeWidth="5" stroke="#000000" />
      </g>
    );
  }
}

class Eurobot2020Distributor extends React.Component {
  render() {
    let x, y, color, angle;

    x = 300;
    y = 300;
    color = cSignalYellow
    angle = 0;

    switch(this.props.id)
    {
      case 0:
        x = 640.5;
        y = -134;
        color = cSilexGray;
        angle = 0;
        break;

      case 1:
        x = 1940.5;
        y = -134;
        color = cSilexGray;
        angle = 0;
        break;

      case 2:
        x = -22;
        y = 0;
        color = cSilexGray;
        angle = -90;
        break;
    }

    return (
     <g transform={"translate("+x+", "+y+")"}>
      <g transform={"rotate("+angle+")"}>
       <rect height="134" width="419" x="0" y="0" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
       <rect height="22" width="419" x="0" y="0" fill={color} strokeWidth="2" stroke="#000000"/>);
       <rect height="90" width="22" x="0" y="22" fill={color} strokeWidth="2" stroke="#000000"/>);
       <rect height="90" width="22" x="397" y="22" fill={color} strokeWidth="2" stroke="#000000"/>);
       <rect height="22" width="419" x="0" y="112" fill={color} strokeWidth="2" stroke="#000000"/>);
      </g>
    </g>
    );
  }
}

class Eurobot2020 extends React.Component {

  renderBorders() {
    const borders = [];

    //4 bordures principales
    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="2022" width="22" x="-22" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="2022" width="22" x="3000" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="2000" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    //obstacles port
    borders.push(<rect key={borders.length} height="150" width="22" x="889" y="1850" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="150" width="22" x="2089" y="1850" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="300" width="22" x="1489" y="1700" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    return borders;
  } 
  

  render() {
    return (
      <g>
        <image href="f2018_cprs.jpg" x="0" y="0" height="2000" width="3000" />
        {this.renderBorders()}

        <Eurobot2020LightHouse x={0} y={-22} color={cSignalBlue}/>
        <Eurobot2020LightHouse x={2550} y={-22} color={cSignalYellow}/>

        <Eurobot2020SmallTower x={-44} y={0} orientation={1} color={cSignalYellow}/>
        <Eurobot2020SmallTower x={-44} y={950} orientation={1} color={cSignalBlue}/>
        <Eurobot2020SmallTower x={-44} y={1900} orientation={1} color={cSignalYellow}/>

        <Eurobot2020SmallTower x={3044} y={0} orientation={-1} color={cSignalBlue}/>
        <Eurobot2020SmallTower x={3044} y={950} orientation={-1} color={cSignalYellow}/>
        <Eurobot2020SmallTower x={3044} y={1900} orientation={-1} color={cSignalBlue}/>

        <Eurobot2020BigTower x={1500} y={-22} />

        <Eurobot2020Distributor id={0} />
        <Eurobot2020Distributor id={1} />
        <Eurobot2020Distributor id={2} />
        <Eurobot2020Distributor id={3} />

        <Eurobot2020Cup id="0" />
        <Eurobot2020Cup id="1" />

        <Eurobot2020Cup id="2" />
        <Eurobot2020Cup id="3" />
        <Eurobot2020Cup id="4" />
        <Eurobot2020Cup id="5" />

        <Eurobot2020Cup id="6" />
        <Eurobot2020Cup id="7" />

        <Eurobot2020Cup id="8" />
        <Eurobot2020Cup id="9" />

        <Eurobot2020Cup id="10" />
        <Eurobot2020Cup id="11" />

        <Eurobot2020Cup id="12" />
        <Eurobot2020Cup id="13" />
        <Eurobot2020Cup id="14" />
        <Eurobot2020Cup id="15" />

        <Eurobot2020Cup id="16" />
        <Eurobot2020Cup id="17" />
        <Eurobot2020Cup id="18" />
        <Eurobot2020Cup id="19" />

        <Eurobot2020Cup id="20" />
        <Eurobot2020Cup id="21" />
        <Eurobot2020Cup id="22" />
        <Eurobot2020Cup id="23" />

        {this.props.children}

        <Actions />
        <Pathfinder />
      </g>
    );
  }
}
export default Eurobot2020;
