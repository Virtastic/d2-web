import assert from 'node:assert/strict';
import { createMissileSounds } from './missile-sounds.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Sounds.txt': enc('Sound\tIndex\nholybolt_1\t10\nholybolt_hit\t11\nexp_snd\t12\nvol_1\t13\ninf_1\t14\ndeb_1\t15\n'),
  'data\\global\\excel\\Skills.txt': enc('skill\tcharclass\tcltmissile\nFire Bolt\tsor\tfb\n'),
  'data\\global\\excel\\Missiles.txt': enc('Missile\tId\tTravelSound\tHitSound\tExplosionMissile\nholybolt\t5\tholybolt_1\tholybolt_hit\t\nfb\t6\t\t\texpl\nexpl\t7\texp_snd\t\t\nvolcano\t479\tvol_1\t\t\nvolover\t480\tinf_1\t\t\ndeb\t483\tdeb_1\t\t\n'),
};
const got = [], s = createMissileSounds((p) => files[p], (r, e) => got.push([r, e.kind]));
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
