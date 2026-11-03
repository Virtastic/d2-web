// Electron main process for the d2-web desktop app. A window onto the staged site, served from a privileged
// app://d2/ origin by lib/site-protocol.mjs (the answer rules, pure and tested). The page is not changed:
// it is the same index.html the browser build serves, with the same engine.
//
// Game data is never in the package. Every start the shell tries the saved folder, then the usual install places
// (lib/detect.mjs), validating each (lib/retail.mjs: d2data.mpq and the 1.13c Patch_D2.mpq; lib/startup.mjs holds the
// decisions). Found: door art is cut into userData/derived (lib/derive.mjs) and the archives are served from the folder as
// app://d2/retail/<name>, Range included; the page is told it is a hosted host (site-config.json) and opens at the full
// front door. Not found: a modal offers "Choose files..." (a folder or .mpq files; saved, derived, window reloaded, no
// restart) or "Not now" (the page's own byo front door). The menu item Game files > Change folder... and the byo door's
// buttons (window.d2Desktop.chooseGameFiles) take the same route.
//
// Environment (for tests and the smoke run; a player never sets these):
//   D2_DESKTOP_SITE        staged site root (default: app-site beside this file, or resources/site when packaged)
//   D2_DESKTOP_TEST_ANSWER, D2_DESKTOP_TEST_PICK, D2_DESKTOP_NO_DETECT  answer the dialogs / skip detection (see ui below)
//   D2_DESKTOP_USERDATA    userData directory override (smoke runs keep their config out of the profile)
//   D2_DESKTOP_SMOKE=1     load the page, wait for the engine, print one D2_DESKTOP_SMOKE line, exit 0 or 1
import { app, BrowserWindow, Menu, dialog, ipcMain, protocol, session } from 'electron';
import { createReadStream, writeSync } from 'node:fs';
import { Readable } from 'node:stream';
import { delimiter, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { HOST, PRIVILEGES, SCHEME, createSiteHandler } from './lib/site-protocol.mjs';
import { detectRetailDir } from './lib/detect.mjs';
import { ensureDerived } from './lib/derive.mjs';
import { checkRetailDir, listArchives, readConfig, writeConfig } from './lib/retail.mjs';
import { configFor, resolveStartup, validatePick } from './lib/startup.mjs';

const HERE = dirname(fileURLToPath(import.meta.url));
const SMOKE = process.env.D2_DESKTOP_SMOKE === '1';
const SITE = resolve(process.env.D2_DESKTOP_SITE || (app.isPackaged ? join(process.resourcesPath, 'site') : join(HERE, 'app-site')));
const PAGE = `${SCHEME}://${HOST}/`;
const ALLOWED_PERMISSIONS = new Set(['fullscreen', 'pointerLock']);

if (process.env.D2_DESKTOP_USERDATA) app.setPath('userData', resolve(process.env.D2_DESKTOP_USERDATA));
const CONFIG = join(app.getPath('userData'), 'config.json');

// Must run before the app is ready. Standard, secure, fetch-capable and streamable: the page's fetch() and
// its HEAD/Range reads of the archives go through this scheme.
protocol.registerSchemesAsPrivileged([{ scheme: SCHEME, privileges: PRIVILEGES }]);
// The boot movie and the sound effects start on their own at the title screen.
app.commandLine.appendSwitch('autoplay-policy', 'no-user-gesture-required');

let win = null;
let retailDir = null;      // the player's folder, or null (byo)
let retailMap = null;     // lower-case name -> path, from listArchives(retailDir)
let derivedCur = null;    // userData/derived/current once door-art/ is cut from the archives, else null
let canUseBr = () => false;

function setRetail(dir) {
  retailDir = dir || null;
  retailMap = retailDir ? listArchives(retailDir) : null;
}

const isOurs = (url) => {
  try { const u = new URL(url); return u.protocol === `${SCHEME}:` && u.hostname === HOST; } catch { return false; }
};

// One answer rule for every request. Electron hands us the Request; plan() decides; the body is a stream of the file slice.
const plan = createSiteHandler({
  siteRoot: SITE,
  retailFiles: () => retailMap,
  derivedDir: () => derivedCur,
  siteConfig: () => ({ assets: retailMap ? 'hosted' : 'byo' }),
  canUseBr: (plain, br) => canUseBr(plain, br),
});


// The dialogs. A test hook answers them in the smoke run (never set by a player):
//   D2_DESKTOP_TEST_ANSWER  'choose' drives the chooser when nothing was found (any value also answers the error modal: cancel)
//   D2_DESKTOP_TEST_PICK    path(s), separated by the OS path delimiter, returned by the native chooser
//   D2_DESKTOP_NO_DETECT=1  skip the install-location search (a smoke run on a host that has a game)
const TEST_ANSWER = process.env.D2_DESKTOP_TEST_ANSWER || '';
const TEST_PICKS = process.env.D2_DESKTOP_TEST_PICK ? process.env.D2_DESKTOP_TEST_PICK.split(delimiter) : null;
const ui = {
  /** The native chooser: a folder or several .mpq files. null on cancel. */
  async pick() {
    if (TEST_PICKS) { const p = TEST_PICKS.splice(0); return p.length ? p : null; }
    const r = await dialog.showOpenDialog(win || undefined, { title: 'Choose your Diablo II folder or its .mpq files',
      properties: ['openDirectory', 'openFile', 'multiSelections'], filters: [{ name: 'Diablo II archives', extensions: ['mpq'] }] });
    return r.canceled || !r.filePaths.length ? null : r.filePaths;
  },
  async invalid(message) {
    if (TEST_ANSWER) return false;
    const { response } = await dialog.showMessageBox(win || undefined, { type: 'error', title: 'Diablo II files cannot be used',
      message: 'Those files cannot be used', detail: message, buttons: ['Choose again', 'Cancel'], defaultId: 0, cancelId: 1, noLink: true });
    return response === 0;
  },
};

let choosing = false;
/** Choose until valid (a clear error, then retry) or cancelled. On success: save, derive, hosted, reload the window. Resolves true when applied. */
async function chooseAndApply() {
  if (choosing) return false;
  choosing = true;
  try {
    for (;;) {
      const paths = await ui.pick();
      if (!paths) return false;
      const v = validatePick(paths, checkRetailDir);
      if (v.ok) {
        writeConfig(CONFIG, configFor(v.dir, 'chosen'));
        setRetail(v.dir);
        await deriveArt();
        if (win) await new Promise((done) => { win.webContents.once('did-finish-load', done); win.webContents.reload(); });
        return true;
      }
      console.error('choose: ' + v.message.split('\n')[0]);
      if (!await ui.invalid(v.message)) return false;
    }
  } finally { choosing = false; }
}

/** Every launch: the saved folder first, then the usual places. True when a valid install was found (hosted). */
function startupRetail() {
  const r = resolveStartup({ saved: readConfig(CONFIG).retailDir, check: checkRetailDir,
    detect: () => (process.env.D2_DESKTOP_NO_DETECT === '1' ? null : detectRetailDir(checkRetailDir)) });
  if (!r) return false;
  if (r.source === 'detected') writeConfig(CONFIG, configFor(r.dir, 'detected'));
  setRetail(r.dir);
  return true;
}

/** Cuts door-art/ from the player's archives into userData/derived/ (once; cached by their sizes and mtimes). */
async function deriveArt() {
  derivedCur = null;
  if (!retailDir || !retailMap) return;
  derivedCur = await ensureDerived({ derivedRoot: join(app.getPath('userData'), 'derived'), archives: retailMap, siteRoot: SITE, srcDir: retailDir,
    log: (m) => console.error(m) });
}

function buildMenu() {
  const mac = process.platform === 'darwin';
  // No accelerators on game keys. F11 is the only accelerator here; the mac app menu keeps Cmd+Q (a game key never uses Cmd).
  Menu.setApplicationMenu(Menu.buildFromTemplate([
    ...(mac ? [{ role: 'appMenu' }] : []),
    { label: 'Game', submenu: [
      { label: 'Game files', submenu: [{ label: 'Change folder…', click: () => chooseAndApply() }] },
      { label: 'Full screen', accelerator: 'F11', click: () => win && win.setFullScreen(!win.isFullScreen()) },
      ...(mac ? [] : [{ label: 'Quit', click: () => app.quit() }]),
    ] },
  ]));
}

ipcMain.handle('d2:choose-game-files', (e) => (isOurs(e.senderFrame ? e.senderFrame.url : '') ? chooseAndApply() : false));

function createWindow() {
  win = new BrowserWindow({
    width: 1280, height: 800, title: 'Diablo II', show: true, backgroundColor: '#000000',
    autoHideMenuBar: true,
    webPreferences: {
      preload: join(HERE, 'preload.cjs'),
      contextIsolation: true, nodeIntegration: false, sandbox: true, webSecurity: true,
      spellcheck: false,
    },
  });
  win.on('closed', () => { win = null; });
  win.loadURL(PAGE);
}

app.on('web-contents-created', (_e, contents) => {
  // Nothing leaves the app: links, redirects, popups and webviews are refused.
  contents.on('will-navigate', (e, url) => { if (!isOurs(url)) e.preventDefault(); });
  contents.on('will-redirect', (e, url) => { if (!isOurs(url)) e.preventDefault(); });
  contents.setWindowOpenHandler(() => ({ action: 'deny' }));
  contents.on('will-attach-webview', (e) => e.preventDefault());
});


/** The app:// scheme and the permission answers. Both need the default session, so they are set once the app is ready. */
function registerSiteAndPermissions() {
  protocol.handle(SCHEME, (request) => {
    let p;
    try {
      p = plan({ url: request.url, method: request.method, headers: Object.fromEntries(request.headers) });
    } catch (err) {
      console.error('site: ' + (err && err.message));
      return new Response(null, { status: 500 });
    }
    if (p.text !== undefined) return new Response(p.text, { status: p.status, headers: p.headers });
    const body = p.file && request.method !== 'HEAD' && (p.status === 200 || p.status === 206)
      ? Readable.toWeb(createReadStream(p.file, { start: p.start, end: p.end }))
      : null;
    return new Response(body, { status: p.status, headers: p.headers });
  });

  session.defaultSession.setPermissionRequestHandler((_wc, permission, callback, details) =>
    callback(ALLOWED_PERMISSIONS.has(permission) && isOurs(details.requestingUrl || '')));
  session.defaultSession.setPermissionCheckHandler((_wc, permission, origin) =>
    ALLOWED_PERMISSIONS.has(permission) && isOurs(origin || ''));
}

/** The smoke run: the real page, the real engine. Prints one line of facts, exits 0 when the engine is up. */
async function smoke() {
  writeSync(1, 'D2_DESKTOP_SMOKE start\n');
  // Diagnostics: what the renderer said and which requests failed (the smoke's verdict line stays one line).
  const diag = [];
  const note = (s) => { if (diag.length < 60) diag.push(String(s).slice(0, 300)); };
  win.webContents.on('console-message', (e) => { if ((e.level === 'error' || e.level === 'warning') ) note(`console.${e.level}: ${e.message} @${e.sourceId}:${e.lineNumber}`); });
  session.defaultSession.webRequest.onErrorOccurred((d) => note(`request failed: ${d.method} ${d.url} ${d.error}`));
  win.webContents.on('render-process-gone', (_e, d) => note('render-process-gone ' + d.reason));
  const probe = `(() => ({ byo_door: !!window.__d2Door, isolated: self.crossOriginIsolated === true, origin: location.origin,
    d2: window.__d2 ? { ready: window.__d2.ready === true, error: window.__d2.error || null, smoke: window.__d2.smoke ?? null,
      retail: window.__d2.retail ?? null, byo: window.__d2.byo ?? null } : null }))()`;
  let s = null;
  const deadline = Date.now() + 240_000;
  while (Date.now() < deadline) {
    s = await win.webContents.executeJavaScript(probe).catch((e) => ({ error: String(e) }));
    if (s.d2 && (s.d2.ready || s.d2.error)) break;
    if (!retailDir && s.byo_door) break;   // byo with no folder: the front door is the state the page reaches
    await new Promise((r) => setTimeout(r, 1000));
  }
  const extra = await win.webContents.executeJavaScript(`(async () => {
    const site = await (await fetch('site-config.json')).json();
    const expect = await (await fetch('smoke.json')).json();
    const out = { siteConfig: site.assets, expectSmoke: expect.smoke };
    const dm = await fetch('door-art/meta.json');
    out.doorArt = { status: dm.status, derived: dm.headers.get('x-d2-derived') === '1', bytes: dm.ok ? (await dm.arrayBuffer()).byteLength : 0 };
    if (window.__d2 && window.__d2.retail) {
      const r = await fetch('retail/d2data.mpq', { headers: { Range: 'bytes=0-31' } });
      out.range = { status: r.status, length: (await r.arrayBuffer()).byteLength, contentRange: r.headers.get('content-range') };
    }
    return out;
  })()`).catch((e) => ({ error: String(e) }));
  const byoOk = !retailDir && !!(s && s.isolated && s.byo_door && !(s.d2 && s.d2.error) && diag.length === 0);
  const ok = byoOk || !!(s && s.isolated && s.d2 && s.d2.ready && !s.d2.error && s.d2.smoke === extra.expectSmoke
    && (!retailDir || (extra.range && extra.range.status === 206 && extra.doorArt && extra.doorArt.status === 200 && extra.doorArt.derived)));
  writeSync(1, 'D2_DESKTOP_SMOKE ' + JSON.stringify({ ok, ...s, ...extra, retailDir: !!retailDir, diag }) + '\n');
  app.exit(ok ? 0 : 1);
}

const gotLock = SMOKE || app.requestSingleInstanceLock();
if (!gotLock) app.quit();
else {
  app.on('second-instance', () => { if (win) { if (win.isMinimized()) win.restore(); win.focus(); } });
  app.on('window-all-closed', () => { if (process.platform !== 'darwin') app.quit(); });
  app.on('activate', () => { if (!win) createWindow(); });
  app.whenReady().then(async () => {
    // The br check is the staged site's own (play/br-check.mjs, copied beside the page): content, not mtime.
    try { ({ brMayAnswer: canUseBr } = await import(pathToFileURL(join(SITE, 'br-check.mjs')).href)); }
    catch { canUseBr = () => false; }
    registerSiteAndPermissions();
    buildMenu();
    const found = startupRetail();
    await deriveArt();
    createWindow();
    // Not found: the page's own front door asks (its buttons call d2Desktop.chooseGameFiles); no native modal.
    // The smoke run can still answer 'choose' to drive the chooser route, and waits for the reload before it looks.
    const loaded = new Promise((r) => win.webContents.once('did-finish-load', r));
    const asked = (found || TEST_ANSWER !== 'choose') ? loaded : loaded.then(() => chooseAndApply())
      .catch((e) => console.error('desktop: ' + (e && e.stack || e)));
    if (SMOKE) asked.then(() => smoke());
  }).catch((err) => { console.error('desktop: ' + (err && err.stack || err)); app.exit(1); });
}

