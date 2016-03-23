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
            <div className="col-md-3">
              <label>&Eacute;tat:</label>
            </div>
            <div className="col-md-3">
              {state}
            </div>
          </div>
          <div className="row">
            <div className="col-md-6">
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.close}>Fermer</button>
            </div>
            <div className="col-md-6">
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.open}>Ouvrir</button>
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
            <div className="col-md-2">
              <label>&Eacute;tat:</label>
            </div>
            <div className="col-md-2">
              {state}
            </div>
            <div className="col-md-2">
            </div>
            <div className="col-md-6">
              <Parameter name="servo droit" command="meca_seashell_right_servo" show={this.props.update}/>
            </div>
          </div>
          <div className="row">
            <div className="col-md-2">
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.close}>Fermer</button>
            </div>
            <div className="col-md-2">
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.open}>Ouvrir</button>
            </div>
            <div className="col-md-2">
              <button type="submit" className="btn btn-xs btn-warning" onClick={this.take}>Prendre</button>
            </div>
            <div className="col-md-6">
              <Parameter name="servo gauche" command="meca_seashell_left_servo" show={this.props.update}/>
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
                  <div className="col-md-3">
                    <MecaUmbrella update={this.props.show}/>
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
