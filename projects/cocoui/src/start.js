const electron = require('electron')
const app = electron.app
const path = require('path')
const isDev = require('electron-is-dev')
require('electron-reload')
const BrowserWindow = electron.BrowserWindow
const Protocol = require('./server/protocol');

//create protocol handler
const protocol = new Protocol();

//create main window
let mainWindow = null;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: true,
    },
  })

  mainWindow.loadURL(
    isDev
      ? 'http://localhost:3000'
      : `file://${path.join(__dirname, '../build/index.html')}`,
  )

  mainWindow.on('closed', () => {
    mainWindow = null
  })
}

//register signals
app.on('ready', createWindow)
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
app.on('activate', () => {
  if (mainWindow === null) {
    createWindow()
  }
})
process.on('uncaughtException', function (err) {
  console.error(err.stack);
  app.quit();
});


