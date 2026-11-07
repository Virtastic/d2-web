// End-to-end audit of a pad Chrome on Android reports with mapping '' (the Retroid Pocket's built-in controller):
// synthetic gamepad samples in, the exact DOM event records out. Each test names the part of the path it pins.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { pad, B, install, down, up, tap, poll, drain, keyTaps } from './pad-fixture.mjs';
import { pickPad } from '../play/assets/pad-pick.mjs';
import { isBackKey } from '../play/assets/back-guard.mjs';

pad.mapping = '';
pad.id = 'Retroid Pocket Controller (Vendor: 2020 Product: 3001)';
pad.axes = [0, 0, 0, 0, 0, 0];
let now = 1000;
performance.now = () => now;
const wait = (ms) => { now += ms; poll(); };
const project = (x, y) => [400 + x * 10, 300 + y * 10];
const snapshot = { player: { x: 0, y: 0, hpMax: 100 }, playerId: 1, level: 1, entities: [], items: [], objects: [], warps: [] };
let targets = {};
const shell = { screen: 'game', uiTargets: () => targets, snapshot, project, controlCodeFor: () => null };
await install(shell);
poll(); drain();
const click = (x, y) => [`pointermove:${x},${y}`, `pointerdown:${x},${y}`, `pointerup:${x},${y}`];
const settle = () => { pad.axes.fill(0); pad.buttons.forEach((b) => { b.pressed = false; }); now += 1000; poll(); drain(); };

test('pickPad: unmapped pads count only when they are game-controller sized; standard wins', () => {
  const mk = (o) => ({ connected: true, index: 0, mapping: '', axes: [0, 0, 0, 0], buttons: new Array(17).fill({}), timestamp: 1, ...o });
  assert.equal(pickPad([mk({})]).index, 0);
  assert.equal(pickPad([mk({ axes: [0, 0] })]), null, 'two axes: a stray HID device');
  assert.equal(pickPad([mk({ buttons: new Array(8).fill({}) })]), null, 'eight buttons: not a pad');
  assert.equal(pickPad([mk({ mapping: 'other' })]), null, 'a named non-standard mapping is not guessed');
  assert.equal(pickPad([mk({ index: 0 }), mk({ index: 1, mapping: 'standard' })]).index, 1);
  assert.equal(pickPad([mk({ index: 0, timestamp: 5 }), mk({ index: 1, timestamp: 9 })]).index, 1, 'newest input');
  assert.equal(pickPad([mk({ index: 0, timestamp: 5 }), mk({ index: 1, timestamp: 9 })], 0).index, 0, 'the pad in use stays');
});

test('left stick dead zone: inside 0.25 nothing, outside walks; diagonals and every cardinal aim', () => {
  pad.axes[0] = 0.2; pad.axes[1] = 0.2; poll(); assert.deepEqual(drain(), click(513.5, 413.5), 'magnitude 0.283 is outside 0.25: walks');
  settle();
  pad.axes[0] = 0.17; pad.axes[1] = 0.17; poll(); assert.deepEqual(drain(), [], 'magnitude 0.24 is inside');
  settle();
  const aims = [[1, 0, 560.5, 300.5], [-1, 0, 240.5, 300.5], [0, 1, 400.5, 460.5], [0, -1, 400.5, 140.5]];
  for (const [x, y, cx, cy] of aims) { pad.axes[0] = x; pad.axes[1] = y; poll(); assert.deepEqual(drain(), click(cx, cy)); settle(); }
  pad.axes[0] = 0.7071; pad.axes[1] = 0.7071; poll();
  assert.deepEqual(drain(), click(513.5, 413.5), 'a diagonal walks 160 px along the diagonal');
  settle();
});

test('right stick in the world moves retail\'s pointer and A clicks there; the left stick hands A back to the picker', () => {
  pad.axes[2] = 1; poll(); pad.axes[2] = 0; poll();
  assert.deepEqual(drain(), ['pointermove:414.5,300.5'], 'one poll at full deflection is one 14 px pointer step');
  tap(B.A);
  assert.deepEqual(drain(), click(414.5, 300.5), 'A clicks at the pointer');
  assert.equal(globalThis.__d2Controller.evidence.mode, 'pointer');
  pad.axes[0] = 1; poll(); pad.axes[0] = 0; drain();
  assert.equal(globalThis.__d2Controller.evidence.mode, 'world', 'walking hides the pointer road');
  tap(B.A);
  assert.deepEqual(drain(), [], 'back in the world, A with nothing in reach is consumed');
  settle();
});

test('skill buttons: X, Y, RB, RT are skills 2, 4, 5, 6; LT held banks them to 1, 3, 7, 8; LT alone does nothing', () => {
  for (const [b, n] of [[B.X, 2], [B.Y, 4], [B.RB, 5], [B.RT, 6]]) { tap(b); assert.deepEqual(drain(), keyTaps(`F${n}`)); }
  down(B.LT); drain();
  for (const [b, n] of [[B.X, 1], [B.Y, 3], [B.RB, 7], [B.RT, 8]]) { tap(b); assert.deepEqual(drain(), keyTaps(`F${n}`)); }
  up(B.LT); assert.deepEqual(drain(), []);
  settle();
});

test('belt/potion buttons: the d-pad is belt 1-4 clockwise from Up', () => {
  for (const [b, n] of [[B.UP, 1], [B.RIGHT, 2], [B.DOWN, 3], [B.LEFT, 4]]) { tap(b); assert.deepEqual(drain(), keyTaps(`Digit${n}`)); }
});

test('shoulders and sticks: RB is Shift on a click, LB holds Show Items, R3 swaps, L3 toggles run on a press', () => {
  down(B.LB); assert.deepEqual(drain(), ['keydown:AltLeft']);
  up(B.LB); assert.deepEqual(drain(), ['keyup:AltLeft']);
  tap(B.R3); assert.deepEqual(drain(), keyTaps('KeyW'));
  down(B.L3); up(B.L3); assert.deepEqual(drain(), keyTaps('KeyR'), 'one L3 press toggles run');
  settle();
});

test('Start is Escape; Back tapped is the automap; Back held banks the panel keys and the tap then does nothing', () => {
  tap(B.START); assert.deepEqual(drain(), keyTaps('Escape'));
  tap(B.BACK); assert.deepEqual(drain(), keyTaps('Tab'));
  down(B.BACK); tap(B.UP); tap(B.LEFT); tap(B.RIGHT); tap(B.DOWN);
  assert.deepEqual(drain(), [...keyTaps('KeyI'), ...keyTaps('KeyC'), ...keyTaps('KeyT'), ...keyTaps('KeyQ')]);
  up(B.BACK); assert.deepEqual(drain(), [], 'releasing a chord Back is not an automap tap');
  settle();
});

test('Back+Start opens the controller settings and its dead zone row changes the walk threshold', () => {
  down(B.BACK); down(B.START); up(B.START); up(B.BACK); drain();
  assert.equal(drain().length, 0);
  tap(B.RIGHT); tap(B.RIGHT); tap(B.RIGHT); tap(B.RIGHT); tap(B.RIGHT); // deadzone row, +0.05 x 5 = 0.5
  tap(B.B); drain();
  pad.axes[0] = 0.4; poll();
  assert.deepEqual(drain(), [], 'a 0.4 deflection is now inside the 0.5 dead zone');
  settle();
  pad.axes[0] = 0.6; poll();
  assert.deepEqual(drain(), click(560.5, 300.5));
  settle();
});

test('menu navigation: the d-pad snaps across the screen, A clicks the point, B is the EXIT/NO route, Start is Escape', () => {
  shell.screen = 'select';
  targets = { select: { characters: {}, rows: {}, play: [100, 500], create: [300, 500], exit: [500, 500] } };
  poll(); drain();
  const at = () => globalThis.__d2Controller.evidence.cursor.x;
  tap(B.RIGHT); assert.equal(at(), 300);
  tap(B.A); assert.deepEqual(drain().filter((e) => !e.startsWith('pointermove')), ['pointerdown:300.5,500.5', 'pointerup:300.5,500.5']);
  tap(B.B); assert.deepEqual(drain().filter((e) => !e.startsWith('pointermove')), ['pointerdown:500.5,500.5', 'pointerup:500.5,500.5'], 'B cancels to EXIT');
  tap(B.START); assert.deepEqual(drain(), keyTaps('Escape'));
  assert.equal(globalThis.__d2Controller.evidence.mode, 'panel');
  shell.screen = 'game'; targets = {};
  poll(); drain();
});

test('B as cancel in the world is a right click, never a key the browser maps to Back', () => {
  tap(B.B);
  assert.deepEqual(drain().filter((e) => e.startsWith('key')), [], 'B sends no key event of its own');
  for (const e of [{ key: 'GoBack' }, { key: 'BrowserBack' }, { key: 'Unidentified' }]) assert.equal(isBackKey(e), true);
  for (const e of [{ key: 'Escape', code: 'Escape' }, { key: 'b', code: 'KeyB' }, { key: 'Unidentified', code: 'KeyZ' }]) assert.equal(isBackKey(e), false);
});

test('a disconnected pad stops all events and reports disconnected', () => {
  const was = navigator.getGamepads;
  Object.defineProperty(globalThis, 'navigator', { value: { getGamepads: () => [null] }, configurable: true });
  poll(); drain();
  assert.equal(globalThis.__d2Controller.evidence.connected, false);
  Object.defineProperty(globalThis, 'navigator', { value: { getGamepads: was }, configurable: true });
});

test('a pad reporting mapping "" (Retroid): Back toggles the automap, L3 toggles run, Start is only Escape', async () => {
  const { pickPad } = await import('../play/assets/pad-pick.mjs');
  const raw = { index: 0, id: 'Retroid Pocket Controller', mapping: '', connected: true, axes: [0, 0, 0, 0], buttons: Array.from({ length: 17 }, () => ({ pressed: false })) };
  assert.equal(pickPad([raw]), raw);
  drain();
  tap(B.BACK); assert.deepEqual(drain(), keyTaps('Tab'));
  tap(B.L3); assert.deepEqual(drain(), keyTaps('KeyR'));
  tap(B.START); assert.deepEqual(drain(), keyTaps('Escape'));
});

test('R3 press swaps weapons once; holding it adds nothing (D2R opens an emote wheel there; the client has none, so it needs UI)', () => {
  drain();
  down(B.R3); for (let i = 0; i < 20; i++) wait(50); up(B.R3);
  assert.deepEqual(drain(), keyTaps('KeyW'));
});
