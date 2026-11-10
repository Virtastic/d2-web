import test from 'node:test';
import assert from 'node:assert/strict';
import { autosaveDue } from './autosave-gate.mjs';

const base = { session: {}, named: true, inFlight: false, fromPackets: false, joined: false, ticks: 0, interval: 3, actChanged: false };

test('the engine game saves on the interval and at once on an act change', () => {
  assert.deepEqual(autosaveDue({ ...base, ticks: 1 }), { count: 2, due: false });
  assert.deepEqual(autosaveDue({ ...base, ticks: 2 }), { count: 3, due: true });
  assert.equal(autosaveDue({ ...base, actChanged: true }).due, true);
});
test('a packet game neither counts nor saves until its join has finished', () => {
  assert.deepEqual(autosaveDue({ ...base, fromPackets: true, actChanged: true, ticks: 2 }), { count: 2, due: false });
  assert.equal(autosaveDue({ ...base, fromPackets: true, joined: true, ticks: 2 }).due, true);
});
test('no save without a game, a name, or while one is being written', () => {
  for (const off of [{ session: null }, { named: false }, { inFlight: true }]) assert.equal(autosaveDue({ ...base, ...off, actChanged: true }).due, false);
});
