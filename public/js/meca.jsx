var MecaUmbrella = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    return {
      state: 'close',
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
var Meca = React.createClass({
  getInitialState: function() {
    return {
      umbrella_state: 'close',
    };
  },

  componentDidMount: function() {
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
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
