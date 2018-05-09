import electron from 'electron';
import React from 'react';
import { 
  Container,
  Row, Col,
  Form, FormGroup, Label,
  Card, CardHeader, CardBody, CardTitle, CardText,
  Badge,
  Button,
  InputGroup, InputGroupAddon, InputGroupText, Input,
} from 'reactstrap';
import { connect } from 'react-redux';

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
      });
  }

  render() {
    return (
      <Col md="3">
        <Button  onClick={() => this._action()} color="primary">{this.props.name}</Button>
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

    const servos = [];

    for(let i = 0; i < 12; i += 1) {
      servos.push(
        <ServoParameter key={i} id={i} active={this.props.active}/>
      );
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
                <Row>
                  <MecaAction active={this.props.active} id="0" name="Bee init"/> { ' '}
                  <MecaAction active={this.props.active} id="1" name="Bee prepare"/> { ' ' }
                  <MecaAction active={this.props.active} id="2" name="Bee action"/> { ' ' }
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

const Meca = connect(
  mapStateToProps,
  null,
)(MecaComponent);



export default Meca;


