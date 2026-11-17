import assert from 'node:assert/strict';
import { createMissileSounds } from './missile-sounds.mjs';
// A fixture of the wasm client's rule exports (missile_sound_rules.cpp) for a few classes: holybolt 5 (travel 10, hit 11),
// a fireball-like 6 whose explosion class 7 sounds 12, the volcano 479 (travel 13), its overlay 480 (14) and debris pair 483 (15).
const travelRow = { 5: 10, 7: 12, 479: 13, 480: 14, 483: 15 };
const rules = {
  field: (id, f) => (f === 0 ? travelRow[id] ?? 0 : f === 1 ? (id === 5 ? 11 : 0) : -1),
  born: () => 0,
  birth: (id) => ({ mode: id === 479 ? 2 : id === 5 ? 4 : 0, row: travelRow[id] ?? 0, own: false }),
  removal: (id) => ({ row: id === 5 ? 11 : id === 6 ? 12 : 0, monster: false, expl: 0 }),
};
const got = [], s = createMissileSounds(rules, (r, e) => got.push([r, e.kind]));
const m = (id, classId) => ({ kind: 'missile', id, classId });
// travel sounds one tick after the spawn is seen; the hit on removal
s.update([m(1, 5)], 0); s.update([m(1, 5)], 1); s.update([], 2);
assert.deepEqual(got.splice(0), [[10, 'missile-travel'], [11, 'missile-hit']]);
s.update([m(2, 6)], 3); s.update([], 4);
assert.deepEqual(got.splice(0), [[12, 'missile-hit']]);
// volcano (479) and its client overlay (480) sound their travel on the spawn tick
s.update([m(3, 479)], 5); s.update([], 6);
assert.deepEqual(got.splice(0), [[14, 'missile-travel'], [13, 'missile-travel']]);
// a volcano debris (481, 4 frames remaining => 11 ticks of life) sounds its 483 pair when the client copy ends, one tick sooner for a late bucket
const d = { kind: 'missile', id: 9, classId: 481, missile: { remaining: 4 } };
for (let t = 10; t < 20; t++) { s.update([d], t); assert.deepEqual(got.splice(0), []); }
s.update([d], 20); assert.deepEqual(got.splice(0), [[15, 'missile-travel']]);
console.log('missile-sounds ok');
