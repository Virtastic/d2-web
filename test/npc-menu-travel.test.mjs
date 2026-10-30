import test from 'node:test';
import assert from 'node:assert/strict';
import { npcMenuEntries, withTravelRow, TRAVEL_ROWS } from '../play/assets/npc-menu.mjs';

const ids = (e) => e.map((x) => x.stringId);
const done = (set) => (act, quest) => set.has(`${act}:${quest}`);

test('Warriv (Act I) gets Go East only once Andariel is done, ahead of Cancel', () => {
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(155), 155, done(new Set()))), [3381, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(155), 155, done(new Set(['1:6'])))), [3381, 3382, 3400]);
  assert.equal(withTravelRow(npcMenuEntries(155), 155, done(new Set(['1:6'])))[1].action, 'other');
});

test('Meshif (Act II) gets Sail East once Duriel is done; the always-on rows stay as the table has them', () => {
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(210), 210, done(new Set(['2:6'])))), [3381, 3384, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(175), 175, done(new Set()))), [3381, 3383, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(264), 264, done(new Set()))), [3381, 3385, 3400]);
  assert.deepEqual(Object.keys(TRAVEL_ROWS), ['155', '210']);
});
