import React from 'react';
import { 
  Container,
  Row, Col,
  Card, CardHeader, CardBody,
  Button,
  InputGroup, InputGroupAddon, Input,
} from 'reactstrap';
import { connect } from 'react-redux';
const electron = window.require("electron");

const ipcRenderer = electron.ipcRenderer;

class MecaAction extends React.Component {
  _action() {
    ipcRenderer.send('pkt', {
      pid: 0x1001,
      fmt: "B",
      args: [parseInt(this.props.id)],
    });
  }

  render() {
    let color = this.props.color;
    if(color === undefined) {
      color = "primary";
    }
    return (
      <Col md="3">
        <Button  onClick={() => this._action()} color={color}>{this.props.name}</Button>
      </Col>
    );
  }
}

class ServoParameter extends React.Component {

  constructor(props) {
     super(props);
     this.UPDATE_PERIOD_MS = 1000;
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
      ipcRenderer.send('pkt', {
        pid: 0x1000,
        fmt: "BD",
        args: [this.props.id, parseInt(this.state.value)],
      });
      this.setState({value: ''});
    }
  }

  render() {
    return (
     <Row>
      <InputGroup size="sm" >
       <InputGroupAddon addonType="prepend">Servo {this.props.id}: </InputGroupAddon>
       <Input value={this.state.value} onChange={(e) => this.handleChange(e)} onKeyPress={(e) => this.handleKeyPress(e)} />
      </InputGroup>
     </Row>
    );
  }
}

class MecaComponent extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
    };
  }

  

  render() {
    const actions = [
      {
        name: "Init",
        value: 0,
        color: "warning",
      },
      {
        name: "FlagUp",
        value: 25,
        color: "warning",
      },
      {
        newline: true,
      },
      {
        name: "F Up",
        value: 1,
        color: "primary",
      },
      {
        name: "F Down",
        value: 2,
        color: "primary",
      },
      {
        name: "F All Open",
        value: 3,
        color: "primary",
      },
      {
        name: "F All Close",
        value: 4,
        color: "primary",
      },
      {
        newline: true,
      },
      {
        name: "F left O",
        value: 5,
        color: "success",
      },
      {
        name: "F center O",
        value: 7,
        color: "success",
      },
      {
        name: "F right O",
        value: 9,
        color: "success",
      },
      {
        newline: true,
      },
      {
        name: "F left C",
        value: 6,
        color: "success",
      },
      {
        name: "F center C",
        value: 8,
        color: "success",
      },
      {
        name: "F right C",
        value: 10,
        color: "success",
      },
      {
        newline: true,
      },
      {
        name: "B Up",
        value: 11,
        color: "primary",
      },
      {
        name: "B Down",
        value: 12,
        color: "primary",
      },
      {
        name: "B All Open",
        value: 13,
        color: "primary",
      },
      {
        name: "B All Close",
        value: 14,
        color: "primary",
      },
      {
        newline: true,
      },
      {
        name: "B left O",
        value: 15,
        color: "success",
      },
      {
        name: "B center O",
        value: 17,
        color: "success",
      },
      {
        name: "B rigth O",
        value: 19,
        color: "success",
      },
      {
        newline: true,
      },
      {
        name: "B left C",
        value: 16,
        color: "success",
      },
      {
        name: "B center C",
        value: 18,
        color: "success",
      },
      {
        name: "B rigth C",
        value: 20,
        color: "success",
      },
      {
        newline: true,
      },
      {
        name: "L WindS Down",
        value: 21,
        color: "danger",
      },
      {
        name: "L WindS Up",
        value: 22,
        color: "danger",
      },
      {
        name: "R WindS Down",
        value: 23,
        color: "danger",
      },
      {
        name: "R WindS Up",
        value: 24,
        color: "danger",
      },
    ];

    const servos = [];
    for(let i = 0; i < 13; i += 1) {
      servos.push(
        <ServoParameter key={i} id={i} active={this.props.active}/>
      );
    }

    let actions_ui = [];
    let row = [];
    for(let i = 0; i < actions.length; i += 1) {
      if(actions[i].newline === true) {
        actions_ui.push(<Row>{row}</Row>);
        actions_ui.push(<Row><p /><p /></Row>);
        row = [];
      }
      else {
        row.push(<MecaAction active={this.props.active} id={actions[i].value} name={actions[i].name} color={actions[i].color}/>);
      }
    }
    if(row.length > 0) {
      actions_ui.push(<Row>{row}</Row>);
    }

    return (
      <Container style={{ marginTop: '15px' }}>
        <Row>
          <Col md="6">
            <Card>
              <CardHeader>
                Servo
              </CardHeader>
              <CardBody>
                {servos}
              </CardBody>
            </Card>
          </Col>
          <Col md="6">
            <Card>
              <CardHeader>
                Meca 2
              </CardHeader>
              <CardBody>
                {actions_ui}
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
    return asservs;
  });
  return {
    active: state.conns.get('active'),
    asservs: asservs,
  }
}

const Meca = connect(
  mapStateToProps,
  null,
)(MecaComponent);



export default Meca;



