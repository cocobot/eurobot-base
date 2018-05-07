import electron from 'electron';
import React from 'react';
import { 
  Container,
  Row, Col,
  Form, FormGroup, Label, Input,
  Card, CardHeader, CardBody, CardTitle, CardText,
  Badge,
} from 'reactstrap';
import { connect } from 'react-redux';

const ipcRenderer = electron.ipcRenderer;

class USIRComponent extends React.Component {
  constructor(props) {
    super(props);
    setInterval(() => {

      this.props.active.map((x, key) => {
        ipcRenderer.send('pkt', {
          pid: 0x8009,
          fmt: "",
          args: [],
          client: key, 
        });
      });
    }, 250);
  }

  _setForceOn(e, i, v) {
    e.preventDefault();
    this.props.active.map((x, key) => {
      ipcRenderer.send('pkt', {
        pid: 0x800b,
        fmt: "BB",
        args: [i, v],
        client: key, 
      });
    });
  }

  renderUsir(usirs, i) {
    let name = '';
    let state = null;
    const usir = usirs[i];

    switch(i) {
      case 0:
        name = "Devant gauche";
        break;

      case 1:
        name = "Devant droit";
        break;

      case 2:
        name = "Arrière gauche";
        break;

      case 3:
        name = "Arrière droit";
        break;
    }

    if(usir.alert == 1) {
      state =  <Badge color="warning">Alerte</Badge>;
    }
    else if(usir.alert == 2) {
      state =  <div className="label label-danger"><span className="glyphicon glyphicon-arrow-up" aria-hidden="true"></span>&nbsp;Alerte (HT)</div>;
      state =  <Badge color="danger">Alerte (HT)</Badge>;
    }
    else if(usir.alert_activated) {
      state =  <Badge color="success">Activé</Badge>;
    }
    else {
      state =  <Badge color="dark">Repos</Badge>;
    }

    return (
      <div>
        <div>
          <b>{name}</b>:<br /> <small>US = {usir.us}mm / IR = {usir.ir}mm</small>
        </div>
        {state}
        <Form inline >
          <FormGroup inline className="mb-2 mr-sm-2 mb-sm-0">
            <Label check size="sm">
              <Input type="checkbox" onChange={(e) => this._setForceOn(e, i, (usir.force_on + 1) % 2)} checked={usir.force_on == 1} />Force on
            </Label>
          </FormGroup>
        </Form>
      </div>
    );
  }

  renderRobot(usir, key) {
    if(usir.length < 4) {
      return <Row />;
    }
    return (
      <Row>
        <Col xs="12">
          <Card>
            <CardHeader>
              Adversaire ({key})
            </CardHeader>
            <CardBody>
              <Row>
                <Col xs="6">
                  {this.renderUsir(usir, 0)}
                </Col>
                <Col xs="6">
                  {this.renderUsir(usir, 1)}
                </Col>
              </Row>
              <Row>
                <Col xs="6">
                  {this.renderUsir(usir, 2)}
                </Col>
                <Col xs="6">
                  {this.renderUsir(usir, 3)}
                </Col>
              </Row>
            </CardBody>
          </Card>
        </Col>
      </Row>
    );
  }

  render() {
    return (
      <Container style={{ marginTop: '15px' }}>
        {this.props.usirs.map((u, k) => this.renderRobot(u, k))} 
      </Container>
    );
  }
}

const mapStateToProps = (state, ownProps) => {
  const usirs = [];
  state.conns.get('active').map((x, key) => {
    const usir = state.robots.getIn([key, 'usirs', 'usir'], []);
    if(usir != null) {
      usirs.push(usir);
    }
  });
  return {
    active: state.conns.get('active'),
    usirs: usirs,
  }
}

const USIR = connect(
  mapStateToProps,
  null,
)(USIRComponent);


export default USIR;

