//CSS
import 'bootstrap/dist/css/bootstrap.css';
//
import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';
import Cocoui from './components/cocoui';
import State from './state';

//render the layout
ReactDOM.render(<Provider store={State.getStore()}><Cocoui /></Provider>, document.getElementById('app'));
