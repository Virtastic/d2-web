import assert from 'node:assert/strict';
import { createSkillFx } from './skill-fx.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tTrans\tAnimRate\ncast_undead\tHolyBoltCloud\t16\t3\t16\nfire_cast_2\tFireCast2\t16\t3\t16\nice_cast_1\tIceCastNew01\t15\t3\t16\nlight_cast_1\tLightCast\t8\t0\t16\n'),
  'data\\global\\excel\\Missiles.txt': enc('Missile\tId\tRange\tAnimLen\tanimrate\nblood1\t18\t128\t9\t256\nblaze\t67\t90\t37\t1024\nfrostnova\t119\t14\t14\t1024\n'),
  'data\\global\\excel\\skills.txt': enc('skill\tId\tcastoverlay\tcltmissile\tcltmissilea\tcltdofunc\nHolyBolt\t101\tcast_undead\t\t\t\nSacrifice\t96\t\t\tblood1\t34\nFrost Nova\t44\tice_cast_2\t\tfrostnova\t25\nBlaze\t46\tfire_cast_2\tblaze\tfirewall\t\nFireBall\t47\tfire_cast_2\t\t\t\nChainLightning\t53\tlight_cast_1\t\t\t\n'),
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
assert.ok(a[0].predraw);   // a retail recording: retail draws blood1 before the player (first unit G 10041), at its own world position
for (let i = 0; i < 100; i++) a = fx.update(h(1, 0, 90), 96);
assert.deepEqual(a.map((e) => e.age), [8], 'splatter holds its last frame for Range ticks');
for (let i = 0; i < 40; i++) a = fx.update(h(1, 0, 90), 96);
assert.equal(a.length, 1, 'retail: blood1 is never freed (drawn to tick 400)');
// Cast-mode client missiles, spawned on the cast entry tick (D2Client 0xA2C90 spawns skill cltmissile; cltdofunc 25 = 0x73CB0 spawns cltmissilea).
// Missiles.txt animrate 1024 = 4 frames per tick; a non-looping missile holds its last AnimLen frame.
const castAges = (skill) => { const f = createSkillFx((p) => files[p]); f.update(h(1, 0, 100), skill);
  return Array.from({ length: 10 }, () => f.update(h(10, 0, 100), skill).filter((e) => e.kind === 'missile').map((e) => [e.classId, e.age])); };
const fn = castAges(44), bl = castAges(46);
assert.deepEqual(fn[0], [[119, 0]], 'Frost Nova spawns frostnova (cltmissilea) on the cast tick');
assert.deepEqual(fn.map((r) => r[0]?.[1]), [0, 4, 8, 12, 13, 13, 13, 13, 13, 13], 'frostnova advances 4 frames/tick, holds AnimLen-1=13');
assert.deepEqual(bl[0], [[67, 0]], 'Blaze spawns blaze (cltmissile) on the cast tick');
assert.deepEqual(bl.map((r) => r[0]?.[1]), [0, 4, 8, 12, 16, 20, 24, 28, 32, 36], 'blaze advances 4 frames/tick (AnimLen 37)');
console.log('skill-fx ok');
// SQ casts (mode 18) start the castoverlay as well: Chain Lightning (SequenceTbls.cpp Whirlwind is one such record).
const sq = createSkillFx((p) => files[p]); sq.update(h(1, 0, 100), 53);
assert.deepEqual(sq.update(h(18, 0, 100), 53).map((e) => [e.kind, e.file]), [['overlay', 'LightCast']]);
// Retail f563 (FireCast2, AnimRate 16): 12 consecutive game frames show cframe 4..15, one frame per tick.
const retail = createSkillFx((p) => files[p]); retail.update(h(1, 0, 100), 47);
const fages = [];
for (let i = 0; i < 12; i++) { const e = retail.update(h(10, 0, 100), 47).find((x) => x.file === 'FireCast2'); fages.push(e.age); }
assert.deepEqual(fages, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11], 'FireCast2 advances 1 frame per game frame');
files['data\\global\\excel\\Overlay.txt'] = enc('overlay\tFilename\tFrames\tTrans\tAnimRate\ncast_undead\tHolyBoltCloud\t16\t3\t8\n');
const slow = createSkillFx((p) => files[p]); slow.update(h(1, 0, 100), 101);
let n = slow.update(h(10, 0, 100), 101).length, t = 0; while (n && t < 100) { t++; n = slow.update(h(10, 256 * t, 100), 101).length; }
assert.equal(t, 32, 'half rate lives twice as long');
// Shiver Armor (skills.txt castoverlay ice_cast_2 = Overlay.txt 15 frames, AnimRate 16): one frame per tick, gone after 15 (D2Client 0x6B300 AnimRate<<4).
const ice = { 'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tTrans\tAnimRate\nice_cast_2\tIceCastNew02\t15\t3\t16\n'), 'data\\global\\excel\\Missiles.txt': files['data\\global\\excel\\Missiles.txt'], 'data\\global\\excel\\skills.txt': enc('skill\tId\tcastoverlay\nShiverArmor\t50\tice_cast_2\n') };
const shiver = createSkillFx((p) => ice[p]); shiver.update(h(1, 0, 100), 50);
const ages = []; for (let t = 0; t < 20; t++) ages.push(shiver.update(h(10, 0, 100), 50).map((e) => e.age).join(','));
assert.deepEqual(ages, [...Array.from({ length: 15 }, (_, i) => String(i)), '', '', '', '', ''], 'ice_cast_2 record stream: age = tick, dropped at 15');
console.log('skill-fx shiver ok');

// Telekinesis (skills.txt Id 43, castoverlay light_cast_2 = Overlay.txt AnimRate 16, Trans 3): the overlay's frame is a
// function of ENGINE ticks (25/s), not of how many render frames ran. Extra render frames inside one engine tick must
// not move the record; 1 engine tick = 1 advance, the frame index is floor((t - cast) * 16 / 256) for 10 frames.
{
  const tk = {
    'data\\global\\excel\\Overlay.txt': enc('overlay\tFilename\tFrames\tTrans\tAnimRate\nlight_cast_2\tLightningCastRunesFront\t10\t3\t16\n'),
    'data\\global\\excel\\Missiles.txt': enc('Missile\tId\tRange\tAnimLen\n'),
    'data\\global\\excel\\skills.txt': enc('skill\tId\tcastoverlay\tcltmissilea\nTelekinesis\t43\tlight_cast_2\t\n'),
  };
  const record = (perTick) => {
    const fx = createSkillFx((p) => tk[p]), rec = [];
    fx.update(h(1, 0, 100), 43, 0);
    for (let t = 1; t <= 200; t++) {
      const hero = h(t < 10 ? 1 : 10, 0, 100);
      let a = [];
      for (let r = 0; r < perTick; r++) a = fx.update(hero, 43, t);
      rec.push(a.map((e) => e.age).join(','));
    }
    return rec;
  };
  const one = record(1), many = record(3);
  assert.deepEqual(many, one, 'render frames inside one engine tick must not advance the overlay');
  for (let t = 10; t <= 200; t++) {
    const want = t - 10 < 10 ? String(t - 10) : ''; // AnimRate*16/256 = 1 frame per engine tick (was /16 under the old AnimRate/256 rule)
    assert.equal(one[t - 1], want, `tick ${t}`);
  }
}
console.log('skill-fx telekinesis clock ok');
