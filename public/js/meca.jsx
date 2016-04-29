var MecaUmbrella = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      state: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("meca_umbrella", this.handleReceiveUmbrellaState);
    
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveUmbrellaState: function(data) {
    this.setState({state: data.answer.data[0]});
  },


  open: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_umbrella", argument: "open"});
  },

  close: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_umbrella", argument: "close"});
  },

  update: function() {
    if(this.props.update) {
      utils.sendCommand({command: "meca_umbrella", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    var state = null;
    if(this.state.state == 'open') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-arrow-up" aria-hidden="true"></span>&nbsp;Ouvert</div>;
    }
    else if(this.state.state == 'close') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-arrow-right" aria-hidden="true"></span>&nbsp;Ferm&eacute;</div>;
    }
    else {
      state =  <div className="label label-default"><span className="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>&nbsp;???</div>;
    }

    return (
      <div className="panel panel-default">
        <div className="panel-heading">
          Parasol
        </div>
        <div className="panel-body">
          <div className="row">
            <div className="col-md-1">
              <label>&Eacute;tat:</label>
            </div>
            <div className="col-md-1">
              {state}
            </div>
            <div className="col-md-2">
            </div>
            <div className="col-md-8">
              <Parameter name="servo rotation" command="meca_umbrella_rot_servo" show={this.props.update}/>
            </div>

          </div>
          <div className="row">
            <div className="col-md-4">
              <div className="btn-group">
                <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.close}>Fermer</button>
                <button type="submit" className="btn btn-xs btn-warning" onClick={this.open}>Ouvrir</button>
              </div>
            </div>
            <div className="col-md-8">
              <Parameter name="servo ouverture" command="meca_umbrella_opn_servo" show={this.props.update}/>
            </div>
          </div>
        </div>
      </div>
    );
  }
});

var MecaSeaShell = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      state: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("meca_seashell", this.handleReceiveState);
    
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveState: function(data) {
    this.setState({state: data.answer.data[0]});
  },

  open: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_seashell", argument: "open"});
  },

  close: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_seashell", argument: "close"});
  },

  take: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_seashell", argument: "take"});
  },

  update: function() {
    if(this.props.update) {
      utils.sendCommand({command: "meca_seashell", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    var state = null;
    if(this.state.state == 'open') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-magnet" aria-hidden="true"></span>&nbsp;Ouvert</div>;
    }
    else if(this.state.state == 'close') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-remove-circle" aria-hidden="true"></span>&nbsp;Ferm&eacute;</div>;
    }
    else if(this.state.state == 'take') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-certificate" aria-hidden="true"></span>&nbsp;Palet attrap&eacute;</div>;
    }
    else {
      state =  <div className="label label-default"><span className="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>&nbsp;???</div>;
    }

    return (
      <div className="panel panel-default">
        <div className="panel-heading">
          Coquillages et crustac&eacute;s
        </div>
        <div className="panel-body">
          <div className="row">
            <div className="col-md-1">
              <label>&Eacute;tat:</label>
            </div>
            <div className="col-md-1">
              {state}
            </div>
            <div className="col-md-2">
            </div>
            <div className="col-md-8">
              <Parameter name="servo droit" command="meca_seashell_right_servo" show={this.props.update}/>
            </div>
          </div>
          <div className="row">
            <div className="col-md-4">
              <div className="btn-group">
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.close}>Fermer</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.open}>Ouvrir</button>
              </div>
            </div>
            <div className="col-md-8">
              <Parameter name="servo gauche" command="meca_seashell_left_servo" show={this.props.update}/>
            </div>
          </div>
        </div>
      </div>
    );
  }
});

var MecaFish = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      state: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("meca_fish", this.handleReceiveState);
    
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveState: function(data) {
    this.setState({state: data.answer.data[0]});
  },

  prepare: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_fish", argument: "prepare"});
  },

  close: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_fish", argument: "close"});
  },

  sweep_left: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_fish", argument: "sweep_left"});
  },

  sweep_right: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_fish", argument: "sweep_right"});
  },

  swim_left: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_fish", argument: "swim_left"});
  },

  swim_right: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_fish", argument: "swim_right"});
  },

  update: function() {
    if(this.props.update) {
      utils.sendCommand({command: "meca_fish", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    var state = null;
    if(this.state.state == 'close') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-remove-circle" aria-hidden="true"></span>&nbsp;Ferm&eacute;</div>;
    }
    else if(this.state.state == 'prepare') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-minus" aria-hidden="true"></span>&nbsp;Prepare</div>;
    }
    else if(this.state.state == 'sweep_left') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-chevron-left" aria-hidden="true"></span>&nbsp;Sweep left</div>;
    }
    else if(this.state.state == 'sweep_right') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-chevron-right" aria-hidden="true"></span>&nbsp;Sweep right</div>;
    }
    else if(this.state.state == 'swim_left') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-backward" aria-hidden="true"></span>&nbsp;Swim left</div>;
    }
    else if(this.state.state == 'swim_right') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-forward" aria-hidden="true"></span>&nbsp;Swim right</div>;
    }
    else if(this.state.state == 'manual') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-pencil" aria-hidden="true"></span>&nbsp;Manual</div>;
    }
    else {
      state =  <div className="label label-default"><span className="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>&nbsp;???({this.state.state})</div>;
    }

    return (
      <div className="panel panel-default">
        <div className="panel-heading">
          Poissons
        </div>
        <div className="panel-body">
          <div className="row">
            <div className="col-md-1">
              <label>&Eacute;tat:</label>
            </div>
            <div className="col-md-1">
              {state}
            </div>
            <div className="col-md-2">
            </div>
            <div className="col-md-8">
              <Parameter name="servo up/down" command="meca_fish_ud_servo" show={this.props.update}/>
            </div>
          </div>
          <div className="row">
            <div className="col-md-4">
              <div className="btn-group">
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.close}>Fermer</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.prepare}>Prepare</button>
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.sweep_left}>Sweep left</button>
              </div>
            </div>
            <div className="col-md-8">
              <Parameter name="servo left/right" command="meca_fish_lr_servo" show={this.props.update}/>
            </div>
          </div>
          <div className="row">
            <div className="col-md-4">
              <div className="btn-group">
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.sweep_right}>Sweep right</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.swim_left}>Swim left</button>
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.swim_right}>Swim right</button>
              </div>
            </div>
            <div className="col-md-8">
              <Parameter name="servo rot" command="meca_fish_rot_servo" show={this.props.update}/>
            </div>
          </div>
        </div>
      </div>
    );
  }
});

var MecaSucker = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      left_state: '',
      right_state: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("meca_sucker_left", this.handleReceiveLeftState);
    utils.onReceiveCommand("meca_sucker_right", this.handleReceiveRightState);
    
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveLeftState: function(data) {
    this.setState({left_state: data.answer.data[0]});
  },

  handleReceiveRightState: function(data) {
    this.setState({right_state: data.answer.data[0]});
  },

  open_left: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_left", argument: "open"});
  },

  close_left: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_left", argument: "close"});
  },


  pump_left: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_left", argument: "pump"});
  },


  disable_left: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_left", argument: "disable"});
  },


  open_right: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_right", argument: "open"});
  },

  close_right: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_right", argument: "close"});
  },


  pump_right: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_right", argument: "pump"});
  },


  disable_right: function(event) {
    event.preventDefault();
    utils.sendCommand({command: "meca_sucker_right", argument: "disable"});
  },


  update: function() {
    if(this.props.update) {
      utils.sendCommand({command: "meca_sucker_left", argument: null});
      utils.sendCommand({command: "meca_sucker_right", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    var left_state = null;
    if(this.state.left_state == 'close') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-remove-circle" aria-hidden="true"></span>&nbsp;Ferm&eacute;</div>;
    }
    else if(this.state.left_state == 'open') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-minus" aria-hidden="true"></span>&nbsp;Ouvert</div>;
    }
    else if(this.state.left_state == 'pump') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-cog" aria-hidden="true"></span>&nbsp;Aspiration</div>;
    }
    else if(this.state.left_state == 'disable') {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-off" aria-hidden="true"></span>&nbsp;Repos</div>;
    }
    else {
      state =  <div className="label label-default"><span className="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>&nbsp;???({this.state.left_state})</div>;
    }

    return (
      <div className="panel panel-default">
        <div className="panel-heading">
          Ventouses
        </div>
        <div className="panel-body">
          <div className="row">
            <div className="col-md-1">
              <label>Gauche:</label>
            </div>
            <div className="col-md-1">
              {state}
            </div>
            <div className="col-md-2">
            </div>
            <div className="col-md-8">
              <Parameter name="servo left" command="meca_sucker_left_servo" show={this.props.update}/>
            </div>
          </div>
          <div className="row">
            <div className="col-md-4">
              <div className="btn-group">
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.close_left}>Fermer</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.open_left}>Ouvrir</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.pump_left}>Aspirer</button>
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.disable_left}>Désactiver</button>
              </div>
            </div>
            <div className="col-md-8">
            </div>
          </div>
          <div className="row">
            <div className="col-md-1">
              <label>Droite:</label>
            </div>
            <div className="col-md-1">
              {state}
            </div>
            <div className="col-md-2">
            </div>
            <div className="col-md-8">
              <Parameter name="servo right" command="meca_sucker_right_servo" show={this.props.update}/>
            </div>
          </div>
          <div className="row">
            <div className="col-md-4">
              <div className="btn-group">
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.close_right}>Fermer</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.open_right}>Ouvrir</button>
              <button type="submit" className="btn btn-xs btn-warning small-margin-right" onClick={this.pump_right}>Aspirer</button>
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.disable_right}>Désactiver</button>
              </div>
            </div>
            <div className="col-md-8">
            </div>
          </div>

        </div>
      </div>
    );
  }
});

var SMeca = React.createClass({
  getInitialState: function() {
    return {
      name: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("info", this.handleReceiveInfo);
  },

  handleReceiveInfo: function(data) {
    if(data.answer.data.length >= 2) {
      this.setState({
        name: data.answer.data[0],
      });
    }
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    return (
      <div className={divClasses}>
        <div className="row">
          <div className="col-md-12 main">
            <div className="panel panel-primary">
              <div className="panel-heading">
                Meca
              </div>
              <div className="panel-body">
                <div className="row">
                  <div className="col-md-6">
                    <MecaFish update={this.props.show}/>
                  </div>
                  <div className="col-md-6">
                    <MecaSeaShell update={this.props.show}/>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});

var PMeca = React.createClass({
  getInitialState: function() {
    return {
      name: '',
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("info", this.handleReceiveInfo);
  },

  handleReceiveInfo: function(data) {
    if(data.answer.data.length >= 2) {
      this.setState({
        name: data.answer.data[0],
      });
    }
  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    return (
      <div className={divClasses}>
        <div className="row">
          <div className="col-md-12 main">
            <div className="panel panel-primary">
              <div className="panel-heading">
                Meca
              </div>
              <div className="panel-body">
                <div className="row">
                  <div className="col-md-6">
                    <MecaSucker update={this.props.show}/>
                  </div>
                  <div className="col-md-6">
                    <MecaUmbrella update={this.props.show}/>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
