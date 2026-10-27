// D18: the hero's skill stsound fires on every cast entry, including the same mode re-entered back to back.
import assert from 'node:assert/strict';
import { createCombatSoundTrigger } from './combat-sound-trigger.mjs';
const enc = (s) => new TextEncoder().encode(s);
const files = {
  'data\\global\\excel\\monstats.bin': new Uint8Array(8), 'data\\global\\excel\\MonSounds.bin': new Uint8Array(8),
  'data\\global\\excel\\Sounds.txt': enc('Sound\tIndex\tx\ty\tGroup\nsk\t7\t\t\t1\n'),
  'data\\global\\excel\\skills.txt': enc('skill\tId\tstsound\nSacrifice\t96\tsk\n'),
  'data\\global\\excel\\weapons.txt': enc('a\n'),
};
const rows = [];
const t = createCombatSoundTrigger((p) => { if (!files[p]) throw new Error(p); return files[p]; }, (r, i) => rows.push([r, i.kind]));
const snap = { playerId: 1, player: { castSkillId: 96 }, items: [] };
const ent = (mode, fp) => [{ type: 0, id: 1, classId: 3, mode, animation: { framePrecise: fp } }];
t.update(ent(1, 0), snap); t.update(ent(7, 0), snap); t.update(ent(7, 2560), snap); t.update(ent(7, 0), snap);
assert.equal(rows.filter(([, k]) => k === 'cast').length, 2, JSON.stringify(rows));
console.log('combat-sound-trigger-cast ok');
