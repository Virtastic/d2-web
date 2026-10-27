import assert from 'node:assert/strict';
import { createSkillFx } from './skill-fx.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tTrans\tAnimRate\ncast_undead\tHolyBoltCloud\t16\t3\t256\n'),
  'data\\global\\excel\\Missiles.txt': enc('Missile\tId\nblood1\t18\n'),
  'data\\global\\excel\\skills.txt': enc('skill\tId\tcastoverlay\tcltmissilea\nHolyBolt\t101\tcast_undead\t\nSacrifice\t96\t\tblood1\n'),
};
const fx = createSkillFx((p) => files[p]);
const h = (mode, fp, hp) => ({ mode, hp, animation: { framePrecise: fp } });
fx.update(h(1, 0, 100), 101);
let a = fx.update(h(10, 0, 100), 101);
assert.deepEqual(a.map((e) => [e.kind, e.file, e.frames, e.trans]), [['overlay', 'HolyBoltCloud', 16, 3]]);
for (let i = 0; i < 16; i++) a = fx.update(h(10, 256 * i, 100), 101);
assert.equal(a.length, 0, 'overlay ends after Frames');
fx.update(h(1, 0, 100), 96); fx.update(h(7, 0, 100), 96);
assert.equal(fx.update(h(7, 256, 100), 96).length, 0);
a = fx.update(h(7, 512, 90), 96);
assert.deepEqual(a.map((e) => [e.kind, e.classId]), [['missile', 18]]);
console.log('skill-fx ok');
files['data\\global\\excel\\Overlay.txt'] = enc('overlay\tFilename\tFrames\tTrans\tAnimRate\ncast_undead\tHolyBoltCloud\t16\t3\t128\n');
const slow = createSkillFx((p) => files[p]); slow.update(h(1, 0, 100), 101);
let n = slow.update(h(10, 0, 100), 101).length, t = 0; while (n && t < 100) { t++; n = slow.update(h(10, 256 * t, 100), 101).length; }
assert.equal(t, 32, 'half rate lives twice as long');
