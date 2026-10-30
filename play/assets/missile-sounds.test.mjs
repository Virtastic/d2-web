import assert from 'node:assert/strict';
import { createMissileSounds } from './missile-sounds.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Sounds.txt': enc('Sound\tIndex\nholybolt_1\t10\nholybolt_hit\t11\nexp_snd\t12\n'),
  'data\\global\\excel\\Skills.txt': enc('skill\tcharclass\tcltmissile\nFire Bolt\tsor\tfb\n'),
  'data\\global\\excel\\Missiles.txt': enc('Missile\tId\tTravelSound\tHitSound\tExplosionMissile\nholybolt\t5\tholybolt_1\tholybolt_hit\t\nfb\t6\t\t\texpl\nexpl\t7\texp_snd\t\t\n'),
};
const got = [], s = createMissileSounds((p) => files[p], (r, e) => got.push([r, e.kind]));
const m = (id, classId) => ({ kind: 'missile', id, classId });
s.update([m(1, 5)]); s.update([m(1, 5)]); s.update([]);
assert.deepEqual(got.splice(0), [[10, 'missile-travel'], [11, 'missile-hit']]);
s.update([m(2, 6)]); s.update([]);
assert.deepEqual(got, [[12, 'missile-hit']]);
console.log('missile-sounds ok');
