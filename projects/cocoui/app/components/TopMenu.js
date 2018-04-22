import React from 'react';
import { Navbar, NavbarBrand,
         UncontrolledDropdown, DropdownToggle, DropdownItem, DropdownMenu,
} from 'reactstrap';

class TopMenu extends React.Component {
  render() {
    return (
      <div>
        <Navbar color="dark" dark>
        <NavbarBrand href="#">CocoUI</NavbarBrand>
        <UncontrolledDropdown nav inNavbar>
          <DropdownToggle nav caret>
            Options
          </DropdownToggle>
          <DropdownMenu >
            <DropdownItem>
              Option 1
            </DropdownItem>
            <DropdownItem>
              Option 2
            </DropdownItem>
            <DropdownItem divider />
            <DropdownItem>
              Reset
            </DropdownItem>
          </DropdownMenu>
        </UncontrolledDropdown>
        </Navbar>
      </div>
    );
  }
}

export default TopMenu;
