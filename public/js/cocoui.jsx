var fireError = function() {
  //dummy function
};

var GlobalUtils = function() {
  var self = this;

  this.receiveHandler = [];
  this.receiveHandlerFiltered = {};
  this.connected = false;    
  this.responseTime = -1;
  this.socket = io.connect('http://' + document.domain + ':' + location.port);
  this.socket.on('receive', function(data) { self.handleReceive(data); });
};

GlobalUtils.prototype.sendCommand = function(command) {
  this.socket.emit('send', command);
};


GlobalUtils.prototype.handleReceive = function(data) {
  if(data.answer.async) {
    data.request = {};
    var cmd = data.answer.data[0].split("=");
    data.request.command = cmd[0];
  }

  for(var i in this.receiveHandler) {
    this.receiveHandler[i](data);
  }

  if(data.request != null) {
    if(!data.answer.async) {
      this.responseTime = data.answer.date - data.request.date;
    }

    if(data.request.command in this.receiveHandlerFiltered) {
      var arr = this.receiveHandlerFiltered[data.request.command];
      for(var i in arr) {
        arr[i](data);
      }
    }
  }
};

GlobalUtils.prototype.onReceiveCommand = function(filter, handler) {
  if(filter == "*") {
    this.receiveHandler.push(handler);
  }
  else {
    if(!(filter in this.receiveHandlerFiltered)) {
      this.receiveHandlerFiltered[filter] = [];
    }
    this.receiveHandlerFiltered[filter].push(handler);
  }
};


var utils = new GlobalUtils();

var Loading = React.createClass({
  getInitialState: function() {
    return {};
  },

   render: function() {
    var cname = "panel-body";

    if(!this.props.loading) {
      cname += " hide";
    }

    return (
      <div className={cname}>
        <div className="alert alert-info">
          <span className="glyphicon glyphicon-refresh" />
          &nbsp;Chargement de la page...
        </div>
      </div>
    );
  }
});

var LoadingLogic = {
  getParentInitialState: function(obj) {
    obj.loading = true;
    obj.loading_inprogress = false;
    return obj;
  },

  handleDidMount: function(cls) {
    LoadingLogic.handleWillReceiveProps(cls, cls.props);
  },

  handleWillReceiveProps: function(cls, nextProp) {
    var loading = false;
    if(nextProp.actionID != cls.props.actionID) {
      cls.setState({loading: true, loading_inprogress: false});
      loading = true;
      if(cls.state.debug)
      {
        console.log("FORCE L");
        console.log(cls.state);
      }
    }

    if((((cls.state.loading == true) && (cls.state.loading_inprogress == false)) || loading) && (nextProp.show == true)) {
      if(cls.state.debug)
      {
        console.log("REQUEST LOAD");
        console.log(cls.state);
      }
      cls.setState({loading_inprogress: true});
      cls.state.loading_inprogress = true; //UGLY HACK
      cls.load(true, nextProp.actionID, nextProp.pageArgs);
    }

    if((nextProp.show == false) && (cls.state.loading == false)) {
      var initState = cls.getInitialState();
      initState.loading_inprogress = cls.state.loading_inprogress;
      cls.setState(initState);
    }
      if(cls.state.debug)
      {
        console.log("--END--");
        console.log(cls.state);
      }
  },

  handleLoaded: function(cls, actionID) {
    if(actionID == cls.props.actionID) {
      cls.setState({loading_inprogress: false, loading: false});
      return true;
    }
    else {
      return false;
    }
  },

};

//create the page layout
var Cocoui = React.createClass({
  UPDATE_PERIOD_MS: 750,

  //set default state
  getInitialState: function() {
    var self = this;

    History.Adapter.bind(window,'statechange',function(){
      self.setState(History.getState()['data']);
    });

    return {
      page: DEFAULT_PAGE, //set default tab
      pageArgs: DEFAULT_PAGE_ARGS,
      actionID: 1,
      topMenuItem: [
        this.createTopMenuItem('Position', 'position'), 
        this.createTopMenuItem('Console', 'console'), 
        this.createTopMenuItem('Asserv', 'asserv'), 
        this.createTopMenuItem('Meca', 'smeca'), 
      ],
      errors: [],
      protocol: this.getProtocolInitialState(),
      bootloader_msg: '',
    };
  },

  getProtocolInitialState: function() {
    return {
      connected: false,
      bootloader: false,
      available: [],
    };
  },

  componentDidMount: function() {
    var self = this;

    fireError = this.fireError;
    setTimeout(this.updateProtocolStatus, this.UPDATE_PERIOD_MS);
    utils.socket.on('bootloader', function(data) { self.setState({bootloader_msg: data.msg}); });
  },

  updateProtocolStatus: function() {
    var self = this;

    $.ajax({
      url: '/api/protocol',
      type: 'GET',
      success: function(data) {
        self.setState({protocol: data});
        utils.connected = data.connected;
        setTimeout(self.updateProtocolStatus, self.UPDATE_PERIOD_MS);
      },
      error: function(data) {
        self.setState({protocol: self.getProtocolInitialState()});
        utils.connected = false;
        setTimeout(self.updateProtocolStatus, self.UPDATE_PERIOD_MS);
      }
    });

  },
  
  //create valid topMenuItem
  createTopMenuItem: function(name, pkey) {
    return {name: name, pkey: pkey, key: this.assignNewKey()};
  },


  //create new unique key
  assignNewKey: function() {
    if(this.current_key === undefined) {
      this.current_key = 0;
    }
    this.current_key += 1;
    return this.current_key;
  },
    

  //handle top menu clicks (change tab)
  TopMenuChange: function(newpage) {
    this.goTo(newpage);
  },

  goTo: function(page, args, link) {
    var url = "/page/" + page;

    args = args || [];
    for(var i = 0, _l = args.length; i < _l; i += 1) {
      url += "/" + args[i];
    }

    this.setState({page: page, pageArgs: args});
    History.pushState({page: page, pageArgs:args}, "", url);
    this.setState({actionID: this.state.actionID + 1});
  },

  renderGoTo: function(page, args, link) {
    var self = this;

    var url = "/page/" + page;

    args = args || [];
    for(var i = 0, _l = args.length; i < _l; i += 1) {
      url += "/" + args[i];
    }

    return(
     <a href={url} onClick={function(evt) { evt.preventDefault(); self.goTo(page, args); }}>{link}</a>
    );
  },


  //dynamically generate top menu items
  renderTopMenuItem: function(item) {
    return(<TopMenuItem key={item.key} name={item.name} pkey={item.pkey} page={this.state.page} onClick={this.TopMenuChange} />);
  },


  fireError: function(title, msg) {
    var err = this.state.errors;
    err.push({title: title, msg: msg});
    this.setState({errors: err});
  },

  closeError: function(idx) {
    var err = this.state.errors;
    err.splice(idx, 1);
    this.setState({errors: err});
  },
  
  //create components
  render: function() {
    var errors = [];
    for(var i in this.state.errors) {
      (function(errors, self, i, data) {
       errors.push(<Error data={data} key={i} onClose={function() {self.closeError(i)}}/>);
      })(errors, this, i, this.state.errors[i]);
    }

    var bloaderCls = "row";
    if(!this.state.protocol.bootloader) {
      bloaderCls += " hide";
    }

    return(
      <div>
        <TopMenu protocol={this.state.protocol}>
          { this.state.topMenuItem.map(this.renderTopMenuItem) }
        </TopMenu>
        <div className={bloaderCls}>
          <div className="col-md-6 col-md-offset-3 main">
            <div className="alert alert-info" role="alert">
              <strong>Bootloader in progress:</strong> {this.state.bootloader_msg}
            </div>
          </div>
        </div>
        {errors}
        <Position page={this.state.page} pageArgs={this.state.pageArgs} chrono={this} show={this.state.page == 'position'} actionID={this.state.actionID}/>
        <RConsole page={this.state.page} pageArgs={this.state.pageArgs} chrono={this} show={this.state.page == 'console'} actionID={this.state.actionID}/>
        <Asserv page={this.state.page} pageArgs={this.state.pageArgs} chrono={this} show={this.state.page == 'asserv'} actionID={this.state.actionID}/>
        <SMeca page={this.state.page} pageArgs={this.state.pageArgs} chrono={this} show={this.state.page == 'smeca'} actionID={this.state.actionID}/>
      </div>
    );
  }
});
//render the layout
ReactDOM.render(<Cocoui />, document.getElementById('app'));

