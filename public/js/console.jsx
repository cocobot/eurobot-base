var RConsole = React.createClass({
  UPDATE_PERIOD_MS: 100,
  MAX_LINES: 50,

  cachedLines: [],

  getInitialState: function() {
    utils.onReceiveCommand("*", this.handleReceive);

    return {
      lines: [],
      command: "",
      showAuto: false,
      showAsync: false,
    }
  },

  componentDidMount: function() {
    setTimeout(this.updateConsole, this.UPDATE_PERIOD_MS);
  },

  updateConsole: function() {
    for(var i in this.cachedLines) {
      if(this.cachedLines[i].rendered == undefined) {
        this.cachedLines[i].rendered = this.renderLine(this.cachedLines[i]);
      }
    }

    this.setState({lines: this.cachedLines});
    setTimeout(this.updateConsole, this.UPDATE_PERIOD_MS);
  },

  handleReceive: function(data) {
    if(this.cachedLines.length >= this.MAX_LINES - 1) {
      this.cachedLines.shift();
    }
    this.cachedLines.push(data);
  },

  componentDidUpdate: function() {
    var node = document.getElementById("console-table-body");
    node.scrollTop = node.scrollHeight
  },

  renderLine: function(line) {
    
    var badge = null;
    if((line.request != undefined) && (line.request.console != undefined)) {
      badge = <span className="label label-space label-success">Manuel</span>;
    }
    else if(line.answer.async) {
      badge = <span className="label label-space label-warning">Async</span>;
    }
    else {
      badge = <span className="label label-space label-default">Auto</span>;
    }

    var dt = new Date(line.answer.date);
    var content = [];
    if(line.request != null) {
      content.push(
        <div key={content.length}>
          <span className="glyphicon glyphicon-chevron-right" aria-hidden="true"></span>&nbsp;
          {line.request.command}
        </div>
      );
    }
    for(var i in line.answer.data) {
      content.push(
        <div key={content.length}>
          <span className="glyphicon glyphicon-chevron-left" aria-hidden="true"></span>&nbsp;
          {line.answer.data[i]}
        </div>
      );
    }

    return (
      <tr>
        <td style={{width: 75 + 'px'}}>{{badge}}</td>
        <td>{dt.toLocaleString('FR')}</td>
        <td style={{width: 75 + '%'}}>{content}</td>
      </tr>
    );
  },

  onChange: function(event) {
    this.setState({command: event.target.value});
  },

  send: function(event) {
    event.preventDefault();
    if(this.state.command != "") {
      utils.sendCommand({command: this.state.command, argument: null, console: "12345"});
      this.setState({command: ""});
    }
  },

  drawLine: function(line) {
    return line.rendered;
  },

  onChangeShowAsync: function(event) {
    this.setState({showAsync: event.target.checked});
  },

  onChangeShowAuto: function(event) {
    this.setState({showAuto: event.target.checked});
  },

  render: function() {

    var divClasses = 'container-fluid';
    var panelClasses = 'panel-body';

    if(!this.props.show) {
      divClasses += ' hide';
    }

    var filterList = this.state.lines;
    if(!this.state.showAsync) {
      filterList = filterList.filter(function(x) { return !x.answer.async;});
    }
    if(!this.state.showAuto) {
      filterList = filterList.filter(function(x) { return (x.request != null) && (x.request.console != undefined);});
    }

    return (
      <div className={divClasses}>
        <div className="row">
          <div className="col-md-12 main">
            <div className="panel panel-primary">
              <div className="panel-heading">
                Console
              </div>
              <div className="panel-body">
                <form className="form-inline">
                  <div className="checkbox ">
                    <label>
                      <input type="checkbox" checked={this.state.showAuto} onChange={this.onChangeShowAuto}/> Montrer les commandes automatiques &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                    </label>
                  </div>
                  <div className="checkbox">
                    <label>
                      <input type="checkbox" checked={this.state.showAsync} onChange={this.onChangeShowAsync}/> Montrer les commandes asynchrones
                    </label>
                  </div>
                </form>
                <table className="table table-bordered table-condensed header-fixed">
                  <tbody id="console-table-body">
                    { filterList.map(this.drawLine) }
                  </tbody>
                </table>
                <form className="form-horizontal" onSubmit={this.send}>
                  <div className="form-group">
                    <label className="col-md-1 control-label">
                      Commande:
                    </label>
                    <div className="col-md-11">
                      <input className="form-control" type="text" placeholder="Commande" value={this.state.command} onChange={this.onChange}/>
                    </div>
                  </div>
                </form>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
