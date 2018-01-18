import React from 'react';
import { Navbar, NavbarBrand } from 'reactstrap';

class TopMenu extends React.Component {
  render() {
    return (
      <div>
        <Navbar color="dark" dark>
        <NavbarBrand href="#">CocoUI</NavbarBrand>
        </Navbar>
      </div>
    );
  }
}

export default TopMenu;
