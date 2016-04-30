var OpponentUsir = React.createClass({
  UPDATE_PERIOD_MS: 1000,

  getInitialState: function() {
    var obj = {
      us: -1,
      ir: -1,
      alert_activated: false,
      alert: false,
    };
    return {
      state: [obj, obj, obj, obj],
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("opponent_usir", this.handleReceiveUsirState);
    
    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  handleReceiveUsirState: function(data) {
    var state = [];
    for(var i = 0; i < 4; i += 1) {
      var dec = data.answer.data[i].split(' ');
      var obj = {
        us: parseInt(dec[0]), 
        ir: parseInt(dec[1]), 
        alert_activated: dec[2] == "0" ? false : true, 
        alert: dec[3] == "0" ? false : true, 
        forceon: dec[4] == "0" ? false : true, 
      };
      state.push(obj);
    }
    this.setState({state: state});
  },

  update: function() {
    if(this.props.update) {
      utils.sendCommand({command: "opponent_usir", argument: null});
    }

    setTimeout(this.update, this.UPDATE_PERIOD_MS);
  },

  setForceOn: function(i,v) {
    if(v) {
      v = " 1";
    }
    else {
      v = " 0";
    }
    utils.sendCommand({command: "opponent_usir_force "+ i + v, argument: null});
    utils.sendCommand({command: "opponent_usir", argument: null});
  },

  renderUsir: function(usir, i) {
    var name = '';
    var state = null;

    switch(i) {
      case 0:
        name = "Devant gauche";
        break;

      case 1:
        name = "Devant droit";
        break;

      case 2:
        name = "Arrière gauche";
        break;

      case 3:
        name = "Arrière droit";
        break;
    }

    if(usir.alert) {
      state =  <div className="label label-warning"><span className="glyphicon glyphicon-arrow-up" aria-hidden="true"></span>&nbsp;Alerte</div>;
    }
    else if(usir.alert_activated) {
      state =  <div className="label label-success"><span className="glyphicon glyphicon-arrow-right" aria-hidden="true"></span>&nbsp;Activé</div>;
    }
    else {
      state =  <div className="label label-default"><span className="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>&nbsp;Repos</div>;
    }

    var self = this;

    return (
      <div>
        <div>
          <b>{name}</b>: US = {usir.us}mm / IR = {usir.ir}mm
        </div>
        {state}
        <div className="checkbox small-margin-right">
          <label>
            <input type="checkbox" checked={usir.forceon} onChange={function() { self.setForceOn(i, !usir.forceon)}}/>Force on
          </label>
        </div>
      </div>
   );

  },

  render: function() {
    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    

    return (
      <div className="panel panel-default">
        <div className="panel-heading">
          USIR
        </div>
        <div className="panel-body">
          <div className="row">
            <div className="col-md-6">
              {this.renderUsir(this.state.state[0], 0)}
            </div>
            <div className="col-md-6">
              {this.renderUsir(this.state.state[1], 1)}
            </div>
          </div>
          <div className="row">
            <div className="col-md-6">
              {this.renderUsir(this.state.state[2], 2)}
            </div>
            <div className="col-md-6">
              {this.renderUsir(this.state.state[3], 3)}
            </div>
          </div>
        </div>
      </div>
    );
  }
});
var Opponent = React.createClass({
  getInitialState: function() {
    return {
    };
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
                Adversaire
              </div>
              <div className="panel-body">
                <div className="row">
                  <div className="col-md-12">
                    <OpponentUsir update={this.props.show}/>
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
