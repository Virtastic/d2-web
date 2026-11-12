// A controller Chrome on Android reports with an empty mapping (a handheld's built-in pad) drives the
// game: the faked pad below is the Retroid Pocket's shape, and its samples produce the exact event
// records a standard pad's produce. Before, the controller read only mapping 'standard' pads and this
// sequence produced no events at all.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { pad, B, install, down, up, poll, drain, keyTaps } from './pad-fixture.mjs';

pad.mapping = '';
pad.id = 'Retroid Pocket Controller (Vendor: 2020 Product: 3001)';
let now = 1000;
performance.now = () => now;
const wait = (ms) => { now += ms; poll(); };
const project = (x, y) => [400 + x * 10, 300 + y * 10];
const snapshot = { player: { x: 0, y: 0, hpMax: 100 }, playerId: 1, level: 1, entities: [], items: [], objects: [], warps: [] };
await install({ screen: 'game', uiTargets: () => ({}), snapshot, project, controlCodeFor: () => null, selectedSkills: { left: 0, right: 101 }, selectSkill: () => {},
  padSkills: { name: 'Retroid', choices: [{ id: 0, leftSkill: true }, { id: 101 }], hotkeys: [], inTown: false } });
const clickAt = (x, y) => [`pointermove:${x},${y}`, `pointerdown:${x},${y}`, `pointerup:${x},${y}`];

test('the unmapped pad is connected and in world mode', () => {
  poll();
  const ev = globalThis.__d2Controller.evidence;
  assert.equal(ev.connected, true);
  assert.equal(ev.index, 0);
  assert.equal(ev.mode, 'world');
  assert.deepEqual(drain(), []);
});

test('left stick walks: a click ahead of the hero, repeated every 250 ms while held', () => {
  pad.axes[0] = 1; poll();
  assert.deepEqual(drain(), clickAt(560.5, 300.5));
  wait(200); assert.deepEqual(drain(), []);
  wait(50); assert.deepEqual(drain(), clickAt(560.5, 300.5));
  pad.axes[0] = 0; wait(300);
  assert.deepEqual(drain(), []);
});

test('RB (the right skill) with nothing in reach casts at the ground ahead; Start opens and closes the panel list; the d-pad is the belt', () => {
  down(B.RB);
  assert.deepEqual(drain(), ['pointermove:560.5,300.5', 'pointerdown:560.5,300.5']);
  up(B.RB);
  assert.deepEqual(drain(), ['pointerup:560.5,300.5']);
  down(B.START); up(B.START); down(B.START); up(B.START);
  assert.deepEqual(drain(), []);
  down(B.UP); up(B.UP);
  assert.deepEqual(drain(), keyTaps('Digit1'));
});
