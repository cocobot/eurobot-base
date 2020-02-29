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
    this.props.active.map((x, key) => {
        ipcRenderer.send('pkt', {
          pid: 0x1001,
          fmt: "B",
          args: [parseInt(this.props.id)],
          client: key, 
        });
      return {};
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
      this.props.active.map((x, key) => {
        ipcRenderer.send('pkt', {
          pid: 0x1000,
          fmt: "BD",
          args: [this.props.id, parseInt(this.state.value)],
          client: key, 
        });

        return {};
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
        newline: true,
      },
      {
        name: "Action 1",
        value: 1,
        color: "primary",
      },
      {
        name: "Action 2",
        value: 2,
        color: "danger",
      },
      {
        name: "Action 3",
        value: 3,
        color: "primary",
      },
      {
        newline: true,
      },
      {
        name: "Action 4",
        value: 4,
        color: "warning",
      },
      {
        name: "Action 5",
        value: 5,
        color: "warning",
      },
      {
        name: "Action 6",
        value: 6,
        color: "success",
      },
    ];

    const servos = [];
    for(let i = 0; i < 12; i += 1) {
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



