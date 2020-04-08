import React from 'react';
import { connect } from 'react-redux';
import { Map } from 'immutable';
import { 
  Container,
  Row,
  Navbar, NavbarBrand,
  Table
} from 'reactstrap';


class BoardsComponent extends React.Component {
  render() {

    let board_list = [];
    let data_list = [];

    console.log(this.props.boards);
    this.props.boards.entrySeq().forEach(([key, data]) => {
      board_list.push(<th key={key}>{key}</th>);
      const bdata = data.entrySeq().map(([key, data]) => {

        const bdata = data.entrySeq().map(([key, data]) => {
          if(key.startsWith("_")) { return undefined; }
          return <li key={key}>{key} = {data}</li>
        });

        return <li key={key}>{data.getIn(['_name'])}<ul>{bdata}</ul></li>
      });
      data_list.push(<td key={key}><ul>{bdata}</ul></td>);
    });

    return (
    <div>
      <Navbar color="dark" dark expand="md">
        <NavbarBrand href="#">CocoUI: Boards: {this.props.robot}
        </NavbarBrand>
      </Navbar>
      <Container style={{ marginTop: '15px' }}>
        <Row>
          <Table>
            <thead>
                <tr>{board_list}</tr>
            </thead>
            <tbody>
                <tr>{data_list}</tr>
            </tbody>
          </Table>
        </Row>
      </Container>
    </div>
    );
  }
}


const mapBoardsStateToProps = (state, ownProps) => {
  return {
    boards: state.robots.getIn([ownProps.robot], new Map()),
  }
}

const Boards = connect(
  mapBoardsStateToProps,
  null,
)(BoardsComponent);

export default Boards;
