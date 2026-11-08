import test from 'node:test';
import assert from 'node:assert/strict';
import { relationPacket, partyRequestPacket, canBeHostile, Roster, RELATION } from './bnet-party.mjs';

test('relation packet is 7 bytes with the id little endian', () => {
  assert.deepEqual([...relationPacket('hostile', true, 0x01020304)], [0x5d, 4, 1, 4, 3, 2, 1]);
  assert.deepEqual([...relationPacket('loot', false, 5)], [0x5d, 1, 0, 5, 0, 0, 0]);
  assert.throws(() => relationPacket('x', 1, 1));
});
test('party request packet is 6 bytes', () => {
  assert.deepEqual([...partyRequestPacket(9, 0xdeadbeef)], [0x5e, 9, 0xef, 0xbe, 0xad, 0xde]);
});
test('hostile needs level 9 both ways and a target in town', () => {
  assert.equal(canBeHostile({ level: 9 }, { level: 9, inTown: true }), true);
  assert.equal(canBeHostile({ level: 8 }, { level: 9, inTown: true }), false);
  assert.equal(canBeHostile({ level: 9 }, { level: 9, inTown: false }), false);
});
test('roster toggles flags and colours names', () => {
  const r = new Roster();
  r.set(1, { name: 'me', party: 3 }); r.set(2, { party: 3 }); r.set(3, { party: 4 }); r.set(4, {});
  assert.equal(r.colour(2, 1), 'ÿc2');
  assert.equal(r.colour(3, 1), 'ÿc8');
  assert.equal(r.colour(4, 1), 'ÿc0');
  assert.equal(r.toggle(4, RELATION.HOSTILE), true);
  assert.equal(r.colour(4, 1), 'ÿc1');
  assert.equal(r.toggle(4, RELATION.HOSTILE), false);
  r.remove(4);
  assert.equal(r.toggle(4, 1), null);
});
import { relationRequest } from './bnet-party.mjs';
test('a relation toggle flips the flag and sends the new state; hostility needs level 9 and town', () => {
  const r = new Roster(); const me = { id: 1, level: 20 };
  r.set(1, { name: 'me' }); r.set(2, { name: 'al', level: 20, inTown: true }); r.set(3, { name: 'lo', level: 3, inTown: true });
  assert.deepEqual([...relationRequest(r, me, 'loot', 2)], [0x5d, 1, 1, 2, 0, 0, 0]);
  assert.deepEqual([...relationRequest(r, me, 'loot', 2)], [0x5d, 1, 0, 2, 0, 0, 0]);
  assert.deepEqual([...relationRequest(r, me, 'hear', 2)], [0x5d, 2, 1, 2, 0, 0, 0]);
  assert.equal(relationRequest(r, me, 'hostile', 3), null);
  assert.equal(relationRequest(r, me, 'hostile', 2)[2], 1);
  assert.equal(relationRequest(r, me, 'loot', 1), null);
  assert.equal(relationRequest(r, me, 'loot', 99), null);
});
