import test from 'node:test';
import assert from 'node:assert/strict';
import { npcMenuEntries, withTravelRow, TRAVEL_ROWS, TRAVEL_LEVEL } from '../play/assets/npc-menu.mjs';

const ids = (e) => e.map((x) => x.stringId);
const bits = (set) => (word) => set.has(word);

test('Warriv (Act I) gets Go East on bit 0 of quest word 6 (A1Q6), ahead of Cancel', () => {
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(155), 155, bits(new Set()))), [3381, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(155), 155, bits(new Set([6])))), [3381, 3382, 3400]);
  assert.equal(withTravelRow(npcMenuEntries(155), 155, bits(new Set([6])))[1].action, 'other');
});

test('Meshif (Act II) gets Sail East on word 14; the always-on rows stay as the table has them', () => {
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(210), 210, bits(new Set([14])))), [3381, 3384, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(210), 210, bits(new Set([6])))), [3381, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(175), 175, bits(new Set()))), [3381, 3383, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(264), 264, bits(new Set()))), [3381, 3385, 3400]);
});

test('Tyrael (Act IV) gets his Harrogath row on word 28 in an expansion game only', () => {
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(367), 367, bits(new Set([28])))), [3381, 22738, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(367), 367, bits(new Set([28])), false)), [3381, 3400]);
  assert.deepEqual(ids(withTravelRow(npcMenuEntries(367), 367, bits(new Set([26])))), [3381, 3400]);
  assert.deepEqual(Object.keys(TRAVEL_ROWS), ['155', '210', '367']);
  assert.deepEqual(TRAVEL_LEVEL, { 3382: 40, 3383: 1, 3384: 75, 3385: 40, 22738: 109 });
});
