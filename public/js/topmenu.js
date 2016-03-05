var TopMenuProtocol = React.createClass({
  UPDATE_PERIOD_MS: 750,
  
  getInitialState: function() {
    return {
      connected: false,
      available: [],
      selected: null,
    };
  },

  componentDidMount: function() {
    setTimeout(this.updateProtocolStatus, this.UPDATE_PERIOD_MS);
  },

  updateProtocolStatus: function() {
    var self = this;

    $.ajax({
      url: '/api/protocol',
      type: 'GET',
      success: function(data) {
        self.setState({
          connected: data.connected,
          available: data.available,
        });
        if(self.state.selected == null) {
          self.setState({selected: data.available[0].addr});
        }
        setTimeout(self.updateProtocolStatus, self.UPDATE_PERIOD_MS);
      },
      error: function(data) {
        self.setState(self.getInitialState());
        setTimeout(self.updateProtocolStatus, self.UPDATE_PERIOD_MS);
      }
    });

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


  renderAvailable: function(pcol, idx) {
    return (
      <option key={idx} value={pcol.addr}>{pcol.type}: {pcol.addr}</option>
    );
  },

  handleChange: function(event) {
    this.setState({selected: event.target.value});
  },

  render: function() {
    if(this.state.connected) {
      return(
        <div className="navbar-form navbar-right">
          <button type="submit" className="btn btn-danger" onClick={this.disconnect}>Deconnexion</button>
        </div>
      );

    }
    else {
      return(
        <div className="navbar-form navbar-right">
          <div className="form-group">
            <select className="form-control small-margin-right" style={{width: 200 + "px"}} value={this.state.selected} onChange={this.handleChange} >
            { this.state.available.map(this.renderAvailable) }
            </select>
          </div>
          <button type="submit" className="btn btn-success" onClick={this.connect}>Connexion</button>
        </div>
      );
    }
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
            <TopMenuProtocol />
          </div>
        </div>
      </nav>
    );
  }
});

