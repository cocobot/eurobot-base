import React from 'react';
import Pathfinder from '../Pathfinder';
import Actions from '../Actions';

const cSilexGray = "rgb(181, 176, 161)";
const cYellowGreen = "rgb(97, 153, 59)";
const cSignalOrange = "rgb(208, 93, 40)";
const cBlack = "rgb(14, 14, 16)";
const cSignalYellow = "rgb(247, 181, 0)";
const cSignalBlue = "rgb(0, 124, 176)";

class Eurobot2018Cube extends React.Component {
  render() {
    return (
      <g>
        <rect height="58" width="58" x="-29" y="-29" fill={this.props.color} strokeWidth="2" stroke="#000000" />
      </g>
    );
  }
}

class Eurobot2018CubeGroup extends React.Component {
  _getTranslation() {
    let x = 0;
    let y = 0;
    switch(parseInt(this.props.id)) {
      case 0:
        x = 850;
        y = 540;
        break;

      case 1:
        x = 2150;
        y = 540;
        break;

      case 2:
        x = 300;
        y = 1190;
        break;

      case 3:
        x = 2700;
        y = 1190;
        break;

      case 4:
        x = 1100;
        y = 1500;
        break;

      case 5:
        x = 1900;
        y = 1500;
        break;
    }
    return "translate("+x+","+y+")";
  }

  render() {
    return (
      <g transform={this._getTranslation()}>
        <g transform="translate(58, 0)">
          <Eurobot2018Cube color={cSignalOrange} />
        </g>
        <g transform="translate(-58, 0)">
          <Eurobot2018Cube color={cYellowGreen} />
        </g>
        <g transform="translate(0, 58)">
          <Eurobot2018Cube color={cSignalBlue} />
        </g>
        <g transform="translate(0, -58)">
          <Eurobot2018Cube color={cBlack} />
        </g>
        <Eurobot2018Cube color={cSignalYellow} />
        <text fontSize="60" y="5" x="0">
          <tspan textAnchor="middle" alignmentBaseline="middle">{this.props.id}</tspan>
         </text>
      </g>
    );
  }
}

class Eurobot2018BallFeeder extends React.Component {
  render() {
    let x = 0;
    let y = 0;
    let a = 0;
    let feederColor = cBlack;
    let ball1Color = cBlack;
    let ball2Color = cBlack;
    switch(parseInt(this.props.id)) {
      case 0:
        x = 0;
        y = 850;
        a = 0;
        feederColor = cYellowGreen;
        ball1Color = cYellowGreen;
        ball2Color = cYellowGreen;
        break;

      case 1:
        x = 3000;
        y = 850;
        a = 180;
        feederColor = cSignalOrange;
        ball1Color = cSignalOrange;
        ball2Color = cSignalOrange;
        break;

     case 2:
        x = 2390;
        y = 2000;
        a = -90;
        feederColor = cYellowGreen;
        ball1Color = cYellowGreen;
        ball2Color = cSignalOrange;
        break;

     case 3:
        x = 610;
        y = 2000;
        a = -90;
        feederColor = cSignalOrange;
        ball1Color = cSignalOrange;
        ball2Color = cYellowGreen;
        break;
    }
    return (
      <g transform={"translate(" + x + ", " + y + ") rotate(" + a + ")"}>
        <linearGradient id={"ball" + this.props.id} x1="0" x2="0" y1="0" y2="1">
          <stop offset="0%" stopColor={ball1Color}/>
          <stop offset="50%" stopColor={ball1Color}/>
          <stop offset="50%" stopColor={ball2Color}/>
          <stop offset="100%" stopColor={ball2Color}/>
        </linearGradient>
        <rect height="22" width="89" x="-22" y="-11" fill={feederColor} strokeWidth="2" stroke="#000000" />
        <circle r="27" cx="45" cy="0" fill={feederColor} strokeWidth="2" stroke="#000000" />
        <rect height="54" width="32.5" x="12.5" y="-27" fill={feederColor} strokeWidth="2" stroke={feederColor} />
        <circle r="27" cx="12.5" cy="0" fill={feederColor} strokeWidth="2" stroke="#000000" />
        <circle r="20" cx="12.5" cy="0" fill={"url(#ball"+this.props.id+")"} strokeWidth="4" stroke="#000000" />
      </g>
    );
  }
}

class Eurobot2018 extends React.Component {

  renderBorders() {
    const borders = [];

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="2022" width="22" x="-22" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="2022" width="22" x="3000" y="-22" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="3044" x="-22" y="2000" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="150" width="22" x="-44" y="2000" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="150" width="22" x="3022" y="2000" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="30" x="1240" y="2128" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="962" x="278" y="2128" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="300" x="-22" y="2128" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="30" x="1730" y="2128" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="962" x="1760" y="2128" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="300" x="2722" y="2128" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="230" width="22" x="1270" y="2022" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="230" width="22" x="1478" y="2022" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="230" width="22" x="1500" y="2022" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="230" width="22" x="1708" y="2022" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="22" width="186" x="1292" y="2230" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="186" x="1522" y="2230" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="250" width="1200" x="900" y="1750" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="250" width="22" x="1489" y="1750" fill={cSilexGray} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="250" width="12" x="888" y="1750" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="600" x="900" y="1750" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="600" x="900" y="1978" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);

    borders.push(<rect key={borders.length} height="250" width="12" x="2100" y="1750" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="600" x="1500" y="1750" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    borders.push(<rect key={borders.length} height="22" width="600" x="1500" y="1978" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);


    return borders;
  } 
  
  renderBoxes() {
    const boxes = [];

    boxes.push(<rect key={boxes.length} height="370" width="370" x="-10" y="-392" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="370" width="10" x="-10" y="-392" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="10" width="360" x="-10" y="-392" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="370" width="10" x="350" y="-392" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="10" width="360" x="0" y="-32" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="360" width="22" x="0" y="-382" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="22" width="328" x="22" y="-382" fill={cYellowGreen} strokeWidth="2" stroke="#000000"/>);

    boxes.push(<rect key={boxes.length} height="370" width="370" x="2640" y="-392" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="370" width="10" x="3000" y="-392" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="10" width="360" x="2650" y="-392" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="370" width="10" x="2640" y="-392" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="10" width="360" x="2640" y="-32" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="360" width="22" x="2978" y="-382" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);
    boxes.push(<rect key={boxes.length} height="22" width="328" x="2650" y="-382" fill={cSignalOrange} strokeWidth="2" stroke="#000000"/>);

    return boxes;
  }

  renderCenter() {
    const center = [];

    center.push(<rect key={center.length} height="10" width="200" x="1400" y="0" fill="white" strokeWidth="2" stroke="#000000"/>);
    center.push(<rect key={center.length} height="10" width="80" x="1090" y="0" fill="white" strokeWidth="2" stroke="#000000"/>);
    center.push(<rect key={center.length} height="10" width="80" x="1830" y="0" fill="white" strokeWidth="2" stroke="#000000"/>);

    return center;
  }


  render() {
    return (
      <g>
        <image href="f2018_cprs.jpg" x="0" y="0" height="2000" width="3000" />
        {this.renderBorders()}
        {this.renderBoxes()}
        {this.renderCenter()}

        <Eurobot2018CubeGroup id="0" />
        <Eurobot2018CubeGroup id="1" />
        <Eurobot2018CubeGroup id="2" />
        <Eurobot2018CubeGroup id="3" />
        <Eurobot2018CubeGroup id="4" />
        <Eurobot2018CubeGroup id="5" />

        <Eurobot2018BallFeeder id="0" />
        <Eurobot2018BallFeeder id="1" />
        <Eurobot2018BallFeeder id="2" />
        <Eurobot2018BallFeeder id="3" />

        {this.props.children}

        <Actions />
        <Pathfinder />
      </g>
    );
  }
}

export default Eurobot2018;

