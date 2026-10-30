import test from 'node:test';
import assert from 'node:assert/strict';
import { npcMenuEntries, withHireRow, deadHireling, resurrectPacket, HIRE_ROWS } from '../play/assets/npc-menu.mjs';

const ids = (e) => e.map((x) => `${x.stringId}:${x.action}`);
const done = (set) => (act, quest) => set.has(`${act}:${quest}`);

test('Kashya lists Hire after A1Q2 and Qual-Kehk after A5Q1, between Talk and Cancel', () => {
  assert.deepEqual(ids(withHireRow(npcMenuEntries(150), 150, done(new Set()))), ['3381:talk', '3400:cancel']);
  assert.deepEqual(ids(withHireRow(npcMenuEntries(150), 150, done(new Set(['1:2'])))), ['3381:talk', '3397:hire', '3400:cancel']);
  assert.deepEqual(ids(withHireRow(npcMenuEntries(515), 515, done(new Set(['5:1'])))), ['3381:talk', '3397:hire', '3400:cancel']);
  assert.deepEqual(Object.keys(HIRE_ROWS), ['150', '515']);
});

test('NPCs whose table record already has Hire are left alone, as are non-hirers', () => {
  const all = done(new Set(['1:2', '5:1']));
  assert.deepEqual(ids(withHireRow(npcMenuEntries(198), 198, all)), ['3381:talk', '3397:hire', '3400:cancel']);
  assert.deepEqual(ids(withHireRow(npcMenuEntries(154), 154, all)), ids(npcMenuEntries(154)));
});

test('Hire on a dead mercenary raises it with packet 0x62 and the NPC id', () => {
  assert.equal(deadHireling([{ flags: 0 }]), null);
  assert.deepEqual(deadHireling([{ flags: 0 }, { flags: 1, guid: 7 }]), { flags: 1, guid: 7 });
  assert.deepEqual([...resurrectPacket(0x01020304)], [0x62, 4, 3, 2, 1]);
});

import { NPC_MENU_TABLE, npcMenuChoiceOnRelease } from '../play/assets/npc-menu.mjs';
test('every row of every NPC menu carries an action the shell dispatches, and a release over it chooses it', () => {
  const handled = new Set(['talk', 'trade', 'gamble', 'hire', 'identify', 'cancel']);
  for (const [cls] of NPC_MENU_TABLE) for (const e of npcMenuEntries(cls)) {
    assert.ok(handled.has(e.action) || (e.action === 'other' && e.stringId >= 3382 && e.stringId <= 3385), `${cls}: ${e.stringId} -> ${e.action}`);
    assert.equal(npcMenuChoiceOnRelease({ entryAtRelease: e, selectedEntry: { ...e } }), e);
  }
});
