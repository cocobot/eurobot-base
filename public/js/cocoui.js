var socket = io.connect('http://' + document.domain + ':' + location.port);

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
        this.createTopMenuItem('Console', 'console'), 
      ],
    };
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

  
  //create components
  render: function() {
    return(
      <div>
        <TopMenu>
          { this.state.topMenuItem.map(this.renderTopMenuItem) }
        </TopMenu>
        <RConsole page={this.state.page} pageArgs={this.state.pageArgs} chrono={this} show={this.state.page == 'console'} actionID={this.state.actionID}/>
      </div>
    );
  }
});
//render the layout
React.render(<Cocoui />, document.getElementById('app'));

