import { test } from 'node:test';
import assert from 'node:assert/strict';
import { B, pad, events, install, tap, down, up, poll, drain, keyTaps } from './pad-fixture.mjs';
import { adjustAxes, createSettings, createRunToggle, SETTINGS_KEY } from '../play/assets/pad-settings.mjs';

const store = new Map([[SETTINGS_KEY, JSON.stringify({ deadzone: 0.4, swapSticks: false, latchItems: false, autoGold: false })]]);
globalThis.localStorage = { getItem: (k) => store.get(k) ?? null, setItem: (k, v) => store.set(k, v) };
let t = 1000;
performance.now = () => t;
const wait = (ms) => { t += ms; poll(); };
const shell = { screen: 'game', uiTargets: () => ({}), snapshot: { player: { x: 0, y: 0, hpMax: 100 } }, project: () => [400, 300], controlCodeFor: () => null };
await install(shell);

test('run toggle: one L3 press toggles once, on the press; holding does not repeat', () => {
  drain();
  down(B.L3);
  assert.deepEqual(drain(), keyTaps('KeyR'));
  for (let i = 0; i < 10; i++) wait(50);
  up(B.L3);
  assert.deepEqual(drain(), [], 'holding on does not repeat');
  wait(100);
  down(B.L3); up(B.L3);
  assert.deepEqual(drain(), keyTaps('KeyR'), 'a second press toggles back');
});

test('createRunToggle alone, no DOM', () => {
  const r = createRunToggle();
  assert.deepEqual([r.fire(true), r.fire(true), r.fire(false), r.fire(true)], [true, false, false, true]);
});

test('R3 swaps weapons; LB held shows items; the panel bank is Back held, and LB inside it is Clear Screen', () => {
  drain();
  down(B.R3); up(B.R3);
  assert.deepEqual(drain(), keyTaps('KeyW'));
  down(B.LB); up(B.LB);
  assert.deepEqual(drain(), ['keydown:AltLeft', 'keyup:AltLeft']);
  down(B.BACK); down(B.LB); up(B.LB); up(B.BACK);
  assert.deepEqual(drain(), keyTaps('Space'), 'no Alt, no automap');
  down(B.BACK); up(B.BACK);
  assert.deepEqual(drain(), keyTaps('Tab'), 'a lone Back tap is the automap, sent on release');
});

test('adjustAxes: deadzone zeroes, sensitivity scales and clamps, swap exchanges the sticks', () => {
  const d = { deadzone: 0.25, sensitivity: 1, swapSticks: false, latchItems: false, autoGold: false };
  assert.deepEqual(adjustAxes([0.2, 0, 0, 0.3], d), [0, 0, 0, 0.3]);
  assert.deepEqual(adjustAxes([0.3, 0, 0, 0], { ...d, sensitivity: 2 }), [0.6, 0, 0, 0]);
  assert.deepEqual(adjustAxes([0.8, 0, 0, 0], { ...d, sensitivity: 2 }), [1, 0, 0, 0]);
  assert.deepEqual(adjustAxes([0.5, 0, 0, -0.5], { ...d, swapSticks: true, latchItems: false, autoGold: false }), [0, -0.5, 0.5, 0]);
});

test('settings persist: bad values are clamped, writes land in storage, a fresh store reads them back', () => {
  const a = createSettings(globalThis.localStorage);
  assert.deepEqual(a.get(), { deadzone: 0.4, sensitivity: 1, swapSticks: false, latchItems: false, autoGold: false }, 'the stored 0.4 was loaded');
  a.set({ deadzone: 9, sensitivity: 'x' });
  assert.deepEqual(a.get(), { deadzone: 0.5, sensitivity: 1, swapSticks: false, latchItems: false, autoGold: false });
  a.nudge('swapSticks', 1); a.nudge('sensitivity', 1);
  assert.deepEqual(JSON.parse(store.get(SETTINGS_KEY)), { deadzone: 0.5, sensitivity: 1.25, swapSticks: true, latchItems: false, autoGold: false });
  assert.deepEqual(createSettings(globalThis.localStorage).get(), a.get());
  createSettings(globalThis.localStorage).set({ deadzone: 0.4, sensitivity: 1, swapSticks: false, latchItems: false, autoGold: false });
});

test('the overlay: Back+Start opens it (no Escape, no automap), d-pad edits, B closes; the stored deadzone gates the stick', () => {
  const panel = globalThis.__d2PadSettings.panel;
  assert.equal(globalThis.__d2PadSettings.get().deadzone, 0.4);
  drain();
  down(B.BACK); down(B.START); up(B.START); up(B.BACK);
  assert.equal(panel.open, true);
  assert.deepEqual(drain(), []);
  tap(B.RIGHT); tap(B.DOWN); tap(B.RIGHT); tap(B.DOWN); tap(B.A);
  assert.deepEqual(globalThis.__d2PadSettings.get(), { deadzone: 0.45, sensitivity: 1.25, swapSticks: true, latchItems: false, autoGold: false });
  tap(B.X);
  assert.deepEqual(drain(), [], 'the game does not hear the pad while the overlay is up');
  tap(B.B);
  assert.equal(panel.open, false);
  assert.deepEqual(drain(), [], 'closing sends nothing');
  globalThis.__d2PadSettings.set({ deadzone: 0.4, sensitivity: 1, swapSticks: false, latchItems: false, autoGold: false });
});

test('a stick under the stored deadzone does not walk; over it, it does; swapped, the right stick walks', () => {
  t += 1000;
  pad.axes[0] = 0.35; poll();
  assert.deepEqual(drain().filter((e) => e.startsWith('pointer')), [], '0.35 < 0.4');
  pad.axes[0] = 0.5; t += 300; poll();
  assert.equal(drain().filter((e) => e.startsWith('pointerdown')).length, 1);
  pad.axes[0] = 0; poll(); drain();
  globalThis.__d2PadSettings.set({ swapSticks: true, latchItems: false, autoGold: false });
  pad.axes[2] = 0.9; t += 300; poll();
  assert.equal(drain().filter((e) => e.startsWith('pointerdown')).length, 1, 'right stick walks when swapped');
  pad.axes[2] = 0; poll();
  pad.axes[0] = 0.9; t += 300; poll();
  assert.equal(drain().filter((e) => e.startsWith('pointerdown')).length, 0, 'left stick no longer walks');
  pad.axes[0] = 0; poll();
});

test('latch setting: LB press toggles Alt on and off; a held LB does not; a chord with Back does not latch', () => {
  globalThis.__d2PadSettings.set({ latchItems: true, autoGold: false });
  drain();
  down(B.LB); up(B.LB);
  assert.deepEqual(drain(), ['keydown:AltLeft'], 'one press latches it on and release leaves it on');
  down(B.LB); up(B.LB);
  assert.deepEqual(drain(), ['keyup:AltLeft'], 'the next press latches it off');
  down(B.BACK); down(B.LB); up(B.LB); up(B.BACK);
  assert.deepEqual(drain(), keyTaps('Space'), 'Back+LB is Clear Screen, no latch');
  globalThis.__d2PadSettings.set({ latchItems: false, autoGold: false });
  down(B.LB);
  assert.deepEqual(drain(), ['keydown:AltLeft']);
  up(B.LB);
  assert.deepEqual(drain(), ['keyup:AltLeft'], 'off again: hold behaviour');
});
