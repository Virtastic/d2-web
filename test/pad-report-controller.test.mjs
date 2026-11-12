// A handheld pad that reports its d-pad as a hat and its triggers as analog axes (mapping ''), driven through
// the real controller poll loop: the exact key records out for each synthetic sample sequence.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { pad, install, poll, drain, keyTaps } from './pad-fixture.mjs';

pad.mapping = '';
pad.id = 'Retroid Pocket Controller (Vendor: 2020 Product: 3001)';
pad.axes = [0, 0, 0, 0, 0, 0, 0, 0];
const snapshot = { player: { x: 0, y: 0, hpMax: 100 }, playerId: 1, level: 1, entities: [], items: [], objects: [], warps: [] };
const shell = { screen: 'game', uiTargets: () => ({}), snapshot, project: (x, y) => [400 + x * 10, 300 + y * 10], controlCodeFor: () => null };
await install(shell);
poll(); drain();
const set = (axes) => { pad.axes = axes; poll(); return drain(); };
const hat = (x, y) => [0, 0, 0, 0, 0, 0, x, y];

test('the d-pad hat moves the belt: a direction is one belt press, and a hat that never moved is none', () => {
  assert.deepEqual(set(hat(0, 0)), []);
  assert.deepEqual(set(hat(1, 0)), [], 'the first axis to move is not yet a pair');
  assert.deepEqual(set(hat(0, -1)), keyTaps('Digit1'), 'up is belt 1 once the second axis has moved');
  assert.deepEqual(set(hat(0, 0)), []);
  assert.deepEqual(set(hat(1, 0)), keyTaps('Digit2'), 'right is belt 2');
  assert.deepEqual(set(hat(0, 0)), []);
  assert.deepEqual(set(hat(-1, 0)), keyTaps('Digit4'), 'left is belt 4');
  assert.deepEqual(set(hat(0, 0)), []);
  assert.deepEqual(set(hat(0, 1)), keyTaps('Digit3'), 'down is belt 3');
  assert.deepEqual(set(hat(0, 0)), []);
});

test('an analog trigger on axis 4: a light pull does nothing, a full-pull tap swaps weapons, a full pull with X is no swap', () => {
  assert.deepEqual(set([0, 0, 0, 0, 0.3, 0, 0, 0]), [], 'under half travel');
  assert.deepEqual(set([0, 0, 0, 0, 0, 0, 0, 0]), [], 'a light pull is no tap');
  assert.deepEqual(set([0, 0, 0, 0, 1, 0, 0, 0]), [], 'a full pull holds LT; LT alone sends nothing yet');
  assert.deepEqual(set([0, 0, 0, 0, 0, 0, 0, 0]), keyTaps('KeyW'), 'let go at once: Swap Weapons');
  set([0, 0, 0, 0, 1, 0, 0, 0]);
  pad.buttons[2].pressed = true; poll();
  pad.buttons[2].pressed = false; poll();
  assert.deepEqual(set([0, 0, 0, 0, 0, 0, 0, 0]), [], 'X during the pull: the second skill set, never a swap');
  drain();
});
