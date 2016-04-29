var TopMenuProtocol = React.createClass({

  getInitialState: function() {
    return {
      selected: null,
    };
  },

  componentWillReceiveProps: function(nextProp) {
    if(this.state.selected == null) {
      if(this.props.protocol.available.length > 0) {
        this.setState({selected: this.props.protocol.available[0].addr});
      }
    }
  },

  connect: function() {
    var self = this;
    var toserver = {
      addr: this.state.selected,
    };

    $.ajax({
      url: '/api/protocol',
      type: 'POST',
      contentType: 'application/json',
      data: JSON.stringify(toserver),
      success: function(data) {
        self.setState({
          connected: data.connected,
          available: data.available,
        });
      },
      error: function(data) {
        fireError("Connexion au robot", "Erreur serveur lors de la tentative de connection");
      }
    });
  },

  disconnect: function() {
    var self = this;

    $.ajax({
      url: '/api/protocol',
      type: 'DELETE',
      success: function(data) {
        self.setState({
          connected: data.connected,
          available: data.available,
        });
      },
      error: function(data) {
        fireError("Connexion au robot", "Erreur serveur lors de la tentative de deconnection");
      }
    });
  },

  reboot: function() {
    utils.sendCommand({command: "system_reboot", argument: null});
  },


  renderAvailable: function(pcol, idx) {
    return (
      <option key={idx} value={pcol.addr}>{pcol.type}: {pcol.addr}</option>
    );
  },

  handleChange: function(event) {
    this.setState({selected: event.target.value});
  },

  render: function() {
    if(this.props.protocol.connected) {
      return(
        <div className="navbar-form navbar-right">
          <button type="submit" className="btn btn-warning small-margin-right" onClick={this.disconnect}>Deconnexion</button>
          <button type="submit" className="btn btn-danger" onClick={this.reboot}>Reboot du robot</button>
        </div>
      );

    }
    else {
      return(
        <div className="navbar-form navbar-right">
          <div className="form-group">
            <select className="form-control small-margin-right" style={{width: 200 + "px"}} value={this.state.selected} onChange={this.handleChange} >
            { this.props.protocol.available.map(this.renderAvailable) }
            </select>
          </div>
          <button type="submit" className="btn btn-success" onClick={this.connect}>Connexion</button>
        </div>
      );
    }
  },
});

var TopMenuInfo = React.createClass({
  UPDATE_PERIOD_MS: 1000,
  
  getInitialState: function() {
    return {
      battery : -1,
      responseTime: -1,
      name: "",
    };
  },

  componentDidMount: function() {
    utils.onReceiveCommand("info", this.handleReceive);
    setTimeout(this.updateInfo, this.UPDATE_PERIOD_MS);
  },

  updateInfo: function() {
    utils.sendCommand({command: "info", argument: null});
    this.setState({});
    setTimeout(this.updateInfo, this.UPDATE_PERIOD_MS);
  },

  handleReceive: function(data) {
    if(data.answer.data.length >= 2) {
      this.setState({
        name: data.answer.data[0],
        battery: parseFloat(data.answer.data[1]) / 1000.0,
        responseTime: utils.responseTime,
      });
      this.props.ui.setState({name: data.answer.data[0]});
    }
  },

  render: function() {
    var batteryDanger = 11.3;
    var batteryWarning = 11.6;
    var batteryClass = "label small-margin-right";

    if(this.state.battery < batteryDanger) {
      batteryClass += " label-danger";
    }
    else if(this.state.battery < batteryWarning) {
      batteryClass += " label-warning";
    }
    else {
      batteryClass += " label-success";
    }

    var responseTimeDanger = 80;
    var responseTimeWarning = 25;
    var responseTimeClass = "label small-margin-right";

    if(this.state.responseTime > responseTimeDanger) {
      responseTimeClass += " label-danger";
    }
    else if(this.state.responseTime > responseTimeWarning) {
      responseTimeClass += " label-warning";
    }
    else {
      responseTimeClass += " label-success";
    }

    var generalClasses = "navbar-form navbar-right";

    if(!utils.connected) {
      generalClasses += " hide";
    }

    return(
      <div className={generalClasses}>
        <span className="label label-info small-margin-right">
          {this.state.name}
        </span>      
        <span className={batteryClass}>
          Batterie: {this.state.battery} V
        </span>      
        <span className={responseTimeClass}>
          Temps de r&eacute;ponse: {this.state.responseTime} ms
        </span>      
      </div>
    );
  
  },
});

//handle top menu item (layout and action)
var TopMenuItem = React.createClass({
  //handle click
  onClick: function() {
    this.props.onClick(this.props.pkey);
  },

  //generate the layout
  render: function() {
    var activeClass = '';

    //if active, put specific css class
    if(this.props.page == this.props.pkey) {
      activeClass = "active";
    }

    return(
      <li className={activeClass}><a href="#" onClick={this.onClick}>{this.props.name}</a></li>
    );
  }
});


//generate the top menu
var TopMenu = React.createClass({

  //generate the layout
  render: function() {
    return(
      <nav className="navbar navbar-inverse navbar-fixed-top">
        <div className="container">
          <div className="navbar-header">
            <a className="navbar-brand" href="#">Chrono</a>            
          </div>
          <div className="collapse navbar-collapse">
            <ul className="nav navbar-nav">
              {this.props.children}
            </ul>
            <TopMenuProtocol protocol={this.props.protocol}/>
            <TopMenuInfo ui={this.props.ui}/>
          </div>
        </div>
      </nav>
    );
  }
});

