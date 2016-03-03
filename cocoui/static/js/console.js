var RConsole = React.createClass({
  getInitialState: function() {
    return {
      lines: [],
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
                Console
              </div>
            </div>
          </div>
        </div>
      </div>
    );
  }
});
