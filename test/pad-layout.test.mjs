import { test } from 'node:test';
import assert from 'node:assert/strict';
import { B, install, tap, down, up, drain, keyTaps } from './pad-fixture.mjs';

const shell = { screen: 'game', uiTargets: () => ({}), snapshot: { player: { x: 0, y: 0, hpMax: 100 } }, project: () => [400, 300], controlCodeFor: () => null };
await install(shell);

test('d-pad is the four belt slots, clockwise from Up', () => {
  drain();
  for (const b of [B.UP, B.RIGHT, B.DOWN, B.LEFT]) tap(b);
  assert.deepEqual(drain(), keyTaps('Digit1', 'Digit2', 'Digit3', 'Digit4'));
});

test('D2R order in the world: X, Y, RB, RT are skills 2, 4, 5, 6; LT held banks them to 1, 3, 7, 8; LT alone does nothing', () => {
  drain();
  tap(B.X); tap(B.Y); tap(B.RB); tap(B.RT);
  assert.deepEqual(drain(), keyTaps('F2', 'F4', 'F5', 'F6'));
  down(B.LT); tap(B.X); tap(B.Y); tap(B.RB); tap(B.RT); up(B.LT);
  assert.deepEqual(drain(), keyTaps('F1', 'F3', 'F7', 'F8'));
});

test('Back held banks the d-pad and face buttons to the panel and utility keys', () => {
  drain();
  down(B.BACK);
  for (const b of [B.UP, B.LEFT, B.RIGHT, B.DOWN, B.X, B.Y, B.A, B.B, B.RT, B.L3, B.R3, B.LB]) tap(b);
  up(B.BACK);
  assert.deepEqual(drain(), keyTaps('KeyI', 'KeyC', 'KeyT', 'KeyQ', 'KeyO', 'KeyM', 'KeyW', 'KeyS', 'Enter', 'Backquote', 'KeyH', 'Space'));
});

test('Back is the automap, Start is Escape, R3 swaps weapons, LB held shows items', () => {
  drain();
  tap(B.BACK); tap(B.L3); tap(B.START);
  assert.deepEqual(drain(), keyTaps('Tab', 'KeyR', 'Escape'), 'Back is the map, L3 is run, Start is Escape');
  tap(B.R3);
  assert.deepEqual(drain(), keyTaps('KeyW'));
  down(B.LB);
  assert.deepEqual(drain(), ['keydown:AltLeft']);
  up(B.LB);
  assert.deepEqual(drain(), ['keyup:AltLeft']);
  down(B.BACK); tap(B.UP); up(B.BACK);
  assert.deepEqual(drain(), keyTaps('KeyI'), 'a Back chord is not an automap tap');
});

test('keys follow the player\'s own Configure Controls bindings', () => {
  shell.controlCodeFor = (label) => ({ 'Use Belt 1': 'KeyZ', 'Inventory Screen': 'KeyB' })[label] ?? null;
  drain();
  tap(B.UP);
  down(B.BACK); tap(B.UP); up(B.BACK);
  assert.deepEqual(drain(), keyTaps('KeyZ', 'KeyB'));
  shell.controlCodeFor = () => null;
});

test('RB held makes the click a shift-click', () => {
  shell.screen = 'create'; shell.uiTargets = () => ({ create: { classes: { amazon: [98, 320] }, exit: [97, 556] } });
  drain();
  down(B.RB); tap(B.A); up(B.RB);
  const clicks = drain();
  assert.ok(clicks.includes('pointerdown:98.5,320.5:shift'), clicks.join(' '));
});
