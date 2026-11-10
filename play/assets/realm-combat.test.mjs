import test from 'node:test';
import assert from 'node:assert/strict';
import { realmLeftMelee, realmHoverTarget, realmCombatSoundFrame } from './realm-combat.mjs';
import { createRealmInput } from './realm-input.mjs';
import { plateBoxes } from './hover-lifebar.mjs';
import { readFileSync } from 'node:fs';
import { createCombatSoundTrigger } from './combat-sound-trigger.mjs';
import { realmViewSnapshot } from './realm-client-view.mjs';

test('the plain attack is hand-to-hand unless a bow or crossbow is held', () => {
  assert.equal(realmLeftMelee(0, 'hth'), true);
  assert.equal(realmLeftMelee(0, '1HS'), true);
  assert.equal(realmLeftMelee(0, undefined), true);
  assert.equal(realmLeftMelee(0, 'bow'), false);
  assert.equal(realmLeftMelee(0, 'XBW'), false);
  assert.equal(realmLeftMelee(36, 'hth'), false);
  assert.equal(realmLeftMelee(-1, 'hth'), false);
});

// The pointer over a drawn monster, through the realm input's own pick, to the plate the single-player label draws.
test('a hovered monster is named with the server life byte as the plate fill', () => {
  const monsters = [{ guid: 77, classId: 5, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true },
    { guid: 78, classId: 5, mode: 12, x: 120, y: 101, anchorX: 300, anchorY: 120, drawn: true },
    { guid: 79, classId: 99, mode: 1, x: 130, y: 101, anchorX: 400, anchorY: 120, drawn: true }];
  const input = createRealmInput({ transmit: () => true, now: () => 0, scene: () => ({ ready: true, player: { x: 100, y: 100 }, monsters, objects: [], items: [],
    isNpc: (c) => c === 99, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }) }) });
  const name = (classId) => ({ stringIndex: 1000 + classId, label: classId === 5 ? 'Zombie' : 'Akara' });
  const views = [{ guid: 77, mode: 1, hpPct: 50, typeFlags: 0 }, { guid: 78, mode: 12, hpPct: 0, typeFlags: 0 }];
  const target = realmHoverTarget(input.pick(200, 120), views, name);
  assert.deepEqual(target, { kind: 'unit', id: 77, classId: 5, stringIndex: 1005, label: 'Zombie', colour: 0, x: 101, y: 101, life: 64 });
  // The plate's rectangles for that fill: the life colour over the left half, the dark remainder.
  assert.deepEqual(plateBoxes(10, 18, 100, 16, target.life), [[10, 18, 60, 34, 5, 1], [60, 18, 110, 34, 0, 2]]);
  // No life packet yet: a full bar. A champion is blue, a unique gold.
  assert.equal(realmHoverTarget(input.pick(200, 120), [], name).life, 128);
  assert.equal(realmHoverTarget(input.pick(200, 120), [{ guid: 77, mode: 1, hpPct: 100, typeFlags: 4 }], name).colour, 3);
  assert.equal(realmHoverTarget(input.pick(200, 120), [{ guid: 77, mode: 1, hpPct: 100, typeFlags: 8 }], name).colour, 4);
  // A corpse, a person and bare ground name no monster.
  assert.equal(realmHoverTarget(input.pick(300, 120), views, name), null);
  assert.equal(realmHoverTarget(input.pick(400, 120), views, name), null);
  assert.equal(realmHoverTarget(input.pick(600, 120), views, name), null);
  assert.equal(realmHoverTarget(input.pick(200, 120), views, () => ({ stringIndex: -1, label: '' })), null);
});

// A fight as the server's monster views tell it, tick by tick, through the realm snapshot into the single-player trigger.
test('a realm fight plays the monster attack, hit and death cues and the hero cast cue', (t) => {
  const dir = new URL('../../retail/tables-1.13c/', import.meta.url);
  const read = (p) => readFileSync(new URL(p.split('\\').pop(), dir));
  try { read('monstats.bin'); } catch { t.skip('retail tables absent'); return; }
  const got = []; const trigger = createCombatSoundTrigger(read, (row, e) => got.push([e.kind, e.unitType, e.unitId, row]));
  const hero = (mode) => ({ source: 'realm', guid: 1, classId: 1, renderState: { mode, framePrecise: 0 } });
  const tick = (monsterMode, heroMode, castSkill = 36) => {
    const views = [{ guid: 7, classId: 5, mode: monsterMode, x: 10, y: 10, direction: 0, typeFlags: 0, hpPct: 50 }];
    const snapshot = realmViewSnapshot({ base: null, resources: null, monsters: views, missiles: [], players: [], level: 2, actor: () => null, frame: 0 });
    const { entities, frame } = realmCombatSoundFrame(snapshot, views, hero(heroMode), castSkill);
    trigger.update(entities, frame);
  };
  // Zombie (class 5): neutral, attack, neutral, get hit, neutral, death, corpse. The hero stands (mode 1) throughout.
  for (const mode of [1, 4, 1, 3, 1, 0, 12]) tick(mode, 1);
  assert.deepEqual(got, [['attack', 1, 7, 2203], ['hit', 1, 7, 2209], ['death', 1, 7, 2215]]);
  // The hero enters the cast mode (10) with Fire Bolt (36) selected: the skill's start sound, on the hero.
  got.length = 0; tick(12, 10); tick(12, 1);
  assert.equal(got.length, 1);
  assert.deepEqual(got[0].slice(0, 3), ['cast', 0, 1]);
  assert.ok(got[0][3] > 0);
});

test('the realm sound frame carries no hit points and adds the hero only when the realm draws it', () => {
  const snapshot = { entities: [{ type: 1, id: 7, classId: 5, mode: 1, hp: 0 }, { type: 0, id: 2, classId: 3, mode: 2, hp: 0 }, { type: 3, id: 9, classId: 1, mode: 0 }], player: { hp: 5 } };
  const { entities, frame } = realmCombatSoundFrame(snapshot, [{ guid: 7, typeFlags: 8 }], { source: 'none' }, 0);
  assert.deepEqual(entities, [{ type: 1, id: 7, classId: 5, mode: 1, hp: undefined, typeFlags: 8 }, { type: 0, id: 2, classId: 3, mode: 2, hp: undefined }]);
  assert.equal(frame.playerId, undefined);
  assert.equal(frame.player.castSkillId, 0);
});
