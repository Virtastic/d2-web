import { test } from 'node:test';
import assert from 'node:assert/strict';

// Exercise the actual timer callback and dispatched DOM events. The shell owns
// targets/field state; this fixture supplies those without starting the engine.
const events = [], polls = [];
const element = () => ({
  style: {}, children: [],
  appendChild(child) { this.children.push(child); },
  dispatchEvent(event) { events.push(event); return true; },
});
const viewport = element();
const canvas = {
  ...element(), width: 800, height: 600,
  offsetLeft: 0, offsetTop: 0, offsetWidth: 800, offsetHeight: 600,
  parentElement: viewport,
  getBoundingClientRect: () => ({ left: 0, top: 0, width: 800, height: 600 }),
};
globalThis.window = globalThis;
globalThis.document = {
  readyState: 'complete', body: viewport,
  getElementById: () => canvas, querySelector: () => canvas,
  createElement: element,
};
globalThis.getComputedStyle = () => ({ position: 'relative' });
globalThis.PointerEvent = globalThis.KeyboardEvent = globalThis.MouseEvent = class {
  constructor(type, init) { this.type = type; Object.assign(this, init); }
};
globalThis.dispatchEvent = (event) => { events.push(event); return true; };
globalThis.setInterval = (poll) => { polls.push(poll); };
const pad = {
  index: 0, mapping: 'standard', axes: [0, 0, 0, 0],
  buttons: Array.from({ length: 17 }, () => ({ pressed: false })),
};
Object.defineProperty(globalThis, 'navigator', {
  value: { getGamepads: () => [pad] }, configurable: true,
});
const targets = { create: { classes: { amazon: [98, 320], barbarian: [409, 286] }, exit: [97, 556] } };
globalThis.__d2Shell = {
  screen: 'create', charCreateEvidence: { chosen: null, name: '' },
  uiTargets: () => targets,
};
await import('../play/assets/input-controller.mjs');
const poll = () => polls[0]();
const tap = (button) => {
  pad.buttons[button].pressed = true; poll();
  pad.buttons[button].pressed = false; poll();
};

test('hiding the name grid restores class navigation; showing it reclaims the d-pad', () => {
  poll();
  const initial = __d2Controller.evidence.cursor;
  pad.axes[2] = 1;
  for (let i = 0; i < 20; i++) poll();
  assert.deepEqual(__d2Controller.evidence.cursor, initial, 'class selection intentionally uses snap navigation');
  pad.axes[2] = 0;
  tap(15);
  assert.deepEqual(__d2Controller.evidence.cursor, { x: 409, y: 286 });
  __d2Shell.charCreateEvidence.chosen = 'barbarian'; poll();
  assert.equal(__d2Controller.evidence.mode, 'field');
  assert.equal(__d2Controller.nameEntry.visible, true);

  tap(3);
  assert.equal(__d2Controller.nameEntry.visible, false);
  assert.equal(__d2Controller.evidence.mode, 'panel');
  events.length = 0;
  tap(14);
  assert.deepEqual(__d2Controller.evidence.cursor, { x: 98, y: 320 });
  assert.equal(events.some((event) => event.type === 'keydown'), false, 'navigation must not leak belt keys into the name field');
  tap(0);
  const pressed = events.find((event) => event.type === 'pointerdown');
  assert.deepEqual([pressed.clientX, pressed.clientY], [98.5, 320.5]);

  tap(3);
  assert.equal(__d2Controller.nameEntry.visible, true);
  assert.equal(__d2Controller.evidence.mode, 'field');
  const cursor = __d2Controller.evidence.cursor;
  events.length = 0;
  tap(15);
  assert.equal(__d2Controller.nameEntry.label, 'B');
  assert.deepEqual(__d2Controller.evidence.cursor, cursor);
  tap(0);
  assert.equal(events.some((event) => event.type === 'keydown' && event.key === 'B'), true);
  assert.equal(events.some((event) => event.type === 'pointerdown'), false);
});

test('hiding the gold grid frees the dialog cursor while its modal still blocks walking', () => {
  __d2Shell.screen = 'game';
  __d2Shell.stashEvidence = { gold: { drawn: { modal: { amount: 10, available: 100 } } } };
  __d2Shell.uiTargets = () => ({ stashGold: {} });
  __d2Shell.snapshot = { player: { x: 0, y: 0, hpMax: 100 } };
  __d2Shell.project = () => [400, 300];
  poll();
  assert.equal(__d2Controller.goldEntry.visible, true);
  const before = __d2Controller.evidence.cursor;
  events.length = 0;
  pad.axes = [1, 0, 1, 0];
  tap(3);
  assert.equal(__d2Controller.goldEntry.visible, false);
  assert.equal(__d2Controller.evidence.mode, 'cursor');
  assert.ok(__d2Controller.evidence.cursor.x > before.x);
  assert.equal(events.some((event) => event.type === 'pointerdown'), false, 'hidden grid does not release the gold dialog modal');
  assert.equal(events.some((event) => event.type === 'keydown'), false, 'Y toggles the field without firing F2');
  pad.axes = [0, 0, 0, 0];
});
