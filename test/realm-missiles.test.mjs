import test from 'node:test';
import assert from 'node:assert/strict';
import { parseRealmMissiles } from '../play/assets/realm-missiles.mjs';
test('realm missile readback preserves canonical identity, precise position, clock and seed', () => {
  const bytes = new Uint8Array(76), v = new DataView(bytes.buffer);
  const fields = [1,76,9,62,1,768,1280,256,17,5,100*65536+32768,200*65536,2,1024,47,49,123,123,666];
  fields.forEach((n,i) => v.setUint32(i*4,n,true));
  const [m] = parseRealmMissiles(bytes,1);
  assert.equal(m.classId,62); assert.equal(m.ownerGuid,1024); assert.equal(m.skill,47);
  assert.equal(m.x,100.5); assert.equal(m.y,200); assert.equal(m.framePrecise,768);
  assert.equal(m.remaining,49); assert.equal(m.initSeed,123); assert.equal(m.seedHigh,666);
  v.setUint32(12,265,true); assert.equal(parseRealmMissiles(bytes,1)[0].classId,265);
  v.setUint32(4,64,true); assert.throws(() => parseRealmMissiles(bytes,1),/invalid client missile/);
  assert.throws(() => parseRealmMissiles(bytes.subarray(0,75),1),/record span/);
  assert.deepEqual(parseRealmMissiles(new Uint8Array(),0),[]);
});
