import React from 'react';
import { 
  Container,
  Row, Col,
  Form, FormGroup, Label, Input,
  Card, CardHeader, CardBody, CardTitle, CardText,
  Badge,
} from 'reactstrap';

class USIR extends React.Component {
  renderUsir(i) {
    let name = '';
    let state = null;
    let usir = {alert: 0};

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
        <br />
      </div>
    );
  }

  render() {
    return (
      <Container style={{ marginTop: '15px' }}>
        <Row>
            <Col xs="12">
            <Card>
              <CardHeader>
                Adversaire
              </CardHeader>
              <CardBody>
                <Row>
                  <Col xs="6">
                    {this.renderUsir(0)}
                  </Col>
                  <Col xs="6">
                    {this.renderUsir(1)}
                  </Col>
                </Row>
                <Row>
                  <Col xs="6">
                    {this.renderUsir(2)}
                  </Col>
                  <Col xs="6">
                    {this.renderUsir(3)}
                  </Col>
                </Row>
              </CardBody>
            </Card>
          </Col>
        </Row>
      </Container>
    );
  }
}

export default USIR;

