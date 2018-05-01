//CSS
import 'bootstrap/dist/css/bootstrap.css';
//
import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';
import Cocoui from './components/cocoui';
import USIR from './components/usir';
import State from './state';


const url_string = window.location.href;
const url = new URL(url_string);
const page = url.searchParams.get("page");

//render the layout
switch(page) {
  case 'USIR':
    ReactDOM.render(<Provider store={State.getStore()}><USIR /></Provider>, document.getElementById('app'));
    break;

  default:
    ReactDOM.render(<Provider store={State.getStore()}><Cocoui /></Provider>, document.getElementById('app'));
    break;
}
