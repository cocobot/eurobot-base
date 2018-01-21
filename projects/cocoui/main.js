const electron = require('electron');
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;

const Protocol = require('./protocol');

const protocol = new Protocol();
 
app.on('ready', () => {
  mainWindow = new BrowserWindow({
    height: 650,
    width: 1200,
  });

  const url = require('url').format({
    protocol: 'file',
    slashes: true,
    pathname: require('path').join(__dirname, 'static', 'app.html')
  });
 
  mainWindow.loadURL(url);
});
