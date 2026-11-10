import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmUnits, realmCombatSoundFrame } from './realm-combat.mjs';
import { routeRealmPackets } from './realm-packet-routes.mjs';
import { createMonsterTable } from './realm-tables.mjs';
import { createRealmInput } from './realm-input.mjs';
import { plateBoxes } from './hover-lifebar.mjs';
import { readFileSync } from 'node:fs';
import { createCombatSoundTrigger } from './combat-sound-trigger.mjs';
import { realmViewSnapshot } from './realm-client-view.mjs';

// The pointer over a drawn monster, through the realm input's own pick, to the plate the single-player label draws.
test('a hovered monster is named with the server life byte as the plate fill', () => {
  const monsters = [{ guid: 77, classId: 5, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true },
    { guid: 78, classId: 5, mode: 12, x: 120, y: 101, anchorX: 300, anchorY: 120, drawn: true },
    { guid: 79, classId: 99, mode: 1, x: 130, y: 101, anchorX: 400, anchorY: 120, drawn: true }];
  const input = createRealmInput({ transmit: () => true, now: () => 0, scene: () => ({ ready: true, player: { x: 100, y: 100 }, monsters, objects: [], items: [],
    isNpc: (c) => c === 99, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20, toScreen: (x, y) => ({ x: x * 2, y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }) }) });
  const name = (classId) => ({ stringIndex: 1000 + classId, label: classId === 5 ? 'Zombie' : 'Akara' });
  const realmHoverTarget = createRealmUnits({ rowOf: (c) => (c === 99 ? { npc: true, interact: true, killable: false, align: 1, named: true } : { killable: true, align: 0, named: true }) }).hoverTarget;
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

// The town and the field of a joined game, sorted by the MonStats rows and the server's pet packets: what each
// unit is, which plate and name colour it gets and which cursor the pointer takes over it. The monster records
// are the engine's views of the server's monster assigns (class, type flags, life byte); the pet packet is bytes.
test('a realm unit is sorted into person, enemy, hireling or pet, with its plate, colour and cursor', (t) => {
  const dir = new URL('../../retail/tables-1.13c/', import.meta.url);
  let table;
  try { const text = readFileSync(new URL('monstats.txt', dir)); table = createMonsterTable(() => text); table.row(0); } catch { t.skip('retail tables absent'); return; }
  const ME = 1, OTHER = 2, hostilePlayers = new Set();
  const units = createRealmUnits({ rowOf: table.row, hostilePlayer: (guid) => hostilePlayers.has(guid) });
  // Akara 148, Charsi 154, Zombie 5 (plain, champion, unique), Fallen 19, the Rogue hireling 271, a Necromancer's skeleton 363, a Valkyrie 357.
  const views = [[10, 148, 0, 100], [11, 154, 0, 100], [20, 5, 0, 50], [21, 5, 0x04, 100], [22, 5, 0x08, 100], [23, 19, 0x10, 25],
    [30, 271, 0, 80], [31, 363, 0, 100], [32, 357, 0, 100]].map(([guid, classId, typeFlags, hpPct], i) =>
    ({ guid, classId, typeFlags, hpPct, mode: 1, x: 100 + i * 10, y: 100, anchorX: 100 + i * 50, anchorY: 120, drawn: true }));
  const pet = (kind, classId, owner, guid) => { const p = new Uint8Array(20), v = new DataView(p.buffer);
    p[0] = 0x81; p[1] = kind; v.setUint16(2, classId, true); v.setUint32(4, owner, true); v.setUint32(8, guid, true); return p; };
  const input = createRealmInput({ transmit: () => true, now: () => 0, scene: () => ({ ready: true, player: { x: 90, y: 100 }, monsters: views, objects: [], items: [],
    isNpc: table.isNpc, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20, toScreen: (x, y) => ({ x, y }), pointerWorld: (px, py) => ({ x: px, y: py }) }) });
  const name = (classId) => ({ stringIndex: classId, label: `name${classId}` });
  const at = (guid) => { const hit = input.pick(views.find((m) => m.guid === guid).anchorX, 120); assert.equal(hit.unit.guid, guid);
    const target = units.hoverTarget(hit, views, name), cls = units.classOf(hit.unit);
    return [cls.kind, cls.hostile, target.npc ? 'above' : 'top', target.colour ?? null, target.life ?? null, target.label, units.cursorContext(hit).kind]; };

  // Town people: the bare name above the head, no life bar, the talk cursor, never an enemy.
  assert.deepEqual(at(10), ['npc', false, 'above', null, null, 'name148', 'talk']);
  assert.deepEqual(at(11), ['npc', false, 'above', null, null, 'name154', 'talk']);
  // Enemies: the top plate with the life byte as its fill, white, champion blue, unique gold; a minion is white.
  assert.deepEqual(at(20), ['monster', true, 'top', 0, 64, 'name5', 'attack']);
  assert.deepEqual(at(21), ['monster', true, 'top', 3, 128, 'name5', 'attack']);
  assert.deepEqual(at(22), ['monster', true, 'top', 4, 128, 'name5', 'attack']);
  assert.deepEqual(at(23), ['monster', true, 'top', 0, 32, 'name19', 'attack']);
  // Before any pet packet the hireling and the summons are friendly by their rows: the monster plate, the hand.
  assert.deepEqual(at(30), ['friendly', false, 'top', 0, 102, 'name271', 'hand']);
  assert.deepEqual(at(31), ['friendly', false, 'top', 0, 128, 'name363', 'hand']);

  // The server lists my hireling, my skeleton and another player's Valkyrie; other packets in the same read pass by.
  const sizes = { 0x81: 20, 0x8f: 33 }, stream = new Uint8Array(20 * 3 + 33);
  stream.set(pet(7, 271, ME, 30), 0); stream[20] = 0x8f; stream.set(pet(4, 363, ME, 31), 53); stream.set(pet(2, 357, OTHER, 32), 73);
  const done = routeRealmPackets(stream, (b) => sizes[b[0]], { applyPet: units.applyPet });
  assert.deepEqual(done.map((d) => d.result), [{ guid: 30, kind: 7, owner: ME }, { guid: 31, kind: 4, owner: ME }, { guid: 32, kind: 2, owner: OTHER }]);
  assert.deepEqual(at(30), ['hireling', false, 'top', 0, 102, 'name271', 'hand']);
  assert.deepEqual(at(31), ['pet', false, 'top', 0, 128, 'name363', 'hand']);
  assert.deepEqual(units.classOf(views[8]), { kind: 'pet', owner: OTHER, hostile: false, plate: 'monster' });
  // A pet takes its owner's side: the other player goes hostile, and so does the Valkyrie.
  hostilePlayers.add(OTHER);
  assert.deepEqual(at(32), ['pet', true, 'top', 0, 128, 'name357', 'attack']);
  assert.equal(units.classOf(views[6]).hostile, false);
  // A new hireling replaces the owner's old one; a summon does not.
  units.applyPet(pet(7, 338, ME, 40));
  assert.equal(units.classOf({ guid: 30, classId: 271 }).kind, 'friendly');
  assert.equal(units.classOf({ guid: 40, classId: 338 }).kind, 'hireling');
  assert.equal(units.classOf({ guid: 31, classId: 363 }).kind, 'pet');
  assert.equal(units.applyPet(new Uint8Array([0x81, 7, 0])), null);

  // The snapshot the renderer and the sound trigger read carries the same sorting; with no sorter nothing is an enemy.
  const snap = (classOf) => realmViewSnapshot({ base: null, resources: null, monsters: views, missiles: [], players: [], level: 1, actor: () => null, frame: 0, classOf })
    .entities.map((e) => [e.id, e.hostile, e.npc, e.unitClass]);
  assert.deepEqual(snap(units.classOf), [[10, false, true, 'npc'], [11, false, true, 'npc'], [20, true, false, 'monster'], [21, true, false, 'monster'],
    [22, true, false, 'monster'], [23, true, false, 'monster'], [30, false, false, 'friendly'], [31, false, false, 'pet'], [32, true, false, 'pet']]);
  assert.ok(snap(null).every(([, hostile, npc]) => hostile === false && npc === false));
  // Over an item, an object or bare ground the cursor is the hand.
  for (const kind of ['item', 'object', 'ground']) assert.deepEqual(units.cursorContext({ kind, unit: { guid: 1 } }), { kind: 'hand' });
  assert.deepEqual(units.cursorContext(null), { kind: 'hand' });
  units.clear();
  assert.equal(units.classOf(views[6]).kind, 'friendly');
});
