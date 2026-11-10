import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { chromeGates } from './scene-chrome.mjs';
import { realmViewSnapshot, realmFrameReads } from './realm-client-view.mjs';
import { realmScenePlayers, realmPlayerGate, selectPlayerRenderSource } from './realm-player-state.mjs';
import { fitSize } from './fit-size.mjs';

const have = { hasControlPanel: {}, hasPointerLayer: {}, hasBelt: {}, clearScreen: false, screenIsGame: true };

test('a realm scene draws the HUD, belt and pointer like a local game, without engine reads', () => {
  const realm = chromeGates({ engineLive: false, realmSceneActive: true, ...have });
  const local = chromeGates({ engineLive: true, realmSceneActive: false, ...have });
  assert.deepEqual({ ...realm, engineReads: 0 }, { ...local, engineReads: 0 });
  assert.equal(realm.hud && realm.pointerLayer && realm.belt, true);
  assert.equal(realm.engineReads, false);
  assert.equal(chromeGates({ engineLive: false, realmSceneActive: false, ...have }).hud, false);
  assert.equal(chromeGates({ engineLive: false, realmSceneActive: true, ...have, clearScreen: true }).hud, false);
});

test('a realm scene draws the skill buttons beside the orbs, so the bar has no gap a local game lacks', () => {
  const realm = chromeGates({ engineLive: false, realmSceneActive: true, ...have, hasSkillSelect: {} });
  const local = chromeGates({ engineLive: true, realmSceneActive: false, ...have, hasSkillSelect: {} });
  assert.equal(realm.skillSelect, true);
  assert.equal(realm.skillSelect, local.skillSelect);
  assert.equal(chromeGates({ engineLive: false, realmSceneActive: true, ...have, hasSkillSelect: {}, clearScreen: true }).skillSelect, false);
  const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const realmChrome = shell.slice(shell.indexOf('function ensureRealmChrome()'), shell.indexOf('async function syncRealmScene()'));
  assert.match(realmChrome, /skillSelectRenderer \?\?= createSkillSelectRenderer\(/, 'the joined game builds the skill buttons');
  assert.ok(shell.includes('if (sceneChrome().skillSelect) {'), 'the skill buttons draw on the shared gate, not on a local engine');
});

test('the realm HUD binds no hotkey before the server binds one', () => {
  const r = realmFrameReads(null);
  assert.equal(r.hotkeys.length, 8);
  assert.ok(r.hotkeys.every((k) => k.skill === -1));
});

const belts = (counts) => ({ inventory: { playerRow: () => null, grid: () => null, components: () => null },
  belt: { boxes: (record) => Array.from({ length: counts[record] }, (_, i) => ({ left: 422 + (i % 4) * 31, right: 450 + (i % 4) * 31, top: 562 - (i >> 2) * 32, bottom: 590 - (i >> 2) * 32 })) } });
test('the realm belt shows the HUD row closed and the same popup rows as a local game when open', () => {
  const world = (record) => ({ beltRecord: () => record });
  const sash = realmFrameReads(null, world(1), belts({ 1: 8 }));
  assert.equal(sash.beltShownfalse.length, 4);
  assert.ok(sash.beltShownfalse.every((b) => b.bottom === 590));
  assert.equal(sash.beltShowntrue.length, 8);
  const girdle = realmFrameReads(null, world(3), belts({ 3: 16 }));
  assert.equal(girdle.beltShowntrue.length, 16);
  const none = realmFrameReads(null, world(2), belts({ 2: 4 }));
  assert.equal(none.beltShowntrue, none.beltShownfalse);
  const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const shown = shell.slice(shell.indexOf('function beltShownBoxes()'), shell.indexOf('function readAdvancement()'));
  assert.ok(shown.includes('if (realmSceneActive) return realmReads'), 'the belt hover reads the realm boxes, not an absent engine session');
});

const base = { tick: 5, act: 1, level: 3, playerId: 7, player: { x: 10, y: 20, hp: 99, hpMax: 99 } };
const actor = (m, f) => ({ code: m.classId, frame: f });
test('the realm view snapshot carries the client state in the single-player shape', () => {
  const snap = realmViewSnapshot({ base, resources: { hp: 50 << 8, hpMax: 100 << 8, mana: 20 << 8, manaMax: 40 << 8 },
    monsters: [{ guid: 9, classId: 4, mode: 1, x: 11, y: 21, direction: 2 }],
    missiles: [{ id: 1, classId: 2, mode: 1, x: 5, y: 6, level: 3, direction: 1, framePrecise: 256, trans: 0 }, { id: 2, level: 4 }],
    level: 3, actor, frame: 8 });
  assert.equal(snap.entities.length, 2);
  assert.deepEqual([snap.entities[0].kind, snap.entities[0].id, snap.entities[0].animation.frame], ['monster', 9, 8]);
  assert.deepEqual([snap.entities[1].kind, snap.entities[1].missile.framePrecise], ['missile', 256]);
  assert.deepEqual([snap.player.hp, snap.player.hpMax, snap.player.mana, snap.player.manaMax], [50 << 8, 100 << 8, 20 << 8, 40 << 8]);
  assert.deepEqual([snap.objects, snap.items, snap.town], [[], [], false]);
  assert.equal(snap.playerId, 7);
});

test('before the first PlayerAssign the realm snapshot is empty rather than missing', () => {
  const snap = realmViewSnapshot({ base: null, resources: null, monsters: [], missiles: [], level: 0, actor, frame: 0 });
  assert.deepEqual([snap.entities, snap.player.hp, snap.player.mana], [[], 0, 0]);
});

test('the realm frame reads feed the stamina bar and keep the engine-only parts empty', () => {
  const r = realmFrameReads({ stamina: 30 << 8, staminaMax: 60 << 8 });
  assert.deepEqual([r.characterScreen[4], r.characterScreen[5]], [30, 60]);
  assert.equal(r.advancement, null);
  assert.equal(realmFrameReads(null).characterScreen, null);
});

const hero = { guid: 7, active: true, clientRoomActive: true, clientRoomLevel: 0, x: 100, y: 200, classId: 1 };
test('the selected player is drawn even when its room level disagrees with the world level', () => {
  assert.deepEqual(realmScenePlayers([hero], hero, 1).map((p) => p.guid), [7]);
  assert.deepEqual(realmScenePlayers([], hero, 1).map((p) => p.guid), [7]);
  const other = { ...hero, guid: 8, clientRoomLevel: 1 };
  assert.deepEqual(realmScenePlayers([hero, other], { ...hero, clientRoomLevel: 1 }, 1).map((p) => p.guid).sort(), [7, 8]);
  assert.deepEqual(realmScenePlayers([hero], { ...hero, x: 0, y: 0 }, 1), []);
});

test('the gate names why a player is not drawn', () => {
  assert.equal(realmPlayerGate(hero), '');
  assert.equal(realmPlayerGate({ ...hero, active: false }), 'inactive');
  assert.equal(realmPlayerGate({ ...hero, clientRoomActive: false }), 'client-room-inactive');
  assert.equal(realmPlayerGate({ ...hero, x: 0, y: 0 }), 'no-position');
});

test('the camera follows the realm player and the view size is the one a local game gets', () => {
  const src = selectPlayerRenderSource({ tick: 0 }, {}, true, hero, 1);
  assert.equal(src.source, 'realm');
  assert.deepEqual([src.x, src.y], [100, 200]);
  const fit = fitSize({ cssW: 1920, cssH: 1080, dpr: 1 });
  assert.deepEqual(fit, { w: 1920, h: 1080, S: 1 });
});

test('the joined game is drawn by the shared drawFrame, with no second drawing path', () => {
  const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  assert.ok(!shell.includes('drawRealmFrame') && !shell.includes('drawRealmHud'));
  assert.ok(shell.includes('if (realmSceneActive) refreshRealmView();'));
  assert.ok(shell.includes('const clientView = () => (realmSceneActive ? realmClientView : spClientView);'));
  assert.ok(shell.includes('if (sceneChrome().pointerLayer)'));
});

test('other players join the entity list with the render state their unit is drawn from', () => {
  const renderState = { playerClass: 2, mode: 2, direction: 5, framePrecise: 512, weaponClass: '1hs', items: [{ code: 1 }] };
  const snap = realmViewSnapshot({ base, resources: null, monsters: [], missiles: [], level: 3, actor,
    frame: 0, players: [{ guid: 77, name: 'Ann', classId: 2, x: 12, y: 22, clientRoomLevel: 3, renderState }] });
  assert.equal(snap.entities.length, 1);
  const e = snap.entities[0];
  assert.deepEqual([e.kind, e.id, e.mode, e.direction, e.x, e.y], ['player', 77, 2, 5, 12, 22]);
  assert.equal(e.playerRender, renderState);
});

// A game joined straight from the title screen never runs the local game's setup, so the joined game must build
// the object and ground item renderers itself: without them every object (the stash first) is silently not drawn.
test('a joined game builds the object and ground item renderers', () => {
  const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const realmChrome = shell.slice(shell.indexOf('function ensureRealmChrome()'), shell.indexOf('async function syncRealmScene()'));
  assert.match(realmChrome, /objectRenderer \?\?= createObjectRenderer\(/);
  assert.match(realmChrome, /itemRenderer \?\?= createItemRenderer\(/);
  assert.match(shell, /tickClientUnits = \(\) => \{[^\n]*realmWorldData\(\)\.tickObjects\(\)/, 'the joined game ticks its objects');
});
