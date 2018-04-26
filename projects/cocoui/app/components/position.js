import React from 'react';
import { connect } from 'react-redux';
import { 
  Container,
  Row, Col,
  Form, FormGroup, Label, Input,
  Card, CardHeader, CardBody, CardTitle, CardText,
} from 'reactstrap';
import { updateDebugPathfinder } from '../actions';
import Field from './Field';
import Robots from './Robots';
import Eurobot2018 from './Fields/Eurobot2018';
import TrajectoryOrders from './TrajectoryOrders';

class PositionComponent extends React.Component {
  _onChangeDebugPathfinder(e, checked) {
    this.props.onChangeDebugPathfinder(e.target.checked);
  }

  render() {
    return (
      <Container style={{ marginTop: '15px' }}>
        <Row>
            <Col>
          <Card>
            <CardHeader>
              <Form inline className="float-right" style={{marginBottom: 0}}>
                <FormGroup inline className="mb-2 mr-sm-2 mb-sm-0">
                  <Label check size="sm">
                    <Input type="checkbox" onChange={(e, checked) => this._onChangeDebugPathfinder(e, checked)} checked={this.props.debugPathfinder} />Debug pathfinder
                  </Label>
                <FormGroup inline className="mb-2 mr-sm-2 mb-sm-0">
                </FormGroup>
                  <Label check size="sm">
                    <Input type="checkbox" />Debug strat
                  </Label>
                </FormGroup>
                <FormGroup inline className="mb-2 mr-sm-2 mb-sm-0">
                  <Label check size="sm">
                    <Input type="checkbox"  />Pause action scheduler
                  </Label>
                </FormGroup>
              </Form>
              Position
            </CardHeader>
            <CardBody>
              <Row>
                <Col xs="8">
                  <Field>
                    <Eurobot2018>
                      <Robots />
                    </ Eurobot2018>
                  </Field>
                </Col>
                <Col xs="4">
                  <TrajectoryOrders />
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

const mapStateToProps = (state, ownProps) => {
  const id = state.win.get('id');
  return {
    debugPathfinder: state.options.getIn(['debugPathfinder'], false),
  }
};

const mapDispatchToProps = (dispatch) => {
  return {
    onChangeDebugPathfinder: (value) => {
      dispatch(updateDebugPathfinder(value));
    }
  }
};

const Position = connect(
  mapStateToProps,
  mapDispatchToProps,
)(PositionComponent);

export default Position;
