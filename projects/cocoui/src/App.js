//CSS
import 'bootstrap/dist/css/bootstrap.css';
import './app/app.css'

import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';

import State from './app/State.js';
import Cocoui from './app/components/Cocoui';
import Meca from './app/components/Meca';
import Canon from './app/components/Canon';
import Boards from './app/components/Boards';
import LowBat from './app/components/LowBat';

//parse URL to get page information
const url_string = window.location.href;
const url = new URL(url_string);
const page = url.searchParams.get("page");
const robot = url.searchParams.get("robot");

//debug
//const page="boards";
//const robot="secondaire";

let App = function() {
  //render the layout
  switch(page) {
    case 'Meca':
      ReactDOM.render(<Provider store={State.getStore()}><Meca /><LowBat /></Provider>, document.getElementById('root'));
      break;

    case 'canon':
      ReactDOM.render(<Provider store={State.getStore()}><Canon robot={robot}/><LowBat /></Provider>, document.getElementById('root'));
      break;

    case 'boards':
      ReactDOM.render(<Provider store={State.getStore()}><Boards robot={robot}/><LowBat /></Provider>, document.getElementById('root'));
      break;

    default:
      ReactDOM.render(<Provider store={State.getStore()}><Cocoui /><LowBat /></Provider>, document.getElementById('root'));
      break;
  }
}

export default App;
