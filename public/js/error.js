var Error = React.createClass({
  render: function() {
    return(
        <div className="row">
          <div className="col-md-6 col-md-offset-3 main">
            <div className="alert alert-danger" role="alert" onClick={this.props.onClose}>
              <button type="button" className="close"><span aria-hidden="true">&times;</span></button>
              <strong>{this.props.data.title}:</strong> {this.props.data.msg}
            </div>
        </div>
      </div>
    );
  },
});

