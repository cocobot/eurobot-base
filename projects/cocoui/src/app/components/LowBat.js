import React from 'react';
import { 
  Modal, ModalFooter, ModalBody,
  Alert, Badge,
  Button,
} from 'reactstrap';
import State from '../State.js';


class LowBat extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      batteries: [],
      needShow: true,
      forceHide: false,
    }

    this._verifyNow();
  }

  _verifyNow() {
    if(this.state.forceHide) {
      if(Date.now() > this.state.forceHideTimeout) {
        this.setState({forceHide: false});
      }
    }

    const batteries = [];
    const robots = State.getStore().getState().robots;
    const principal = robots.getIn(["principal", "PBrain", "game_state", "battery"], undefined);
    const secondaire = robots.getIn(["secondaire", "SBrain", "game_state", "battery"], undefined);

    if(principal !== undefined) {
      batteries.push({
        robot: 'principal',
        value: principal / 1000.0,
      });
    }
    if(secondaire !== undefined) {
      batteries.push({
        robot: 'secondaire',
        value: secondaire / 1000.0,
      });
    }

    this.setState({batteries: batteries});
    setTimeout(() => this._verifyNow(), 1000);
  }

  render() {
    const alerts = [];
    let show = false;

    const notNow = () => {
      this.setState({forceHide: true, forceHideTimeout: Date.now() + 5000});
    }

    for(let i = 0; i < this.state.batteries.length; i += 1) {
      const batteryVal = this.state.batteries[i].value;
      let color = "";
      switch(this.state.batteries[i].robot) {
        case "principal":
          if(batteryVal < 18) {
            color = "danger";
            show = true;
          }
          else if(batteryVal < 19.5) {
            color = "warning";
          }
          else {
            color = "success";
          }
          break;

        case "secondaire":
          if(batteryVal < 11.3) {
            color = "danger";
            show = true;
          }
          else if(batteryVal < 11.6) {
            color = "warning";
          }
          else {
            color = "success";
          }
          break

        default:
          break;
      }

      alerts.push(<p key={i}>Robot {this.state.batteries[i].robot}: <Badge color={color}>{batteryVal.toFixed(2)} V</Badge></p>);
    }

    return (
    <Modal isOpen={show && !this.state.forceHide}>
      <ModalBody>
        <Alert color="danger">Vérification batterie</Alert>
        {alerts}
      </ModalBody>
      <ModalFooter>
        <Button color="secondary" onClick={notNow}>Ok, mais pas maintenant !</Button>
      </ModalFooter>
    </Modal>
    );
  }
}

export default LowBat;
