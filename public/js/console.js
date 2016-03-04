var RConsole = React.createClass({
  UPDATE_PERIOD_MS: 100,
  MAX_LINES: 200,

  cachedLines: [],

  getInitialState: function() {
    socket.on('receive', this.handleReceive);


    return {
      lines: [],
    }
  },

  componentDidMount: function() {
    setTimeout(this.updateConsole, this.UPDATE_PERIOD_MS);
  },

  updateConsole: function() {
    this.setState({lines: this.cachedLines});
    setTimeout(this.updateConsole, this.UPDATE_PERIOD_MS);
  },

  handleReceive: function(data) {
    if(this.cachedLines.length >= this.MAX_LINES - 1) {
      this.cachedLines = this.cachedLines.slice(1);
    }
    this.cachedLines.push(data);
  },

  componentDidUpdate: function() {
    var node = document.getElementById("console-table-body");
    console.log("scroll");
    node.scrollTop = node.scrollHeight
  },

  renderLine: function(line) {
    
    var badge = null;
    if(line.requested) {
      badge = <span className="label label-space label-success">Manuel</span>;
    }
    else if(line.async) {
      badge = <span className="label label-space label-warning">Async</span>;
    }
    else {
      badge = <span className="label label-space label-default">Auto</span>;
    }

    var dt = new Date(line.date);

    return (
      <tr>
        <td style={{width: 75 + 'px'}}>{{badge}}</td>
        <td>{dt.toLocaleString('FR')}</td>
        <td style={{width: 75 + '%'}}>{line.command}={line.data}</td>
      </tr>
    );
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
                Console
              </div>
              <div className="panel-body">
                <table className="table table-bordered table-condensed header-fixed">
                  <tbody id="console-table-body">
                    { this.state.lines.map(this.renderLine) }
                  </tbody>
                </table>
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
