//CSS
import 'bootstrap/dist/css/bootstrap.css';

import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';

import State from './app/State.js';
import Cocoui from './app/components/Cocoui';

//parse URL to get page information
const url_string = window.location.href;
const url = new URL(url_string);
const page = url.searchParams.get("page");

let App = function() {
  //render the layout
  switch(page) {
  default:
    ReactDOM.render(<Provider store={State.getStore()}><Cocoui /></Provider>, document.getElementById('root'));
    break;
  }
}

export default App;
