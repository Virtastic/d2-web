import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmUnits, realmCombatSoundFrame, parseRealmUnitInfo, shrineDirectHpLoss } from './realm-combat.mjs';
import { createMonsterTable } from './realm-tables.mjs';
import { createRealmInput } from './realm-input.mjs';
import { plateBoxes } from './hover-lifebar.mjs';
import { readFileSync } from 'node:fs';
import { createCombatSoundTrigger, hitSounds } from './combat-sound-trigger.mjs';
import { realmViewSnapshot } from './realm-client-view.mjs';

// The pointer over a drawn monster, through the realm input's own pick, to the plate the single-player label draws.
test('a hovered monster is named with the server life byte as the plate fill', () => {
  const monsters = [{ guid: 77, classId: 5, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true },
    { guid: 78, classId: 5, mode: 12, x: 120, y: 101, anchorX: 300, anchorY: 120, drawn: true },
    { guid: 79, classId: 99, mode: 1, x: 130, y: 101, anchorX: 400, anchorY: 120, drawn: true }];
  const input = createRealmInput({ transmit: () => true, now: () => 0, scene: () => ({ ready: true, player: { x: 100, y: 100 }, monsters, objects: [], items: [],
    isNpc: (c) => c === 99, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }) }) });
  const name = (classId) => ({ stringIndex: 1000 + classId, label: classId === 5 ? 'Zombie' : 'Akara' });
  const realmHoverTarget = createRealmUnits({ rowOf: (c) => (c === 99 ? { npc: true, interact: true, killable: false, align: 1, named: true } : { killable: true, align: 0, named: true }),
    unitInfo: (type, guid) => ({ enemy: guid !== 79, enemyIfHostile: guid !== 79, rosterGuid: 0xFFFFFFFF, name: '' }) }).hoverTarget;
  const views = [{ guid: 77, mode: 1, hpPct: 50, typeFlags: 0 }, { guid: 78, mode: 12, hpPct: 0, typeFlags: 0 }];
  const target = realmHoverTarget(input.pick(200, 120), views, name);
  assert.deepEqual(target, { kind: 'unit', id: 77, classId: 5, stringIndex: 1005, label: 'Zombie', unitClass: 'monster', colour: 0, x: 101, y: 101, life: 64 });
  // The plate's rectangles for that fill: the life colour over the left half, the dark remainder.
  assert.deepEqual(plateBoxes(10, 18, 100, 16, target.life), [[10, 18, 60, 34, 5, 1], [60, 18, 110, 34, 0, 2]]);
  // No life packet yet: a full bar. A champion is blue, a unique gold.
  assert.equal(realmHoverTarget(input.pick(200, 120), [], name).life, 128);
  assert.equal(realmHoverTarget(input.pick(200, 120), [{ guid: 77, mode: 1, hpPct: 100, typeFlags: 4 }], name).colour, 3);
  assert.equal(realmHoverTarget(input.pick(200, 120), [{ guid: 77, mode: 1, hpPct: 100, typeFlags: 8 }], name).colour, 4);
  // A person is named above the head, with no life bar. A corpse and bare ground name nothing.
  assert.equal(realmHoverTarget(input.pick(300, 120), views, name), null);
  assert.deepEqual(realmHoverTarget(input.pick(400, 120), views, name), { kind: 'unit', id: 79, classId: 99, stringIndex: 1099, label: 'Akara', unitClass: 'npc', npc: true, x: 130, y: 101, preciseX: undefined, preciseY: undefined });
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

// The engine's unit record as bytes, laid out as d2_realm_client_unit_info writes it.
const unitRecord = (f) => { const b = new Uint8Array(468), v = new DataView(b.buffer), u = (at, x) => v.setUint32(at, x >>> 0, true);
  u(0, 1); u(4, 468); u(8, 1); u(12, f.guid); u(16, f.selectable ?? f.enemy); u(20, f.enemy); u(24, f.enemyIfHostile ?? f.enemy); u(28, f.rosterGuid ?? 0xFFFFFFFF);
  u(32, f.alignment ?? 0); u(36, f.petType ? 1 : 0); u(40, f.petType ?? 0); u(44, f.petOwner ?? 0); u(48, f.petDead ?? 0); u(52, f.petNameId ?? 0);
  u(56, f.typeFlags ?? 0); u(60, f.superUnique ?? -1); u(64, f.nameSeed ?? 0); u(68, f.ownerGuid ?? 0); b.set(f.umods ?? [], 72);
  b.set(new TextEncoder().encode(f.name ?? ''), 84); b.set(new TextEncoder().encode(f.line ?? ''), 212); return b; };

test('the engine unit record parses field for field and rejects another layout', () => {
  const info = parseRealmUnitInfo(unitRecord({ guid: 0x12345678, enemy: 0, enemyIfHostile: 1, rosterGuid: 9, alignment: 2, petType: 7, petOwner: 9, petDead: 1, petNameId: 0x421,
    typeFlags: 0x0a, superUnique: 3, nameSeed: 0xdeadbeef, ownerGuid: 5, umods: [1, 5, 9], name: 'Bishibosh', line: 'Demon Fire Enchanted' }));
  assert.deepEqual(info, { unitType: 1, guid: 0x12345678, selectable: false, enemy: false, enemyIfHostile: true, rosterGuid: 9, alignment: 2, isPet: true, petType: 7, petOwner: 9,
    petDead: true, petNameId: 0x421, typeFlags: 0x0a, superUnique: 3, nameSeed: 0xdeadbeef, ownerGuid: 5, umods: [1, 5, 9, 0, 0, 0, 0, 0, 0], name: 'Bishibosh', line: 'Demon Fire Enchanted' });
  assert.equal(parseRealmUnitInfo(unitRecord({ guid: 1, enemy: 1 })).superUnique, -1);
  const wrong = unitRecord({ guid: 1, enemy: 1 }); wrong[0] = 2;
  assert.throws(() => parseRealmUnitInfo(wrong), /invalid realm unit record/);
  assert.throws(() => parseRealmUnitInfo(new Uint8Array(100)), /invalid realm unit record/);
});

// The town and the field of a joined game, sorted by the MonStats rows and the engine's unit records: what each
// unit is, which plate, name and name colour it gets and which cursor the pointer takes over it. The records
// here are bytes in the engine's layout; the engine fills them from the server's packets (its own test feeds those).
test('a realm unit is sorted into person, enemy, hireling or pet, with its plate, name, colour and cursor', (t) => {
  const dir = new URL('../../retail/tables-1.13c/', import.meta.url);
  let table;
  try { const text = readFileSync(new URL('monstats.txt', dir)); table = createMonsterTable(() => text); table.row(0); } catch { t.skip('retail tables absent'); return; }
  const ME = 1, OTHER = 2, hostilePlayers = new Set(), NONE = 0xFFFFFFFF;
  // Akara 148, Charsi 154, Zombie 5 (plain, champion, unique), Fallen 19 (minion), the Rogue hireling 271, a skeleton 363, a Valkyrie 357.
  const records = new Map([
    [10, { enemy: 0 }], [11, { enemy: 0 }], [20, { enemy: 1 }], [21, { enemy: 1, typeFlags: 0x04, name: 'Zombie Champion' }],
    [22, { enemy: 1, typeFlags: 0x08, name: 'Gloom Maw the Hungry', line: 'Undead Extra Strong' }], [23, { enemy: 1, typeFlags: 0x10, line: 'Demon Minion' }],
    [30, { enemy: 0 }], [31, { enemy: 0 }], [32, { enemy: 0 }]]);
  const units = createRealmUnits({ rowOf: table.row, hostilePlayer: (guid) => hostilePlayers.has(guid),
    unitInfo: (type, guid) => (type === 1 && records.has(guid) ? parseRealmUnitInfo(unitRecord({ guid, ...records.get(guid) })) : null) });
  const views = [[10, 148, 0, 100], [11, 154, 0, 100], [20, 5, 0, 50], [21, 5, 0x04, 100], [22, 5, 0x08, 100], [23, 19, 0x10, 25],
    [30, 271, 0, 80], [31, 363, 0, 100], [32, 357, 0, 100], [33, 5, 0, 100]].map(([guid, classId, typeFlags, hpPct], i) =>
    ({ guid, classId, typeFlags, hpPct, mode: 1, x: 100 + i * 10, y: 100, anchorX: 100 + i * 50, anchorY: 120, drawn: true }));
  const input = createRealmInput({ transmit: () => true, now: () => 0, scene: () => ({ ready: true, player: { x: 90, y: 100 }, monsters: views, objects: [], items: [],
    isNpc: table.isNpc, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20, toScreen: (x, y) => ({ x, y }), pointerWorld: (px, py) => ({ x: px, y: py }) }) });
  const name = (classId) => ({ stringIndex: classId, label: `name${classId}` });
  const at = (guid) => { const hit = input.pick(views.find((m) => m.guid === guid).anchorX, 120); assert.equal(hit.unit.guid, guid);
    const target = units.hoverTarget(hit, views, name), cls = units.classOf(hit.unit);
    return [cls.kind, cls.hostile, target.npc ? 'above' : 'top', target.colour ?? null, target.life ?? null, target.label, units.cursorContext(hit).kind]; };
  const line = (guid) => units.hoverTarget(input.pick(views.find((m) => m.guid === guid).anchorX, 120), views, name).line;

  // Town people: the bare name above the head, no life bar, the talk cursor, never an enemy.
  assert.deepEqual(at(10), ['npc', false, 'above', null, null, 'name148', 'talk']);
  assert.deepEqual(at(11), ['npc', false, 'above', null, null, 'name154', 'talk']);
  // Enemies: the top plate with the life byte as its fill. A plain monster keeps its class name and type line;
  // a champion and a unique take the engine's name, a unique and a minion the engine's line (an empty one too).
  assert.deepEqual(at(20), ['monster', true, 'top', 0, 64, 'name5', 'attack']);
  assert.deepEqual(at(21), ['monster', true, 'top', 3, 128, 'Zombie Champion', 'attack']);
  assert.deepEqual(at(22), ['monster', true, 'top', 4, 128, 'Gloom Maw the Hungry', 'attack']);
  assert.deepEqual(at(23), ['monster', true, 'top', 0, 32, 'name19', 'attack']);
  assert.deepEqual([line(20), line(21), line(22), line(23)], [undefined, undefined, 'Undead Extra Strong', 'Demon Minion']);
  records.set(23, { enemy: 1, typeFlags: 0x10 });
  assert.equal(line(23), '');
  // A monster the engine calls no enemy (its alignment matches the hero's) is friendly whatever its row says.
  assert.deepEqual(at(30), ['friendly', false, 'top', 0, 102, 'name271', 'hand']);
  records.set(20, { enemy: 0, alignment: 2 });
  assert.deepEqual(at(20), ['friendly', false, 'top', 0, 64, 'name5', 'hand']);
  // A unit the engine holds no record of is nobody's enemy.
  assert.deepEqual(at(33), ['friendly', false, 'top', 0, 128, 'name5', 'hand']);

  // The engine lists my hireling (with its own name), my skeleton and another player's Valkyrie.
  records.set(30, { enemy: 0, petType: 7, petOwner: ME, name: 'Aliza' });
  records.set(31, { enemy: 0, petType: 4, petOwner: ME });
  records.set(32, { enemy: 0, enemyIfHostile: 1, rosterGuid: OTHER, petType: 2, petOwner: OTHER });
  assert.deepEqual(at(30), ['hireling', false, 'top', 0, 102, 'Aliza', 'hand']);
  assert.deepEqual(at(31), ['pet', false, 'top', 0, 128, 'name363', 'hand']);
  assert.deepEqual(units.classOf(views[8]), { kind: 'pet', owner: OTHER, hostile: false, plate: 'monster' });
  // A pet takes its owner's side: the other player goes hostile, and so does the Valkyrie. Mine stay mine.
  hostilePlayers.add(OTHER);
  assert.deepEqual(at(32), ['pet', true, 'top', 0, 128, 'name357', 'attack']);
  assert.equal(units.classOf(views[6]).hostile, false);
  assert.equal(units.classOf(views[2]).hostile, false);
  // A dismissed pet leaves the engine's list and is a plain unit again.
  records.set(31, { enemy: 0 });
  assert.equal(units.classOf(views[7]).kind, 'friendly');

  // The snapshot the renderer and the sound trigger read carries the same sorting; with no sorter nothing is an enemy.
  const snap = (classOf) => realmViewSnapshot({ base: null, resources: null, monsters: views.slice(0, 9), missiles: [], players: [], level: 1, actor: () => null, frame: 0, classOf })
    .entities.map((e) => [e.id, e.hostile, e.npc, e.unitClass]);
  assert.deepEqual(snap(units.classOf), [[10, false, true, 'npc'], [11, false, true, 'npc'], [20, false, false, 'friendly'], [21, true, false, 'monster'],
    [22, true, false, 'monster'], [23, true, false, 'monster'], [30, false, false, 'hireling'], [31, false, false, 'friendly'], [32, true, false, 'pet']]);
  assert.ok(snap(null).every(([, hostile, npc]) => hostile === false && npc === false));
  // Over an item, an object or bare ground the cursor is the hand.
  for (const kind of ['item', 'object', 'ground']) assert.deepEqual(units.cursorContext({ kind, unit: { guid: 1 } }), { kind: 'hand' });
  assert.deepEqual(units.cursorContext(null), { kind: 'hand' });
});

test('a struck unit sounds the impact its hit class names', (t) => {
  const dir = new URL('../../retail/tables-1.13c/', import.meta.url);
  const read = (p) => readFileSync(new URL(p.split('\\').pop(), dir));
  try { read('monstats.bin'); } catch { t.skip('retail tables absent'); return; }
  const got = []; const trigger = createCombatSoundTrigger(read, (row, e) => got.push([e.kind, e.unitType, e.unitId, row]));
  const frameOf = (mode, hitClassOf) => realmCombatSoundFrame({ entities: [{ type: 1, id: 7, classId: 5, mode }, { type: 3, id: 7, classId: 1, mode: 0 }] }, [{ guid: 7, typeFlags: 0 }], { source: 'realm', guid: 1, classId: 1, renderState: { mode: 1, framePrecise: 0 } }, 0, hitClassOf);
  const asked = [];
  const struck = frameOf(3, (type, id) => { asked.push([type, id]); return type === 1 ? 0x21 : 0; });
  assert.deepEqual(asked, [[1, 7], [0, 1]]);
  assert.deepEqual(struck.entities.map((e) => e.lastHitClass), [0x21, 0]);
  { const f = frameOf(1, () => 0); trigger.update(f.entities, f.frame); }
  trigger.update(struck.entities, struck.frame);
  assert.deepEqual(got.filter((g) => g[0] === 'impact').map((g) => g.slice(1)), hitSounds(0x21, 1, 3).map((r) => [1, 7, r]));
  assert.ok(hitSounds(0x21, 1, 3).length > 0);
  // Without the engine's answer the frame is as before: no hit class, no impact.
  assert.equal(frameOf(3).entities[0].lastHitClass, undefined);
});

test('a monster with unit life: the plate fills by hit points over the maximum, and a reported hit marks the sound frame', () => {
  const monsters = [{ guid: 77, classId: 5, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true }];
  const input = createRealmInput({ transmit: () => true, now: () => 0, scene: () => ({ ready: true, player: { x: 100, y: 100 }, monsters, objects: [], items: [],
    isNpc: () => false, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }) }) });
  const units = createRealmUnits({ rowOf: () => ({ killable: true, align: 0, named: true }), unitInfo: () => ({ enemy: true, enemyIfHostile: true, rosterGuid: 0xFFFFFFFF, name: '' }) });
  const name = () => ({ stringIndex: 1005, label: 'Zombie' });
  // An assigned life byte of 0x55 is 0x5500 of 0x8000: a fill of 0x55 of 128, not 85 percent.
  const life = (view) => units.hoverTarget(input.pick(200, 120), [{ guid: 77, mode: 1, typeFlags: 0, ...view }], name).life;
  assert.equal(life({ hpPct: 0x55, hp: 0x5500, hpMax: 0x8000 }), 0x55);
  assert.equal(life({ hpPct: 0x55, hp: 0x4100, hpMax: 0x8000 }), 0x41);   // a hit record lowered the unit's life
  assert.equal(life({ hpPct: 50 }), 64);                                    // no unit life yet: the life byte
  // The sound frame: the monster carries its unit's hit points and, on the tick of the hit, the hit mark.
  const snapshot = { entities: [{ type: 1, id: 77, classId: 5, mode: 1 }, { type: 1, id: 78, classId: 5, mode: 1 }, { type: 0, id: 2, classId: 1, mode: 1 }] };
  const views = [{ guid: 77, typeFlags: 8, hp: 0x4100, hpMax: 0x8000 }, { guid: 78, typeFlags: 0 }];
  const hits = new Map([['1:77', 0x31], ['0:9', 0]]);
  const { entities } = realmCombatSoundFrame(snapshot, views, { source: 'realm', guid: 9, classId: 3, renderState: { mode: 1, framePrecise: 0 } }, 0,
    (type, id) => (type === 1 && id === 77 ? 0x31 : 0), (type, id) => hits.get(`${type}:${id}`));
  assert.deepEqual(entities.map((e) => [e.type, e.id, e.hp, e.lastHitClass, e.hitPacket === true]),
    [[1, 77, 0x4100, 0x31, true], [1, 78, undefined, 0, false], [0, 2, undefined, 0, false], [0, 9, undefined, 0, true]]);
  assert.equal(entities[0].typeFlags, 8);
});

test('the no-hit life loss of a shrine comes from the activations the client took, and reaches the sound frame', () => {
  assert.equal(shrineDirectHpLoss([]), false);
  assert.equal(shrineDirectHpLoss(undefined), false);
  assert.equal(shrineDirectHpLoss([{ shrine: 0 }, { shrine: 23 }]), false);
  assert.equal(shrineDirectHpLoss([{ shrine: 0 }, { shrine: 1 }]), true);
  assert.equal(shrineDirectHpLoss([{ shrine: 22 }]), true);
  const snapshot = { entities: [], directHpLoss: true };   // a stale engine value never leaks into a packet game's frame
  assert.equal(realmCombatSoundFrame(snapshot, [], null, 0).frame.directHpLoss, false);
  assert.equal(realmCombatSoundFrame(snapshot, [], null, 0, undefined, undefined, true).frame.directHpLoss, true);
});
