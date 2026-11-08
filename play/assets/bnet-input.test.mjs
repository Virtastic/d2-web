import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createBnetInput } from './bnet-input.mjs';
import { canvasPoint } from './canvas-point.mjs';
import { createFrontendPointerStates } from './frontend-pointer-states.mjs';
import { createBnetLobbyScreen, BNET_LOBBY_LAYOUT as LL } from './bnet-lobby-screen.mjs';
import { createBnetLoginScreen } from './bnet-login-screen.mjs';

// The input layer driven the way the page drives it: a canvas with a CSS box, pointer / key / wheel events at window
// coordinates, a hidden <input> for the touch keyboard, and the real screens behind it with stub realm callbacks.
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const frame = { width: 10, height: 10, offsetX: 0, offsetY: 0, indices: new Uint8Array(100), opaque: new Uint8Array(100).fill(1) };
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const common = { read: (p) => (p.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: Array(6).fill(frame) }),
  decodeDatPalette: () => new Uint8Array(1024), now: () => 0 };
const tick = () => new Promise((r) => setImmediate(r));

function fakeDocument() {
  const listeners = {}; let active = null;
  const input = { style: {}, value: '', type: '', setAttribute() {}, addEventListener(t, f) { (listeners[t] ??= []).push(f); },
    focus() { active = input; }, blur() { if (active === input) { active = null; fire('blur'); } } };
  const fire = (type, init = {}) => { const e = { type, preventDefault() { this.prevented = true; }, ...init }; for (const f of listeners[type] ?? []) f(e); return e; };
  const canvases = () => ({ width: 0, height: 0, getContext: () => ctx });
  return { input, fire, doc: { createElement: (t) => (t === 'input' ? input : canvases()), body: { appendChild() {} }, get activeElement() { return active; } } };
}

// kind: 'lobby' | 'login'. (w, h) is the bitmap; the CSS box is (cssW, cssH) at (left, top) in the window.
function rig({ kind, w = 800, h = 600, cssW = w, cssH = h, left = 0, top = 0, hooks = {}, character = { name: 'Ama', charClass: 0, level: 3 } }) {
  const log = [], dom = fakeDocument();
  const screen = kind === 'lobby'
    ? createBnetLobbyScreen({ ...common, string: () => '', character, documentObject: dom.doc, onQuit: () => log.push('quit'), onEnterChat: () => log.push('enter-chat'),
        onSendChat: (t) => { log.push(`say ${t}`); return true; }, onRefreshGames: () => log.push('refresh'),
        onCreateGame: async (g) => { log.push(['create', g.gameName, g.gamePassword]); }, onJoinGame: async (g) => { log.push(['join', g.gameName, g.gamePassword]); }, ...hooks })
    : createBnetLoginScreen({ ...common, string: (i) => `s${i}`, documentObject: dom.doc, lastAccount: '', onAuthenticate: async (c) => { log.push(['logon', c.account, c.password]); }, ...hooks });
  const captured = [];
  const canvas = { width: w, height: h, getBoundingClientRect: () => ({ left, top, width: cssW, height: cssH }), focus() { log.push('focus'); },
    setPointerCapture(id) { captured.push(id); } };
  let renders = 0;
  const input = createBnetInput({ canvas, current: () => ({ kind, screen }), render: () => { renders++; }, documentObject: dom.doc });
  const states = createFrontendPointerStates();
  // The scene's letterbox, computed here from the 800x600 contract rather than asked of the screen.
  const sc = Math.min(w / 800, h / 600), ox = Math.round((w - 800 * sc) / 2), oy = Math.round((h - 600 * sc) / 2);
  const ev = (sx, sy, extra = {}) => ({ clientX: left + (ox + sx * sc) * cssW / w, clientY: top + (oy + sy * sc) * cssH / h, pointerId: 1, button: 0, pointerType: 'mouse',
    preventDefault() { this.prevented = true; }, ...extra });
  // The shell's pointerdown / pointerup routing for these screens.
  const down = (e) => { if (input.down(e)) return; states.press(e.pointerId, screen, ...canvasPoint(canvas, e), w, h, e.button); };
  const up = (e) => { if (input.dragEnd(e)) return; states.release(e.pointerId, screen, ...canvasPoint(canvas, e), w, h, e.button); input.tap(e); };
  const click = (sx, sy, extra) => { const e = ev(sx, sy, extra); down(e); up(e); };
  const key = (code, k = '', extra = {}) => input.key({ code, key: k, preventDefault() {}, ...extra });
  const typeText = (t) => { for (const c of t) key(c === ' ' ? 'Space' : `Key${c.toUpperCase()}`, c); };
  const centre = (id) => { const b = screen.evidence.boxes.find((x) => x.id === id); return [b.x + b.w / 2, b.y + b.h / 2]; };
  const clickId = (id, extra) => click(...centre(id), extra);
  return { screen, input, log, dom, canvas, ev, down, up, click, key, typeText, centre, clickId, captured, view: { sc, ox, oy }, get renders() { return renders; } };
}
// 800x600 bitmap, and 16:9 with the CSS box a different size and place (a retina window with a safe-area inset).
const VIEWPORTS = [{ name: '800x600', w: 800, h: 600 }, { name: '16:9', w: 1920, h: 1080, cssW: 960, cssH: 540, left: 12, top: 30 }];

test('window point -> canvas pixels: the CSS box, its offset and the bitmap ratio', () => {
  const canvas = { width: 1920, height: 1080, getBoundingClientRect: () => ({ left: 12, top: 30, width: 960, height: 540 }) };
  assert.deepEqual(canvasPoint(canvas, { clientX: 12, clientY: 30 }), [0, 0]);
  assert.deepEqual(canvasPoint(canvas, { clientX: 492, clientY: 300 }), [960, 540]);
  assert.deepEqual(canvasPoint(canvas, { clientX: 972, clientY: 570 }), [1920, 1080]);
});

for (const vp of VIEWPORTS) {
  test(`${vp.name}: a click lands on the control under it in the 800x600 scene, and the letterbox is dead`, () => {
    const r = rig({ kind: 'lobby', ...vp });
    const [qx, qy] = r.centre('create');
    r.click(qx, qy); assert.equal(r.screen.panel, 'create');
    r.click(qx, qy); assert.equal(r.screen.panel, 'create', 'a second CREATE click is swallowed, as in retail');
    r.screen.activate('panel-cancel'); r.click(qx + 600, qy); assert.equal(r.screen.panel, null);
    // Left of the scene by 20 scene pixels: the same height as CREATE, but the window beside the 800x600 scene (the widescreen margin).
    if (r.view.ox > 0) { const e = r.ev(qx - 20 - r.view.ox / r.view.sc, qy); r.down(e); r.up(e); assert.equal(r.screen.panel, null); }
    // The button's edges: its first and last scene pixel hit, the one outside does not.
    const b = r.screen.evidence.boxes.find((x) => x.id === 'create');
    r.click(b.x + 0.5, b.y + 0.5); assert.equal(r.screen.panel, 'create'); r.screen.activate('panel-cancel');
    r.click(b.x + b.w - 0.5, b.y + b.h - 0.5); assert.equal(r.screen.panel, 'create'); r.screen.activate('panel-cancel');
    r.click(b.x - 0.5, b.y + 5); assert.equal(r.screen.panel, null);
  });

  test(`${vp.name}: a latching button opens on the press, and sliding off before the release changes nothing more`, () => {
    const r = rig({ kind: 'lobby', ...vp });
    const [qx, qy] = r.centre('create'), d = r.ev(qx, qy), u = r.ev(qx + 300, qy);
    r.down(d); assert.equal(r.screen.panel, 'create'); r.up(u); assert.equal(r.screen.panel, 'create');
    const h = rig({ kind: 'lobby', ...vp }), [hx, hy] = h.centre('help'), hd = h.ev(hx, hy), hu = h.ev(hx + 300, hy);
    h.down(hd); h.up(hu); assert.equal(h.screen.panel, null, 'HELP, a plain button, still acts on the release');
  });

  test(`${vp.name}: the game list, a row click, the wheel is ignored, a scrollbar drag, JOIN GAME`, async () => {
    const r = rig({ kind: "lobby", ...vp }), J = LL.joinGame.list;
    r.screen.setGames(Array.from({ length: 20 }, (_, i) => ({ name: `game${i}`, description: `d${i}` })));
    r.clickId("join"); assert.equal(r.screen.panel, "join"); assert.equal(r.log.filter((l) => l === "refresh").length, 1);
    const row = (i) => [J.x + 10, J.y - J.h + i * J.pitch + 3];
    r.click(...row(2)); assert.equal(r.screen.selectedGame().name, "game2");
    // Retail ignores the wheel in the join list.
    r.screen.draw(ctx, vp.w, vp.h);
    const e = { ...r.ev(...row(0)), deltaY: 100, deltaMode: 0 };
    assert.equal(r.input.wheel(e), false);
    r.click(...row(0)); assert.equal(r.screen.selectedGame().name, "game0");
    // The scrollbar is the screen's own press, drag and release.
    const S = J.scrollbar, x = S.x + 2;
    r.down(r.ev(x, S.top + 10)); r.screen.hover(...canvasPoint({ width: vp.w, height: vp.h, getBoundingClientRect: () => ({ left: vp.left ?? 0, top: vp.top ?? 0, width: vp.cssW ?? vp.w, height: vp.cssH ?? vp.h }) }, r.ev(x, S.bottom - 10)), vp.w, vp.h);
    r.up(r.ev(x, S.bottom - 10));
    r.click(...row(0)); assert.equal(r.screen.selectedGame().name, `game${20 - J.rows}`);
    r.click(...row(4)); assert.equal(r.screen.selectedGame().name, `game${24 - J.rows}`);
    r.clickId("field-password"); r.typeText("pw"); r.clickId("join-game"); await tick();
    assert.deepEqual(r.log.filter((l) => Array.isArray(l)), [["join", `game${24 - J.rows}`, "pw"]]);
    assert.ok(r.screen.busy);
  });

  test(`${vp.name}: chat log: Enter opens chat, typing, Backspace repeat, Enter sends, the log scrolls by wheel and drag`, () => {
    const r = rig({ kind: 'lobby', ...vp }), C = LL.chatLog;
    r.screen.chatLive(true);
    r.key('Enter', 'Enter'); assert.equal(r.screen.mode, 'chat'); assert.deepEqual(r.log.filter((l) => l !== 'focus'), ['enter-chat']);
    r.typeText('hello'); assert.equal(r.screen.editValue(), 'hello');
    for (let i = 0; i < 3; i++) r.key('Backspace', 'Backspace', { repeat: i > 0 }); assert.equal(r.screen.editValue(), 'he');
    r.key('KeyA', 'a', { ctrlKey: true }); assert.equal(r.screen.editValue(), 'he', 'a chord is not text');
    r.key('Enter', 'Enter'); assert.ok(r.log.includes('say he')); assert.equal(r.screen.editValue(), '');
    r.key('Enter', 'Enter'); assert.equal(r.log.filter((l) => String(l).startsWith('say')).length, 1, 'an empty line sends nothing');
    for (let i = 0; i < 40; i++) r.screen.chatLine({ text: `line ${i}`, colour: 0 });
    r.screen.draw(ctx, vp.w, vp.h);
    const x = C.scrollbar.x + 2, up = r.ev(x, C.scrollbar.top + 10);
    r.down(up); assert.ok(r.input.dragging, 'the bar is there once the log overflows'); r.up(up);
    assert.deepEqual(r.screen.evidence.chat.length, 41);
    const w1 = { ...r.ev(C.x + 10, C.y - 20), deltaY: -100, deltaMode: 0 }; assert.equal(r.input.wheel(w1), true);
    // Escape: one QUIT per press, a held key's repeats ignored.
    r.key('Escape', 'Escape'); r.key('Escape', 'Escape', { repeat: true }); r.key('Escape', 'Escape', { repeat: true });
    assert.equal(r.log.filter((l) => l === 'quit').length, 1);
  });

  test(`${vp.name}: CREATE panel: boxes by click and Tab, Enter creates; a rejected Enter in the lobby with no panel does not`, async () => {
    const r = rig({ kind: 'lobby', ...vp });
    r.clickId('create'); r.typeText('my game'); r.key('Tab', 'Tab'); r.typeText('pw');
    r.clickId('field-name'); r.typeText('x'); assert.equal(r.screen.evidence.form.name, 'my gamex');
    r.key('Enter', 'Enter'); await tick();
    assert.deepEqual(r.log.filter((l) => Array.isArray(l)), [['create', 'my gamex', 'pw']]);
    r.key('Enter', 'Enter'); r.key('Enter', 'Enter'); await tick();
    assert.equal(r.log.filter((l) => Array.isArray(l)).length, 1, 'busy: Enter again creates nothing');
  });

  test(`${vp.name}: login: click a box, type, Tab, Backspace, Enter logs on once with the typed credentials`, async () => {
    const r = rig({ kind: 'login', ...vp });
    r.clickId('field-account'); r.typeText('Ama'); r.key('Tab', 'Tab'); r.typeText('secrex');
    r.key('Backspace', 'Backspace'); r.key('Backspace', 'Backspace', { repeat: true }); r.typeText('et');
    assert.deepEqual([r.screen.evidence.account, r.screen.evidence.passwordLength, r.screen.focus], ['Ama', 6, 'password']);
    r.key('Enter', 'Enter'); r.key('Enter', 'Enter'); await tick();
    assert.deepEqual(r.log.filter((l) => Array.isArray(l)), [['logon', 'Ama', 'secret']]);
  });

  test(`${vp.name}: login by touch: a tap on a box raises the keyboard, typing and Go reach it, the password box is a password input`, async () => {
    const r = rig({ kind: 'login', ...vp }), t = (id) => r.clickId(id, { pointerType: 'touch' });
    t('field-account');
    assert.equal(r.dom.doc.activeElement, r.dom.input); assert.equal(r.dom.input.type, 'text');
    for (const v of ['A', 'Am', 'Ama']) { r.dom.input.value = v; r.dom.fire('input'); }
    assert.equal(r.screen.evidence.account, 'Ama');
    t('field-password'); assert.equal(r.dom.input.type, 'password'); assert.equal(r.dom.input.value, '');
    for (const v of ['p', 'pw']) { r.dom.input.value = v; r.dom.fire('input'); }
    // Go on the keyboard: the input's own keydown, which the window also sees; one logon.
    const enter = { code: 'Enter', key: 'Enter', target: r.dom.input, preventDefault() {} };
    r.dom.fire('keydown', enter); r.input.key(enter); await tick();
    assert.deepEqual(r.log.filter((l) => Array.isArray(l)), [['logon', 'Ama', 'pw']]);
    // A tap on a button (not a box) puts the keyboard away.
    r.screen.reset(); t('field-account'); assert.equal(r.dom.doc.activeElement, r.dom.input);
    t('exit'); assert.equal(r.dom.doc.activeElement, null);
  });

  test(`${vp.name}: lobby by touch: any tap raises the keyboard on the focused box, the chat line takes typing and Go sends`, () => {
    const r = rig({ kind: 'lobby', ...vp }), t = (...a) => r.click(...a, { pointerType: 'touch' });
    r.screen.chatLive(true);
    t(...r.centre('enter-chat')); assert.equal(r.screen.mode, 'chat');
    assert.equal(r.dom.doc.activeElement, r.dom.input, 'the chat line is the box that takes text');
    for (const v of ['h', 'hi']) { r.dom.input.value = v; r.dom.fire('input'); }
    assert.equal(r.screen.editValue(), 'hi');
    r.dom.fire('keydown', { code: 'Enter', key: 'Enter', target: r.dom.input, preventDefault() {} });
    assert.ok(r.log.includes('say hi'));
    t(...r.centre('create')); assert.equal(r.screen.panel, 'create'); assert.equal(r.dom.doc.activeElement, r.dom.input, 'the name box has focus');
    for (const v of ['g']) { r.dom.input.value = v; r.dom.fire('input'); }
    assert.equal(r.screen.evidence.form.name, 'g');
  });
}

test('the screens draw where the click maps: the present rect is the letterbox the hit test inverts', () => {
  for (const [w, h] of [[800, 600], [1920, 1080], [1280, 1024]]) {
    const calls = [], out = new Proxy({}, { get: (_, k) => (k === 'drawImage' ? (...a) => calls.push(a) : () => {}), set: () => true });
    const r = rig({ kind: 'lobby', w, h });
    r.screen.draw(out, w, h);
    const [, ox, oy, dw, dh] = calls.find((c) => c.length === 5);
    const s = Math.min(w / 800, h / 600);
    assert.deepEqual([ox, oy, dw, dh], [Math.round((w - 800 * s) / 2), Math.round((h - 600 * s) / 2), 800 * s, 600 * s]);
    assert.deepEqual([ox, oy], [r.view.ox, r.view.oy]);
    // The target of CREATE (the screen's own centre) is inside the painted rect at the same scene point.
    const [tx, ty] = r.screen.targets(w, h).create, [cx, cy] = r.centre('create');
    assert.ok(Math.abs(tx - (ox + cx * s)) <= 1 && Math.abs(ty - (oy + cy * s)) <= 1);
  }
});

test('the shell hands each event to this layer, and keeps no copy of the lobby or login input', () => {
  const t = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  for (const call of ['bnetInput.key(e)', 'bnetInput.wheel(e)', 'bnetInput.down(e)', 'bnetInput.move(e)', 'bnetInput.dragEnd(e)', 'bnetInput.tap(e)', 'bnetInput.sync()']) assert.ok(t.includes(call), call);
  assert.ok(!/bnetLobbyScreen\??\.typeKey/.test(t) && !/lobbyTextEntry/.test(t));
});

// The shell's flow between the screens, with a stub realm: each callback does what the shell's does (switch the screen on show).
function flow({ hooks = {} } = {}) {
  const dom = fakeDocument(), state = { cur: null }, log = [];
  const canvas = { width: 800, height: 600, getBoundingClientRect: () => ({ left: 0, top: 0, width: 800, height: 600 }), focus() {}, setPointerCapture() {} };
  const input = createBnetInput({ canvas, current: () => state.cur, render: () => {}, documentObject: dom.doc });
  const states = createFrontendPointerStates();
  let lobby = null;
  const login = createBnetLoginScreen({ ...common, string: (i) => `s${i}`, documentObject: dom.doc, lastAccount: '',
    onAuthenticate: async (c) => { log.push(['logon', c.account]); if (hooks.logonFails) throw new Error('refused'); state.cur = null; /* the character select */ },
    onExit: () => { log.push('exit'); state.cur = null; } });
  const enterLobby = () => {
    lobby = createBnetLobbyScreen({ ...common, string: () => '', character: { name: 'Ama', charClass: 0, level: 3 }, documentObject: dom.doc,
      onQuit: () => { log.push('quit'); state.cur = null; lobby = null; },
      onCreateGame: async (g) => { log.push(['create', g.gameName]); await hooks.create?.(g); },
      onJoinGame: async (g) => { log.push(['join', g.gameName]); await hooks.join?.(g, { login, state, lobbyGone: () => { lobby = null; } }); } });
    state.cur = { kind: 'lobby', screen: lobby };
  };
  state.cur = { kind: 'login', screen: login };
  return { dom, state, log, input, states, login, canvas, enterLobby, get lobby() { return lobby; } };
}
const press = (f, screen, id, extra = {}) => {
  const [x, y] = (() => { const b = screen.evidence.boxes.find((q) => q.id === id); return [b.x + b.w / 2, b.y + b.h / 2]; })();
  const e = { clientX: x, clientY: y, pointerId: 1, button: 0, pointerType: 'mouse', preventDefault() {}, ...extra };
  f.states.press(1, f.state.cur?.screen, x, y, 800, 600, 0); f.states.release(1, f.state.cur?.screen, x, y, 800, 600, 0); f.input.tap(e);
};

test('lobby QUIT (key, button) leaves nothing of the lobby behind: no keyboard, no drag, no armed press, no input taken', async () => {
  const f = flow(); f.enterLobby(); f.lobby.chatLive(true);
  press(f, f.lobby, 'enter-chat', { pointerType: 'touch' });
  assert.equal(f.dom.doc.activeElement, f.dom.input);
  // A button armed on the lobby, then the screen goes (the escape key arrives mid-gesture).
  const q = f.lobby.evidence.boxes.find((b) => b.id === 'quit'), qx = q.x + 2, qy = q.y + 2;
  f.states.press(7, f.lobby, qx, qy, 800, 600, 0);
  f.input.key({ code: 'Escape', key: 'Escape', preventDefault() {} });
  assert.deepEqual(f.log, ['quit']); assert.equal(f.state.cur, null);
  f.input.sync(); assert.equal(f.dom.doc.activeElement, null, 'the keyboard went down with the screen');
  assert.equal(f.states.release(7, f.state.cur?.screen ?? null, qx, qy, 800, 600, 0), null); assert.deepEqual(f.states.armed, []);
  assert.equal(f.input.key({ code: 'KeyA', key: 'a', preventDefault() {} }), false, 'the next screen gets the keyboard');
  assert.equal(f.input.wheel({ deltaY: 100, clientX: 5, clientY: 5, preventDefault() {} }), false);
  assert.equal(f.input.dragEnd({ pointerId: 1 }), false);
});

test('a rejected create keeps the panel and its form; the notice swallows keys until OK; the next create goes out', async () => {
  let n = 0;
  const f = flow({ hooks: { create: async () => { if (n++ === 0) throw Object.assign(new Error('exists'), { mcpStage: 'create', mcpResult: 0x1f }); } } });
  f.enterLobby();
  const key = (code, k = '') => f.input.key({ code, key: k, preventDefault() {} });
  press(f, f.lobby, 'create'); for (const c of 'abc') key(`Key${c.toUpperCase()}`, c);
  key('Enter', 'Enter'); await tick();
  assert.ok(f.lobby.evidence.notice); assert.equal(f.lobby.busy, false); assert.equal(f.lobby.panel, 'create');
  for (const c of 'zz') key(`Key${c.toUpperCase()}`, c); assert.equal(f.lobby.evidence.form.name, 'abc', 'typing under the notice goes nowhere');
  key('Enter', 'Enter'); assert.equal(f.lobby.evidence.notice, null); assert.equal(f.log.length, 1, 'OK did not create again');
  key('Enter', 'Enter'); await tick();
  assert.deepEqual(f.log, [['create', 'abc'], ['create', 'abc']]); assert.ok(f.lobby.busy, 'stays busy until the game replaces the lobby');
  key('Escape', 'Escape'); assert.equal(f.log.length, 2, 'Escape while busy does not quit');
});

test('a join that loses the session drops to the login screen with its box; OK, then a fresh login works and the old lobby is inert', async () => {
  const f = flow({ hooks: { join: async (g, { login, state, lobbyGone }) => {
    state.cur = { kind: 'login', screen: login }; lobbyGone(); login.reset(); login.fail(new Error('lost')); throw Object.assign(new Error('lost'), { sessionLost: true }); } } });
  f.enterLobby(); const old = f.lobby;
  old.setGames([{ name: 'g1' }]); old.activate('join');
  const key = (code, k = '') => f.input.key({ code, key: k, preventDefault() {} });
  const row = LL.joinGame.list; f.states.press(1, old, row.x + 5, row.y - row.h + 3, 800, 600, 0); f.states.release(1, old, row.x + 5, row.y - row.h + 3, 800, 600, 0);
  assert.equal(old.selectedGame().name, 'g1');
  key('Enter', 'Enter'); await tick();
  assert.deepEqual(f.log, [['join', 'g1']]); assert.equal(old.evidence.notice, null, 'a lost session shows no lobby notice'); assert.equal(old.busy, false);
  assert.equal(f.state.cur.kind, 'login'); assert.ok(f.login.evidence.message, 'the login failure box is up');
  key('KeyX', 'x'); assert.equal(f.login.evidence.account, '', 'typing under the box goes nowhere');
  key('Enter', 'Enter'); assert.equal(f.login.evidence.message, null);
  for (const c of 'ama') key(`Key${c.toUpperCase()}`, c); key('Tab', 'Tab'); key('KeyP', 'p'); key('Enter', 'Enter'); await tick();
  assert.deepEqual(f.log.at(-1), ['logon', 'ama']); assert.equal(f.state.cur, null);
  assert.equal(f.login.evidence.passwordLength, 1); assert.equal(f.login.busy, false);
});

test('login: a refused logon shows its box and leaves the screen usable; EXIT leaves, with the keyboard down', async () => {
  const f = flow({ hooks: { logonFails: true } }), key = (code, k = '') => f.input.key({ code, key: k, preventDefault() {} });
  press(f, f.login, 'field-account', { pointerType: 'touch' }); assert.equal(f.dom.doc.activeElement, f.dom.input);
  for (const v of ['a', 'ab']) { f.dom.input.value = v; f.dom.fire('input'); }
  key('Tab', 'Tab'); for (const v of ['p']) { f.dom.input.value = v; f.dom.fire('input'); }
  f.dom.fire('keydown', { code: 'Enter', key: 'Enter', target: f.dom.input, preventDefault() {} }); await tick();
  assert.ok(f.login.evidence.message); f.input.sync(); assert.equal(f.dom.doc.activeElement, null, 'no box takes text under the failure box');
  key('Enter', 'Enter'); assert.equal(f.login.evidence.message, null); assert.equal(f.login.evidence.account, 'ab', 'the account is back in its box');
  key('Escape', 'Escape'); assert.deepEqual(f.log.at(-1), 'exit'); assert.equal(f.states.armed.length, 0);
});
