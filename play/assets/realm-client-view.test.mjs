// The joined game's view data: what the panels and the scene read is the server's, shaped as the offline snapshot is.
import test from 'node:test';
import assert from 'node:assert/strict';
import { realmViewSnapshot, realmFrameReads } from './realm-client-view.mjs';

const actor = () => null;
const base = (level) => ({ tick: 1, seed: 2, act: 1, level, difficulty: 0, player: { id: 7, classId: 1, x: 10, y: 20, mode: 1, hp: 0, hpMax: 0 } });
const snap = (level, monsters = []) => realmViewSnapshot({ base: base(level), resources: null, monsters, missiles: [], players: [], level, actor, frame: 0 });
const hud = (level) => ({
  advancement: () => (level ? { level } : null), attributeTotals: () => ({ strength: 30, energy: 10, dexterity: 20, vitality: 25 }),
  characterSlots: (cls, screen) => { screen[0] = 5; return true; }, classSkills: () => null, waypointTable: () => null, questWords: null,
});

test('the town flag follows the level the server placed the hero in', () => {
  for (const level of [1, 40, 75, 103, 109]) assert.equal(snap(level).town, true, `level ${level}`);
  for (const level of [0, 2, 39, 41, 108, 110]) assert.equal(snap(level).town, false, `level ${level}`);
  assert.equal(realmViewSnapshot({ base: null, resources: null, monsters: [], missiles: [], level: 0, actor, frame: 0 }).town, false);
});

test('a monster carries the life the server reported, as a share of 100; dead is its mode, not 0% life', () => {
  const mon = (hpPct, mode = 1) => ({ guid: 3, classId: 5, mode, x: 1, y: 2, direction: 0, hpPct });
  const [hurt, nearlyDead, dying, corpse, unknown] = snap(2, [mon(40), mon(0), mon(0, 0), mon(30, 12), { ...mon(0), hpPct: undefined }]).entities;
  assert.deepEqual([hurt.hp, hurt.hpMax, hurt.dead], [40, 100, false]);
  assert.deepEqual([nearlyDead.hp, nearlyDead.hpMax, nearlyDead.dead], [1, 100, false]);   // 0% and still standing: alive
  assert.deepEqual([dying.hp, dying.dead], [0, true]);
  assert.deepEqual([corpse.hp, corpse.dead], [0, true]);                                   // a corpse whose last life share was not 0
  assert.deepEqual([unknown.hp, unknown.hpMax, unknown.dead], [0, 0, false]);
});

test('the character screen and attribute totals are the hero unit\'s once it is read', () => {
  const screen = [120, 300, 4, 9, 0, 80, 10, 20, 30, 40, 5000, 0, 200, 0, 90, 0, 70];
  const calls = [];
  const hero = { screen, stat: (id) => { calls.push(id); return 100 + id; } };
  const res = { hp: 150 << 8, hpMax: 180 << 8, mana: 33 << 8, manaMax: 50 << 8, stamina: 61 << 8, staminaMax: 70 << 8 };
  const r = realmFrameReads(res, null, null, 1, hud(12), null, null, null, hero);
  assert.equal(r.characterScreen.length, 17);
  assert.deepEqual(r.characterScreen.slice(0, 4), [120, 300, 4, 9]);            // defense, attack rating, damage: the unit's
  assert.deepEqual(r.characterScreen.slice(6, 11), [10, 20, 30, 40, 5000]);     // resistances and next level: the unit's
  assert.deepEqual([r.characterScreen[4], r.characterScreen[11], r.characterScreen[13]], [61, 150, 33]);   // current values: the packets'
  assert.deepEqual([r.characterScreen[5], r.characterScreen[12], r.characterScreen[14], r.characterScreen[16]], [80, 200, 90, 70]);   // maxima: the unit's
  assert.deepEqual(r.attributeTotals, { strength: 100, energy: 101, dexterity: 102, vitality: 103 });
  assert.deepEqual(calls, [0, 1, 2, 3]);
  assert.deepEqual(screen[4], 0, 'the protocol\'s array is not written');
});

test('without a hero read, or before the server sends a level, the screen is the server\'s raw stats', () => {
  const res = { stamina: 30 << 8, staminaMax: 60 << 8 };
  const raw = realmFrameReads(res, null, null, 1, hud(12));
  assert.equal(raw.characterScreen[0], 5);
  assert.deepEqual(raw.attributeTotals, { strength: 30, energy: 10, dexterity: 20, vitality: 25 });
  const early = realmFrameReads(res, null, null, 1, hud(0), null, null, null, { screen: new Array(17).fill(9), stat: () => 9 });
  assert.equal(early.advancement, null);
  assert.equal(early.characterScreen[0], 5);
  assert.deepEqual(early.attributeTotals, { strength: 30, energy: 10, dexterity: 20, vitality: 25 });
  const noScreen = realmFrameReads(res, null, null, 1, hud(12), null, null, null, { screen: null, stat: () => 9 });
  assert.equal(noScreen.characterScreen[0], 5);
});
