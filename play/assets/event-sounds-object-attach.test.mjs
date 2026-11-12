// An object's mode sound is attached to the object (unit type 2), and the player finds that object's position in the
// snapshot's object list while its room has a client.
import test from 'node:test';
import assert from 'node:assert/strict';
import { createEventSounds } from './event-sounds.mjs';
import { unitSoundSourcePosition } from './unit-sound-spatial.mjs';

const files = { 'data\\global\\excel\\Sounds.txt': new TextEncoder().encode('Sound\tIndex\n') };

test('an object mode sound carries the object as its source unit', () => {
  const got = [], s = createEventSounds((f) => files[f], (row, event) => got.push([row, event]));
  for (const m of [0, 1]) s.update({ objects: [{ id: 7, classId: 181, mode: m }] }, 0);
  assert.equal(got.length, 1);
  assert.equal(got[0][0], 2547);
  assert.equal(got[0][1].unitType, 2);
  assert.equal(got[0][1].unitId, 7);
});

test('an object source is placed from the snapshot objects, and has no place once its room loses its client', () => {
  const o = { id: 7, x: 30, y: 40, inClientRoom: true };
  assert.equal(unitSoundSourcePosition({ unitType: 2, unitId: 7 }, { entities: [], objects: [o] }), o);
  assert.equal(unitSoundSourcePosition({ unitType: 2, unitId: 7 }, { entities: [], objects: [{ ...o, inClientRoom: false }] }), null);
  assert.equal(unitSoundSourcePosition({ unitType: 2, unitId: 8 }, { entities: [], objects: [o] }), null);
});
