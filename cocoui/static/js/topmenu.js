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
          </div>
        </div>
      </nav>
    );
  }
});

