import { createSessionTerrain, parseSessionTiles, readLevelTileFiles, worldToScreen, screenDeltaToWorld } from './assets/session-terrain.mjs';
import { COMMAND, CommandInput, DEFAULT_SESSION_FIXTURES, hudLines, normalizeSnapshot, parseSessionRooms, parseSessionSnapshot, parseSessionUnits, parseSessionWarps, selectPrimaryTarget, viewportToWorld } from './gameplay-model.mjs';
import { createMpqAssetReader } from './assets/mpq-assets.mjs';
import { ACT_TILE_ASSETS, createActTileAtlas, decodeActTileAssets } from './assets/retail-tile-view.mjs';
import { decodeDatPalette, decodeDt1 } from './assets/d2-formats.mjs';
import { createDs1Canvas, loadActPresetComposition } from './assets/ds1-compositor.mjs';
import { createPlayerRenderer, playerClassInfo } from './assets/unit-renderer.mjs';
import { createMonsterRenderer, parseMonsterAnimations } from './assets/monster-renderer.mjs';
import { createObjectRenderer, parseObjectPresentations } from './assets/object-renderer.mjs';
import { createItemRenderer, parseItemPresentations } from './assets/item-renderer.mjs';
import { createStashPanelRenderer, stashGeometry, BIG_BANK_PAGE_1 } from './assets/stash-panel.mjs';
import { createWaypointPanelRenderer } from './assets/waypoint-panel.mjs';
import { createControlPanelRenderer } from './assets/control-panel.mjs';
import { createInventoryPanelRenderer } from './assets/inventory-panel.mjs';
import { createCharacterPanelRenderer } from './assets/character-panel.mjs';
import { createSkillPanelRenderer, parseClassSkills, SESSION_SKILL_BYTES } from './assets/skill-panel.mjs';
import { createBeltRenderer } from './assets/belt-renderer.mjs';
import { createActMusicPlayer } from './assets/act-music.mjs';
import { createTitleScreen } from './assets/frontend.mjs';
import { createCharacterCreate } from './assets/charcreate.mjs';
import { createCharacterSelect } from './assets/charselect.mjs';
import { createSaveStore, CLASS_NAMES } from './assets/save-store.mjs';
import { createNpcPanel } from './assets/npc-panel.mjs';
import { decodeDc6 } from './assets/d2-formats.mjs';

const canvas = document.querySelector('#game');
const hud = document.querySelector('#hud');
const status = document.querySelector('#session-status');
const actSelect = document.querySelector('#act-select');
const classSelect = document.querySelector('#class-select');
const ctx = canvas.getContext('2d', { alpha: false });
const input = new CommandInput();
let snapshot = normalizeSnapshot(null);
let last = performance.now(), accumulator = 0, frames = 0, fps = 0, fpsAt = last;
const STEP = 1000 / 60;
let session = null, bootstrapStarted = false, sessionError = '';
let fixtures = [];
// Debug overlays (grid, room rectangles, warp diamonds) are exactly what made this read as a test
// harness rather than a game. They are opt-in now: ?debug=1.
const DEBUG_OVERLAY = new URLSearchParams(location.search).get('debug') === '1';
let terrain = null, terrainFiles = null, terrainStats = null, terrainError = '', terrainLevel = -1;
const tileViews = new Map();
let tileView = null, tileViewLoading = 0, tileViewError = '';
let playerRenderer = null, playerRenderError = '', motion = {};
let frameError = '', frameErrors = 0;
let npcCount = 0;
let npcPanel = null, npcPanelError = '', npcState = null, npcPanelEvidence = null;
// A long walk in progress: where we are going, what to do on arrival, and how it is going.
let walkGoal = null, walkIdle = 0, walkLast = null, walkEvidence = { active: false, hops: 0, reason: 0 };
let lastPurchase = null, lastSale = null;
let monsterRenderer = null;
let objectRenderer = null;
let itemRenderer = null;
// The engine's own advancement record (D2SessionAdvancement). Cached per frame and dropped
// whenever something might have moved it, so the panel never draws a number the engine has already
// changed -- and never a number this file worked out for itself.
const ADVANCEMENT = Object.freeze({ level: 12, experience: 16, nextExperience: 24,
  statPoints: 28, skillPoints: 32, strength: 36, energy: 40, dexterity: 44, vitality: 48 });
// The player's class skill tree, from the engine's tables. Re-read whenever a point is spent so a
// skill's level is the engine's, never a local increment.
const SKILL_CAP = 128;
function readClassSkills() {
  if (classSkills) return classSkills;
  if (!session || typeof Module === 'undefined') return null;
  // wasm64: a pointer ARGUMENT is BigInt at the ccall boundary, even though a pointer RETURN reads
  // back fine as a number. Passing it as a number throws "Cannot convert N to a BigInt".
  const buf = Number(Module.ccall('malloc', 'bigint', ['number'], [SKILL_CAP * SESSION_SKILL_BYTES]));
  if (!buf) return null;
  try {
    const n = Module.ccall('d2_session_class_skills', 'number', ['bigint', 'number'], [BigInt(buf), SKILL_CAP]);
    return (classSkills = parseClassSkills(
      Module.HEAPU8.subarray(buf, buf + n * SESSION_SKILL_BYTES), n));
  } finally { Module.ccall('free', null, ['bigint'], [BigInt(buf)]); }
}

// The belt's box rectangles, from Belts.txt through retail's own accessor. Read once per session:
// the table does not change, and the record is chosen by the belt the player is wearing (index 2 is
// the four-box row a character has with no belt equipped -- selecting by equipped belt is still to
// do).
const BELT_RECORD_NO_BELT = 2;
function readBeltBoxes() {
  if (beltBoxCache) return beltBoxCache;
  if (!session || typeof Module === 'undefined') return null;
  const cap = 1 + 16 * 4;
  const buf = Number(Module.ccall('malloc', 'bigint', ['number'], [cap * 4]));
  if (!buf) return null;
  try {
    const n = Module.ccall('d2_session_belt_boxes', 'number', ['number', 'number', 'bigint', 'number'],
      [BELT_RECORD_NO_BELT, 1, BigInt(buf), cap]);
    if (!n) return null;
    const v = new DataView(Module.HEAPU8.buffer, 0, Module.HEAPU8.byteLength);
    const boxes = [];
    for (let i = 0; i < n; i++) {
      const b = buf + 4 + i * 16;
      boxes.push({ left: v.getInt32(b, true), right: v.getInt32(b + 4, true),
        top: v.getInt32(b + 8, true), bottom: v.getInt32(b + 12, true) });
    }
    return (beltBoxCache = boxes);
  } finally { Module.ccall('free', null, ['bigint'], [BigInt(buf)]); }
}

function readAdvancement() {
  if (advancement) return advancement;
  if (!session || typeof Module === 'undefined') return null;
  const ptr = Number(Module.ccall('d2_session_advancement', 'number', [], []));
  if (!ptr) return null;
  const v = new DataView(Module.HEAPU8.buffer, 0, Module.HEAPU8.byteLength);
  const out = {};
  for (const [key, offset] of Object.entries(ADVANCEMENT)) out[key] = v.getInt32(ptr + offset, true);
  return (advancement = out);
}

let inventoryPanelRenderer = null, inventoryPanelEvidence = null;
let characterPanelRenderer = null, characterPanelEvidence = null, advancement = null;
let skillPanelRenderer = null, skillPanelEvidence = null, classSkills = null;
// The stash is open because the engine sent 0x77 UPDATEUI_OPENSTASH, and closed when retail's
// TRADEBTN_CLOSESTASH was accepted -- never because of a click alone.
let stashPanelRenderer = null, stashPanelError = '', stashEvidence = null, stashOpen = false, stashLayout = null;
// The waypoint menu is open because the engine sent 0x63; waypointMenu holds that waypoint's unit id.
let waypointPanelRenderer = null, waypointPanelError = '', waypointEvidence = null, waypointMenu = null, lastWaypointTravel = null;
let beltRenderer = null, beltEvidence = null, beltBoxCache = null;
let controlPanelRenderer = null, controlPanelEvidence = null;
let actMusic = null, musicError = '';
let selectedSkill = 0, lastSkillCommand = null;
const PLAY_MODE = new URLSearchParams(location.search).get('play') === '1';
// Retail opens on a title screen, so that is what a plain visit gets. `play=1` keeps booting
// straight into a session: every browser scenario drives the game directly and must not have to
// click through a menu to reach it.
const SCREEN = { TITLE: 'title', SELECT: 'select', CREATE: 'create', GAME: 'game' };
let screen = PLAY_MODE ? SCREEN.GAME : SCREEN.TITLE;
let titleScreen = null, titleError = '', titleTick = 0;
let charCreate = null, charCreateError = '';
let charSelect = null, charSelectError = '', savedCharacters = [];
// The character to load once a session exists. Loading needs a live player (see save-file.md), so
// picking a saved character creates a session of its class and then applies the file.
let pendingLoad = null, lastSaveError = '', savesWritten = 0;
// Tracks applying a picked save. `pending` is true between choosing a character and the file
// actually being applied: bootstrapSession awaits asset loading first, so `session` and `snapshot`
// exist for a short window BEFORE the saved character has been restored. Anything that reads
// character state after picking a save must wait for `pending` to clear, or it will observe the
// freshly created level-1 character instead of the saved one.
let lastLoad = { attempted: false, pending: false, rc: null, bytes: 0 };
// Autosave bookkeeping. Retail writes the character periodically and when you leave a game; a
// browser tab can be closed at any moment, so the same applies here with more force.
let autosaveAt = 0, autosaveAct = 0, autosaveInFlight = false;
const AUTOSAVE_TICKS = 600;   // update() runs at the session's step rate; ~30s of play
const saveStore = createSaveStore();
// The class the player chose on the creation screen. Null until they choose, in which case the
// dev dropdown still decides -- that path is what `play=1` and the browser scenarios use.
let chosenClass = null, chosenName = '';
const TERRAIN_ENABLED = new URLSearchParams(location.search).get('terrain') !== '0';
let inventoryVisible = new URLSearchParams(location.search).get('inventory') === '1';
let characterVisible = new URLSearchParams(location.search).get('character') === '1';
let skillsVisible = new URLSearchParams(location.search).get('skills') === '1';

// Build terrain for the CURRENT level from the engine's own tile list. Keyed by level, not act:
// that distinction is the whole bug -- every level in an act used to draw the same mocked ground.
function loadSessionTerrain(act0) {
  terrain = null; terrainFiles = null; terrainStats = null; terrainError = ''; terrainLevel = -1;
  try {
    terrainFiles = readLevelTileFiles(Module);
    if (!terrainFiles.some(Boolean)) { terrainError = 'engine reported no tile libraries for this level'; terrainFiles = null; return; }
    terrain = createSessionTerrain({
      read: createMpqAssetReader(Module), decodeDt1, decodeDatPalette,
      palettePath: ACT_TILE_ASSETS[act0]?.palette || ACT_TILE_ASSETS[0].palette,
    });
  } catch (error) { terrainError = String(error); }
}

async function loadActTileView(act0) {
  const request = ++tileViewLoading;
  tileView = null; tileViewError = '';
  try {
    let view = tileViews.get(act0);
    if (!view) {
      await new Promise((resolve) => setTimeout(resolve, 0));
      const read = createMpqAssetReader(Module);
      const decoded = decodeActTileAssets(read, act0);
      view = { ...decoded, atlas: createActTileAtlas(decoded) };
      try {
        const composition = loadActPresetComposition(read, act0);
        view.composition = composition;
        view.mapCanvas = createDs1Canvas(composition);
      } catch (error) { view.compositionError = String(error); }
      tileViews.set(act0, view);
    }
    if (request === tileViewLoading) tileView = view;
  } catch (error) {
    if (request === tileViewLoading) { tileView = null; tileViewError = String(error); }
  }
}

function sessionAdapter() {
  if (window.Module?._d2_session_create && window.Module?._d2_session_snapshot) {
    return {
      create(f, playerClass = 0) { return Module.ccall('d2_session_create_class', 'number', ['number', 'number', 'number', 'number', 'number'], [f.seed, f.difficulty, f.act0, f.level, playerClass]); },
      move(x, y) { return Module.ccall('d2_session_command_move', 'number', ['number', 'number'], [x, y]); },
      attack(targetId) { return Module.ccall('d2_session_command_primary_attack', 'number', ['number'], [targetId]); },
      throwSkill(targetId) { return Module.ccall('d2_session_command_throw', 'number', ['number'], [targetId]); },
      kick(targetId) { return Module.ccall('d2_session_command_kick', 'number', ['number'], [targetId]); },
      pickup(itemId) { return Module.ccall('d2_session_command_pickup', 'number', ['number'], [itemId]); },
      drop(itemId) { return Module.ccall('d2_session_command_drop', 'number', ['number'], [itemId]); },
      useBelt(slot) { return Module.ccall('d2_session_command_use_belt', 'number', ['number'], [slot]); },
      operate(objectId) { return Module.ccall('d2_session_command_object', 'number', ['number'], [objectId]); },
      talk(npcId) { return Module.ccall('d2_session_command_npc', 'number', ['number'], [npcId]); },
      talkNearest() { return Module.ccall('d2_session_command_nearest_npc', 'number', ['number'], [6]); },
      travelNearest() { const npc = Module._d2_session_nearest_travel_npc ? Module.ccall('d2_session_nearest_travel_npc', 'number', ['number'], [6]) : 0; return npc ? Module.ccall('d2_session_command_travel', 'number', ['number'], [npc]) : 0; },
      interact() { return Module.ccall('d2_session_command_nearest_warp', 'number', ['number'], [6]); },
      respawn() { return Module.ccall('d2_session_command_respawn', 'number', [], []); },
      tick(n) { return Module.ccall('d2_session_tick', 'number', ['number'], [n]); },
      motion() {
        if (!Module._d2_session_motion) return {};
        const p = Number(Module.ccall('d2_session_motion', 'number', [], []));
        if (!p) return {};
        const view = new DataView(Module.HEAPU8.buffer, p, 52);
        if (view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 52) return {};
        return { framePrecise: view.getUint32(40, true), frameCountPrecise: view.getUint32(44, true),
          direction: view.getUint8(50), moving: view.getUint8(51) !== 0 };
      },
      snapshot() {
        const p = Number(Module.ccall('d2_session_snapshot', 'number', [], []));
        if (!p) throw new Error('d2_session_snapshot returned null');
        const base = parseSessionSnapshot(Module.HEAPU8.subarray(p, p + 68));
        if (Module._d2_session_resources) {
          const rp = Number(Module.ccall('d2_session_resources', 'number', [], [])), rv = new DataView(Module.HEAPU8.buffer, rp, 24);
          if (rv.getUint32(0, true) === 1 && rv.getUint32(4, true) === 24) {
            base.player.hp = rv.getInt32(8, true); base.player.hpMax = rv.getInt32(12, true);
            base.player.mana = rv.getInt32(16, true); base.player.manaMax = rv.getInt32(20, true);
          }
        }
        if (!Module._d2_session_rooms || !Module._d2_session_units) return base;
        const roomCount = Module.ccall('d2_session_room_count', 'number', [], []);
        const roomPtr = Number(Module.ccall('d2_session_rooms', 'number', [], []));
        const unitCount = Module.ccall('d2_session_unit_count', 'number', [], []);
        const unitPtr = Number(Module.ccall('d2_session_units', 'number', [], []));
        const entities = parseSessionUnits(Module.HEAPU8.subarray(unitPtr, unitPtr + unitCount * 44), unitCount);
        if (Module._d2_session_monster_animations) {
          const animationCount = Module.ccall('d2_session_monster_animation_count', 'number', [], []);
          const animationPtr = Number(Module.ccall('d2_session_monster_animations', 'number', [], []));
          const byId = new Map(parseMonsterAnimations(Module.HEAPU8.subarray(animationPtr, animationPtr + animationCount * 92), animationCount).map((a) => [a.id, a]));
          for (const entity of entities) if (entity.type === 1) entity.animation = byId.get(entity.id);
        }
        let objects = [];
        if (Module._d2_session_object_presentations) {
          const count = Module.ccall('d2_session_object_presentation_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_object_presentations', 'number', [], []));
          objects = parseObjectPresentations(Module.HEAPU8.subarray(ptr, ptr + count * 40), count);
        }
        let items = [];
        if (Module._d2_session_item_presentations) {
          const count = Module.ccall('d2_session_item_presentation_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_item_presentations', 'number', [], []));
          items = parseItemPresentations(Module.HEAPU8.subarray(ptr, ptr + count * 112), count);
        }
        let warps = [];
        if (Module._d2_session_warps) {
          const count = Module.ccall('d2_session_warp_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_warps', 'number', [], []));
          warps = parseSessionWarps(Module.HEAPU8.subarray(ptr, ptr + count * 68), count);
        }
        return normalizeSnapshot({ ...base,
          rooms: parseSessionRooms(Module.HEAPU8.subarray(roomPtr, roomPtr + roomCount * 36), roomCount),
          entities, objects, items, warps,
        });
      },
      destroy() { Module.ccall('d2_session_destroy', null, [], []); },
    };
  }
  return null;
}

async function bootstrapSession() {
  bootstrapStarted = true;
  try {
    fixtures = DEFAULT_SESSION_FIXTURES;
    if (!fixtures.length) throw new Error('monspawn vectors contain no playable fixtures');
    actSelect.replaceChildren(...fixtures.map((fixture, index) => Object.assign(document.createElement('option'), {
      value: String(index), textContent: fixture.label || `Act ${fixture.act0 + 1} · Level ${fixture.level}`,
    })));
    actSelect.disabled = false;
    classSelect.replaceChildren(...Array.from({ length: 7 }, (_, playerClass) => Object.assign(document.createElement('option'), {
      value: String(playerClass), textContent: playerClassInfo(playerClass).name,
    })));
    classSelect.disabled = false;
    // A class picked on the creation screen wins over the dev dropdown; keep the dropdown in sync
    // so the two controls cannot disagree about what is on screen.
    if (chosenClass != null) classSelect.value = String(chosenClass);
    // The name has to reach the engine BEFORE the session is created: the save header reads the
    // character's identity from the client record, which is built during create.
    if (chosenName) Module.ccall('d2_session_set_character_name', 'number', ['string'], [chosenName]);
    const fixture = fixtures[0];
    const api = sessionAdapter();
    if (!api?.create(fixture, Number(classSelect.value))) throw new Error('d2_session_create failed');
    session = api; snapshot = api.snapshot();
    motion = api.motion();
    const assetReader = createMpqAssetReader(Module);
    playerRenderer = createPlayerRenderer(assetReader); playerRenderError = '';
    monsterRenderer = createMonsterRenderer(createMpqAssetReader(Module));
    objectRenderer = createObjectRenderer(createMpqAssetReader(Module));
    itemRenderer = createItemRenderer(createMpqAssetReader(Module));
    inventoryPanelRenderer = createInventoryPanelRenderer(createMpqAssetReader(Module));
    characterPanelRenderer = createCharacterPanelRenderer(createMpqAssetReader(Module));
    skillPanelRenderer = createSkillPanelRenderer(createMpqAssetReader(Module));
    try { stashPanelRenderer = createStashPanelRenderer(createMpqAssetReader(Module)); } catch (error) { stashPanelError = String(error); }
    try { waypointPanelRenderer = createWaypointPanelRenderer(createMpqAssetReader(Module)); } catch (error) { waypointPanelError = String(error); }
    beltRenderer = createBeltRenderer(createMpqAssetReader(Module));
    controlPanelRenderer = createControlPanelRenderer(assetReader);
    try { actMusic = createActMusicPlayer(assetReader); await actMusic.prepare(fixture.act0); musicError = ''; } catch (error) { musicError = String(error); }

    if (TERRAIN_ENABLED) { loadSessionTerrain(fixture.act0); loadActTileView(fixture.act0); }
    // A character picked from the list is applied to the player the session just created.
    if (pendingLoad) {
      const p = Module._malloc(pendingLoad.length);
      try {
        Module.HEAPU8.set(pendingLoad, Number(p));
        const rc = Module.ccall('d2_session_load_d2s', 'number', ['bigint', 'number'], [BigInt(p), pendingLoad.length]);
        lastLoad = { attempted: true, pending: false, rc, bytes: pendingLoad.length };
        if (rc !== 0) sessionError = `loading ${chosenName} failed with PLRSAVE2 error ${rc}`;
        else { snapshot = session.snapshot(); motion = session.motion(); }
      } finally { Module._free(p); pendingLoad = null; }
    }
    window.__d2.session = { fixture, snapshot: () => session.snapshot(), move: (x, y) => session.move(x, y), attack: (targetId) => session.attack(targetId), throwSkill: (targetId) => session.throwSkill(targetId), kick: (targetId) => session.kick(targetId), pickup: (itemId) => session.pickup(itemId), drop: (itemId) => session.drop(itemId), useBelt: (slot) => session.useBelt(slot), operate: (objectId) => session.operate(objectId), talk: (npcId) => session.talk(npcId), travelNearest: () => session.travelNearest(), respawn: () => session.respawn(),
      interact: () => { const previousAct = snapshot.act, accepted = session.interact(); if (accepted) { snapshot = session.snapshot(); motion = session.motion(); if (TERRAIN_ENABLED && snapshot.act !== previousAct) { loadSessionTerrain(snapshot.act - 1); loadActTileView(snapshot.act - 1); } } return accepted; },
      tick: (n = 1) => session.tick(n) };
    // A newly created character is written straight away, so it is in the list the moment it
    // exists rather than only after some later autosave the player never triggers.
    autosaveAt = 0; autosaveAct = snapshot.act; autosaveInFlight = false;
    if (chosenName) persistCurrentCharacter();
  } catch (error) { sessionError = String(error?.stack || error); }
}

actSelect.addEventListener('change', () => {
  if (!session) return;
  const fixture = fixtures[Number(actSelect.value)];
  if (!fixture) return;
  session.destroy();
  if (!session.create(fixture, Number(classSelect.value))) { sessionError = `unable to create Act ${fixture.act0 + 1} session`; session = null; return; }
  snapshot = session.snapshot();
  motion = session.motion();
  actMusic?.prepare(fixture.act0).catch((error) => { musicError = String(error); });
  if (TERRAIN_ENABLED) { loadSessionTerrain(fixture.act0); loadActTileView(fixture.act0); }
});

classSelect.addEventListener('change', () => {
  if (!session) return;
  const fixture = fixtures[Number(actSelect.value)];
  if (!fixture) return;
  session.destroy();
  if (!session.create(fixture, Number(classSelect.value))) { sessionError = 'unable to create character session'; session = null; return; }
  snapshot = session.snapshot(); motion = session.motion(); playerRenderError = '';
});

// The title screen needs the retail MPQs, so it cannot be built until the engine is up.
function ensureTitleScreen() {
  if (screen !== SCREEN.TITLE || titleScreen || titleError) return;
  if (!window.__d2?.ready || window.__d2.error) return;
  try {
    titleScreen = createTitleScreen({
      read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      items: [
        { label: 'SINGLE PLAYER', action: () => { enterCharacterSelect(); } },
        // TODO(client-parity C2/C3): retail's five entries, in retail's order. Only SINGLE PLAYER
        // is wired, and it skips character select because there are no save files yet.
        { label: 'BATTLE.NET', action: () => {} },
        { label: 'OTHER MULTIPLAYER', action: () => {} },
        { label: 'CREDITS', action: () => {} },
        { label: 'EXIT DIABLO II', action: () => {} },
      ],
    });
  } catch (error) { titleError = String(error); }
}

function startGame() {
  if (!bootstrapStarted && window.__d2?.ready && !window.__d2.error && sessionAdapter()) bootstrapSession();
}

// SINGLE PLAYER goes to the saved-character list; with nothing saved there is nothing to choose
// from, so it goes straight to creation, which is what retail does with an empty character list.
async function enterCharacterSelect() {
  charSelect = null; charSelectError = '';
  try { savedCharacters = await saveStore.list(); }
  catch (error) { charSelectError = String(error); savedCharacters = []; }
  if (!savedCharacters.length) { screen = SCREEN.CREATE; ensureCharCreate(); return; }
  screen = SCREEN.SELECT;
  ensureCharSelect();
}

function ensureCharSelect() {
  if (screen !== SCREEN.SELECT || charSelect) return;
  if (!window.__d2?.ready || window.__d2.error) return;
  try {
    charSelect = createCharacterSelect({
      read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      characters: savedCharacters,
      onPlay: (name) => { playSavedCharacter(name); },
      onCreate: () => { screen = SCREEN.CREATE; ensureCharCreate(); },
      onCancel: () => { screen = SCREEN.TITLE; },
    });
  } catch (error) { charSelectError = String(error); }
}

// Write the live character to the store. Internal twin of __d2Shell.persistCharacter, so the
// boot path and the scenario surface cannot drift apart.
async function persistCurrentCharacter() {
  if (!session) return null;
  const cap = 16384, p = Module._malloc(cap);
  let bytes = null;
  try {
    const n = Module.ccall('d2_session_save_d2s', 'number', ['bigint', 'number'], [BigInt(p), cap]);
    if (n <= 0) { lastSaveError = `save failed (${Module.ccall('d2_session_save_d2s_error', 'number', [], [])})`; return null; }
    bytes = Module.HEAPU8.slice(Number(p), Number(p) + n);
  } finally { Module._free(p); }
  try { const header = await saveStore.put(bytes); savesWritten++; lastSaveError = ''; return header; }
  catch (error) { lastSaveError = String(error); return null; }
}

// Picking a saved character: create a session of its class, then apply the file to that player.
// The engine's loader needs a live player (it is the half of PLRSAVE2_ProcessSaveFile that applies
// sections; the half that CREATES a unit from the header is not ported -- client-parity C3e).
async function playSavedCharacter(name) {
  let bytes = null;
  try { bytes = await saveStore.get(name); }
  catch (error) { charSelectError = String(error); return; }
  if (!bytes) { charSelectError = `no saved character named ${name}`; return; }
  const header = savedCharacters.find((ch) => ch.name === name);
  chosenClass = header ? header.playerClass : 0;
  chosenName = name;
  pendingLoad = bytes;
  lastLoad = { attempted: false, pending: true, rc: null, bytes: bytes.length };
  screen = SCREEN.GAME;
  startGame();
}

// Retail's character creation sits between SINGLE PLAYER and the game. Saved characters
// (client-parity C3) would be selected before this; there is no save file support yet, so every
// game starts by creating one.
function ensureCharCreate() {
  if (screen !== SCREEN.CREATE || charCreate || charCreateError) return;
  if (!window.__d2?.ready || window.__d2.error) return;
  try {
    charCreate = createCharacterCreate({
      read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      onCreate: ({ playerClass, name }) => {
        chosenClass = playerClass; chosenName = name;
        screen = SCREEN.GAME;
        startGame();
      },
      onCancel: () => { screen = SCREEN.TITLE; },
    });
  } catch (error) { charCreateError = String(error); }
}

// What each unit is, from the engine: bit 0 talkable NPC, bit 1 hostile (MonStats alignment).
// Town NPCs are UNIT_MONSTER like everything else, so without this the client paints Charsi -- and
// the Rogue guards -- as enemies.
// Read the live NPC conversation out of the engine: who we are talking to, what they said, and
// their stock if a trade is open. All of it is engine state; the panel only draws it.
function readNpcState() {
  try {
    const dialogPtr = Number(Module.ccall('d2_session_npc_dialog', 'number', [], []));
    if (!dialogPtr) return null;
    const view = new DataView(Module.HEAPU8.buffer, dialogPtr, 64);
    const npcId = view.getUint32(16, true);
    if (!npcId) return null;
    const messageCount = view.getUint32(32, true);
    const greeting = messageCount
      ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [view.getUint16(36, true)])
      : '';
    const items = [];
    const count = Module.ccall('d2_session_vendor_item_count', 'number', [], []);
    if (count > 0) {
      const base = Number(Module.ccall('d2_session_vendor_items', 'number', [], []));
      // The stock's PRESENTATION records carry each item's retail inventory sprite, keyed by id.
      // Same 112-byte format as the player's own items, so one reader serves both.
      const sprites = new Map();
      const pCount = Module.ccall('d2_session_vendor_presentation_count', 'number', [], []);
      if (pCount > 0) {
        const pBase = Number(Module.ccall('d2_session_vendor_presentations', 'number', [], []));
        for (let i = 0; i < pCount; i++) {
          const o = pBase + i * 112, pv = new DataView(Module.HEAPU8.buffer, o, 112);
          let invFile = '';
          for (let k = 0; k < 32; k++) {
            const ch = Module.HEAPU8[o + 72 + k];
            if (!ch) break;
            invFile += String.fromCharCode(ch);
          }
          sprites.set(pv.getUint32(0, true), { invFile,
            gridX: pv.getInt16(104, true), gridY: pv.getInt16(106, true), invPage: Module.HEAPU8[o + 108] });
        }
      }
      for (let i = 0; i < count; i++) {
        const b = base + i * 32, row = new DataView(Module.HEAPU8.buffer, b, 32);
        const code = row.getUint32(8, true);
        const codeText = String.fromCharCode(code & 255, (code >> 8) & 255, (code >> 16) & 255, (code >> 24) & 255)
          .replace(/\0/g, '').trim();
        const id = row.getUint32(0, true);
        const art = sprites.get(id) || {};
        items.push({ id, code: codeText, cost: row.getUint32(20, true),
          invFile: art.invFile || '', gridX: art.gridX ?? -1, gridY: art.gridY ?? -1,
          invPage: art.invPage ?? 255,
          name: Module.ccall('d2_lang_string', 'string', ['string'], [codeText]) || codeText });
      }
    }
    const gold = Module.ccall('d2_session_player_gold', 'number', [], []);
    return { npcId, greeting, items, gold };
  } catch { return null; }
}

// Buy a vendor item. The engine owns the transaction: it recomputes the price, moves the gold and
// the item, and refuses if the player cannot afford it -- this only names the item that was clicked.
function buyFromVendor(itemId) {
  if (!npcState) return false;
  try {
    const ok = Module.ccall('d2_session_command_vendor_buy', 'number', ['number', 'number'],
      [npcState.npcId, itemId]);
    lastPurchase = { itemId, ok: Boolean(ok), goldBefore: npcState.gold };
    // Re-read: the stock, the gold and the inventory all moved.
    npcState = readNpcState();
    lastPurchase.goldAfter = npcState ? npcState.gold : -1;
    snapshot = session.snapshot();
    return Boolean(ok);
  } catch (error) { lastPurchase = { itemId, ok: false, error: String(error) }; return false; }
}

// Sell an item to the open vendor. Same division as buying: the engine prices it, moves the gold
// and takes the item; this only names what was clicked.
function sellToVendor(itemId) {
  if (!npcState) return false;
  try {
    const ok = Module.ccall('d2_session_command_vendor_sell', 'number', ['number', 'number'],
      [npcState.npcId, itemId]);
    lastSale = { itemId, ok: Boolean(ok), goldBefore: npcState.gold };
    npcState = readNpcState();
    lastSale.goldAfter = npcState ? npcState.gold : -1;
    snapshot = session.snapshot();
    return Boolean(ok);
  } catch (error) { lastSale = { itemId, ok: false, error: String(error) }; return false; }
}

// Repair ONE item. d2_session_command_vendor_repair resolves the id to a real item unit and refuses
// anything it cannot find, so there is no repair-all sentinel to pass -- an earlier version of this
// defaulted the argument to -1 with a comment claiming retail did that, which was unfounded.
function repairAtVendor(itemId) {
  if (!npcState || !(itemId >= 0)) return false;
  try {
    const ok = Module.ccall('d2_session_command_vendor_repair', 'number', ['number', 'number'],
      [npcState.npcId, itemId >>> 0]);
    npcState = readNpcState();
    snapshot = session.snapshot();
    return Boolean(ok);
  } catch { return false; }
}

function ensureNpcPanel() {
  if (npcPanel || npcPanelError) return npcPanel;
  try { npcPanel = createNpcPanel({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette }); }
  catch (error) { npcPanelError = String(error); }
  return npcPanel;
}

// Walk to a distant point, around whatever is in the way.
//
// Retail's walk command only accepts a target within 50 subtiles, and its pathfinder then routes
// around obstacles for that leg. A long trip is therefore several legs -- but picking those legs in
// a straight line walks into pockets the router cannot escape (it answers MOVE_NO_PATH). So the
// legs are chosen by ASKING THE ENGINE: a fan of candidate waypoints is filtered to standable
// ground, ordered by how much closer it gets, and offered to the walk command until one is
// accepted. The engine stays the authority on what is reachable; this only decides where to aim.
const WALK_FAN = 16, WALK_RANGES = [45, 30, 18, 10];

function chebyshev(ax, ay, bx, by) { return Math.max(Math.abs(ax - bx), Math.abs(ay - by)); }

function walkStep() {
  if (!walkGoal || !session) return;
  const p = snapshot.player;
  const dist = chebyshev(p.x, p.y, walkGoal.x, walkGoal.y);
  if (dist <= (walkGoal.within ?? 4)) { const done = walkGoal.onArrive; walkGoal = null; walkEvidence.active = false; done?.(); return; }

  // Only pick a new leg once the last one has finished moving us.
  if (walkLast && walkLast.x === p.x && walkLast.y === p.y) walkIdle++; else walkIdle = 0;
  walkLast = { x: p.x, y: p.y };
  if (walkIdle < 2 && walkEvidence.hops) return;

  const candidates = [];
  for (let a = 0; a < WALK_FAN; a++) {
    const angle = (a / WALK_FAN) * Math.PI * 2;
    for (const range of WALK_RANGES) {
      const r = Math.min(range, Math.max(4, dist));
      const tx = Math.round(p.x + Math.cos(angle) * r), ty = Math.round(p.y + Math.sin(angle) * r);
      if (!Module.ccall('d2_session_walkable', 'number', ['number', 'number'], [tx, ty])) continue;
      const score = chebyshev(tx, ty, walkGoal.x, walkGoal.y);
      if (score < dist) candidates.push({ tx, ty, score });
    }
  }
  candidates.sort((u, v) => u.score - v.score);
  for (const candidate of candidates.slice(0, 12)) {
    if (session.move(candidate.tx, candidate.ty)) {
      walkEvidence = { active: true, hops: walkEvidence.hops + 1, reason: 0 };
      walkIdle = 0;
      return;
    }
  }
  // Nothing reachable makes progress: stop rather than spin.
  walkEvidence = { active: false, hops: walkEvidence.hops, reason: Module.ccall('d2_session_move_reason', 'number', [], []) };
  walkGoal = null;
}

function readUnitKinds(count) {
  if (!count) return null;
  const p = Module._malloc(count);
  try {
    const n = Module.ccall('d2_session_unit_kinds', 'number', ['bigint', 'number'], [BigInt(p), count]);
    return n > 0 ? Module.HEAPU8.slice(Number(p), Number(p) + n) : null;
  } catch { return null; } finally { Module._free(p); }
}

function update() {
  // The engine advances every tick, so last tick's advancement record is stale by definition.
  advancement = null; classSkills = null;
  if (screen === SCREEN.TITLE) { ensureTitleScreen(); titleTick++; return; }
  if (screen === SCREEN.SELECT) { ensureCharSelect(); return; }
  if (screen === SCREEN.CREATE) { ensureCharCreate(); charCreate?.step(); return; }
  if (!bootstrapStarted && PLAY_MODE && /[?&]retail=1/.test(location.search) && window.__d2?.ready && !window.__d2.error && sessionAdapter()) bootstrapSession();
  const commands = input.drain();
  if (session) {
    for (const command of commands) {
      if (command.type === COMMAND.MOVE) {
        if (command.directional) session.move(snapshot.player.x + command.x, snapshot.player.y + command.y);
        else {
          // A ground click. A new click replaces any walk in progress, as in retail. Retail's walk
          // command only routes a short leg (MOVE_NO_PATH beyond it -- s000 stalled in town on
          // exactly that); a far click is routed in legs by the client, so a refused click hands
          // the SAME goal to the hop walker the NPC walk already uses.
          const d = screenDeltaToWorld(command.x - canvas.width / 2, command.y - canvas.height / 2);
          const tx = Math.round(snapshot.player.x + d.x), ty = Math.round(snapshot.player.y + d.y);
          walkGoal = null;
          // Retail's LEFT click acts on what is under the cursor before it walks: an item is picked
          // up, a hostile is attacked with the left-hand skill (normal attack). Only bare ground
          // walks. (This shell sent every left click as a walk, so a monster could only be fought
          // with the right button -- s000 clicked one 400 times and never swung.)
          const underItem = snapshot.items.filter((item) => item.mode === 3 && item.invPage === 255)
            .map((item) => ({ item, d: (item.x - tx) ** 2 + (item.y - ty) ** 2 })).filter((entry) => entry.d <= 4)
            .sort((a, b) => a.d - b.d || a.item.id - b.item.id)[0]?.item;
          if (underItem) { session.pickup(underItem.id); continue; }
          const underTarget = selectPrimaryTarget(snapshot, { x: tx, y: ty }, 2);
          if (underTarget) {
            const result = session.attack(underTarget);
            lastSkillCommand = { skill: 0, targetId: underTarget, result };
            // A monster out of reach (rooms away, or no path for the walk-into-range leg): retail
            // walks the player toward it and attacks on arrival, as clicking an NPC walks and talks.
            if (!result) {
              const m = snapshot.entities.find((e) => e.id === underTarget);
              if (m) { walkEvidence = { active: true, hops: 0, reason: 0 };
                walkGoal = { x: m.x, y: m.y, within: 2, onArrive: () => { lastSkillCommand = { skill: 0, targetId: underTarget, result: session.attack(underTarget) }; } }; }
            }
            continue;
          }
          if (!session.move(tx, ty)) { walkEvidence = { active: true, hops: 0, reason: 0 }; walkGoal = { x: tx, y: ty, within: 2 }; }
        }
      } else if (command.type === COMMAND.PRIMARY) {
        const point = command.x == null ? snapshot.player : (() => { const d = screenDeltaToWorld(command.x - canvas.width / 2, command.y - canvas.height / 2); return { x: Math.round(snapshot.player.x + d.x), y: Math.round(snapshot.player.y + d.y) }; })();
        const ground = command.x == null ? null : snapshot.items.filter((item) => item.mode === 3 && item.invPage === 255)
          .map((item) => ({ item, d: (item.x - point.x) ** 2 + (item.y - point.y) ** 2 })).filter((entry) => entry.d <= 4)
          .sort((a, b) => a.d - b.d || a.item.id - b.item.id)[0]?.item;
        if (ground) { session.pickup(ground.id); continue; }
        const targetId = selectPrimaryTarget(snapshot, point, command.x == null ? Infinity : 2);
        const skill = command.secondary ? selectedSkill : 0;
        const result = targetId ? (skill === 2 ? session.throwSkill(targetId) : skill === 1 ? session.kick(targetId) : session.attack(targetId)) : 0;
        lastSkillCommand = { skill, targetId, result };
      } else if (command.type === COMMAND.BELT) {
        // Retail's belt keys: 1-4 drink/use the bottom item of that belt column, through the engine.
        if (session.useBelt(command.slot)) snapshot = session.snapshot();
      } else if (command.type === COMMAND.SKILL) {
        const skills = [0, 2, 1]; if (command.slot >= 1 && command.slot <= skills.length) selectedSkill = skills[command.slot - 1];
      } else if (command.type === COMMAND.CANCEL) {
        selectedSkill = 0;
      } else if (command.type === COMMAND.INTERACT) {
        const previousAct = snapshot.act;
        let accepted = session.talkNearest();
        let nearest = null, distance = 7;
        for (const object of snapshot.objects) { const candidate = Math.max(Math.abs(object.x - snapshot.player.x), Math.abs(object.y - snapshot.player.y));
          if (candidate < distance || candidate === distance && (!nearest || object.id < nearest.id)) { nearest = object; distance = candidate; } }
        if (!accepted) accepted = nearest ? session.operate(nearest.id) : 0;
        if (!accepted) accepted = session.interact();
        if (accepted) {
          snapshot = session.snapshot(); motion = session.motion();
          if (TERRAIN_ENABLED && snapshot.act !== previousAct) { loadSessionTerrain(snapshot.act - 1); loadActTileView(snapshot.act - 1); }
        }
      } else if (command.type === COMMAND.TRAVEL) {
        // The caravan: Warriv, Meshif, Tyrael (and Cain's portal) carry the player into the next
        // act's town; the recovered server handler needs that act's quest reward.
        const previousAct = snapshot.act;
        if (session.travelNearest()) {
          snapshot = session.snapshot(); motion = session.motion();
          if (TERRAIN_ENABLED && snapshot.act !== previousAct) { loadSessionTerrain(snapshot.act - 1); loadActTileView(snapshot.act - 1); }
        }
      } else if (command.type === COMMAND.RESPAWN) {
        if (snapshot.mode === 17 && session.respawn()) {
          snapshot = session.snapshot(); motion = session.motion();
        }
      }
    }
    try {
      session.tick(1); snapshot = session.snapshot(); motion = session.motion();
      // Hostility is the monster's alignment, not "is a monster"; talkable is its INTERACT flag.
      walkStep();
      npcState = readNpcState();
      const kinds = readUnitKinds(snapshot.entities.length);
      npcCount = 0;
      if (kinds) for (let i = 0; i < snapshot.entities.length && i < kinds.length; i++) {
        const entity = snapshot.entities[i], kind = kinds[i];
        entity.npc = (kind & 1) !== 0;
        entity.hostile = (kind & 2) !== 0;
        if (entity.npc) npcCount++;
      }
    }
    catch (error) { sessionError = String(error); session = null; }
  }
  // Write the character periodically, and immediately on an act change -- the two moments retail
  // treats as worth persisting. Guarded against overlap: the store is async and a second save
  // starting before the first finished would race on the same key.
  if (session && chosenName && !autosaveInFlight) {
    autosaveAt++;
    const actChanged = snapshot.act !== autosaveAct;
    if (actChanged || autosaveAt >= AUTOSAVE_TICKS) {
      autosaveAt = 0; autosaveAct = snapshot.act; autosaveInFlight = true;
      persistCurrentCharacter().finally(() => { autosaveInFlight = false; });
    }
  }
  status.title = [tileViewError, musicError].filter(Boolean).join('\n');
  status.textContent = session
    ? `retail-backed simulation connected · ${tileView ? `retail tiles: Act ${tileView.act0 + 1}` : tileViewError ? 'retail tile fallback' : 'loading retail tiles…'} · ${musicError ? 'audio unavailable' : 'retail music ready'}`
    : sessionError || (PLAY_MODE ? 'engine loaded; creating simulation…' : 'open ?retail=1&play=1 to create a gameplay session');
}

// The player always sits at the centre of the viewport; it is a sprite like any other in the depth
// sort, which is why it lives here rather than inline at the end of render().
function drawPlayer(c) {
  const w = canvas.width, h = canvas.height;
  if (playerRenderer) {
    try { playerRenderer.draw(c, w / 2, h / 2, { ...motion, playerClass: snapshot.playerClass, mode: snapshot.mode, tick: snapshot.tick, act: snapshot.act }); playerRenderError = ''; }
    catch (error) { playerRenderError = String(error); playerRenderer = null; }
  }
  if (!playerRenderer) {
    c.fillStyle = '#d9b85c'; c.beginPath(); c.arc(w / 2, h / 2, 7, 0, Math.PI * 2); c.fill();
  }
}

// The level's DT1 libraries come from LvlTypes.txt via the level's TYPE, so terrain must be rebuilt
// on every LEVEL change, not every act change. Keying it on the act was a bug twice over: entering
// the catacombs kept the town's tile set, and on first load the engine has not resolved a level
// yet, so the file list came back empty and the map never appeared at all. Doing it here means any
// route into a level -- fixture, waypoint, warp, act travel -- is covered by one rule.
function ensureTerrain() {
  if (!TERRAIN_ENABLED || !session) return;
  if (terrain && terrainLevel === snapshot.level) return;
  if (terrainLevel === snapshot.level && terrainError) return;   // failed for this level; do not spin
  loadSessionTerrain(snapshot.act - 1);
  terrainLevel = snapshot.level;
}

function render() {
  const w = canvas.width, h = canvas.height;
  if (screen === SCREEN.TITLE) {
    ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
    if (titleScreen) titleScreen.draw(ctx, w, h, titleTick);
    else {
      ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '16px Georgia,serif';
      ctx.fillText(titleError || 'loading…', w / 2, h / 2);
    }
    hud.textContent = titleError ? `title screen: ${titleError}` : '';
    return;
  }
  if (screen === SCREEN.SELECT) {
    ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
    if (charSelect) charSelect.draw(ctx, w, h);
    else {
      ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '16px Georgia,serif';
      ctx.fillText(charSelectError || 'loading…', w / 2, h / 2);
    }
    hud.textContent = charSelectError ? `character select: ${charSelectError}` : '';
    return;
  }
  if (screen === SCREEN.CREATE) {
    ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
    if (charCreate) charCreate.draw(ctx, w, h);
    else {
      ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '16px Georgia,serif';
      ctx.fillText(charCreateError || 'loading…', w / 2, h / 2);
    }
    hud.textContent = charCreateError ? `character creation: ${charCreateError}` : '';
    return;
  }
  ensureTerrain();
  ctx.fillStyle = '#080a08'; ctx.fillRect(0, 0, w, h);
  // The real map: one draw per placed tile, floors then walls, positioned by the engine's own
  // isometric screen coordinates. Falls back to the old act atlas only if terrain failed to load.
  // Diablo II is isometric. Everything world-anchored projects through the SAME transform the
  // engine used for its tiles, so sprites land on the ground they are standing on rather than on a
  // parallel flat grid.
  const camera = worldToScreen(snapshot.player.x, snapshot.player.y);
  const proj = (wx, wy) => { const p = worldToScreen(wx, wy); return [p.x - camera.x + w / 2, p.y - camera.y + h / 2]; };

  // Sprites are handed to the terrain pass rather than painted after it, so a wall in front of a
  // monster covers the monster. `depth` is the projected screen Y -- the same key the walls sort on.
  const sprites = [];
  for (const entity of snapshot.entities) {
    const [sx, sy] = proj(entity.x, entity.y);
    sprites.push({ depth: sy, draw(c) {
      // Draw the retail sprite for ANY unit that has an animation. This used to require
      // entity.hostile, which tied "has a body" to "is an enemy" -- so once town NPCs were
      // correctly marked non-hostile they stopped being drawn and Charsi became a dot.
      if (entity.animation && monsterRenderer?.draw(c, sx, sy, entity.animation, snapshot.act)) return;
      c.fillStyle = entity.hostile ? '#b33b2e' : '#958968';
      c.beginPath(); c.arc(sx, sy, 5, 0, Math.PI * 2); c.fill();
    } });
  }
  for (const object of snapshot.objects) {
    const [sx, sy] = proj(object.x, object.y);
    sprites.push({ depth: sy, draw(c) { objectRenderer?.draw(c, sx, sy, object, snapshot.act); } });
  }
  for (const item of snapshot.items) if (item.mode === 3 && item.invPage === 255) {
    const [sx, sy] = proj(item.x, item.y);
    sprites.push({ depth: sy, draw(c) { itemRenderer?.draw(c, sx, sy, item); } });
  }
  sprites.push({ depth: h / 2, draw: drawPlayer });

  terrainStats = null;
  if (terrain && session) {
    try {
      const count = Module.ccall('d2_session_tile_count', 'number', [], []);
      const ptr = Number(Module.ccall('d2_session_tiles', 'number', [], []));
      if (count > 0 && ptr > 0) {
        const tiles = parseSessionTiles(Module.HEAPU8, ptr, count);
        terrainStats = terrain.draw(ctx, tiles, terrainFiles, snapshot.player, w, h, sprites);
      }
    } catch (error) { terrainError = String(error); terrain = null; }
  }
  if (!terrainStats?.drawn && tileView?.atlas) {
    const pattern = tileView.pattern || (tileView.pattern = ctx.createPattern(tileView.atlas, 'repeat'));
    if (pattern) {
      ctx.save(); ctx.globalAlpha = .48;
      pattern.setTransform(new DOMMatrix().translate((w / 2 - snapshot.player.x * 16) % tileView.atlas.width, (h / 2 - snapshot.player.y * 16) % tileView.atlas.height));
      ctx.fillStyle = pattern; ctx.fillRect(0, 0, w, h); ctx.restore();
    }
  }
  // Without terrain there is no wall pass to interleave into, so the sprites still need painting.
  if (!terrainStats?.drawn) for (const sprite of sprites) sprite.draw(ctx);
  const grid = 32, ox = w / 2 - snapshot.player.x * grid, oy = h / 2 - snapshot.player.y * grid;
  if (DEBUG_OVERLAY) {
    ctx.strokeStyle = '#182018'; ctx.lineWidth = 1;
    for (let x = ((ox % grid) + grid) % grid; x < w; x += grid) { ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, h); ctx.stroke(); }
    for (let y = ((oy % grid) + grid) % grid; y < h; y += grid) { ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(w, y); ctx.stroke(); }
    for (const room of snapshot.rooms) {
      ctx.fillStyle = 'rgba(37,45,31,.28)'; ctx.strokeStyle = '#42503a';
      ctx.fillRect(ox + room.x * grid, oy + room.y * grid, room.width * grid, room.height * grid);
      ctx.strokeRect(ox + room.x * grid, oy + room.y * grid, room.width * grid, room.height * grid);
    }
  }
  for (const warp of snapshot.warps) {
    if (!warp.enabled) continue;
    if (!DEBUG_OVERLAY) continue;
    const [x, y] = proj(warp.x, warp.y);
    ctx.save(); ctx.translate(x, y); ctx.rotate(Math.PI / 4);
    ctx.fillStyle = 'rgba(82,156,220,.34)'; ctx.strokeStyle = '#8bc6ff'; ctx.lineWidth = 2;
    ctx.fillRect(-9, -9, 18, 18); ctx.strokeRect(-9, -9, 18, 18); ctx.restore();
  }
  if (!session) {
    ctx.fillStyle = 'rgba(0,0,0,.68)'; ctx.fillRect(0, h / 2 - 44, w, 88);
    ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '20px Georgia,serif';
    ctx.fillText('Gameplay simulation is not connected', w / 2, h / 2 - 5);
    ctx.fillStyle = '#91866d'; ctx.font = '13px monospace';
    ctx.fillText('The renderer and controls are ready for d2_session_*', w / 2, h / 2 + 22);
  }
  if (session && snapshot.mode === 17) {
    ctx.fillStyle = 'rgba(0,0,0,.68)'; ctx.fillRect(0, h / 2 - 48, w, 96);
    ctx.fillStyle = '#a5160f'; ctx.textAlign = 'center'; ctx.font = '28px Georgia,serif';
    ctx.fillText('YOU HAVE DIED', w / 2, h / 2 - 7);
    ctx.fillStyle = '#c9ad68'; ctx.font = '14px monospace';
    ctx.fillText('Press R to return to town', w / 2, h / 2 + 24);
  }
  // An open conversation draws over the world, the way retail's NPC window does.
  if (npcState && ensureNpcPanel()) {
    try { npcPanelEvidence = npcPanel.draw(ctx, w, h, { ...npcState, playerItems: snapshot.items }); npcPanelError = ''; }
    catch (error) { npcPanelError = String(error); npcPanelEvidence = null; }
  } else npcPanelEvidence = null;
  if (inventoryPanelRenderer) inventoryPanelEvidence = inventoryPanelRenderer.draw(ctx, w, h, snapshot, inventoryVisible);
  pollUiEvents();
  if (stashPanelRenderer) {
    try { stashEvidence = stashPanelRenderer.draw(ctx, w, h, stashOpen ? readStashLayout() : null, snapshot.items, stashOpen, snapshot.act); stashPanelError = ''; }
    catch (error) { stashPanelError = String(error); stashEvidence = null; }
  }
  if (waypointPanelRenderer) {
    try { waypointEvidence = waypointPanelRenderer.draw(ctx, w, h, waypointMenu ? readWaypointRows(snapshot.act) : [], snapshot.act, Boolean(waypointMenu)); waypointPanelError = ''; }
    catch (error) { waypointPanelError = String(error); waypointEvidence = null; }
  }
  if (characterPanelRenderer) {
    const adv = readAdvancement();
    // Life and mana are the snapshot's, not the advancement record's: the panel shows what the
    // player has right now, and the engine is the only thing that decides that.
    characterPanelEvidence = characterPanelRenderer.draw(ctx, w, h, adv && {
      ...adv, name: chosenName || '', className: CLASS_NAMES[snapshot.player?.playerClass] || '',
      // Retail stores life and mana as 8.8 fixed point (50 life is 12800) and displays the whole
      // part. Drawing the raw stat put "14848" in the Life slot.
      life: (snapshot.player?.hpMax ?? 0) >> 8, mana: (snapshot.player?.manaMax ?? 0) >> 8,
      string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]),
    }, characterVisible);
  }
  if (skillPanelRenderer) {
    const adv = readAdvancement();
    skillPanelEvidence = skillPanelRenderer.draw(ctx, w, h, skillsVisible ? {
      playerClass: snapshot.player?.playerClass ?? 0, level: adv?.level ?? 1,
      skillPoints: adv?.skillPoints ?? 0, skills: readClassSkills() || [],
      // SkillDesc's wStrName is a .tbl index, so the name is retail's own string, not a table
      // this client keeps.
      name: (skill) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [skill.nameStr]),
    } : null, skillsVisible);
  }
  if (controlPanelRenderer) controlPanelEvidence = controlPanelRenderer.draw(ctx, w, h, snapshot);
  if (beltRenderer) beltEvidence = beltRenderer.draw(ctx, snapshot.items, readBeltBoxes());
  hud.textContent = hudLines(snapshot, Boolean(session), fps).join('\n');
}

// A throw anywhere in update/render used to escape this function, so requestAnimationFrame was
// never re-armed: the loop died and the canvas froze on the last good frame, with the rest of the
// shell still responding to input. That failure mode is indistinguishable from "nothing happened",
// which cost real time to diagnose. Keep the loop alive and record the fault instead.
// The front-end screens are built from a timer, not from the animation loop.
//
// requestAnimationFrame is throttled to nothing in a background or headless tab, and update() only
// runs from it. Building the menu there meant that a page which loaded without ever painting -- a
// reload in a background tab, for instance -- would sit on a blank canvas forever with no error,
// because the code that constructs the title screen simply never ran. setInterval keeps running.
// The ensure* calls are cheap no-ops once their screen exists.
setInterval(() => {
  if (!window.__d2?.ready || window.__d2.error) return;
  if (screen === SCREEN.TITLE) ensureTitleScreen();
  else if (screen === SCREEN.SELECT) ensureCharSelect();
  else if (screen === SCREEN.CREATE) ensureCharCreate();
}, 200);

function frame(now) {
  accumulator = Math.min(accumulator + now - last, STEP * 5); last = now;
  try {
    while (accumulator >= STEP) { update(); accumulator -= STEP; }
    render();
    frameError = '';
  } catch (error) {
    frameError = String(error?.stack || error);
    frameErrors++;
  }
  frames++;
  if (now - fpsAt >= 500) { fps = frames * 1000 / (now - fpsAt); frames = 0; fpsAt = now; }
  requestAnimationFrame(frame);
}

// The talkable NPC under a canvas point, if any. Uses the same projection the world is drawn with.
// Retail's 0x77 UpdateUi codes the engine has sent since the last read (16 opens the stash).
function pollUiEvents() {
  if (!session) return;
  const p = Module._malloc(64);
  try {
    const n = Module.ccall('d2_session_take_ui_events', 'number', ['bigint', 'number'], [BigInt(p), 64]);
    for (const code of Module.HEAPU8.slice(p, p + n)) { if (code === 16) stashOpen = true; else if (code === 17) stashOpen = false; }
    if (Module.ccall('d2_session_take_waypoint_menu', 'number', ['bigint'], [BigInt(p)])) waypointMenu = { id: Module.HEAPU32[p / 4] };
  } finally { Module._free(p); }
}
// The act's waypoints as the engine reports them, named through retail's own strings.
function readWaypointRows(act) {
  const p = Module._malloc(64 * 16);
  try {
    const n = Module.ccall('d2_session_waypoints', 'number', ['bigint', 'number'], [BigInt(p), 256]);
    const a = new Int32Array(Module.HEAPU8.buffer.slice(p, p + n * 16)), rows = [];
    for (let i = 0; i < n; i++) {
      if (a[i * 4 + 2] !== act - 1) continue;
      const key = Module.ccall('d2_level_name', 'string', ['number'], [a[i * 4 + 1]]);
      rows.push({ waypointNo: a[i * 4], levelId: a[i * 4 + 1], active: a[i * 4 + 3] === 1,
        name: Module.ccall('d2_lang_string', 'string', ['string'], [key]) || key });
    }
    return rows;
  } finally { Module._free(p); }
}
// Retail's 0x49: travel to levelId through the open waypoint, or close the menu with 0.
function takeWaypoint(levelId) {
  if (!waypointMenu) return 0;
  const previousAct = snapshot.act;
  const result = Module.ccall('d2_session_command_waypoint', 'number', ['number', 'number'], [waypointMenu.id, levelId]);
  if (result) {
    waypointMenu = null;
    snapshot = session.snapshot(); motion = session.motion();
    if (TERRAIN_ENABLED && snapshot.act !== previousAct) { loadSessionTerrain(snapshot.act - 1); loadActTileView(snapshot.act - 1); }
  }
  if (levelId) lastWaypointTravel = { levelId, result };
  return result;
}
// The stash grid is retail's own Inventory.txt record for a LoD character, via the engine.
function readStashLayout() {
  if (stashLayout) return stashLayout;
  const p = Module._malloc(48);
  try {
    const ok = Module.ccall('d2_session_inventory_layout', 'number', ['number', 'number', 'bigint', 'number'], [BIG_BANK_PAGE_1, 1, BigInt(p), 12]);
    stashLayout = ok ? stashGeometry(Array.from(new Int32Array(Module.HEAPU8.buffer.slice(p, p + 48)))) : null;
  } finally { Module._free(p); }
  return stashLayout;
}
// The level exit under the pointer: an enabled warp whose interaction point (the engine's exitX/Y --
// the spot retail's nearest-warp test measures from) is within 3 subtiles of the clicked ground.
// DECLARED approximation: retail hit-tests the warp's LvlWarp selection box in screen space.
function warpUnderPointer(px, py) {
  if (!session) return null;
  const d = screenDeltaToWorld(px - canvas.width / 2, py - canvas.height / 2);
  const wx = snapshot.player.x + d.x, wy = snapshot.player.y + d.y;
  let best = null, bestDistance = 3.5;
  for (const warp of snapshot.warps || []) {
    if (!warp.enabled || warp.destinationLevel === snapshot.level) continue;
    const dist = Math.max(Math.abs(warp.x - wx), Math.abs(warp.y - wy));
    if (dist < bestDistance) { best = warp; bestDistance = dist; }
  }
  return best;
}
let lastWarpTaken = null;
// The object under the pointer, the same way npcUnderPointer finds a person.
function objectUnderPointer(px, py) {
  if (!session) return null;
  const camera = worldToScreen(snapshot.player.x, snapshot.player.y);
  const w = canvas.width, h = canvas.height;
  let best = null, bestDistance = 36;
  for (const object of snapshot.objects || []) {
    const point = worldToScreen(object.x, object.y);
    const d = Math.hypot(point.x - camera.x + w / 2 - px, point.y - camera.y + h / 2 - py);
    if (d < bestDistance) { best = object; bestDistance = d; }
  }
  return best;
}
function npcUnderPointer(px, py) {
  if (!session) return null;
  const camera = worldToScreen(snapshot.player.x, snapshot.player.y);
  const w = canvas.width, h = canvas.height;
  let best = null, bestDistance = 36;   // within ~36px of the sprite's feet
  for (const entity of snapshot.entities) {
    if (!entity.npc) continue;
    const point = worldToScreen(entity.x, entity.y);
    const ex = point.x - camera.x + w / 2, ey = point.y - camera.y + h / 2;
    const distance = Math.hypot(ex - px, ey - py);
    if (distance < bestDistance) { best = entity; bestDistance = distance; }
  }
  return best;
}

function talkToNpc(npcId) {
  try {
    if (!Module.ccall('d2_session_command_npc', 'number', ['number'], [npcId])) return false;
    // Vendors open their shop; action 1 is trade. A non-trading NPC just keeps the dialogue.
    Module.ccall('d2_session_command_npc_vendor', 'number', ['number', 'number'], [npcId, 1]);
    npcState = readNpcState();
    return true;
  } catch { return false; }
}

function canvasPoint(event) {
  const r = canvas.getBoundingClientRect();
  return [(event.clientX - r.left) * canvas.width / r.width, (event.clientY - r.top) * canvas.height / r.height];
}
addEventListener('keydown', (e) => {
  if (screen === SCREEN.CREATE) { if (charCreate?.typeKey(e.code, e.key)) e.preventDefault(); return; }
  actMusic?.play(snapshot.act - 1).catch((error) => { musicError = String(error); }); if (e.code === 'KeyI' && !e.repeat) { inventoryVisible = !inventoryVisible; e.preventDefault(); return; }
  if (e.code === 'KeyC' && !e.repeat) { characterVisible = !characterVisible; e.preventDefault(); return; }
  // Esc, as retail: it closes what is open before it does anything else. An open conversation is
  // ended through retail's own 0x30 TerminateEntityChat handler, and the window reflects what the
  // ENGINE then reports rather than being hidden locally; then the open panels. With nothing open
  // it falls through to the command path (which cancels the selected skill, s46).
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && (waypointMenu || stashOpen || npcState || inventoryVisible || characterVisible || skillsVisible)) {
    if (waypointMenu) {
      // Retail closes the waypoint menu with 0x49 and level 0.
      takeWaypoint(0);
    } else if (stashOpen) {
      // Retail's client closes the stash and tells the server with 0x4F TRADEBTN_CLOSESTASH (18);
      // the window closes when the engine accepts it.
      if (Module.ccall('d2_session_command_button', 'number', ['number', 'number'], [18, 0])) stashOpen = false;
    } else if (npcState) {
      Module.ccall('d2_session_command_end_npc', 'number', ['number'], [npcState.npcId]);
      npcState = readNpcState();
    } else { inventoryVisible = false; characterVisible = false; skillsVisible = false; }
    e.preventDefault(); return;
  }
  if (e.code === 'KeyT' && !e.repeat) { skillsVisible = !skillsVisible; e.preventDefault(); return; }
  if (input.key(e.code, true, e.repeat)) e.preventDefault(); });
addEventListener('keyup', (e) => { if (input.key(e.code, false)) e.preventDefault(); });
addEventListener('blur', () => { input.keys.clear(); });
canvas.addEventListener('pointermove', (e) => {
  const [x, y] = canvasPoint(e);
  if (skillsVisible && skillPanelRenderer && session)
    skillPanelRenderer.hover(x, y, canvas.width, canvas.height, { skills: readClassSkills() || [] });
  if (screen === SCREEN.TITLE) titleScreen?.hover(x, y, canvas.width, canvas.height);
  else if (screen === SCREEN.CREATE) charCreate?.hover(x, y, canvas.width, canvas.height);
});
canvas.addEventListener('pointerdown', (e) => {
  if (screen === SCREEN.TITLE || screen === SCREEN.CREATE || screen === SCREEN.SELECT) {
    const [tx, ty] = canvasPoint(e);
    if (screen === SCREEN.TITLE) titleScreen?.click(tx, ty, canvas.width, canvas.height);
    else if (screen === SCREEN.SELECT) charSelect?.click(tx, ty, canvas.width, canvas.height);
    else charCreate?.click(tx, ty, canvas.width, canvas.height);
    canvas.focus(); e.preventDefault(); return;
  }
  const [x, y] = canvasPoint(e);
  actMusic?.play(snapshot.act - 1).catch((error) => { musicError = String(error); });
  const beltItem = beltRenderer?.itemAt(x, y, snapshot.items, readBeltBoxes()); if (beltItem && session?.useBelt(beltItem.gridX)) { snapshot = session.snapshot(); canvas.focus(); e.preventDefault(); return; }
  // An open waypoint menu takes the click: an active row travels there, anything else is swallowed.
  if (waypointMenu && waypointPanelRenderer) {
    const row = waypointPanelRenderer.rowAt(x, y, canvas.width, canvas.height, readWaypointRows(snapshot.act));
    if (row?.active) takeWaypoint(row.levelId);
    canvas.focus(); e.preventDefault(); return;
  }
  // An open NPC window takes the click before the world does, so buying does not also walk.
  if (npcState && npcPanel) {
    const item = npcPanel.itemAt(x, y, canvas.width, canvas.height,
      { ...npcState, playerItems: snapshot.items });
    // The trade screen has two grids: the shop's stock above, the player's items below. A click on
    // your own item is a SALE, not a purchase -- the panel says which side was hit.
    if (item?.mine) { sellToVendor(item.id); canvas.focus(); e.preventDefault(); return; }
    if (item) { buyFromVendor(item.id); canvas.focus(); e.preventDefault(); return; }
  }
  // Left-clicking a talkable NPC walks over and starts the conversation, which is what a player
  // expects; anything else falls through to the normal command path.
  const npcHit = e.button === 0 && session ? npcUnderPointer(x, y) : null;
  // Left-clicking an OBJECT walks to it and operates it, as retail does. What happens next is the
  // engine's: the stash, for one, answers with 0x77 UPDATEUI_OPENSTASH (pollUiEvents).
  const objectHit = !npcHit && e.button === 0 && session && !stashOpen ? objectUnderPointer(x, y) : null;
  // Left-clicking a level exit walks to it and takes it, through the engine's warp command (which
  // only accepts a real generated exit of this level).
  const warpHit = !npcHit && !objectHit && e.button === 0 && session ? warpUnderPointer(x, y) : null;
  if (warpHit) {
    walkEvidence = { active: true, hops: 0, reason: 0 };
    walkGoal = { x: warpHit.x, y: warpHit.y, within: 2, onArrive: () => {
      const previousAct = snapshot.act;
      const result = Module.ccall('d2_session_command_warp', 'number', ['number', 'number'], [warpHit.destinationLevel, warpHit.warpId]);
      lastWarpTaken = { destinationLevel: warpHit.destinationLevel, warpId: warpHit.warpId, result };
      if (result) { snapshot = session.snapshot(); motion = session.motion();
        if (TERRAIN_ENABLED && snapshot.act !== previousAct) { loadSessionTerrain(snapshot.act - 1); loadActTileView(snapshot.act - 1); } }
    } };
  } else if (npcHit) {
    walkEvidence = { active: true, hops: 0, reason: 0 };
    walkGoal = { x: npcHit.x, y: npcHit.y, within: 4, onArrive: () => talkToNpc(npcHit.id) };
  } else if (objectHit) {
    walkEvidence = { active: true, hops: 0, reason: 0 };
    walkGoal = { x: objectHit.x, y: objectHit.y, within: 2, onArrive: () => { session.operate(objectHit.id); snapshot = session.snapshot(); pollUiEvents(); } };
  } else {
    // A "+" on the character screen spends a point through retail's 0x3A handler. Re-read rather
    // than adjust a local copy: the engine decides what a point did, including refusing it.
    // The skill tree takes the click before the world does, so learning a skill does not also walk.
    const skillHit = skillsVisible && skillPanelRenderer && session
      ? skillPanelRenderer.skillAt(x, y, canvas.width, canvas.height,
          { skills: readClassSkills() || [] }) : null;
    if (skillHit?.tab) { skillPanelRenderer.tab = skillHit.tab; canvas.focus(); e.preventDefault(); return; }
    if (skillHit?.skill) {
      Module.ccall('d2_session_command_add_skill', 'number', ['number'], [skillHit.skill.id]);
      classSkills = null; advancement = null;
      canvas.focus(); e.preventDefault(); return;
    }
    const statHit = characterVisible && characterPanelRenderer && session
      ? characterPanelRenderer.buttonAt(x, y, canvas.width, canvas.height, readAdvancement()) : null;
    if (statHit !== null && statHit !== undefined) {
      Module.ccall('d2_session_command_add_stat', 'number', ['number', 'number'], [statHit, 1]);
      advancement = null;
      canvas.focus(); e.preventDefault(); return;
    }
    const carried = inventoryVisible && inventoryPanelRenderer
      ? inventoryPanelRenderer.itemAt(x, y, canvas.width, canvas.height, snapshot) : null;
    // With a shop open, clicking something you are carrying sells it; otherwise the inventory
    // click is swallowed as before and anything else goes to the world.
    if (carried && npcState) sellToVendor(carried.id);
    else if (!carried) input.pointer(x, y, e.button);
  }
  canvas.focus(); e.preventDefault(); });
canvas.addEventListener('contextmenu', (e) => { if (session && inventoryVisible && inventoryPanelRenderer) {
    const [x, y] = canvasPoint(e), item = inventoryPanelRenderer.itemAt(x, y, canvas.width, canvas.height, snapshot);
    if (item && session.drop(item.id)) snapshot = session.snapshot();
  } e.preventDefault(); });
window.__d2Shell = { input, get snapshot() { return snapshot; }, get tileView() { return tileView; }, get tileViewError() { return tileViewError; },
  step() { update(); },
  // Force one frame. requestAnimationFrame is throttled hard in a headless/background tab, so a
  // caller that drives the shell with step() and then reads PIXELS can otherwise be looking at a
  // frame from before its own input -- which reads exactly like "the screen never changed".
  renderNow() { render(); },
  // World (subtile) -> canvas pixel, through the SAME isometric transform render() and the pointer
  // path use. Exposed so a caller aiming at a unit does not re-derive the projection and quietly
  // drift from it; input.pointer(...project(u.x, u.y)) round-trips back to that unit.
  project(wx, wy) {
    const c = worldToScreen(snapshot.player.x, snapshot.player.y), p = worldToScreen(wx, wy);
    return [p.x - c.x + canvas.width / 2, p.y - c.y + canvas.height / 2];
  },
  get playerRenderer() { return playerRenderer; }, get monsterRenderer() { return monsterRenderer; },
  get objectRenderer() { return objectRenderer; },
  get itemRenderer() { return itemRenderer; },
  get inventoryPanelEvidence() { return inventoryPanelEvidence && { ...inventoryPanelEvidence }; },
  get characterPanelEvidence() { return characterPanelEvidence && { ...characterPanelEvidence }; },
  get skillPanelEvidence() { return skillPanelEvidence && { ...skillPanelEvidence }; },
  get warpEvidence() { return { lastWarpTaken, warps: (snapshot.warps || []).map((w) => ({ destinationLevel: w.destinationLevel, warpId: w.warpId, enabled: w.enabled, x: w.x, y: w.y })) }; },
  get waypointEvidence() { return { open: Boolean(waypointMenu), waypointId: waypointMenu?.id ?? 0, error: waypointPanelError,
    drawn: waypointEvidence && JSON.parse(JSON.stringify(waypointEvidence)), art: waypointPanelRenderer?.evidence ?? null,
    lastTravel: lastWaypointTravel }; },
  get stashEvidence() { return { open: stashOpen, error: stashPanelError, drawn: stashEvidence && JSON.parse(JSON.stringify(stashEvidence)),
    art: stashPanelRenderer?.evidence ?? null }; },
  get skillsVisible() { return skillsVisible; },
  set skillsVisible(v) { skillsVisible = Boolean(v); },
  get classSkills() { return readClassSkills(); },
  skillPanelHitTest(x, y) {
    return skillPanelRenderer && skillsVisible
      ? skillPanelRenderer.skillAt(x, y, canvas.width, canvas.height, { skills: readClassSkills() || [] }) : null;
  },
  // The SAME hit test the click path uses, so a caller aiming at a "+" cannot be testing
  // coordinates it invented while the real button sits elsewhere.
  characterPanelHitTest(x, y) {
    return characterPanelRenderer && characterVisible
      ? characterPanelRenderer.buttonAt(x, y, canvas.width, canvas.height, readAdvancement()) : null;
  },
  get characterVisible() { return characterVisible; },
  set characterVisible(v) { characterVisible = Boolean(v); },
  get advancement() { return readAdvancement(); },
  get beltEvidence() { return beltEvidence && { ...beltEvidence }; },
  get controlPanelEvidence() { return controlPanelEvidence && { ...controlPanelEvidence }; },
  get selectedSkill() { return selectedSkill; }, get lastSkillCommand() { return lastSkillCommand && { ...lastSkillCommand }; },
  get screen() { return screen; },
  get frameError() { return frameError; }, get frameErrors() { return frameErrors; },
  get charSelectEvidence() { return charSelect ? { ...charSelect.evidence, error: charSelectError } : { error: charSelectError || 'not built' }; },
  buyVendorItem(itemId) { return buyFromVendor(itemId); },
  sellVendorItem(itemId) { return sellToVendor(itemId); },
  repairAtVendor(itemId) { return repairAtVendor(itemId); },
  // Gambling is a different counter, not a different button: the vendor must be opened with
  // action 2, and the stock then read is the NPC's GAMBLE inventory rather than its trade stock.
  openGambleWindow(npcId) {
    try {
      const ok = Module.ccall('d2_session_command_npc_vendor', 'number', ['number', 'number'],
        [npcId ?? npcState?.npcId ?? 0, 2]);
      npcState = readNpcState();
      return Boolean(ok);
    } catch { return false; }
  },
  gambleForItem(itemId) {
    if (!npcState) return false;
    try {
      const ok = Module.ccall('d2_session_command_vendor_gamble', 'number', ['number', 'number'],
        [npcState.npcId, itemId]);
      npcState = readNpcState();
      snapshot = session.snapshot();
      return Boolean(ok);
    } catch { return false; }
  },
  get saleEvidence() { return lastSale ? { ...lastSale } : null; },
  // What the player is carrying that a vendor would take: inventory pages, not the ground.
  // The code is decoded to its four-character form so this list and the vendor's speak the same
  // language -- the raw u32 is the same value, just unreadable, and it is what the string tables
  // are keyed by.
  get carriedItems() {
    return snapshot.items.filter((i) => i.invPage !== 255).map((i) => {
      const code = typeof i.code === 'number'
        ? String.fromCharCode(i.code & 255, (i.code >> 8) & 255, (i.code >> 16) & 255, (i.code >> 24) & 255)
            .replace(/\0/g, '').trim()
        : i.code;
      return { id: i.id, code, invPage: i.invPage, bodyLoc: i.bodyLoc,
        name: Module.ccall('d2_lang_string', 'string', ['string'], [code]) || code };
    });
  },
  get purchaseEvidence() { return lastPurchase ? { ...lastPurchase } : null; },
  get walkEvidence() { return { ...walkEvidence, goal: walkGoal ? { x: walkGoal.x, y: walkGoal.y } : null }; },
  walkTo(x, y, within = 4) { walkEvidence = { active: true, hops: 0, reason: 0 }; walkGoal = { x, y, within }; },
  talkToNearestNpc() {
    // Actually the nearest: picking the first NPC in the entity list walked past Charsi to Gheed.
    const p = snapshot.player;
    let npc = null, best = Infinity;
    for (const entity of snapshot.entities) {
      if (!entity.npc) continue;
      const d = chebyshev(entity.x, entity.y, p.x, p.y);
      if (d < best) { best = d; npc = entity; }
    }
    if (!npc) return false;
    walkEvidence = { active: true, hops: 0, reason: 0 };
    walkGoal = { x: npc.x, y: npc.y, within: 4, onArrive: () => talkToNpc(npc.id) };
    return true;
  },
  get npcPanelEvidence() {
    return { open: Boolean(npcState), error: npcPanelError,
      npcId: npcState?.npcId ?? 0, greeting: npcState?.greeting ?? '',
      items: npcState?.items?.length ?? 0,
      gold: npcState?.gold ?? -1,
      firstItems: (npcState?.items ?? []).slice(0, 6).map((i) => ({ id: i.id, code: i.code, name: i.name, cost: i.cost, invFile: i.invFile })),
      drawn: npcPanelEvidence, panel: npcPanel?.evidence ?? null };
  },
  get npcEvidence() {
    const ents = snapshot.entities;
    return { npcs: npcCount, hostile: ents.filter((e) => e.hostile).length,
      npcClasses: [...new Set(ents.filter((e) => e.npc).map((e) => e.classId))].sort((a, b) => a - b) };
  },
  get saveEvidence() { return { written: savesWritten, error: lastSaveError, autosaveTicks: autosaveAt, inFlight: autosaveInFlight }; },
  get loadEvidence() { return { ...lastLoad }; },
  listSavedCharacters() { return saveStore.list(); },
  // Persist the live character. Returns the stored header, or throws -- a save that silently did
  // not happen is the worst outcome, because the player only finds out when the character is gone.
  async persistCharacter() {
    const header = await persistCurrentCharacter();
    if (!header) throw new Error(lastSaveError || 'no session to save');
    return header;
  },
  enterCharacterSelect() { return enterCharacterSelect(); },
  selectCharacter(name) { return charSelect ? charSelect.selectByName(name) : -1; },
  playSelectedCharacter() { charSelect?.play(); },
  get charCreateEvidence() { return charCreate ? { ...charCreate.evidence, error: charCreateError } : { error: charCreateError || 'not built' }; },
  chooseClass(key) { return charCreate ? charCreate.chooseByKey(key) : -1; },
  setCharacterName(value) { return charCreate ? charCreate.setName(value) : null; },
  submitCharacter() { charCreate?.submit(); },
  get chosenCharacter() { return chosenClass == null ? null : { playerClass: chosenClass, name: chosenName }; },
  // Set the engine-side character name. Goes through the shell because ccall with a 'string'
  // argument needs the stack helpers, which resolve from the module scope the shell holds but not
  // from the page's global Module.
  setEngineCharacterName(name) {
    return Module.ccall('d2_session_set_character_name', 'number', ['string'], [String(name)]);
  },
  // The live character as a retail .d2s. Returns null when there is no session to save.
  saveCharacter() {
    if (!session) return null;
    const cap = 16384, p = Module._malloc(cap);
    try {
      const n = Module.ccall('d2_session_save_d2s', 'number', ['bigint', 'number'], [BigInt(p), cap]);
      if (n <= 0) return { error: Module.ccall('d2_session_save_d2s_error', 'number', [], []) };
      return { bytes: Module.HEAPU8.slice(Number(p), Number(p) + n), length: n };
    } finally { Module._free(p); }
  },
  get titleEvidence() { return titleScreen ? { ...titleScreen.evidence, error: titleError } : { error: titleError || 'not built' }; },
  /**
   * Canvas-pixel click targets for the front-end screens, from the same hit tests their clicks use.
   * s000 aims real pointer events with these, the way world clicks are aimed with project().
   */
  uiTargets() {
    const w = canvas.width, h = canvas.height, out = { screen };
    if (screen === SCREEN.TITLE && titleScreen) {
      out.title = {};
      for (let y = 0; y < h; y++) {
        const i = titleScreen.hitTest(w / 2, y, w, h);
        const label = i >= 0 ? titleScreen.labelAt(i) : null;
        if (label && !(label in out.title)) {
          let y2 = y; while (y2 < h && titleScreen.hitTest(w / 2, y2, w, h) === i) y2++;
          out.title[label] = [Math.round(w / 2), Math.round((y + y2 - 1) / 2)];
        }
      }
    }
    if (screen === SCREEN.CREATE && charCreate?.targets) out.create = charCreate.targets(w, h);
    return out;
  },
  titleClick(label) {
    if (!titleScreen) return -1;
    // Click a menu entry by name, so a scenario does not hardcode button pixel positions.
    const boxes = titleScreen.evidence, w = canvas.width, h = canvas.height;
    for (let y = 0; y < h; y += 2) for (let x = w / 2 - 1; x < w / 2 + 1; x++) {
      const i = titleScreen.hitTest(x, y, w, h);
      if (i >= 0 && titleScreen.labelAt(i) === label) { titleScreen.click(x, y, w, h); return i; }
    }
    return -1;
  },
  get playerRenderError() { return playerRenderError; }, get sessionError() { return sessionError; },
  // Terrain evidence: what the last frame actually drew, and from which retail DT1s. A scenario
  // asserting "the map rendered" needs this -- a non-empty canvas proves nothing, the mocked-atlas
  // page filled the canvas too.
  // Always an object, never null: a terrain FAILURE is exactly what a caller needs to see, and a
  // null here would hide the reason behind an unexplained timeout.
  get terrainEvidence() { return { drawn: 0, missing: 0, ...(terrainStats || {}), live: Boolean(terrain), files: (terrainFiles || []).filter(Boolean), error: terrainError || '' }; },
  normalizeSnapshot };
Object.defineProperty(window.__d2Shell, 'music', { get: () => actMusic?.evidence || null });
addEventListener('beforeunload', () => { session?.destroy(); actMusic?.close(); });
requestAnimationFrame(frame);
