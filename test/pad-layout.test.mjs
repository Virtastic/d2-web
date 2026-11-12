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

test('in the world the face buttons, shoulders and triggers are the skill sheet\'s; with none read they send nothing', () => {
  drain();
  tap(B.X); tap(B.Y); tap(B.RB); tap(B.RT); tap(B.LB);
  down(B.LT); tap(B.X); tap(B.RT); up(B.LT);
  assert.deepEqual(drain(), []);
});

test('over a panel the old banks still hold: Back held is the panel and utility keys, X Y RT are F-keys', () => {
  shell.uiTargets = () => ({ inventoryGrid: { 'i1': [500, 300] } });
  drain();
  down(B.BACK);
  for (const b of [B.UP, B.LEFT, B.RIGHT, B.DOWN]) tap(b);
  up(B.BACK);
  tap(B.X);
  const out = drain();
  shell.uiTargets = () => ({});
  assert.deepEqual(out.filter((e) => e.startsWith('key')), keyTaps('KeyI', 'KeyC', 'KeyT', 'KeyQ', 'F2'));
});

test('in the world: L3 is run, LT tapped swaps weapons, Back is the skill wheel and Start the panel list', () => {
  drain();
  tap(B.L3); tap(B.LT);
  assert.deepEqual(drain(), keyTaps('KeyR', 'KeyW'));
  tap(B.START); tap(B.UP); tap(B.A);
  assert.deepEqual(drain(), keyTaps('Escape'), 'the list wraps from Character up to Menu');
  tap(B.BACK);
  assert.deepEqual(drain(), [], 'Back is no automap tap');
});

test('keys follow the player\'s own Configure Controls bindings', () => {
  shell.controlCodeFor = (label) => ({ 'Use Belt 1': 'KeyZ', 'Inventory Screen': 'KeyB' })[label] ?? null;
  drain();
  tap(B.UP);
  tap(B.START); tap(B.DOWN); tap(B.A);
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
