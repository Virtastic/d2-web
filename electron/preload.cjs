// Sandboxed preload: exposes read-only facts and one request to the page. The request asks the shell to open the
// native chooser for the Diablo II folder or .mpq files; the shell validates, saves and reloads. The page does not
// call Node or Electron; the game logic never runs here.
const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('d2Desktop', Object.freeze({
  shell: 'electron',
  platform: process.platform,
  chooseGameFiles: () => ipcRenderer.invoke('d2:choose-game-files'),
}));
