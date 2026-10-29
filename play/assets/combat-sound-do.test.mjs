// D2Client 0x14F40 (cltdofunc 16 = Jab): the hero's attack entry submits Skills.txt `dosound a` (Sounds row 286 weapon_1ht_1).
// Zeal (cltdofunc 21) has no dosound submit. Pins row ids and the tick of the entry.
import assert from 'node:assert/strict';
import { createCombatSoundTrigger } from './combat-sound-trigger.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\monstats.bin': new Uint8Array(8), 'data\\global\\excel\\MonSounds.bin': new Uint8Array(8),
  'data\\global\\excel\\Sounds.txt': enc('Sound\tIndex\tx\ty\tGroup\nweapon_1ht_1\t286\t\t\t1\nweapon_2ht_1\t292\t\t\t1\n'),
  'data\\global\\excel\\skills.txt': enc('skill\tId\tcltdofunc\tdosound a\tdosound b\tstsound\nJab\t10\t16\tweapon_1ht_1\tweapon_2ht_1\t\nZeal\t106\t21\t\t\tpaladin_zeal\n'),
  'data\\global\\excel\\weapons.txt': enc('a\n'),
};
const run = (skillId) => {
  const out = [];
  let tick = 0;
  const t = createCombatSoundTrigger((p) => { if (!files[p]) throw new Error(p); return files[p]; }, (r, i) => out.push([tick, r, i.kind]));
  const snap = { playerId: 1, player: { castSkillId: skillId }, items: [] };
  const step = (mode) => { tick++; t.update([{ type: 0, id: 1, classId: 3, mode, animation: { framePrecise: 0 } }], snap); };
  step(0); step(7); step(7);
  return out.filter(([, , k]) => k === 'dosound');
};
assert.deepEqual(run(10), [[2, 286, 'dosound']]);
assert.deepEqual(run(106), []);
console.log('combat-sound-do ok');
