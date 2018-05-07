const webpack = require('webpack');
const EncodingPlugin = require('webpack-encoding-plugin');

module.exports = {
  entry: './app/app.js',
  output: {path: __dirname + '/output', filename: 'bundle.js' },
  devtool: 'source-map',
  target: 'electron-renderer',
  module: {
    loaders: [
      {
        test: /.jsx?$/,
        loader: 'babel-loader',
        exclude: /node_modules/,
        query: {
          presets: ['env', 'react']
        }
      },
      {
        test: /\.(css)$/,
        loader: 'style-loader!css-loader'
      },
    ]
  },
  plugins: [
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify('production')
    }),
    new EncodingPlugin({
      encoding: 'iso-8859-1'
    })
   ],
};
