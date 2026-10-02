import { parseSessionTiles, readSessionTerrain, readSessionLightmap, worldToScreen, screenDeltaToWorld, UNIT_CENTRE_Y } from './assets/session-terrain.mjs';
import { perfSync, perfEnabled, perfT0, perfT, perfSample, perfCountFrame, perfReset, perfReport } from './assets/frame-perf.mjs';
import { COMMAND, CommandInput, DEFAULT_SESSION_FIXTURES, hudLines, normalizeSnapshot, parseSessionRooms, parseSessionSnapshot, parseSessionUnits, parseSessionWarps, inUnitHoverBox, selectPrimaryTarget, viewportToWorld } from './gameplay-model.mjs';
import { createMpqAssetReader } from './assets/mpq-assets.mjs';
import { recordPrim } from './assets/drawlist.mjs';
import { loadLiveLevelTerrain, actPalettePath } from './assets/live-level-terrain.mjs';
import { decodeDatPalette } from './assets/d2-formats.mjs';
import { createPlayerRenderer, playerClassInfo } from './assets/unit-renderer.mjs';
import { createMonsterRenderer, parseMonsterAnimations } from './assets/monster-renderer.mjs';
import { createObjectRenderer, parseObjectPresentations } from './assets/object-renderer.mjs';
import { drawObjectHoverLabel, objectRetailName } from './assets/object-hover-label.mjs';
import { waypointPadAnchor } from './assets/waypoint-pad-anchor.mjs';
import { createItemRenderer, parseItemPresentations } from './assets/item-renderer.mjs';
import { createStashPanelRenderer, stashGeometry, BIG_BANK_PAGE_1, INVPAGE_STASH, INVPAGE_CUBE, TRANSMOGRIFY_BOX_PAGE_1 } from './assets/stash-panel.mjs';
import { createStashGoldRenderer, STASH_GOLD } from './assets/stash-gold-dialog.mjs';
import { createWaypointPanelRenderer, tabActsOf } from './assets/waypoint-panel.mjs';
import { createWaypointMenuInput } from './assets/waypoint-menu-input.mjs';
import { createControlPanelRenderer, HUD_LAYOUT } from './assets/control-panel.mjs';
import { createHudHelp } from './assets/hud-help.mjs';
import { createInventoryPanelRenderer, inventoryLayoutFromEngine } from './assets/inventory-panel.mjs';
import { createCharacterPanelRenderer } from './assets/character-panel.mjs';
import { createQuestLogRenderer, createMessageLogRenderer, QUESTS_IN_ACT, ACT_BASE, ACT_DONE, questStatus, messageScrollMax, COMPLETING } from './assets/quest-log.mjs';
import { createSkillPanelRenderer, parseClassSkills, SESSION_SKILL_BYTES } from './assets/skill-panel.mjs';
import { createSkillPointOverlay } from './assets/skill-plus.mjs';
import { createSkillSelectRenderer, parseSkillChoices, SESSION_SKILL_CHOICE_BYTES } from './assets/skill-select.mjs';
import { dispatchSkillSelectHit } from './assets/hud-hit-test.mjs';
import { hudOrigin } from './assets/hud-origin.mjs';
import { panelSides as panelViewSides, viewCentreX } from './assets/panel-view-shift.mjs';
import { createFreeResArt, barParts, halfParts, stoneTiles, drawParts } from './assets/wide-hud.mjs';
import { createBeltRenderer } from './assets/belt-renderer.mjs';
import { createActMusicPlayer } from './assets/act-music.mjs';
import { createAudioVolume } from './assets/audio-volume.mjs';
import { createUiClickSoundPlayer } from './assets/ui-click-sound.mjs';
import { createUnitSoundPlayer } from './assets/unit-sound-player.mjs';
import { createFont, createTitleScreen, decodeFontTbl, FRONTEND } from './assets/frontend.mjs';
import { createExitSequence } from './assets/exit-sequence.mjs';
import { loadBnetServers } from './assets/bnet-servers.mjs';
import { BOOT_MOVIES, openMovie } from './assets/cinematics.mjs';
import { createClassSelectSoundPlayer } from './assets/class-create-sounds.mjs';
import { createCreditsScreen } from './assets/credits-screen.mjs';
import { createCinematicsScreen } from './assets/cinematics-screen.mjs';
import { createSpeechBox } from './assets/speech-box.mjs';
import { createCharacterCreate } from './assets/charcreate.mjs';
import { createCharacterSelect } from './assets/charselect.mjs';
import { deleteAndRefreshCharacter } from './assets/character-delete-store.mjs';
import { createSaveStore, CLASS_NAMES } from './assets/save-store.mjs';
import { installUnloadAutosave } from './assets/unload-autosave.mjs';
import { createNpcPanel } from './assets/npc-panel.mjs';
import { createGameMenu } from './assets/game-menu.mjs';
import { MODAL_POINTER_OWNER, modalPointerOwner } from './assets/modal-pointer-priority.mjs';
import { createDeathScreen } from './assets/death-screen.mjs';
import { createNpcMenu, npcMenuEntries, talkEntries } from './assets/npc-menu.mjs';
import { npcSpeech } from './assets/npc-topics.mjs';
import { createWeatherRenderer } from './assets/weather-renderer.mjs';
import { weatherWaterAnchors } from './assets/weather-water-anchors.mjs';
import { createMissileRenderer, parseMissileAnimations } from './assets/missile-renderer.mjs';
import { createNpcSpeechAlertRenderer } from './assets/npc-speech-alert.mjs';
import { createAreaNameBlockRenderer } from './assets/area-name-block.mjs';
import { createEntryBanner, ENTRY_BANNER } from './assets/level-entry-banner.mjs';
import { createVendorPanel, SMITH_CLASSES, tooltipSpec } from './assets/vendor-panel.mjs';
import { handleVendorRepairClick } from './assets/vendor-repair-control.mjs';
import { decodeDc6 } from './assets/d2-formats.mjs';
import { createGameCursor } from './assets/game-cursor.mjs';
import { createPointerLayer } from './assets/pointer-layer.mjs';
import { DROPPED_GAMEPLAY_KEYS } from './assets/retail-key-bindings.mjs';
import { createHelpOverlayRenderer } from './assets/help-overlay.mjs';
import { createConfigureControls } from './assets/configure-controls.mjs';
import { createAutomapOverlay } from './assets/automap-overlay.mjs';
import { parseMa0 } from './assets/ma0.mjs';
import { panelCloseAt } from './assets/panel-close.mjs';
import { installScrollSafeCanvasFocus } from './assets/canvas-focus.mjs';
import { createPlugyStash, PLUGY_GRID, PLUGY_NAME_AT } from './assets/plugy-stash-ui.mjs';
import { pagesToSss, sssToPages } from './assets/plugy-stash.mjs';
import { closeStashFromInventory } from './assets/stash-close.mjs';
import { createFrontendPointerStates } from './assets/frontend-pointer-states.mjs';

const canvas = document.querySelector('#game');
// The game always sizes to the viewport (D2GL's resolution model, ): the game view is the
// window's own size in DEVICE pixels (CSS px x devicePixelRatio) divided by one whole-number scale S, so the
// bitmap has the window's aspect and no bars/stretch, and the CSS fill is an exact S-times nearest upscale.
// S keeps the world scale near retail's (game height ~600-1200): floor(deviceH/600), rounded on touch screens.
// D2GL keeps a game size and a viewport offset/scale for mouse mapping; here the canvas rect is that mapping.
// ?view=800 (and the dev page) keeps retail's fixed 800x600.
function fitView() {
  if (!window.__d2Entry?.fill) return;
  const dpr = devicePixelRatio || 1, dw = Math.round(innerWidth * dpr), dh = Math.round(innerHeight * dpr);
  const q = dh / 600, S = Math.max(1, matchMedia('(pointer: coarse)').matches ? Math.round(q) : Math.floor(q));
  const w = Math.max(1, Math.floor(dw / S)), h = Math.max(1, Math.floor(dh / S));
  window.__d2ViewScale = S;
  if (canvas.width !== w) canvas.width = w;
  if (canvas.height !== h) canvas.height = h;
}
fitView(); addEventListener('resize', fitView);
installScrollSafeCanvasFocus(canvas);
const hud = document.querySelector('#hud');
const status = document.querySelector('#session-status');
const classSelect = document.querySelector('#class-select');
const sessionBar = document.querySelector('#session-bar');
const ctx = canvas.getContext('2d', { alpha: false, willReadFrequently: true });
const input = new CommandInput();
const frontendPointerStates = createFrontendPointerStates();
// The front-end screen that owns a pointer gesture, with TITLE's trademark phase exempt: it owns
// its own countdown (s198), so it arms a control of its own that only advances the phase.
const trademarkPointerControl = { name: 'trademark', press: () => 'trademark', release: (token) => (token === 'trademark' ? titleAdvance() : false), cancel: () => {} };
// A boot movie is skipped the same way, and by the same gesture retail uses: a press arms the phase,
// the release on it advances. Nothing here reaches a button -- there is no menu on screen yet.
const moviePointerControl = { name: 'cinematic', press: () => 'cinematic', release: (token) => (token === 'cinematic' ? titleAdvance() : false), cancel: () => {} };
const frontendPointerControl = () => {
  if (screen === SCREEN.TITLE) {
    if (bootMovieSpec(titlePhase)) return moviePointerControl;
    // F91: the exit sequence owns the front end once it starts -- retail's game is gone at this
    // point, so nothing behind the fade is a control any more.
    if (exitSeq?.active) return null;
    return titlePhase === 'trademark' ? trademarkPointerControl : titleScreen;
  }
  if (screen === SCREEN.CREDITS) return creditsScreen;
  if (screen === SCREEN.CINEMATICS) return cinematicsScreen;
  if (screen === SCREEN.SELECT) return charSelect;
  if (screen === SCREEN.CREATE) return charCreate;
  return null;
};
let snapshot = normalizeSnapshot(null);
let last = performance.now(), accumulator = 0, frames = 0, fps = 0, fpsAt = last;
const STEP = 1000 / 60;
// Retail's game clock is ONE 25 Hz timer: the player, the monsters, the
// AI and the per-frame animation pass all advance 25 times a second, and nothing in the engine
// consumes wall-clock time itself. The shell runs at 60 Hz because rendering, input and the front
// end's own clocks (titleTick, char-create) are written against 60 -- so the ENGINE is advanced on
// whole 40 ms slices of that time and 2 frames in every 5 advance it not at all. It used to tick
// once per shell update, which ran the whole game 2.4x fast.
const GAME_STEP = 1000 / 25;
let gameClock = 0, engineTicks = 0;
let session = null, bootstrapStarted = false, sessionError = '', inventoryLayoutKey = '';
let currentFixture = null, fixtures = [], actSelect = null;
// A picked save starts in the town of the act its header says it is in (save-store nTown); retail's
// town level per act is DUNGEON_GetTownLevelIdFromActNo: Rogue Encampment, Lut Gholein, Kurast
// Docks, the Pandemonium Fortress, Harrogath. TODO(client-parity C3k): the game seed is fixture 1,
// not the header dwMapSeed.
let loadDifficulty = 0;   // the picked save's difficulty: which of the header's nTown bytes applies
// Debug overlays (grid, room rectangles, warp diamonds) are exactly what made this read as a test
// harness rather than a game. They are opt-in now: ?debug=1.
const DEBUG_OVERLAY = new URLSearchParams(location.search).get('debug') === '1';
// ?perf=1 turns the frame-cost instrument on from the URL (assets/frame-perf.mjs), so a real browser
// or the desktop build can be measured without injecting the flag first. Off otherwise.
if (new URLSearchParams(location.search).get('perf') === '1') globalThis.__d2Perf = true;
let terrain = null, terrainFiles = null, terrainStats = null, terrainError = '', terrainLevel = -1;
let playerRenderer = null, playerRenderError = '', motion = {};
let corpseRenderer = null, corpseEvidence = null, lastCorpseRecovery = null, missileRenderer = null, missileEvidence = [];
// Retail's weather (client-parity F11b): the rain, and the identity of every rain blit it drew this
// frame -- `{ rain, blits }`, the record shape retail's own draw lists use.
let weatherRenderer = null, weatherEvidence = { rain: -1, blits: [] };
let frameError = '', frameErrors = 0;
let npcCount = 0;
// F22: how many units the ENGINE reports as having a speech pending this frame.
let speechAlertCount = 0;
let npcPanel = null, npcPanelError = '', npcState = null, npcPanelEvidence = null;
// A long walk in progress: where we are going, what to do on arrival, and how it is going.
let walkGoal = null, walkIdle = 0, walkLast = null, walkEvidence = { active: false, hops: 0, reason: 0 };
let worldPointerHeld = false;
let heldPointerTicks = 0;
let walkMovePending = false;
// F371: the last armed goal's cell, which a same-cell re-arm carries its give-up counters across (see
// armWalk). Cleared when a walk ENDS, so a click after a finished walk starts fresh.
let walkMemory = null;
// Which handler spent each click, and what the click was aimed at. A leg of an s000 walk is a pointer
// click, so a leg that walks away from its aim is a question about what its clicks RESOLVED to: bare
// ground, an item, or a hostile unit standing on the aim point, which click dispatch acts on ahead of
// the ground (F14). Until now nothing recorded that, and a corpse run that ping-pongs between two
// points could not say whether its clicks were spent walking, fighting or picking something up.
// Diagnosis only -- no shell logic reads it back -- and the ring is small on purpose: the last few
// seconds of clicking is all a failed leg needs.
// Two fields a refused click could not answer without: WHO the player was when the click was sent and
// WHY the engine turned it down. s000c step 9's corpse run spent hundreds of world clicks that all read
// `move (..) -> 0` -- a refusal with no subject and no reason -- and the only way to learn that the hero
// was on the death screen was a separate dump of the unit list, taken per run. Measured with these two
// fields (the run in wasm-build/harness-out/act3-246-step9b.log, act III BOOTSTRAP to step 9): of the
// leg's last 400 clicks, 343 were world clicks and 75 were refused walks -- 72 of them reason 6, the
// PLAYER's dispatch refusal, against 3 reason 5, the ground's -- and 72 clicks were sent while the hero
// was in mode 17, retail's death screen. `hero` is the engine's own triple ({hp, hpMax, mode}, the shape
// lib/player.mjs' heroIsDead takes, hpMax 0 meaning no game at all), and `reason` the engine's own
// d2_session_move_reason beside a refused move. Diagnosis only, like the rest of the ring.
const clickDispatch = []; let clickDispatchN = 0;
const noteClick = (x, y, button) => {
  const record = { n: ++clickDispatchN, screen, button, at: [Math.round(x), Math.round(y)],
    hero: { hp: snapshot.player.hp, hpMax: snapshot.player.hpMax, mode: snapshot.mode },
    spent: null, aim: null, reason: null, result: null, walkTo: null };
  clickDispatch.push(record);
  if (clickDispatch.length > 400) clickDispatch.shift();
  return record;
};
// The record of the click that queued this command: the same canvas point, not answered yet.
const claimClick = (x, y) => { for (let i = clickDispatch.length - 1; i >= 0; i--) {
  const c = clickDispatch[i]; if (c.spent === 'world' && !c.result && c.at[0] === x && c.at[1] === y) return c; }
  return null; };
let lastPurchase = null, lastSale = null, lastHire = null, hireWindowEvidence = null;
let monsterRenderer = null;
let npcSpeechAlertRenderer = null;
// F22: one identity per NPC speech-alert blit drawn this frame, so a row can read the pixels back
// from the rectangle the blit actually landed in rather than from a rectangle it re-derived.
let npcAlertEvidence = [];
let areaNameRenderer = null;
// F115 (renamed from F109 at the cycle-273 merge): nothing is carried here about the area-name block.
// The NAME is read from the engine at draw time (d2_level_name), and whether the block is drawn at all
// is the automap's own state on this frame -- see the draw below.
let areaNameEvidence = null, areaNameError = '';
// s693: the level-ENTRY banner. Retail draws the arriving level's own EntryFile art (Levels.txt's
// `EntryFile` column, resolved by the engine) for 120 game frames from the arrival -- the window the
// timed capture measured, the same on both arrivals captured. `entryBannerEntry` is that arrival:
// the level id the shell last drew a world for and the engine tick it arrived on, which is also what
// makes the negative controls in s693 possible (nothing before the change, nothing after +120).
let entryBannerRenderer = null;
let entryBannerEvidence = null, entryBannerError = '', entryBannerLevel = -1, entryBannerAt = 0;
let unitDrawEvidence = [];
let objectDrawEvidence = [];
let objectRenderer = null;
let waypointPadEvidence = [];
let itemRenderer = null;
// F7: the game owns the selected unit/item; this record proves the label we drew is the
// retail string resolved from that selected object's engine identity.
let hoverEvidence = null;
let hoverFont = null, automapFont = null;
// The scenario turns this off once to prove its per-glyph identity assertion rejects a suppressed
// paint. It is otherwise always true in the game.
let hoverGlyphPainting = true;
// Retail's own in-game pointer cel (V4/F6b). Null until the session's assets are decoded.
let gameCursor = null, gameCursorEvidence = null;
// The pointer layer owns the last draw of the frame and the browser arrow with it (assets/pointer-layer.mjs).
let pointerLayer = null;
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

// V19: SkillDesc's 17 tooltip lines for a skill at a level, calcs evaluated by the engine (16 bytes each).
function readSkillDesc(id, level) {
  if (!session || typeof Module === 'undefined' || !Module._d2_session_skill_desc) return [];
  const buf = Number(Module.ccall('malloc', 'bigint', ['number'], [17 * 16]));
  try {
    if (!Module.ccall('d2_session_skill_desc', 'number', ['number', 'number', 'bigint'], [id, level, BigInt(buf)])) return [];
    const dv = new DataView(Module.HEAPU8.buffer, buf, 17 * 16);
    return Array.from({ length: 17 }, (_, i) => ({ type: dv.getUint16(i * 16, true), a: dv.getUint16(i * 16 + 2, true),
      b: dv.getUint16(i * 16 + 4, true), calcA: dv.getInt32(i * 16 + 8, true), calcB: dv.getInt32(i * 16 + 12, true) }));
  } finally { Module.ccall('free', null, ['bigint'], [BigInt(buf)]); }
}

// The belt's box rectangles, from Belts.txt through retail's own accessor. The ROW is the belt the
// player is WEARING (`d2_session_belt_index` = retail's UNITS_GetBeltType of the equipped belt, 2
// with none) -- the table read V10 landed, and what `readBeltHudBoxes` below takes the HUD row from.
// Belts.txt does not change, so the boxes are cached per row and re-read when the worn belt does.
const BELT_RECORD_NO_BELT = 2;
function readBeltIndex() {
  if (typeof Module === 'undefined' || !Module._d2_session_belt_index) return BELT_RECORD_NO_BELT;
  return Module.ccall('d2_session_belt_index', 'number', [], []);
}
function readBeltBoxes() {
  if (!session || typeof Module === 'undefined') return null;
  const record = readBeltIndex();
  if (beltBoxCache && beltBoxRecord === record) return beltBoxCache;
  const cap = 1 + 16 * 4;
  const buf = Number(Module.ccall('malloc', 'bigint', ['number'], [cap * 4]));
  if (!buf) return null;
  try {
    const n = Module.ccall('d2_session_belt_boxes', 'number', ['number', 'number', 'bigint', 'number'],
      [record, 1, BigInt(buf), cap]);
    if (!n) return null;
    const v = new DataView(Module.HEAPU8.buffer, 0, Module.HEAPU8.byteLength);
    const boxes = [];
    for (let i = 0; i < n; i++) {
      const b = buf + 4 + i * 16;
      boxes.push({ left: v.getInt32(b, true), right: v.getInt32(b + 4, true),
        top: v.getInt32(b + 8, true), bottom: v.getInt32(b + 12, true) });
    }
    beltBoxRecord = record;
    return (beltBoxCache = boxes);
  } finally { Module.ccall('free', null, ['bigint'], [BigInt(buf)]); }
}

// The boxes the HUD DRAWS. The record above is the worn belt's -- Belts.txt through retail's own
// accessor, which for a Girdle is sixteen boxes in four rows -- but retail's HUD paints only the
// bottom row: across the 41 captured scenes whose draw list shows a belt, every one records exactly
// four 28x28 belt-poticon cels, all at y=590, and no belt cel at y 558/526/494 ever. That includes
// both of ELDRA's (retail/saves/ELDRA-act5.d2s: a Girdle whose sixteen slots are all filled, so the
// four cels are four of sixteen items), and skilltree-act5's own frame buffer has no poticon pixel
// below the panel's edge. So the HUD row is the record's bottom row; the rows above are the belt's
// refill queue, not pixels.
function readBeltHudBoxes() {
  const boxes = readBeltBoxes();
  if (!boxes || !boxes.length) return boxes;
  let bottom = boxes[0].bottom;
  for (const b of boxes) if (b.bottom > bottom) bottom = b.bottom;
  return boxes.filter((b) => b.bottom === bottom);
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
// V8c: the quest log (Q) and message log (M). State is the engine's quest record; the client keeps only which
// tab and quest are showing. `messageLines` is the log's feed (nothing wired yet: V8c).
let questLogRenderer = null, messageLogRenderer = null, questVisible = false, messageVisible = false, questAct = 0, questSelected = 0, questDescrOn = false, questEvidence = null, messageEvidence = null;
const messageLines = [];
let chatEvidence = null, postedEvidence = null, postedAt = 0;   // V8c: bar/posted-line geometry evidence; the line shows for POSTED_MS after posting
const POSTED_MS = 10000;   // V8d: retail's posted-line duration, measured in F191 (posted-multi-caps: present 9.7 s, gone 10.2 s)
let chat = null;   // V8c: the Enter chat bar's text while open (null = closed)
let messageScroll = 0;   // V8d: the message log's scroll offset in lines (0 = newest line at the top)
let questOpenedAt = 0;   // V8d: the engine tick the quest log was opened on, for the completion animation
// V8d: which quest icons play the completion animation this open (the engine's QFLAG_COMPLETEDNOW), and
// which have already played it -- the client's stand-in for retail moving COMPLETEDNOW -> COMPLETEDBEFORE
// when the log closes (there is no engine export to clear the record bit; declared in client-parity.md).
const questAnim = new Set(), questPlayed = new Set();
const readQuestWords = () => { const p = Number(Module.ccall('d2_session_quest_record', 'number', [], [])); return p ? new Uint16Array(Module.HEAPU8.buffer.slice(p, p + 96)) : new Uint16Array(48); };
// An act shows a tab once any of its OWN quest words is set (its six quests, or the trailing COMPLETED
// word where it has one). Act V's words are 35..40, not 32..39 -- ACT_BASE, F411.
const questTabs = (words, act) => { let top = act; for (let a = 1; a <= 5; a++) { const end = ACT_DONE[a - 1] || QUESTS_IN_ACT[a - 1];
  for (let i = 1; i <= end; i++) if (words[ACT_BASE[a - 1] + i]) { top = Math.max(top, a); break; } }
  return Array.from({ length: Math.min(5, top) }, (_, i) => i + 1); };
const questFirst = (words, act) => { for (let q = 1; q <= QUESTS_IN_ACT[act - 1]; q++) if (questStatus(words, act, q) !== 'locked') return q; return 0; };
const toggleQuest = () => { questVisible = !questVisible; if (questVisible) { characterVisible = false; questAct = 0; questOpenedAt = engineTicks; questDescrOn = false;
  // On open, the engine's own COMPLETING quests (QFLAG_COMPLETEDNOW) play once: OpenDiablo2's
  // quest_log.go playQuestAnimations, which runs for the quests in QuestStatusCompleting.
  questAnim.clear(); const words = readQuestWords();
  for (let a = 1; a <= 5; a++) for (let q = 1; q <= QUESTS_IN_ACT[a - 1]; q++) { const key = `${a}q${q}`;
    if ((words[ACT_BASE[a - 1] + q] & COMPLETING) && !questPlayed.has(key)) { questAnim.add(key); questPlayed.add(key); } } } };
const toggleMessage = () => { messageVisible = !messageVisible; if (messageVisible) messageScroll = 0; };
// The stash is open because the engine sent 0x77 UPDATEUI_OPENSTASH, and closed when retail's
// TRADEBTN_CLOSESTASH was accepted -- never because of a click alone.
// The Horadric Cube window (P3c): a right click on the cube in the pack USES it (0x20) and its spell sends 0x77 UPDATEUI_OPENCUBE (21),
// its items live on invPage 3 and the Transmute button is TRADEBTN_TRANSMUTE (24).
let cubePanelRenderer = null, cubeOpen = false, cubeLayout = null, cubeEvidence = null;
let stashPanelRenderer = null, stashPanelError = '', stashEvidence = null, stashOpen = false, stashLayout = null;
// PlugY shared-stash page control: OPT-IN (?plugy=1; default is strict retail). Off = nothing here runs, nothing is
// read from or written to the PlugY side store.
// PY1 pages: page 0 is the engine's stash; every other page is a stored "JM" item section (opaque bytes,
// the engine stays the only game state). A page turn exports the live stash, clears it, inserts the target.
let plugyPages = [], plugyReady = false, plugyFail = { export: false, insert: false };   // plugyFail: test hooks (crash simulation)
const plugyBuf = (n, f) => { const b = Number(Module.ccall('malloc', 'bigint', ['number'], [n])); try { return f(b); } finally { Module.ccall('free', null, ['bigint'], [BigInt(b)]); } };
const plugyExport = () => plugyFail.export ? null : plugyBuf(0x4000, (b) => { const n = Module.ccall('d2_session_stash_export', 'number', ['bigint', 'number'], [BigInt(b), 0x4000]); return n > 3 ? Module.HEAPU8.slice(b, b + n) : null; });
const plugyCount = (sec) => sec[2] | (sec[3] << 8);
const plugyInsert = (sec) => plugyBuf(sec.length, (b) => { Module.HEAPU8.set(sec, b); return plugyFail.insert ? -99 : Module.ccall('d2_session_stash_insert', 'number', ['bigint', 'number'], [BigInt(b), sec.length]); });
const plugyKey = () => `_LOD_${chosenName}.sss`;
let plugyLast = Promise.resolve(), plugyError = '';
// Write every page (the live one re-exported) as the character's .sss blob; false = nothing written.
function plugyPersist(cur = plugyStash.page) {
  if (!plugyStash.enabled || !plugyReady || !chosenName) return Promise.resolve(false);
  const live = plugyExport(); if (live) plugyPages[cur] = live;
  const blob = pagesToSss(Array.from({ length: Math.max(plugyPages.length, plugyStash.pages) }, (_, i) => plugyPages[i]), cur);
  return plugyLast = saveStore.putAux(plugyKey(), blob).then(() => true, (e) => { plugyError = String(e); return false; });
}
// A page turn never loses an item: the new pages are stored BEFORE the engine stash is cleared; the insert is
// verified (item count == the section's) and any failure puts the old page back and refuses the turn.
function plugySwap(from, to) {
  if (!plugyReady) { plugyError = 'swap before load finished'; return false; }
  const E = plugyExport(); if (!E) { plugyError = 'stash export failed'; return false; }
  plugyPages[from] = E;
  plugyPersist(from);   // stored bytes (still cur=from, all pages) reach IndexedDB before the clear below
  const T = plugyPages[to];
  const put = (sec) => { const cr = Module.ccall('d2_session_stash_clear', 'number', [], []); plugyError = 'clear rc ' + cr; if (cr < 0) return false; if (!sec || sec.length <= 4) return true;
    const rc = plugyInsert(sec), back = plugyExport(); return rc === 0 && back && plugyCount(back) === plugyCount(sec); };
  if (!put(T)) { plugyFail = { export: false, insert: false }; const ok = put(E); plugyError = ok ? 'insert failed; page restored' : 'insert failed; restore failed'; return false; }
  plugyPersist(to); persistCurrentCharacter();   // blob + .d2s now agree on which page the engine holds
  return true;
}
// On entering a game: read this character's blob. Absent/corrupt -> empty pages (the engine stash is untouched).
// The engine's stash is the page the .d2s was saved on: if it equals a stored page's bytes, that page is current.
async function plugyLoad(name) {
  plugyReady = false; plugyPages = []; plugyStash.page = 0;
  let raw = null, st = null;
  try { raw = await saveStore.getAux(`_LOD_${name}.sss`); if (raw) st = sssToPages(raw); }
  catch (e) { plugyError = 'blob unreadable: ' + e; if (raw) saveStore.putAux(`_LOD_${name}.sss.bad`, raw).catch(() => {}); }
  const E = plugyExport();
  if (st && E) {
    const eq = (p) => p.length === E.length && p.every((v, k) => v === E[k]);
    let cur = st.cur; if (!eq(st.pages[cur])) { const i = st.pages.findIndex(eq); if (i >= 0) cur = i; }
    plugyPages = st.pages; plugyPages[cur] = E; plugyStash.page = cur;
  }
  plugyReady = true;
}
const plugyQuery = new URLSearchParams(location.search);
const plugyStash = createPlugyStash({ enabled: plugyQuery.get('plugy') === '1', onPage: plugySwap });
let plugyEvidence = null;
// PY3: PlugY's own art (StashBtns, SharedGoldBtns, TradeStash), fetched from the dev server's /retail/PlugY/ (never hosted; absent = the plain control and retail's panel stay).
if (plugyStash.enabled) (async () => {
  try {
    const get = async (n) => new Uint8Array(await (await fetch('/retail/PlugY/' + n + '.dc6')).arrayBuffer());
    const [btns, gold, trade] = [await get('StashBtns'), await get('SharedGoldBtns'), await get('TradeStash')];
    let pal = null;   // the MPQ mounts after boot: retry until the palette reads
    for (let t = 0; !pal && t < 600; t++) { try { pal = decodeDatPalette(createMpqAssetReader(Module)('data\\global\\palette\\ACT1\\pal.dat')); } catch { await new Promise((r) => setTimeout(r, 500)); } }
    const canv = (bytes) => decodeDc6(bytes).frames.map((f) => { const c = document.createElement('canvas'); c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
      const rgba = new Uint8ClampedArray(f.width * f.height * 4);
      for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(pal.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
      c.getContext('2d').putImageData(new ImageData(rgba, f.width, f.height), 0, 0); return c; });
    for (let t = 0; !stashPanelRenderer && t < 600; t++) await new Promise((r) => setTimeout(r, 500));
    stashPanelRenderer.setArt(trade); stashPanelRenderer.plugy.on = true; stashPanelRenderer.plugy.nameX = PLUGY_NAME_AT.x; stashPanelRenderer.plugy.nameY = PLUGY_NAME_AT.y;
    plugyStash.setArt({ stash: canv(btns), gold: canv(gold) });
  } catch (e) { plugyError = `PlugY art: ${e}`; }
})();
let stashGoldRenderer = null, stashGoldEvidence = null, stashGoldError = '', stashGoldDialog = null;
// The waypoint menu is open because the engine sent 0x63; waypointMenu holds that waypoint's unit id.
let waypointPanelRenderer = null, waypointPanelError = '', waypointEvidence = null, waypointMenu = null, lastWaypointTravel = null;
const waypointMenuInput = createWaypointMenuInput();
let beltRenderer = null, beltEvidence = null, beltBoxCache = null, beltBoxRecord = -1;
let miniDown = -1;
let controlPanelRenderer = null, controlPanelEvidence = null, hudIconRow = true, lastRunToggle = null;
let hudHelp = null, hudHelpEvidence = null, hudHelpError = '';
// F210: retail's HUD Help button (Levelsocket+Level and its "Help (H)" label) is drawn until the Help screen has been used ONCE.
// D2Client's `H` handler and the button's click handler both write the Storm registry value "Help Menu" = 1 (section "Diablo II"), and the
// button's draw (D2Client 6fae7740) reads it back and draws nothing when it is set: per machine, not per character or save, so it never
// returns. The browser's per-machine store is localStorage; `__d2Shell.resetHelpMenu()` (scenarios) shows it again.
const HELP_MENU_KEY = 'd2.helpMenu';
const helpMenuSeen = () => { try { return localStorage.getItem(HELP_MENU_KEY) === '1'; } catch { return false; } };
const markHelpMenuSeen = () => { try { localStorage.setItem(HELP_MENU_KEY, '1'); } catch { /* private window: the button just stays */ } };
// Retail's Help screen (`H`, CfgHelp): a full-screen overlay (assets/help-overlay.mjs). It is a
// SCREEN, not chrome -- ClearScreen does not hide it and it does not hide the HUD behind its border,
// which is the whole point of the screen: its labels' leader lines point at the HUD controls they
// describe. While it is open the world answers no click (OpenDiablo2 game_controls.go consumes the
// press inside the overlay's rect) and Esc closes it before anything else (its onEscKey).
let helpRenderer = null, helpVisible = false, helpEvidence = null, helpError = '';
let automapOverlay = null;
// Retail's Space (CfgClearScreen) closes open panels and hides no chrome (F3c, retail's frames), so this flag is never set;
// the chrome gates below stay as no-ops.
let clearScreen = false;
// The skill buttons beside the belt and their popup (skill-select.mjs); the engine owns the choice.
let skillSelectRenderer = null, skillSelectEvidence = null, skillSelectError = '';
function readSkillChoices() {
  if (!session || typeof Module === 'undefined' || !Module._d2_session_skill_choices) return [];
  const cap = 128, buf = Number(Module.ccall('malloc', 'bigint', ['number'], [cap * SESSION_SKILL_CHOICE_BYTES]));
  if (!buf) return [];
  try {
    const n = Module.ccall('d2_session_skill_choices', 'number', ['bigint', 'number'], [BigInt(buf), cap]);
    return parseSkillChoices(Module.HEAPU8.slice(buf, buf + n * SESSION_SKILL_CHOICE_BYTES), n);
  } finally { Module.ccall('free', null, ['bigint'], [BigInt(buf)]); }
}
function readSelectedSkills() {
  if (!session || typeof Module === 'undefined' || !Module._d2_session_selected_skills) return { left: -1, right: -1 };
  const p = Number(Module.ccall('d2_session_selected_skills', 'number', [], []));
  const v = new DataView(Module.HEAPU8.buffer, p, 8);
  return { left: v.getInt32(0, true), right: v.getInt32(4, true) };
}
// V9b: a skill's wStrName / wStrShort .tbl indices (any id, general skills included) for the button's hover plate.
function readSkillStrings(id) {
  if (!Module._d2_session_skill_strings) return null;
  const v = Module.ccall('d2_session_skill_strings', 'number', ['number'], [id]) >>> 0;
  return { name: v & 0xFFFF, short: v >>> 16 };
}
// V9b: the popup icon's plate is the tree's (buildSkillPlate) over the class tree's record for that skill.
function skillPlateState() {
  return { string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]), line: (index) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]),
    skillDesc: readSkillDesc, skillStrings: readSkillStrings, skillMana: (id, level) => Module.ccall("d2_session_skill_mana", "number", ["number", "number"], [id, level]), record: (id) => (readClassSkills() || []).find((k) => k.id === id) || null };
}
// Retail's 0x3C through the engine; the right hand's choice is also what a right click casts.
function selectSkill(id, side) {
  const rc = Module.ccall('d2_session_command_select_skill', 'number', ['number', 'number'], [id, side === 'left' ? 1 : 0]);
  return rc;
}
let actMusic = null, musicError = '';
// F235: the Sound/Music sliders' values and the players they drive, one store for the whole page --
// the options page writes it and every audio player (music and the three SFX players) is attached to
// it. Retail's own volume persistence is unknown to us; localStorage is declared
const audioVolume = createAudioVolume();
// Constructed while the title is live, before its SINGLE PLAYER click. That first genuine pointer
// gesture therefore unlocks the same AudioContext that later plays class-hover effects.
let charCreateSounds = null, charCreateSoundError = '';
// The selected skills are the ENGINE's (the unit's left/right skill, d2_session_selected_skills): a
// loaded character keeps the hands its save held, and there is no second copy here to disagree.
let lastSkillCommand = null;
// D-pad belt slots (COMMAND.BELT) had no evidence getter at all before this: a controller row
// asserting "the belt slot I sent equals the key the mouse/keyboard path would send" had nothing
// to read. Mirrors lastSkillCommand/lastItemCommand's own pattern.
let lastBeltCommand = null;
// The raw queued MOVE command (canvas x/y a ground click carried) before it is turned into a walk
// goal or consumed by an under-cursor attack/pickup -- a controller row proving the right stick's
// click lands where a mouse click would has nothing else left to read once input.queue was made
// internal-only.
let lastMoveCommand = null;
const PLAY_MODE = new URLSearchParams(location.search).get('play') === '1';
// Retail opens on a title screen, so that is what a plain visit gets. `play=1` keeps booting
// straight into a session: every browser scenario drives the game directly and must not have to
// click through a menu to reach it.
const SCREEN = { TITLE: 'title', CREDITS: 'credits', CINEMATICS: 'cinematics', SELECT: 'select', CREATE: 'create', GAME: 'game' };
let screen = PLAY_MODE ? SCREEN.GAME : SCREEN.TITLE;
let titleScreen = null, titleError = '', titleTick = 0;
let exitSeq = null;   // F91: EXIT DIABLO II's fade + close attempt + notice (assets/exit-sequence.mjs)
// The Battle.net server list, read once from the repo's own config file (`play/assets/bnet-servers.json`,
// loadBnetServers). Battle.net is OFF by default: the shipped file is empty, which resolves to the same
// off state as a missing or malformed one, and the title screen then behaves exactly as retail's does.
let bnetConfigPromise = null;
let cinematicsScreen = null, cinematicsError = '';
let creditsScreen = null, creditsError = '', creditsTick = 0, exitEvidence = { count: 0, screen: null };
// Retail's front end opens on the Blizzard logo movies -- New_Bliz, then BlizNorth -- and only then
// shows the trademark screen and the menu, which is what `titlePhase` counts through (assets/
// cinematics.mjs carries the measurement; the trademark phase is retail's own,
// the retail captures). The initial value below is replaced by setTitlePhase() at the foot
// of this block, so the phase log opens on the first movie rather than on the trademark screen.
let titlePhase = 'trademark';
let bootMovie = null, bootMovieError = '', bootMovieIndex = 0, bootMovieStart = 0, bootMovieOpening = false;
// >= 0 holds the movie on one frame for a row (pinMovieFrame); -1 is live playback.
let moviePin = -1;
let charCreate = null, charCreateError = '';
let charSelect = null, charSelectError = '', savedCharacters = [], componentTablesReady = false;
// Esc's in-game menu (retail: OPTIONS / SAVE AND EXIT GAME / RETURN TO GAME). Single player pauses
// while it is up, as retail does.
let gameMenu = null, gameMenuError = '';
// F218: retail's CONFIGURE CONTROLS panel, the Options page's fourth line. It is a screen of its own
// (retail's own draw list: boxpieces + textslid + FontInGameChat, no options-page cel anywhere in it),
// so it draws instead of the menu's page and owns the pointer and the keys while it is up.
let controlsPanel = null, controlsPanelError = '';
function ensureConfigureControls() {
  if (controlsPanel || controlsPanelError || !window.__d2?.ready) return controlsPanel;
  try {
    controlsPanel = createConfigureControls({ read: createMpqAssetReader(Module),
      palettePath: () => actPalettePath(snapshot?.act ?? 1) });
  } catch (error) { controlsPanelError = String(error); }
  return controlsPanel;
}
function ensureGameMenu() {
  if (gameMenu || gameMenuError || !window.__d2?.ready) return gameMenu;
  try {
    gameMenu = createGameMenu({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      palettePath: () => actPalettePath(snapshot?.act ?? 1),
      initialVolume: audioVolume.values, onVolume: (key, value) => { if (key === 'sound' || key === 'music') audioVolume.set(key, value); },
      onChoose: (item) => {
      if (item === 'CONFIGURE CONTROLS') { ensureConfigureControls()?.openPanel(); return; }
      if (item === 'RETURN TO GAME') gameMenu.close();
      else if (item === 'SAVE AND EXIT GAME') {
        gameMenu.close();
        // Retail writes the character and leaves the game for character select.
        persistCurrentCharacter().finally(() => { enterCharacterSelect(); });
      }
    } });
  } catch (error) { gameMenuError = String(error); }
  return gameMenu;
}
let deathScreen = null, deathScreenError = '', deathScreenEvidence = null;
function ensureDeathScreen() {
  if (deathScreen || deathScreenError || !window.__d2?.ready) return deathScreen;
  try {
    deathScreen = createDeathScreen({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      string: (index) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]) });
  } catch (error) { deathScreenError = String(error); }
  return deathScreen;
}
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
  ({ terrain, files: terrainFiles, error: terrainError } = loadLiveLevelTerrain(Module, act0));
}

function sessionAdapter() {
  if (window.Module?._d2_session_create && window.Module?._d2_session_snapshot) {
    return {
      // bare: no start kit, for a player a saved file is about to be applied to.
      create(f, playerClass = 0, bare = false) { Module.ccall('d2_session_create_stream', 'number', ['number'], [0]); return Module.ccall(bare && Module._d2_session_create_class_bare ? 'd2_session_create_class_bare' : 'd2_session_create_class', 'number', ['number', 'number', 'number', 'number', 'number'], [f.seed, f.difficulty, f.act0, f.level, playerClass]); },
      // Create the character FROM the file: the engine reads the header and everything -- class,
      // act, town level, map seed -- comes out of it (PLRSAVE2_ReadSaveHeader, client-parity C3e).
      // Returns 0, or the PLRSAVE2 error code that refused the file.
      createFromSave(bytes, difficulty) {
        const p = Module._malloc(bytes.length);
        try {
          Module.HEAPU8.set(bytes, Number(p));
          return Module.ccall('d2_session_create_from_save', 'number', ['bigint', 'number', 'number'], [BigInt(p), bytes.length, difficulty]);
        } finally { Module._free(p); }
      },
      move(x, y) { return Module.ccall('d2_session_command_move', 'number', ['number', 'number'], [x, y]); },
      attack(targetId) { return Module.ccall('d2_session_command_primary_attack', 'number', ['number'], [targetId]); },
      rightSkillAt(x, y) { return Module.ccall('d2_session_command_right_skill_at', 'number', ['number', 'number'], [x, y]); },
      throwSkill(targetId) { return Module.ccall('d2_session_command_throw', 'number', ['number'], [targetId]); },
      kick(targetId) { return Module.ccall('d2_session_command_kick', 'number', ['number'], [targetId]); },
      pickup(itemId) { return Module.ccall('d2_session_command_pickup', 'number', ['number'], [itemId]); },
      drop(itemId) { return Module.ccall('d2_session_command_drop', 'number', ['number'], [itemId]); },
      useBelt(slot) { return Module.ccall('d2_session_command_use_belt', 'number', ['number'], [slot]); },
      useItem(itemId) { return Module.ccall('d2_session_command_use_item', 'number', ['number'], [itemId]); },
      swapWeapons() { return Module._d2_session_command_swap_weapons ? Module.ccall('d2_session_command_swap_weapons', 'number', [], []) : 0; },
      operate(objectId) { return Module.ccall('d2_session_command_object', 'number', ['number'], [objectId]); },
      talk(npcId) { return Module.ccall('d2_session_command_npc', 'number', ['number'], [npcId]); },
      talkNearest() { return Module.ccall('d2_session_command_nearest_npc', 'number', ['number'], [6]); },
      travelNearest() { const npc = Module._d2_session_nearest_travel_npc ? Module.ccall('d2_session_nearest_travel_npc', 'number', ['number'], [6]) : 0; return npc ? Module.ccall('d2_session_command_travel', 'number', ['number'], [npc]) : 0; },
      interact() { return Module.ccall('d2_session_command_nearest_warp', 'number', ['number'], [6]); },
      respawn() { return Module.ccall('d2_session_command_respawn', 'number', [], []); },
      tick(n) { return Module.ccall('d2_session_tick', 'number', ['number'], [n]); },
      // The player's path position at the engine's own 16.16 precision (d2_session_player_coords),
      // the value retail's client draws with. Rendering only ever saw the whole subtile, which is
      // what quantised the drawn world to 16 px steps at 25 Hz (s490).
      playerCoords() {
        if (!Module._d2_session_player_coords) return null;
        const p = Number(Module.ccall('d2_session_player_coords', 'number', [], []));
        if (!p) return null;
        const view = new DataView(Module.HEAPU8.buffer, p, 16);
        if (view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 16) return null;
        return { x: view.getUint32(8, true) / 65536, y: view.getUint32(12, true) / 65536 };
      },
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
        // One world rebuild for the dozen reads below (d2_session_hold_world), not one per read.
        const hold = Boolean(Module._d2_session_hold_world);
        if (hold) Module.ccall('d2_session_hold_world', null, ['number'], [1]);
        try { return this.readSnapshot(); } finally { if (hold) Module.ccall('d2_session_hold_world', null, ['number'], [0]); }
      },
      readSnapshot() {
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
        // The same units' 16.16 positions, one record each in the same order (d2_session_unit_coords).
        // The id is checked rather than trusted: this is an index join, and a record that does not
        // name the unit it landed on is dropped back to the whole subtile instead of drawn.
        if (Module._d2_session_unit_coords && Module._d2_session_unit_coord_count) {
          const coordCount = Module.ccall('d2_session_unit_coord_count', 'number', [], []);
          const coordPtr = Number(Module.ccall('d2_session_unit_coords', 'number', [], []));
          const coords = new DataView(Module.HEAPU8.buffer, coordPtr, coordCount * 12);
          for (let i = 0; i < coordCount && i < entities.length; i++) {
            if (coords.getUint32(i * 12, true) !== entities[i].id) continue;
            const x = coords.getUint32(i * 12 + 4, true), y = coords.getUint32(i * 12 + 8, true);
            if (x || y) { entities[i].preciseX = x / 65536; entities[i].preciseY = y / 65536; }
          }
        }
        if (Module._d2_session_monster_animations) {
          const animationCount = Module.ccall('d2_session_monster_animation_count', 'number', [], []);
          const animationPtr = Number(Module.ccall('d2_session_monster_animations', 'number', [], []));
          const byId = new Map(parseMonsterAnimations(Module.HEAPU8.subarray(animationPtr, animationPtr + animationCount * 92), animationCount).map((a) => [a.id, a]));
          for (const entity of entities) if (entity.type === 1) entity.animation = byId.get(entity.id);
        }
        // The missile's own frame clock and facing, per unit, so the CelFile is drawn in the state the
        // ENGINE holds it in (F20) rather than at a frame guessed from the shell's tick.
        if (Module._d2_session_missile_animations) {
          const count = Module.ccall('d2_session_missile_animation_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_missile_animations', 'number', [], []));
          const byId = new Map(parseMissileAnimations(Module.HEAPU8.subarray(ptr, ptr + count * 24), count).map((a) => [a.id, a]));
          for (const entity of entities) if (entity.kind === 'missile') entity.missile = byId.get(entity.id);
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
          const op = Number(Module.ccall('d2_session_warp_origins', 'number', [], []));
          warps = parseSessionWarps(Module.HEAPU8.subarray(ptr, ptr + count * 68), count, new Int32Array(Module.HEAPU8.buffer.slice(op, op + count * 8)));
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

// Fixtures are harness input, not a retail game control.  Keep the switch available to the
// direct-to-game developer route that scenario rows use, but do not even create its DOM on the
// title -> character -> game route a player follows.
function installFixtureSelect() {
  if (!PLAY_MODE || actSelect) return;
  const label = document.createElement('label');
  label.id = 'act-select-label';
  label.append('World ');
  actSelect = document.createElement('select');
  actSelect.id = 'act-select';
  actSelect.disabled = true;
  label.append(actSelect);
  sessionBar.append(label);
  actSelect.addEventListener('change', resetFixtureFromSelect);
}

function resetFixtureFromSelect() {
  if (!session || !actSelect) return;
  const fixture = fixtures[Number(actSelect.value)];
  if (!fixture) return;
  session.destroy();
  if (!session.create(fixture, Number(classSelect.value))) {
    sessionError = `unable to create Act ${fixture.act0 + 1} session`; session = null; return;
  }
  currentFixture = fixture;
  snapshot = session.snapshot();
  motion = session.motion();
  actMusic?.prepare(fixture.act0).catch((error) => { musicError = String(error); });
  if (TERRAIN_ENABLED) loadSessionTerrain(fixture.act0);
}

async function bootstrapSession() {
  bootstrapStarted = true;
  try {
    fixtures = DEFAULT_SESSION_FIXTURES;
    if (!fixtures.length) throw new Error('monspawn vectors contain no playable fixtures');
    installFixtureSelect();
    if (actSelect) {
      actSelect.replaceChildren(...fixtures.map((fixture, index) => Object.assign(document.createElement('option'), {
        value: String(index), textContent: fixture.label || `Act ${fixture.act0 + 1} · Level ${fixture.level}`,
      })));
      actSelect.disabled = false;
    }
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
    const api = sessionAdapter();
    let fixture;
    if (pendingLoad) {
      // A picked character IS its file: the engine reads the header and creates the unit, the act,
      // the town and the map seed out of it (PLRSAVE2_ReadSaveHeader), so nothing here decodes the
      // bytes and the class dropdown has no say. The fixture this session reports is what the
      // engine made, read back from its snapshot.
      const rc = api?.createFromSave(pendingLoad, loadDifficulty);
      lastLoad = { attempted: true, pending: false, rc, bytes: pendingLoad.length };
      if (rc !== 0) throw new Error(`loading ${chosenName || 'the saved character'} failed with PLRSAVE2 error ${rc}`);
      pendingLoad = null;
      session = api; snapshot = api.snapshot(); gameClock = 0;
      fixture = Object.freeze({ seed: snapshot.seed, difficulty: loadDifficulty, act0: snapshot.act - 1, level: snapshot.level });
      classSelect.value = String(snapshot.playerClass);   // the dropdown shows the class the file chose
    } else {
      fixture = fixtures[0];
      if (!api?.create(fixture, Number(classSelect.value))) throw new Error('d2_session_create failed');
      session = api; snapshot = api.snapshot(); gameClock = 0;   // a new game starts its own 25 Hz clock
    }
    currentFixture = fixture;
    if (plugyStash.enabled) plugyLoad(chosenName).catch((e) => { plugyError = 'load: ' + e; plugyReady = true; });
    motion = api.motion();
    clearScreen = false;   // a new game starts with its HUD on screen, whatever the last one hid
    helpVisible = false;   // ... and with no panel of the last game's still up
    const assetReader = createMpqAssetReader(Module);
    // The client already uses Units for the retail font renderer in its front end and NPC menus;
    // Font16's glyphs therefore remain on a loaded, 8-bit retail palette in the browser.
    const hoverPalette = decodeDatPalette(assetReader(FRONTEND.palette));
    hoverFont = createFont({ dc6: decodeDc6(assetReader('data\\local\\font\\latin\\font16.dc6')),
      tbl: decodeFontTbl(assetReader('data\\local\\font\\latin\\font16.tbl')), palette: hoverPalette,
      documentObject: document, path: 'data\\local\\font\\latin\\font16.dc6', drawAs: { op: 'celcolor', mode: 5 } });
    automapFont = createFont({ dc6: decodeDc6(assetReader('data\\local\\font\\latin\\font6.dc6')),
      tbl: decodeFontTbl(assetReader('data\\local\\font\\latin\\font6.tbl')), palette: hoverPalette,
      documentObject: document, path: 'data\\local\\font\\latin\\font6.dc6', drawAs: { op: 'celcolor', mode: 5 } });
    gameCursor = createGameCursor({ read: assetReader, decodeDc6, documentObject: document });
    pointerLayer = createPointerLayer({ gameCursor,
      cursorItem: (c, w, h, item, x, y) => inventoryPanelRenderer?.drawCursorItem(c, w, h, item, x, y) });
    // In the world a unit casts its shadow (F8); the corpse renderer below does not, and retail's own
    // draw lists carry no `shadow dcc:` for a DD-mode sprite.
    playerRenderer = createPlayerRenderer(assetReader, document, { shadow: true }); playerRenderError = '';
    missileRenderer = createMissileRenderer(assetReader, document, (cls) => Module._d2_missile_celfile ? Module.ccall('d2_missile_celfile', 'string', ['number'], [cls]) : '');
    weatherRenderer = createWeatherRenderer({ read: createMpqAssetReader(Module), documentObject: document });
    corpseRenderer = createPlayerRenderer(assetReader);   // its own instance: the player's draw evidence stays the player's
    monsterRenderer = createMonsterRenderer(createMpqAssetReader(Module), document, { shadow: true });
    npcSpeechAlertRenderer = createNpcSpeechAlertRenderer(assetReader);
    areaNameRenderer = createAreaNameBlockRenderer(assetReader, document);
    entryBannerRenderer = createEntryBanner({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette, documentObject: document });
    entryBannerLevel = -1; entryBannerAt = 0; entryBannerEvidence = null;
    // Objects cast their shadow the same way a unit does, from their own COF layers (V3's second half):
    // retail draws `shadow dcc:objects\...` immediately before the matching `cel dcc:` of the same
    // path, frame and anchor.
    objectRenderer = createObjectRenderer(createMpqAssetReader(Module), document, { shadow: true });
    itemRenderer = createItemRenderer(createMpqAssetReader(Module));
    inventoryPanelRenderer = createInventoryPanelRenderer(createMpqAssetReader(Module));
    characterPanelRenderer = createCharacterPanelRenderer(createMpqAssetReader(Module));
    skillPanelRenderer = createSkillPanelRenderer(createMpqAssetReader(Module), document, createSkillPointOverlay());
    try { questLogRenderer = createQuestLogRenderer(createMpqAssetReader(Module), document); messageLogRenderer = createMessageLogRenderer(createMpqAssetReader(Module), document); } catch (error) { sessionError = String(error); }
    try { stashPanelRenderer = createStashPanelRenderer(createMpqAssetReader(Module), document, { string: (i) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [i]),
      stashGold: (limit) => Module._d2_session_stash_gold ? Module.ccall('d2_session_stash_gold', 'number', ['number'], [limit]) : 0 }); } catch (error) { stashPanelError = String(error); }
    try { cubePanelRenderer = createStashPanelRenderer(createMpqAssetReader(Module), document, { page: INVPAGE_CUBE, string: null,
      transmute: Module.ccall('d2_lang_string', 'string', ['string'], ['Transmute']) || 'Transmute' }); } catch (error) { stashPanelError += ' cube: ' + error; }
    try { stashGoldRenderer = createStashGoldRenderer(createMpqAssetReader(Module), document, { string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]) }); } catch (error) { stashGoldError = String(error); }
    try { waypointPanelRenderer = createWaypointPanelRenderer(createMpqAssetReader(Module), document, { string: (i) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [i]) }); } catch (error) { waypointPanelError = String(error); }
    beltRenderer = createBeltRenderer(createMpqAssetReader(Module));
    controlPanelRenderer = createControlPanelRenderer(assetReader);
    try { hudHelp = createHudHelp(createMpqAssetReader(Module)); } catch (error) { hudHelp = null; hudHelpError = String(error); }
    // The help screen's text is the engine's string table read by KEY, the way this screen's own
    // assets name it (Strhelp1..StrHelp22, strClose, strlvlup, strnewskl).
    try { helpRenderer = createHelpOverlayRenderer(createMpqAssetReader(Module), (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key])); helpError = ''; }
    catch (error) { helpRenderer = null; helpError = String(error); }
    try { automapOverlay = createAutomapOverlay({ read: assetReader, Module, isGameplay: () => screen === SCREEN.GAME }); } catch (error) { sessionError = String(error); }
    // The character's saved automap (F186/F268). ?ma0=<url> is the dev shortcut that seeds a layer from
    // a file the page can fetch; otherwise a picked character reads the `<character>.ma0` the save store
    // holds beside its .d2s (written at Save and Exit), so the player's map survives the session.
    try {
      const u = new URLSearchParams(location.search).get('ma0');
      if (u && automapOverlay) automapOverlay.setSaved(parseMa0(new Uint8Array(await (await fetch(u)).arrayBuffer())));
      else if (automapOverlay && chosenName) { const raw = await saveStore.getAux(automapAuxKey(chosenName)); if (raw) automapOverlay.setSaved(parseMa0(raw)); }
    } catch (error) { sessionError = String(error); }
    try { skillSelectRenderer = createSkillSelectRenderer(createMpqAssetReader(Module)); } catch (error) { skillSelectError = String(error); }
    try { actMusic = createActMusicPlayer(assetReader); await actMusic.prepare(fixture.act0); musicError = ''; } catch (error) { musicError = String(error); }
    audioVolume.attach('music', actMusic);
    try { audioVolume.attach('sound', createUiClickSoundPlayer(assetReader, canvas)); } catch (error) { musicError = String(error); }
    try { unitSounds = createUnitSoundPlayer(assetReader); canvas.__d2UnitSound = unitSounds; audioVolume.attach('sound', unitSounds); } catch (error) { musicError = String(error); }

    if (TERRAIN_ENABLED) loadSessionTerrain(fixture.act0);
    // A picked character is not applied to anything: createFromSave above already made it, sections
    // and all, in the act and at the map seed its own header names.
    window.__d2.session = { fixture, snapshot: () => session.snapshot(), move: (x, y) => session.move(x, y), attack: (targetId) => session.attack(targetId), throwSkill: (targetId) => session.throwSkill(targetId), kick: (targetId) => session.kick(targetId), pickup: (itemId) => session.pickup(itemId), drop: (itemId) => session.drop(itemId), useBelt: (slot) => session.useBelt(slot), operate: (objectId) => session.operate(objectId), talk: (npcId) => session.talk(npcId), travelNearest: () => session.travelNearest(), respawn: () => session.respawn(),
      interact: () => { const previousAct = snapshot.act, accepted = session.interact(); if (accepted) { snapshot = session.snapshot(); motion = session.motion(); if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadSessionTerrain(snapshot.act - 1); } return accepted; },
      tick: (n = 1) => session.tick(n) };
    // A newly created character is written straight away, so it is in the list the moment it
    // exists rather than only after some later autosave the player never triggers.
    autosaveAt = 0; autosaveAct = snapshot.act; autosaveInFlight = false;
    if (chosenName) persistCurrentCharacter();
  } catch (error) { sessionError = String(error?.stack || error); }
}

classSelect.addEventListener('change', () => {
  if (!session) return;
  const fixture = currentFixture;
  if (!fixture) return;
  session.destroy();
  if (!session.create(fixture, Number(classSelect.value))) { sessionError = 'unable to create character session'; session = null; return; }
  snapshot = session.snapshot(); motion = session.motion(); playerRenderError = '';
});

/**
 * Retail's boot films, stepped by the wall clock: this is the ONE front-end thing that must run from
 * the shell's timer and not only from the frame loop, because a background or headless tab runs timers
 * and never runs a frame (see the setInterval below -- it is the same trap the trademark countdown was
 * fixed for, and it is what the test runner's tabs do: the first s750 run sat on the first film for two
 * minutes because update() never ran). `ensureBootMovie` opens the phase's file; the film ends when its
 * own header says it does. Both paths call this, and `step` is re-entrancy guarded.
 */
function stepBootMovie() {
  ensureBootMovie();
  if (!bootMovie || !bootMovieStart || moviePin >= 0) return;
  const elapsed = performance.now() - bootMovieStart;
  bootMovie.step(elapsed);
  if (elapsed >= bootMovie.durationMs) advanceTitlePhase();
}

// The boot movie the current phase owns, opened once and kept until the phase changes. It reads the
// retail MPQ, so like the title screen it cannot open until the engine is up; `bootMovieStart` is the
// clock its frames are paced against, set when the decode is actually ready.
function ensureBootMovie() {
  const spec = bootMovieSpec(titlePhase);
  if (!spec || bootMovie || bootMovieOpening || bootMovieError) return bootMovie;
  // The engine failed: there is no movie to be had, so the phase would sit black forever. Move on --
  // the trademark screen reports the same failure in its own evidence.
  if (window.__d2?.ready && window.__d2.error) { advanceTitlePhase(); return null; }
  if (!window.__d2?.ready) return null;
  bootMovieOpening = true;
  openMovie({ read: createMpqAssetReader(Module), path: spec.path, documentObject: document })
    .then((movie) => { bootMovie = movie; bootMovieStart = performance.now(); })
    // A movie that will not open (no retail MPQs on this box, a truncated file) must not strand the
    // front end on black: the error is kept for evidence and the flow moves on to the next phase.
    .catch((error) => { bootMovieError = `${spec.path}: ${error}`; advanceTitlePhase(); })
    .finally(() => { bootMovieOpening = false; });
  return null;
}

// The title screen needs the retail MPQs, so it cannot be built until the engine is up.
//
// `entryPhase` is which front-end phase the rebuilt screen opens in. It is null on the boot path --
// the boot phases were entered at page load (setTitlePhase at the foot of the phase block) and must
// not be restarted here -- and only exitToTitle() passes anything else: a return to the front end is
// not a boot, so it must not replay the films or the copyright screen (see exitToTitle).
function ensureTitleScreen(entryPhase = null) {
  if (screen !== SCREEN.TITLE || titleScreen || titleError) return;
  if (!window.__d2?.ready || window.__d2.error) return;
  // Audio is progressive enhancement: an unavailable Web Audio implementation must not prevent
  // the retail title screen from existing (the player still gets an explicit sound error in its
  // evidence rather than a dead front door).
  try { charCreateSounds ??= createClassSelectSoundPlayer(createMpqAssetReader(Module)); audioVolume.attach('sound', charCreateSounds); charCreateSoundError = ''; }
  catch (error) { charCreateSoundError = String(error); }
  try {
    titleScreen = createTitleScreen({
      read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      string: (index) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]),
      // Under the hood, not a player setting: the page reads the repo's server-list file at boot.
      bnet: (bnetConfigPromise ??= loadBnetServers()),
      items: [
        { label: 'SINGLE PLAYER', action: () => { enterCharacterSelect(); } },
        // TODO(client-parity C2/C3): BATTLE.NET and OTHER MULTIPLAYER need a network backend.
        // With no servers configured BATTLE.NET stays inert, exactly as before. With servers
        // configured it opens the region dialog and points the client at the picked region; it
        // still dials nothing itself (F89: no retail capture of the screens behind it).
        { label: 'BATTLE.NET', action: () => { titleScreen?.openGateways(); } },
        { label: 'OTHER MULTIPLAYER', action: () => {} },
        { label: 'CREDITS', action: () => { creditsTick = 0; screen = SCREEN.CREDITS; ensureCreditsScreen(); } },
        { label: 'CINEMATICS', action: () => { screen = SCREEN.CINEMATICS; ensureCinematicsScreen(); } },
        { label: 'EXIT DIABLO II', action: () => { beginExit(); } },
      ],
    });
    if (entryPhase) setTitlePhase(entryPhase);
  } catch (error) { titleError = String(error); }
}

function ensureCinematicsScreen() {
  if (screen !== SCREEN.CINEMATICS || cinematicsScreen || cinematicsError || !window.__d2?.ready || window.__d2.error) return;
  try { cinematicsScreen = createCinematicsScreen({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette, onExit: () => { cinematicsScreen = null; exitToTitle(); },
    string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]) }); }
  catch (error) { cinematicsError = String(error); }
}

function ensureCreditsScreen() {
  if (screen !== SCREEN.CREDITS || creditsScreen || creditsError || !window.__d2?.ready || window.__d2.error) return;
  try { creditsScreen = createCreditsScreen({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette, onExit: exitToTitle,
    string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]) }); }
  catch (error) { creditsError = String(error); }
}

// P8i: retail's tooltip property lines from the engine (d2_session_item_lines), cached per item id.
const propCache = new Map();
function itemProps(id) {
  if (propCache.has(id)) return propCache.get(id);
  const buf = (itemProps.buf ||= Number(Module.ccall('malloc', 'bigint', ['number'], [512])));
  const n = Module.ccall('d2_session_item_lines', 'number', ['number', 'bigint', 'number'], [id, BigInt(buf), 512]);
  // A line may start "\x01<digit>": its D2Win colour (P8j: 1 red unmet requirement, 3 blue magical property); none = white.
  // "\x01v" (P8j): the label up to ": " is white and the value blue -- a damage line an affix modifies.
  const raw = n ? Module.UTF8ToString(buf).split('\n').filter(Boolean) : [];
  const lines = raw.map((l) => l.replace(/^\x01./, '')), colours = raw.map((l) => (l[0] === '\x01' ? (l[1] === 'v' ? 'v' : +l[1]) : 0));
  const tb = (itemProps.tbuf ||= Number(Module.ccall('malloc', 'bigint', ['number'], [128])));
  const title = Module.ccall('d2_session_item_title', 'number', ['number', 'bigint', 'number'], [id, BigInt(tb), 128]) ? Module.UTF8ToString(tb) : '';
  const r = { lines, colours, title, quality: Module.ccall('d2_session_item_quality', 'number', ['number'], [id]) };
  if (n) propCache.set(id, r);
  return r;
}

// The tooltip fields of an item (shared by the shop grid and __d2Shell.tooltipSpec, the s2160 probe of any item id).
function tipFields(id, codeText) {
  const p = itemProps(id);
  return { name: p.title || Module.ccall('d2_lang_string', 'string', ['string'], [codeText]) || codeText, quality: p.quality, props: p.lines, propColours: p.colours,
    costLabel: Module.ccall('d2_lang_string_by_index', 'string', ['number'], [3329]) };
}

// Return to the front end's MENU: what the CREDITS and CINEMATICS screens' own EXIT/CANCEL do, and
// what Escape out of the credits does (the same primitive).
//
// It comes back AT THE MENU, not at the trademark screen. Retail's trademark screen is a startup
// phase -- it is what the front end opens on -- and every path that reaches this function is a
// RETURN to the front end, not a boot. Replaying the phase here would cost a player ten seconds of
// copyright screen on a normal navigation, which is what it did before the phase was parameterised
// (Codex review of title-b1; asserted by s198 and s287).
//
// The main menu's own EXIT DIABLO II does NOT come through here: that button is retail's app exit,
// and beginExit() is its best browser equivalent (assets/exit-sequence.mjs, F91).
function exitToTitle() {
  screen = SCREEN.TITLE; titleScreen = null; titleError = ''; titleTick = 0;
  exitEvidence = { count: exitEvidence.count + 1, screen };
  ensureTitleScreen('menu');
}

// F91: EXIT DIABLO II -- retail ends the process here; a page cannot, so it runs the fade, tries
// `window.close()` and says so when the browser refuses (assets/exit-sequence.mjs has the reasoning).
function beginExit() {
  if (exitSeq?.active) return;
  exitSeq = createExitSequence({ font: titleScreen?.noticeFont ?? null });
  exitSeq.start();
  exitEvidence = { count: exitEvidence.count + 1, screen };
}

// Retail's front end opens on the trademark screen (the copyright notice), which it holds for ten
// seconds before the menu -- and a click ends it early (OpenD2 Modcode/Client/UI/Menus/Trademark.cpp,
// `m_timeRemaining = 10000`, a click sets it to 0). Measured on retail 1.13c: with the phase's own
// first frame as the origin the menu is up 9.96 s later, so the countdown is the 10000 ms and not a
// dwell the capture invented.
//
// A timer, never a frame count: update() only runs from requestAnimationFrame, which a hidden tab
// throttles to nothing (see the setInterval note above), and a countdown that freezes with the frame
// loop would leave a page opened in a background tab stuck on the trademark screen for as long as it
// stayed hidden. `titlePhaseLog` is the page's own record of the phases it entered and when, so a row
// can measure the countdown without depending on when its own poll arrived (s198).
const TRADEMARK_MS = 10_000;
let titlePhaseLog = [], titlePhaseTimer = 0;
/** The boot movie a phase owns, or null when the phase is one of the screens (assets/cinematics.mjs). */
const bootMovieSpec = (phase) => BOOT_MOVIES.find((m) => m.phase === phase) ?? null;

function setTitlePhase(phase) {
  const movie = bootMovieSpec(phase);
  titlePhase = phase;
  titlePhaseLog.push({ phase, at: performance.now(), movie: movie ? movie.path : null });
  clearTimeout(titlePhaseTimer); titlePhaseTimer = 0;
  moviePin = -1;   // a row's pin belongs to the movie it was taken on, never to the next phase
  // A film belongs to its phase. Dropping it here is what stops a FINISHED film from advancing the
  // phase again on the next timer tick -- a movie that has run out stays "elapsed past its duration"
  // forever, so leaving it attached cut the trademark screen off 200 ms after the second film ended
  // (s198's phase log: blizzard -> blizzard-north -> trademark -> menu, trademark held 200 ms).
  // The new phase's own file opens on the next step (ensureBootMovie), with its clock starting when
  // the decode is actually ready.
  bootMovie = null; bootMovieStart = 0;
  if (movie) return;
  if (phase !== 'trademark') return;
  titlePhaseTimer = setTimeout(() => { if (titlePhase === 'trademark') setTitlePhase('menu'); }, TRADEMARK_MS);
}

/** Retail's boot, step by step: the logo movies, then the trademark screen, then the menu. */
function advanceTitlePhase() {
  const i = BOOT_MOVIES.findIndex((m) => m.phase === titlePhase);
  if (i >= 0) { setTitlePhase(BOOT_MOVIES[i + 1]?.phase ?? 'trademark'); return true; }
  if (titlePhase === 'trademark') { setTitlePhase('menu'); return true; }
  return false;
}

// Any click advances the phase: retail skips a logo movie and ends the trademark screen early on a
// click, and the trademark screen swallows the click rather than passing it to the menu behind it
// (there are no buttons yet). In the menu there is nothing to advance, so this reports false and the
// click falls through to the menu's own controls.
function titleAdvance() {
  if (screen !== SCREEN.TITLE) return false;
  return advanceTitlePhase();
}

// The page opens on retail's first logo movie. The movie FILE opens only once the retail MPQs are up
// (ensureBootMovie) -- until then the phase is the black retail itself shows while the game
// initialises (~6 s measured), not a stand-in for the film.
if (!PLAY_MODE) setTitlePhase(BOOT_MOVIES[0].phase);

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

// DELETE CHARACTER (client-parity C3i): through the confirmation the screen draws, then the store.
async function deleteSavedCharacter(name) {
  const result = await deleteAndRefreshCharacter(saveStore, name);
  if (result.status !== 'deleted') { charSelectError = result.error; return result; }
  savedCharacters = result.characters;
  charSelect?.setCharacters(savedCharacters);
  return result;
}

// CONVERT TO EXPANSION: the engine rewrites the classic file (d2_session_convert_expansion), the store keeps it.
async function convertSavedCharacter(name) {
  try {
    const bytes = await saveStore.get(name), header = savedCharacters.find((ch) => ch.name === name);
    if (!bytes || !header) throw new Error(`no saved character named ${name}`);
    Module.ccall('d2_session_set_character_name', 'number', ['string'], [name]);
    const inP = Module._malloc(bytes.length), cap = 16384, outP = Module._malloc(cap);
    let n;
    try {
      Module.HEAPU8.set(bytes, Number(inP));
      n = Module.ccall('d2_session_convert_expansion', 'number', ['bigint', 'number', 'number', 'bigint', 'number'], [BigInt(inP), bytes.length, header.difficulty ?? 0, BigInt(outP), cap]);
      if (n > 0) await saveStore.put(Module.HEAPU8.slice(Number(outP), Number(outP) + n));
    } finally { Module._free(inP); Module._free(outP); }
    if (n <= 0) { charSelectError = `convert failed (${n})`; return; }
    savedCharacters = await saveStore.list();
    charSelect?.setCharacters(savedCharacters);
  } catch (error) { charSelectError = String(error); }
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
      onDelete: deleteSavedCharacter,
      onConvert: convertSavedCharacter,
      // The portrait's item components: the engine's own component table (needs the data tables).
      componentToken: (i) => { if (!componentTablesReady) componentTablesReady = Boolean(Module.ccall('d2_drlg_init', 'number', [], [])); return Module._d2_component_token ? Module.ccall('d2_component_token', 'string', ['number'], [i]) : ''; },
      // DELETE CHARACTER's confirmation text is retail's own string table (numeric labels YesLabel,
      // NoLabel, DelCharConfLabel: see DELETE_STRINGS).
      string: (index) => Module._d2_lang_string_by_index ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]) : '',
    });
  } catch (error) { charSelectError = String(error); }
}

// Write the live character to the store. Internal twin of __d2Shell.persistCharacter, so the
// boot path and the scenario surface cannot drift apart.
async function persistCurrentCharacter() {
  const bytes = saveCurrentCharacterBytes();
  if (!bytes) return null;
  try { plugyPersist(); const header = await saveStore.put(bytes); await persistAutomap(header?.name); await plugyLast; savesWritten++; lastSaveError = ''; return header; }
  catch (error) { lastSaveError = String(error); return null; }
}

// The character's saved automap, retail's `<character>.ma0`, written beside the .d2s at Save and Exit
// (F268). The layer is the CLIENT's -- a saved cell must not be lost when the room that revealed it is
// freed -- so the shell hands the overlay's cells to the engine, which composes the FILE
// (d2_session_save_ma0 "The .ma0 layout") exactly as it composes the .d2s. The save
// store carries it, keyed the way PlugY's side blob is (`_LOD_<name>.ma0`), so it comes back with the
// character at load. A layer with no cells writes nothing rather than a valid-but-empty file over one
// the character already had.
const automapAuxKey = (name) => `_LOD_${name}.ma0`;
function saveAutomapBytes() {
  if (!automapOverlay || !Module._d2_session_save_ma0) return null;
  const cells = automapOverlay.savedCells();
  if (!cells.length) return null;
  const cap = 0x10000, inP = Module._malloc(cells.length * 4), outP = Module._malloc(cap);
  try {
    Module.HEAP32.set(Int32Array.from(cells), inP >> 2);
    const n = Module.ccall('d2_session_save_ma0', 'number', ['bigint', 'number', 'bigint', 'number'], [BigInt(inP), cells.length / 5, BigInt(outP), cap]);
    return n > 0 ? Module.HEAPU8.slice(Number(outP), Number(outP) + n) : null;
  } finally { Module._free(inP); Module._free(outP); }
}
async function persistAutomap(name) {
  if (!name) return false;
  try { const bytes = saveAutomapBytes(); if (!bytes) return false; await saveStore.putAux(automapAuxKey(name), bytes); return true; }
  catch (error) { lastSaveError = lastSaveError || String(error); return false; }
}

// Saving the engine-owned session is synchronous; only browser storage is asynchronous. Keeping
// this boundary explicit lets beforeunload take an exact final .d2s before the session is destroyed.
function saveCurrentCharacterBytes() {
  if (!session) return null;
  const cap = 16384, p = Module._malloc(cap);
  try {
    const n = Module.ccall('d2_session_save_d2s', 'number', ['bigint', 'number'], [BigInt(p), cap]);
    if (n <= 0) { lastSaveError = `save failed (${Module.ccall('d2_session_save_d2s_error', 'number', [], [])})`; return null; }
    return Module.HEAPU8.slice(Number(p), Number(p) + n);
  } finally { Module._free(p); }
}

// Picking a saved character: name it, then let the engine create it FROM the file. The bytes alone
// decide the class, act, town and map seed (client-parity C3e); the name is retail's own check --
// a character joins a game as the name it logged in with. The listing fields read here are for the
// cards on screen; nothing they say reaches the session the engine builds.
async function playSavedCharacter(name) {
  let bytes = null;
  try { bytes = await saveStore.get(name); }
  catch (error) { charSelectError = String(error); return; }
  if (!bytes) { charSelectError = `no saved character named ${name}`; return; }
  const header = savedCharacters.find((ch) => ch.name === name);
  chosenClass = header ? header.playerClass : 0;
  loadDifficulty = header?.difficulty ?? 0;
  chosenName = name;
  pendingLoad = bytes;
  lastLoad = { attempted: false, pending: true, rc: null, bytes: bytes.length };
  screen = SCREEN.GAME;
  startGame();
}

// The creation screen's Expansion and Hardcore checkboxes are retail's CLIENT SAVE FLAGS, not a
// second piece of browser state: they decide the game's mode and go into the .d2s header the engine
// writes (PLRSAVE2_WriteSaveHeader), and its loader compares them against the game a character joins
// (PLRSAVE2_CheckPlayerFlags). So the screen reports them and the ENGINE holds them -- set before
// the session is created, exactly like the character's name (client-parity C2c).
let lastClientSaveFlags = null;
function applyClientSaveFlags(states) {
  if (!window.Module?._d2_session_set_client_flags) return null;
  const expansion = states?.expansion !== false;   // retail's creation default: Expansion checked
  const hardcore = states?.hardcore === true;
  const ok = Module.ccall('d2_session_set_client_flags', 'number', ['number', 'number'], [expansion ? 1 : 0, hardcore ? 1 : 0]);
  lastClientSaveFlags = ok ? Module.ccall('d2_session_client_flags', 'number', [], []) : null;
  return lastClientSaveFlags;
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
      string: (index) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]),
      onCreate: ({ playerClass, name, checkboxes }) => {
        chosenClass = playerClass; chosenName = name;
        applyClientSaveFlags(checkboxes);
        screen = SCREEN.GAME;
        startGame();
      },
      onCancel: () => { screen = SCREEN.TITLE; },
      onSound: ({ key, trigger }) => {
        charCreateSounds?.play(key, trigger).catch((error) => { charCreateSoundError = String(error); });
      },
    });
  } catch (error) { charCreateError = String(error); }
}

// What each unit is, from the engine: bit 0 talkable NPC, bit 1 hostile (MonStats alignment).
// Town NPCs are UNIT_MONSTER like everything else, so without this the client paints Charsi -- and
// the Rogue guards -- as enemies.
// Read the live NPC conversation out of the engine: who we are talking to, what they said, and
// their stock if a trade is open. All of it is engine state; the panel only draws it.
// Retail plays an NPC's message list in order: each dismissal acks the message and shows the next (npcMsgSkip.n = how many were heard).
let npcMsgSkip = { npcId: 0, n: 0 };
function readNpcState() {
  try {
    const dialogPtr = Number(Module.ccall('d2_session_npc_dialog', 'number', [], []));
    if (!dialogPtr) { npcMsgSkip = { npcId: 0, n: 0 }; return null; }
    const view = new DataView(Module.HEAPU8.buffer, dialogPtr, 64);
    const npcId = view.getUint32(16, true);
    // THE RECORD IS THE LAST ACTIVATION, NOT A LIVE CONVERSATION, and its own verdict says so:
    // `d2_session_command_npc` writes `npcId` AND `result` on the full path, and `result` is the
    // engine's answer -- `SUNIT_GetInteractInfo(...) && guid == npcId && MONSTERAI_IsInteractingWith`
    // -- so a click the engine REFUSES still leaves the unit's id in a record this function read on
    // `npcId` alone. A refused activation opens nothing in retail, but here it opened a conversation
    // nobody was in: the next click is spent closing it (`click.spent = 'npcClose'`), and
    // `d2_session_command_end_npc` refuses to end an interaction that was never established, so the
    // record never clears and EVERY later click is eaten the same way -- the hero can no longer walk
    // by click at all, while clickWalk's own tally still reads MOVE_OK (`d2_session_move_reason` is
    // written only by a move command, so a click that never reached the world leaves the last reason
    // standing). MEASURED, s000e-act5 step 25 (2026-09-30, engine 1fc1357db5b7, chained
    // S000_BOOTSTRAP=1): after the step the record read `{npcId:8, result:0, msgs:0}` with the
    // player's interact guid -1 and `npcPanelEvidence.open` true, and steps 26-28 then spent
    // `clicks by handler {"npcClose":400}`, `world clicks {}` -- step 28 needs a 122-subtile walk to
    // Anya's portal and could not take one.
    if (!npcId || !view.getUint32(12, true)) { npcMsgSkip = { npcId: 0, n: 0 }; return null; }
    const messageCount = view.getUint32(32, true);
    if (npcMsgSkip.npcId !== npcId) npcMsgSkip = { npcId, n: 0 };
    // Only a menu-0 message speaks on the click; a menu-2 one is a Talk topic and waits for the Talk box (retail: Kashya's first click
    // is her introduction, 'Den of Evil' is a topic; s2173).
    // D2Client first builds a text header from the packet (which prepends nodes), then calls
    // TEXT_SortTextNodeListByStringId. The click therefore speaks ascending StringId order, not
    // 0x27 packet order. F463 measured the discriminator: fresh Akara receives [64, 11] but paints
    // string 11 first. Keep the engine's packet untouched; this is presentation order only.
    const all = Array.from({ length: Math.min(messageCount, 8) }, (_, i) => i)
      .sort((a, b) => view.getUint16(36 + 2 * a, true) - view.getUint16(36 + 2 * b, true));
    const zero = all.filter((i) => Module.HEAPU8[dialogPtr + 52 + i] === 0);
    const plain = zero.length ? zero : all;   // a list of topics only is spoken as before (Drognan's ordered messages, s000b step 29)
    // A string id of 0 is a REAL message, so it cannot double as "nothing to say": retail's own 0x27
    // for Warriv is count 1, ids [0], menus [0], and
    // D2's ACT1Q0_Callback11_ScrollMessage acks "0 or 1" by design. With 0 read as silence the client
    // never spoke the A1Q0 gossip, so the A1Q6 quest message behind it (183, REWARDGRANTED on the ack)
    // was never reached and Warriv never offered the boat east. F230 / s000a step 48.
    const msgIdx = npcMsgSkip.n, msgId = msgIdx < plain.length ? view.getUint16(36 + plain[msgIdx] * 2, true) : -1;
    const greeting = msgId >= 0
      ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [msgId])
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
            gridX: pv.getInt16(104, true), gridY: pv.getInt16(106, true), invPage: Module.HEAPU8[o + 108],
            // ItemsTxt nInvWidth/nInvHeight: presentation bytes shared with P3b's item cursor.
            invWidth: Module.HEAPU8[o + 110] || 1, invHeight: Module.HEAPU8[o + 111] || 1 });
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
          invPage: art.invPage ?? 255, invWidth: art.invWidth ?? 1, invHeight: art.invHeight ?? 1,
          ...tipFields(id, codeText) });
      }
    }
    const gold = Module.ccall('d2_session_player_gold', 'number', [], []);
    const spoken = msgId >= 0 ? msgId : null;   // the message the NPC is speaking; null once the list is exhausted
    // The vendor's MonStats class decides its button row (a smith repairs).
    const classId = snapshot?.entities?.find((e) => e.id === npcId && e.type === 1)?.classId ?? -1;
    // WHICH counter the engine has open, from the engine: D2SessionVendor.action is the 0x38 action
    // the window was opened with -- 1 a vendor's trade stock, 2 a gambler's inventory (the engine's
    // own updateSessionVendorItems switches on it), 0 when nothing is open. It decides which engine
    // command a click on an offer is (P8d/P8f); the window itself draws the same either way.
    const vendorPtr = Number(Module.ccall('d2_session_vendor', 'number', [], []));
    const action = vendorPtr ? new DataView(Module.HEAPU8.buffer, vendorPtr, 28).getUint32(16, true) : 0;
    return { npcId, classId, greeting, spoken, items, gold, action };
  } catch (e) { console.error('npcState', e); return null; }
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

// Spend a gambler's offer. A gamble counter is the SAME window over a different inventory (P8f), so
// the click that reaches here is a click on a drawn offer, exactly as a buy is -- but the engine
// command is not the same one: d2_session_command_vendor_gamble reads the NPC's per-player gamble
// inventory and rolls the result item, while d2_session_command_vendor_buy reads the NPC's trade
// stock and would refuse an offer it has never listed. `npcState.npcId` is the same either way.
function gambleAtVendor(itemId) {
  if (!npcState) return false;
  try {
    const ok = Module.ccall('d2_session_command_vendor_gamble', 'number', ['number', 'number'],
      [npcState.npcId, itemId]);
    npcState = npcMenu?.open ? null : readNpcState();   // the menu comes first; the window opens on its entry
    snapshot = session.snapshot();
    return Boolean(ok);
  } catch { return false; }
}

// Sell an item to the open vendor. Same division as buying: the engine prices it, moves the gold
// and takes the item; this only names what was clicked.
let lastGridToBelt = null;
function gridToBelt(itemId) {
  const p = Module._malloc(5), v = new DataView(Module.HEAPU8.buffer, Number(p), 5);
  v.setUint8(0, 0x63); v.setUint32(1, itemId >>> 0, true);
  try { return Module.ccall('d2_session_client_packet', 'number', ['bigint', 'number'], [BigInt(p), 5]); } finally { Module._free(p); }
}

// M10: the reverse of gridToBelt -- retail's shift+click on a BELT box lifts that item onto the
// cursor (0x24, D2GAME_PACKETCALLBACK_Rcv0x24_ItemFromBelt) instead of drinking it, the same
// itemGUID packet shape as 0x63 above and 0x19's lift elsewhere in this file. 
// M10: the engine's dispatch table (src/, out of this change's scope) has no case for 0x24 yet, so
// this currently returns -1 unresolved; the packet shape and the shift-gated branch below are the
// client's whole share of the row and are correct against retail regardless of when that lands.
let lastBeltToCursor = null;
function beltToCursor(itemId) {
  const p = Module._malloc(5), v = new DataView(Module.HEAPU8.buffer, Number(p), 5);
  v.setUint8(0, 0x24); v.setUint32(1, itemId >>> 0, true);
  try { return Module.ccall('d2_session_client_packet', 'number', ['bigint', 'number'], [BigInt(p), 5]); } finally { Module._free(p); }
}

// A shop opening brings retail's inventory up beside it; leaving the shop closes what it opened.
// Run from the draw AND from input: a click that arrives before the next frame is drawn (headless, or
// a fast player) found the inventory still hidden, fell through to the world, and the world click
// closed the trade -- every death shop in s000c sold nothing and bought nothing.
function syncShopInventory() {
  if (shopOpen() && !shopWasOpen && ensureVendorPanel()) { vendorPanel.reset(npcState.items, SMITH_CLASSES.has(npcState.classId)); inventoryVisible = true; }
  if (!shopOpen() && shopWasOpen) inventoryVisible = false;
  shopWasOpen = shopOpen();
}

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

// The smith's Repair All button (retail's 0x35 with the repair-all flag): every carried item mended,
// paid from the inventory's gold and then the stash's.
let lastRepairAll = null;
function repairAllAtVendor() {
  if (!npcState || !Module._d2_session_command_vendor_repair_all) return false;
  const before = Module.ccall('d2_session_player_gold', 'number', [], []);
  const ok = Boolean(Module.ccall('d2_session_command_vendor_repair_all', 'number', ['number'], [npcState.npcId]));
  npcState = readNpcState(); snapshot = session.snapshot();
  lastRepairAll = { npcId: npcState?.npcId ?? -1, ok, goldBefore: before, goldAfter: Module.ccall('d2_session_player_gold', 'number', [], []) };
  return ok;
}

// An open shop is retail's vendor window (play/assets/vendor-panel.mjs), not the trade panel.
let vendorPanel = null, vendorPanelError = '', shopWasOpen = false;
function ensureVendorPanel() {
  if (vendorPanel || vendorPanelError) return vendorPanel;
  try { vendorPanel = createVendorPanel({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
    string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]) }); }
  catch (error) { vendorPanelError = String(error); }
  return vendorPanel;
}
const shopOpen = () => Boolean(npcState?.items?.length);
function ensureNpcPanel() {
  if (npcPanel || npcPanelError) return npcPanel;
  try { npcPanel = createNpcPanel({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette }); }
  catch (error) { npcPanelError = String(error); }
  return npcPanel;
}
let speechBox = null, speechBoxError = '';
function ensureSpeechBox() {
  if (speechBox || speechBoxError) return speechBox;
  try { speechBox = createSpeechBox({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette, createFont, decodeFontTbl }); }
  catch (error) { speechBoxError = String(error); }
  return speechBox;
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
// Legs a walk may take without ONCE getting closer to its goal before it is stuck, not slow. A leg
// the engine accepted is not progress: retail answers an accepted move that goes nowhere, so a walk
// whose goal cannot be occupied re-picks a leg forever. s000a step 37 walked the same 79-95 subtile
// stretch for its whole budget -- every leg accepted, `walkEvidence.reason` 0, the walk never ending,
// and so the corpse click's onStuck (the one thing that makes the engine answer a body it cannot be
// walked to) never ran. The distance is the evidence, not the acceptance.
const WALK_GIVE_UP_LEGS = 24;

function chebyshev(ax, ay, bx, by) { return Math.max(Math.abs(ax - bx), Math.abs(ay - by)); }

// F371: arm a walk. THE GIVE-UP COUNTERS LIVE ON THE GOAL (`walkGoal.stall`/`best`/`closeTries`), so a
// re-arm used to start them at zero -- and a walk toward a goal the player keeps clicking (a stair
// object whose own cell keeps the hero out of `within`) could then neither ARRIVE nor give up, and the
// engine was never asked: `act3-chain10` lost step 26 to exactly that (~50 rounds, 66 s) on a class-386
// stair (F370 records the chain half, F371 this client half). So carry the counters across a re-arm to
// the SAME cell -- a second click on the same thing is the SAME intent, and the walk keeps its patience.
// An arm elsewhere is a new intent and starts fresh. Only the counters carry (not the previous goal's
// callbacks: an arm always brings its own). Everything that sets a walkGoal arms through here.
function armWalk(goal) {
  const same = walkMemory !== null && walkMemory.x === goal.x && walkMemory.y === goal.y;
  if (same) { goal.stall = walkMemory.stall; goal.best = walkMemory.best; goal.closeTries = walkMemory.closeTries; }
  // Keep accepted hops for stall accounting, but let this click issue its first leg without waiting
  // on a move accepted by the previous request.
  walkEvidence = { active: true, hops: same ? walkEvidence.hops : 0, reason: 0 };
  // The prior click's move may still be finishing, but it cannot make this new click wait before
  // issuing its first leg. Keep its accepted-hop count for stall accounting; reset only the wait gate.
  walkMovePending = false;
  walkGoal = goal;
  walkMemory = walkGoal;
  return walkGoal;
}

function walkStep() {
  if (!walkGoal || !session) return;
  const p = snapshot.player;
  const dist = chebyshev(p.x, p.y, walkGoal.x, walkGoal.y);
  if (dist <= (walkGoal.within ?? 4)) { const done = walkGoal.onArrive; walkGoal = null; walkMemory = null; walkMovePending = false; walkEvidence.active = false; done?.(); return; }

  // Only pick a new leg once the last one has finished moving us.
  if (walkLast && walkLast.x === p.x && walkLast.y === p.y) walkIdle++; else walkIdle = 0;
  walkLast = { x: p.x, y: p.y };
  if (walkIdle < 2 && walkMovePending) return;

  // Hops accepted but the distance not closing (a pocket whose fan legs all end against its walls,
  // s000a's Den mouth after an earlier change's retail get-hit timing): ask the engine's own path to the goal.
  if (walkEvidence.hops && dist >= (walkGoal.best ?? Infinity)) walkGoal.stall = (walkGoal.stall || 0) + 1; else walkGoal.stall = 0;
  walkGoal.best = Math.min(walkGoal.best ?? Infinity, dist);

  // Bounded, and honest: this many legs without ever closing on the goal is a wall, not a detour (a
  // detour that gains anything at all resets the count above). Stopping here is what runs a caller's
  // onStuck -- the walk that never stops is a question the engine is never asked.
  if (walkGoal.stall >= WALK_GIVE_UP_LEGS) {
    const reason = Module.ccall('d2_session_move_reason', 'number', [], []);
    walkEvidence = { active: false, hops: walkEvidence.hops, reason, stalled: walkGoal.stall };
    const cb = walkGoal.onStuck; walkGoal = null; walkMemory = null; walkMovePending = false; cb?.(); return;
  }
  if (walkGoal.stall >= 4 && walkGoal.stall % 4 === 0 && session.move(walkGoal.x, walkGoal.y)) {
    walkEvidence = { active: true, hops: walkEvidence.hops + 1, reason: 0 }; walkIdle = 0; walkMovePending = true; return;
  }
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
      walkMovePending = true;
      return;
    }
  }
  // F119: NOTHING THE FAN PROPOSED MADE PROGRESS, but the goal may sit one subtile away across a box
  // the hero's own body hides. The fan can never propose that step -- its radius floors at 4
  // (`WALK_RANGES` via `max(4, dist)`) and its candidate filter is `d2_session_walkable`, which the
  // hero's OWN footprint (COLLIDE_NO_PATH on every subtile his cross covers,
  // COLLISION_CheckCollisionMaskWithAdjacentCells) makes read wall on every cell touching him.
  // MEASURED (s552, engine 7162ff435c47): parked one subtile outside a door's box, the engine's own
  // `d2_session_command_move` to the very next subtile toward the door returned MOVE_OK and moved the
  // hero there, while both walkability predicates read 0 for that same cell. So ask the engine
  // directly, filter-free -- session.move() is its own authority. Placed HERE, AFTER the fan, so a
  // walk the fan can serve is untouched: only a fan that produced no accepted move falls through to
  // it. Bounded to a handful of tries per goal -- a genuine wall (not this park) makes the same
  // refused call every time, and this must not become its own unbounded loop alongside WALK_GIVE_UP_LEGS.
  if (dist <= 6 && (walkGoal.closeTries || 0) < 6) {
    walkGoal.closeTries = (walkGoal.closeTries || 0) + 1;
    const stepx = p.x + Math.sign(walkGoal.x - p.x), stepy = p.y + Math.sign(walkGoal.y - p.y);
    if ((stepx !== p.x || stepy !== p.y) && session.move(stepx, stepy)) {
      walkEvidence = { active: true, hops: walkEvidence.hops + 1, reason: 0 }; walkIdle = 0; walkMovePending = true; return;
    }
  }
  // Nothing reachable makes progress: stop rather than spin.
  walkEvidence = { active: false, hops: walkEvidence.hops, reason: Module.ccall('d2_session_move_reason', 'number', [], []) };
  // A goal whose own footprint keeps the player out of `within` (Harrogath's 1x7 main gate) still
  // gets its interaction: the engine's range test (UNITS_IsObjectInInteractRange) decides, as the
  // server does for retail's client.
  const stuck = walkGoal.onStuck; walkGoal = null; walkMemory = null; walkMovePending = false; stuck?.();
}

function readUnitKinds(count) {
  if (!count) return null;
  const p = Module._malloc(count);
  try {
    const n = Module.ccall('d2_session_unit_kinds', 'number', ['bigint', 'number'], [BigInt(p), count]);
    return n > 0 ? Module.HEAPU8.slice(Number(p), Number(p) + n) : null;
  } catch { return null; } finally { Module._free(p); }
}

// F22: which units carry retail's NPC speech alert, one byte per unit in snapshot.entities order.
// The engine answers it from its own per-player NPC-intro flags (the buffer it syncs to the client
// in server packet 0x91 and clears on client packet 0x4D). No answer -> no alert: an engine without
// the export must not have the client invent one, the same contract the renderer's `pending` has.
function readNpcSpeechAlerts(count) {
  if (!count) return null;
  const p = Module._malloc(count);
  try {
    const n = Module.ccall('d2_session_npc_speech_alerts', 'number', ['bigint', 'number'], [BigInt(p), count]);
    return n > 0 ? Module.HEAPU8.slice(Number(p), Number(p) + n) : null;
  } catch { return null; } finally { Module._free(p); }
}

// Advance the engine by one game frame.
//   forced (a test runner `step()`): exactly ONE game frame, and the wall-clock clock is zeroed, so a
//     scenario's step count stays deterministic however the browser happened to schedule it.
//   otherwise (the rAF loop): the clock decides -- 25 advances a second, whatever the frame rate.
// Returns the number of game frames it advanced by: 1 when forced, else 0 or 1 (2 in a slow frame).
// The caller needs the count, not just the side effect -- the walk controller's stall accounting is
// per ENGINE frame and must not run on an update that left the engine where it was.
function advanceEngine(forced) {
  let n = 0;
  if (forced) { gameClock = 0; engineTicks++; session.tick(1); n = 1; }
  else {
    gameClock += STEP;
    while (gameClock >= GAME_STEP) { gameClock -= GAME_STEP; engineTicks++; session.tick(1); n++; }
  }
  // s693: a change of LEVEL is an arrival, and retail draws the arriving level's EntryFile art for
  // 120 frames from it. Noticed here rather than in the render path, where it used to be: a scenario
  // (and a background tab) advances the engine many frames without drawing, and an arrival first
  // noticed at the next draw would start the window late -- the one thing this gate must not do.
  // The level is the ENGINE's own (d2_session_player_level), not the client's copy of it.
  if (n && session) {
    const level = Module.ccall('d2_session_player_level', 'number', [], []);
    if (level !== entryBannerLevel) { entryBannerLevel = level; entryBannerAt = engineTicks; }
  }
  return n;
}

// S2: play what the engine attached to units this tick (d2_session_take_sounds), through the retail resolver.
// F157: an OBJECT's quest message (packet 0x27 on an object, e.g. Anya's plea from her ice) is drawn as the speech box;
// dismissing it reports it heard (0x31 {guid, string}), which is what advances the quest.
let objectSpeech = null, objectMsgBuf = 0;
function pollObjectSpeech() {
  if (objectSpeech || !session || screen !== SCREEN.GAME || !Module._d2_session_take_object_message) return;
  if (!objectMsgBuf) objectMsgBuf = Number(Module.ccall('malloc', 'bigint', ['number'], [8]));
  if (!Module.ccall('d2_session_take_object_message', 'number', ['bigint'], [BigInt(objectMsgBuf)])) return;
  const v = new Int32Array(Module.HEAPU8.buffer.slice(objectMsgBuf, objectMsgBuf + 8));
  objectSpeech = { guid: v[0] >>> 0, stringId: v[1], text: Module.ccall('d2_lang_string_by_index', 'string', ['number'], [v[1]]) };
}
function dismissObjectSpeech() {
  const m = objectSpeech; objectSpeech = null;
  if (m) Module.ccall('d2_session_command_npc_response_packet', 'number', ['number', 'number'], [m.guid, m.stringId]);
}
let unitSounds = null, soundBuf = 0;
function drainUnitSounds() {
  pollObjectSpeech();
  if (!unitSounds || !session || screen !== SCREEN.GAME || !Module._d2_session_take_sounds) return;
  const cap = 32; if (!soundBuf) soundBuf = Number(Module.ccall('malloc', 'bigint', ['number'], [cap * 24]));
  const n = Module.ccall('d2_session_take_sounds', 'number', ['bigint', 'number'], [BigInt(soundBuf), cap]);
  if (!n) return;
  const v = new Int32Array(Module.HEAPU8.buffer.slice(Number(soundBuf), Number(soundBuf) + n * 24));
  const local = snapshot?.playerId;
  for (let i = 0; i < n; i++) unitSounds.play({ unitType: v[i * 6], unitId: v[i * 6 + 1], classId: v[i * 6 + 2], soundEvent: v[i * 6 + 3], updateUnitType: v[i * 6 + 4], updateUnitId: v[i * 6 + 5] }, local);
}
function update(forceTick = false) {
  drainUnitSounds();
  // The engine advances every tick, so last tick's advancement record is stale by definition.
  advancement = null; classSkills = null;
  if (screen === SCREEN.TITLE) {
    ensureTitleScreen();
    titleTick++;
    // The boot films are paced by the movie file's own frame rate against the wall clock, and stepped
    // from the shell's timer as well (stepBootMovie) -- this loop does not run in a background tab.
    stepBootMovie();
    // The trademark screen's countdown is a timer, not a frame count: it must run in a hidden tab too
    // (setTitlePhase -- this loop does not run there).
    return;
  }
  if (screen === SCREEN.CREDITS) { ensureCreditsScreen(); creditsTick++; return; }
  if (screen === SCREEN.CINEMATICS) { ensureCinematicsScreen(); cinematicsScreen?.step(); return; }
  if (screen === SCREEN.SELECT) { ensureCharSelect(); return; }
  if (screen === SCREEN.CREATE) { ensureCharCreate(); charCreate?.step(); return; }
  // "is the game's data mounted" has ONE answer, `window.__d2Entry.retail`, decided by
  // play/index.html before anything here runs -- not a second regex over the query. The two disagreed
  // the moment the bare host became the player's page: the page would mount the MPQs and this gate
  // would still refuse the session for a URL that never spelled `retail=1`.
  if (!bootstrapStarted && PLAY_MODE && window.__d2Entry.retail && window.__d2?.ready && !window.__d2.error && sessionAdapter()) bootstrapSession();
  if (gameMenu?.open) { gameMenu.step(); controlsPanel?.step(); input.drain(); return; }   // single player pauses under the menu
  const commands = input.drain();
  if (session) {
    for (const command of commands) {
      if (command.type === COMMAND.MOVE) {
        lastMoveCommand = { type: command.type, x: command.x, y: command.y, directional: !!command.directional, held: !!command.held };
        if (command.directional) session.move(snapshot.player.x + command.x, snapshot.player.y + command.y);
        else if (command.held) {
          // Retail keeps the left-button walk command aimed at the current pointer while held.
          // Held retargets are ground moves, so passing over a unit does not turn the gesture into
          // an attack or pickup.
          const target = resolvePointerTarget(command.x, command.y);
          const { x: tx, y: ty } = target.at;
          const moved = session.move(tx, ty);
          walkGoal = null;
          if (!moved) armWalk({ x: tx, y: ty, within: 2 });
        }
        else {
          // A ground click. A new click replaces any walk in progress, as in retail. Retail's walk
          // command only routes a short leg (MOVE_NO_PATH beyond it -- s000 stalled in town on
          // exactly that); a far click is routed in legs by the client, so a refused click hands
          // the SAME goal to the hop walker the NPC walk already uses.
          const resolved = resolvePointerTarget(command.x, command.y);
          const { x: tx, y: ty } = resolved.at;
          // The click record this command answers (see noteClick): what the ground click resolved to
          // and what the shell did with it. A walk's leg is this one line: aimed at floor, resolved to
          // a unit, spent as an attack, walked to the unit's own position -- or resolved to floor and
          // walked to the aim point. Without it a leg that oscillates between two points cannot say
          // which of its clicks were walks at all.
          const record = claimClick(command.x, command.y);
          walkGoal = null;
          // Retail's LEFT click acts on what is under the cursor before it walks: an item is picked
          // up, a hostile is attacked with the left-hand skill (normal attack). Only bare ground
          // walks. (This shell sent every left click as a walk, so a monster could only be fought
          // with the right button -- s000 clicked one 400 times and never swung.)
          // A live monster under the pointer comes before a ground item at its feet: a fight drops
          // loot where the pack stands, and every click on the next monster picked up (or walked to)
          // the drop instead -- s000d logged 400 'busy' rounds on a monster no click ever attacked.
          // (F14: retail's hover order is not recovered; units before items is observed behaviour.)
          const underTarget = resolved.unitId;
          const underItem = resolved.item;
          if (record) record.aim = { world: [tx, ty], unitId: underTarget || 0, itemId: underItem?.id ?? 0 };
          if (underItem) { if (record) record.result = `pickup item ${underItem.id} at (${underItem.x},${underItem.y})`; session.pickup(underItem.id); continue; }
          if (underTarget) {
            // The left hand's chosen skill (V9b): anything but the plain Attack/Throw/Kick runs through the engine's own
            // left skill; Attack keeps the primary path.
            const leftId = Math.max(0, readSelectedSkills().left);
            const useLeft = leftId > 2 && Module._d2_session_command_left_skill;
            const swing = (id) => useLeft ? Module.ccall('d2_session_command_left_skill', 'number', ['number'], [id]) : session.attack(id);
            const result = swing(underTarget);
            lastSkillCommand = { skill: useLeft ? leftId : 0, targetId: underTarget, result };
            // A monster out of reach (rooms away, or no path for the walk-into-range leg): retail
            // walks the player toward it and attacks on arrival, as clicking an NPC walks and talks.
            if (!result) {
              const m = snapshot.entities.find((e) => e.id === underTarget && e.type === 1);   // ids are per unit type
              if (m) armWalk({ x: m.x, y: m.y, within: 2, onArrive: () => { lastSkillCommand = { skill: useLeft ? leftId : 0, targetId: underTarget, result: swing(underTarget) }; } });
              // The leg's aim was floor and it resolved to a unit: whatever the click's own aim point
              // was, the walk now goes to the UNIT, not to it (F14's monsters-first rule). That is the
              // one thing a leg that never closes on its plan can be doing.
              if (record) { record.result = `attack unit ${underTarget} -> ${result}`; record.walkTo = m ? [m.x, m.y] : null; }
            } else if (record) record.result = `attack unit ${underTarget} -> ${result}`;
            continue;
          }
          const moved = session.move(tx, ty);
          if (record) { record.result = `move (${tx},${ty}) -> ${moved}`; record.walkTo = moved ? null : [tx, ty];
            // Read immediately after the command, which is what d2_session_move_reason describes. A
            // refused move has TWO subjects -- the ground (3 NO_ROOM, 5 NO_PATH) and the player (6
            // DISPATCH_REFUSED: dying, dead or uninterruptable, retail's own dispatcher test) -- and
            // the walker's aim loop is only allowed to shorten its aim for the first kind.
            if (!moved) record.reason = Module.ccall('d2_session_move_reason', 'number', [], []); }
          if (!moved) armWalk({ x: tx, y: ty, within: 2 });
        }
      } else if (command.type === COMMAND.PRIMARY) {
        const resolved = command.x == null ? null : resolvePointerTarget(command.x, command.y);
        const targetId = command.x == null ? selectPrimaryTarget(snapshot, snapshot.player, Infinity) : resolved.unitId;
        const ground = command.x == null ? null : resolved.item;
        // A right click is a world click too, and until it claimed its record every one of them sat in
        // the evidence as "unanswered" -- indistinguishable from a left click the shell never drained.
        // The fight sends hundreds of them, so the bucket hid the answer it was meant to give.
        const precord = command.x == null ? null : claimClick(command.x, command.y);
        if (precord) precord.aim = { world: [resolved.at.x, resolved.at.y], unitId: targetId || 0, itemId: ground?.id ?? 0 };
        if (ground) { if (precord) precord.result = `pickup item ${ground.id} at (${ground.x},${ground.y})`; session.pickup(ground.id); continue; }
        const skill = command.secondary ? Math.max(0, readSelectedSkills().right) : 0;
        // Any other right-hand skill (Bash, Double Swing, ...) is the engine's selected right skill.
        const groundSkill = !targetId && command.secondary && skill > 2 && command.x != null;
        const result = targetId ? (skill === 2 ? session.throwSkill(targetId) : skill === 1 ? session.kick(targetId) : skill === 0 ? session.attack(targetId)
          : Module.ccall('d2_session_command_right_skill', 'number', ['number'], [targetId]))
          : groundSkill ? session.rightSkillAt(resolved.at.x, resolved.at.y) : 0;
        if (precord) precord.result = groundSkill
          ? `cast right skill ${skill} at (${resolved.at.x},${resolved.at.y}) -> ${result}`
          : `attack unit ${targetId || 0} with skill ${skill} -> ${result}`;
        lastSkillCommand = { skill, targetId, at: groundSkill ? [resolved.at.x, resolved.at.y] : null, result };
      } else if (command.type === COMMAND.BELT) {
        // Retail's belt keys: 1-4 drink/use the bottom item of that belt column, through the engine.
        const result = session.useBelt(command.slot);
        if (result) snapshot = session.snapshot();
        lastBeltCommand = { slot: command.slot, result };
        window.__d2Step19Trace?.record('belt-use', { source: 'key-command', slot: command.slot, result: Boolean(result) },
          { step: window.__s000?.state?.step, mode: window.__s000?.mode, snapshot });
      } else if (command.type === COMMAND.SKILL) {
        // F1-F8 are the engine's hotkey table (V9b). With the skill popup open and the pointer on an icon the key ASSIGNS that
        // skill (in the popup's hand) to the slot; otherwise it selects the slot's skill into its hand (retail's 0x3C). An
        // unbound key does nothing (retail has no default F-key skills).
        const slot = command.slot - 1, over = skillSelectRenderer?.open && skillSelectRenderer.hoverIcon;
        if (over && Module._d2_session_hotkey_assign) Module.ccall('d2_session_hotkey_assign', 'number', ['number', 'number', 'number'], [slot, over.skill.id, over.side === 'left' ? 1 : 0]);
        else {
          const bound = Number(Module.ccall('d2_session_hotkey_skill', 'number', ['number'], [slot]));
          if (bound >= 0) selectSkill(bound, Module.ccall('d2_session_hotkey_hand', 'number', ['number'], [slot]) ? 'left' : 'right');
        }
      } else if (command.type === COMMAND.CANCEL) {
        // Retail's Esc does not touch the selected skills.
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
          if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadSessionTerrain(snapshot.act - 1);
        }
      } else if (command.type === COMMAND.TRAVEL) {
        // The caravan: Warriv, Meshif, Tyrael (and Cain's portal) carry the player into the next
        // act's town; the recovered server handler needs that act's quest reward.
        const previousAct = snapshot.act;
        if (session.travelNearest()) {
          snapshot = session.snapshot(); motion = session.motion();
          if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadSessionTerrain(snapshot.act - 1);
        }
      } else if (command.type === COMMAND.RESPAWN) {
        if (snapshot.mode === 17 && session.respawn()) {
          snapshot = session.snapshot(); motion = session.motion();
        }
      }
    }
    try {
      const engineAdvanced = advanceEngine(forceTick) > 0;
      // Retail reissues mouse-button actions while a button is held (OpenDiablo2's compatible
      // GameControls path repeats every 250 ms). Re-resolve the stationary screen point against
      // the current camera every six engine ticks: as the hero runs and the camera follows, the
      // world point under the cursor moves too. Pointer-move retargets remain immediate below.
      if (engineAdvanced && worldPointerHeld) {
        heldPointerTicks++;
        if (heldPointerTicks >= 6 && screen === SCREEN.GAME && session && !gameMenu?.open) {
          input.pointer(pointer.x, pointer.y, 0, true);
          heldPointerTicks = 0;
        }
      }
      // One world rebuild per frame, not two: the snapshot and the npc/kind reads share one hold
      // (a rebuild costs ~1 ms, as much as the tick). walkStep can issue a command, so a frame with a
      // walk in progress releases the hold around it and rebuilds after it, as before.
      const hold = Boolean(Module._d2_session_hold_world), holdWorld = (on) => { if (hold) Module.ccall('d2_session_hold_world', null, ['number'], [on ? 1 : 0]); };
      holdWorld(true);
      let kinds, alerts;
      try {
        snapshot = session.readSnapshot(); motion = session.motion();
        // Hostility is the monster's alignment, not "is a monster"; talkable is its INTERACT flag.
        // walkStep runs ONCE PER ENGINE FRAME, not once per shell update. Its idle counter reads
        // "the snapshot's position did not change", which is only evidence of a stall if the engine
        // actually had a chance to move: on the 2 shell updates in 5 that advance the engine not at
        // all, an unchanged position is expected, and counting it let walkIdle reach its 2-update
        // threshold and REPLACE THE ROUTE before the engine had walked the current leg (Codex review
        // of this branch, measured: a long click-walk stalls or takes different hops).
        if (walkGoal && engineAdvanced) { holdWorld(false); walkStep(); holdWorld(true); }
        // P7b: a timed Introduction/Gossip speech is ours (its line may not be in the engine's list): the engine's read must not replace it.
        npcState = npcMenu?.open ? null : npcState?.own ? npcState : readNpcState();   // the menu comes first; the window opens on its entry
        kinds = readUnitKinds(snapshot.entities.length);
        alerts = readNpcSpeechAlerts(snapshot.entities.length);
      } finally { holdWorld(false); }
      npcCount = 0; speechAlertCount = 0;
      if (kinds) for (let i = 0; i < snapshot.entities.length && i < kinds.length; i++) {
        const entity = snapshot.entities[i], kind = kinds[i];
        entity.npc = (kind & 1) !== 0;
        entity.hostile = (kind & 2) !== 0;
        if (entity.npc) npcCount++;
      }
      // F22: the engine's pending-speech bit decides the alert; a unit the engine says nothing about
      // keeps `undefined`, which the renderer already treats as "do not draw".
      if (alerts) for (let i = 0; i < snapshot.entities.length && i < alerts.length; i++) {
        snapshot.entities[i].speechPending = alerts[i] !== 0;
        if (alerts[i]) speechAlertCount++;
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
  status.title = [terrainError, musicError].filter(Boolean).join('\n');
  status.textContent = session
    ? `retail-backed simulation connected · ${terrainStats?.drawn ? 'retail level terrain drawn' : !TERRAIN_ENABLED || terrainError ? 'terrain unavailable (no mock fallback)' : 'loading retail level terrain…'} · ${musicError ? 'audio unavailable' : 'retail music ready'}`
    : sessionError || (PLAY_MODE ? 'engine loaded; creating simulation…' : 'open ?retail=1&play=1 to create a gameplay session');
}

// The camera, which is the player's own position: the hero is drawn at the viewport centre and the
// whole world is offset by this. It is the engine's path position at its own 16.16 precision, the
// same value retail's client draws with (`PATH_UpdateClientCoords`: dwClientCoordX = dwPrecisionX
// >> 11). Nothing here is recomputed in JS -- the simulation's position is untouched.
//
// Reading only the whole subtile (wPosX) made every drawn frame land on a subtile corner, so the
// world stepped 16 px in x / 8 px in y at the engine's 25 Hz and, on a diagonal walk, the drawn
// screen x REVERSED between neighbouring cells: measured over an 18-frame straight diagonal walk,
// screen x ran -2000 -2016 -2000 -2000 -2000 -1984 -2000 -1984 with 3 reversals. At rest the engine
// holds the subtile's centre (PATH_FP16FitToCenter = x.5), which is exactly what the `x + 0.5` this
// replaces meant -- so a standing player draws on the same pixel as before.
function cameraCentre() {
  const centre = { x: snapshot.player.x + 0.5, y: snapshot.player.y + 0.5 };
  return session?.playerCoords?.() || centre;
}
// The integer grid the static pass (floors, walls, objects, ground items) and the unit projection
// are expressed against: half a subtile under the centre. `worldToScreen(cameraCentre() - 0.5)`,
// named rather than repeated because a camera that disagreed with projStatic would slide the
// scenery against the hero.
// The world's screen centre column: the canvas centre, moved to the middle of the free half while a panel is open
// on one side. Retail does this at its own 800x600 too, not only in the wide view (F264: panel-view-shift.mjs).
function panelFlags() { return { character: characterVisible, quest: questVisible, stash: stashOpen, cube: cubeOpen, waypoint: Boolean(waypointMenu), skills: skillsVisible, inventory: inventoryVisible, vendor: shopOpen() }; }
function panelSides() { return panelViewSides(panelFlags()); }
function viewCX() {
  if (screen !== SCREEN.GAME || clearScreen) return canvas.width / 2;
  return viewCentreX(canvas.width, panelSides());
}
// __d2HidePanel: a panel row's no-panel render (lib/panel-frame.mjs toggle), never set by play. It
// hides the side panel WITHOUT closing it, so F264's view shift stays put -- the panel's state (and
// so viewCX) is unchanged, only the drawing is skipped. Closing the panel instead (the row's own key)
// would ALSO move the world back, and the both-ways control then compares the world's own columns, not
// the panel's: that is exactly the 461 px s251 read at x=400, the one column inside retail's panel
// bbox where retail's own frame buffer shows shifted world rather than panel paint. s256 already does
// this for the vendor panel as __d2HideShop.
function panelShown(open) { return Boolean(open) && !globalThis.__d2HidePanel; }
// SGD2FreeRes's art (not retail; staged by ci/stage.sh). Fill mode only -- ?parity=1 never loads it.
let freeResArt = null, wideFrameEvidence = null;
function ensureFreeRes() {
  // `Module` EXISTS as a global from the first line of the emscripten glue but is uninitialized until
  // `__d2.ready`, so `typeof Module === 'undefined'` is not the readiness test: the very first draw in
  // fill mode (the loading screen, 1706px wide in a browser window) builds the reader, `ccall` throws
  // "an initialized d2 wasm Module is required", and the failure is CACHED -- the widescreen wide bar
  // and panel filler never load for the whole session and the alert banner sits over the page. Every
  // sibling ensure* gates on the shell's own readiness flag; this one gates on it too (s1390).
  if (freeResArt || !window.__d2Entry?.fill || !window.__d2?.ready) return freeResArt;
  const loud = (art) => { if (art.ready) return; console.error('SGD2FreeRes art not loaded: ' + art.error);
    const d = document.createElement('div'); d.id = 'freeres-notice'; d.setAttribute('role', 'alert');
    d.style.cssText = 'position:fixed;left:0;right:0;top:0;padding:6px 12px;background:#0b0c09;color:#c9ad68;border-bottom:1px solid #493d27;font:12px sans-serif;z-index:9';
    d.textContent = 'Widescreen HUD art missing (' + art.error + '); the wide bar and panel filler are not drawn.'; document.body.append(d); };
  try { freeResArt = createFreeResArt(createMpqAssetReader(Module)); freeResArt.load().then(() => loud(freeResArt)); } catch (error) { freeResArt = { ready: false, error: String(error) }; loud(freeResArt); }
  return freeResArt;
}
function cameraGrid() {
  const c = cameraCentre();
  return { x: c.x - 0.5, y: c.y - 0.5 };
}
// Where a unit's sprite is anchored in the world. A unit with a dynamic path has the engine's own
// 16.16 position (d2_session_unit_coords); drawing it at the whole subtile made a walking monster
// hold still for several frames and then jump a square -- 16 px in x, 8 px in y, and on a diagonal
// an x-step and a y-step alternately, which is the shake sees around monsters and NPCs (s491).
// The `- 0.5` is the half subtile between a unit's centre and the integer grid proj() is built on --
// the same half projStatic carries -- so an unmoving unit draws on exactly the pixel it did before.
function unitAnchor(entity) {
  return entity.preciseX != null ? [entity.preciseX - 0.5, entity.preciseY - 0.5] : [entity.x, entity.y];
}

// The player always sits at the centre of the viewport; it is a sprite like any other in the depth
// sort, which is why it lives here rather than inline at the end of render().
function drawPlayer(c) {
  const w = canvas.width, h = canvas.height;
  if (playerRenderer) {
    // F195: every class draws its worn items (this was gated to the Barbarian).
    const items = snapshot.items;
    try { playerRenderer.draw(c, viewCX(), (h / 2 + UNIT_CENTRE_Y), { ...motion, playerClass: snapshot.playerClass, mode: snapshot.mode, tick: snapshot.tick, act: snapshot.act, items }); playerRenderError = ''; }
    // One animation that fails to draw is reported, not fatal: the next mode may draw fine.
    catch (error) { playerRenderError = String(error); }
  }
  if (!playerRenderer) {
    c.fillStyle = '#d9b85c'; c.beginPath(); c.arc(viewCX(), (h / 2 + UNIT_CENTRE_Y), 7, 0, Math.PI * 2); c.fill();
  }
}

function itemCode(item) {
  return String.fromCharCode(item.code & 255, (item.code >>> 8) & 255,
    (item.code >>> 16) & 255, (item.code >>> 24) & 255).replace(/\0/g, '').trim();
}

// The selection is deliberately the same F14 resolution that click dispatch uses: the unit or the
// ground item the click point is nearest, with a unit winning a tie. The only client work here is
// project + draw; class/code and their localized names stay engine/table owned.
function pointerWorldAt(x, y) {
  const d = screenDeltaToWorld(x - viewCX(), y - (canvas.height / 2 + UNIT_CENTRE_Y));
  // The static grid lies under the player camera's half-subtile. Every pointer path (move,
  // primary skill and hover) passes through this one rounding point, so it reads the SAME camera
  // the world is drawn with: a pointer resolved against a quantised camera lands up to half a
  // subtile off once the camera glides.
  const cam = cameraCentre();
  return { x: Math.round(cam.x + d.x), y: Math.round(cam.y + d.y) };
}

// WHAT THE CLICK ACTS ON (F14). Retail's own hit test is still not recovered, so this is the
// observed rule, and the observed rule is about what the cursor is OVER. A ground item occupies its
// own cell, and the cursor is on that cell only at distance 0; a unit's sprite is far wider than its
// anchor, so a unit keeps the click anywhere inside the 2-subtile tolerance it always had. The form
// this replaces gave the UNIT the click from anywhere inside that 2-SUBTILE DISC -- wider than an
// item's own cell -- so a click on an item's own projected pixel was stolen by a monster 1-2
// subtiles away: MEASURED in s000d-act4 step 19, the Hellforge Hammer's own pixel resolved to
// `{unitId, itemId: 0}` every round, packet 0x16 was never sent and the item could not be taken
// (harness-out/s000d-act4/step19-run5-click-stolen-live.log). So the ONE case that changed is the
// cursor standing on a ground item's own cell with a live hostile merely near it: that click is the
// item's. A hostile ON that cell still wins (distance 0 for both is the tie the unit wins -- F14's
// measured monster-on-the-item case), and everywhere else the unit-first order is untouched. This
// decides every act's loot click, not only act IV's.
function resolvePointerTarget(x, y) {
  const at = pointerWorldAt(x, y);
  // F14 measured: the unit test is retail's screen-space box, not a subtile disc. The overlap tie-break
  // (nearest world point to the pointer, lower id) is UNMEASURED and unchanged.
  const unitId = selectPrimaryTarget(snapshot, at, Infinity, (entity) => unitBoxHit(entity, x, y));
  const distance2 = (candidate) => (candidate.x - at.x) ** 2 + (candidate.y - at.y) ** 2;
  const ground = (snapshot.items || []).filter((candidate) => candidate.mode === 3 && candidate.invPage === 255)
    .map((candidate) => ({ item: candidate, distance: distance2(candidate) }))
    .filter((candidate) => candidate.distance <= 4)
    .sort((a, b) => a.distance - b.distance || a.item.id - b.item.id)[0] ?? null;
  const unit = unitId ? (snapshot.entities || []).find((entity) => entity.id === unitId && entity.type === 1) : null;
  if (unit && ground?.distance === 0 && distance2(unit) > 0) return { at, unitId: 0, item: ground.item };
  return { at, unitId, item: unit ? null : (ground?.item ?? null) };
}

function hoverTarget() {
  if (!session || screen !== SCREEN.GAME) return null;
  const { unitId, item } = resolvePointerTarget(pointer.x, pointer.y);
  if (unitId) {
    const unit = snapshot.entities.find((entity) => entity.id === unitId && entity.type === 1);
    if (!unit) return null;
    const stringIndex = Module.ccall('d2_monster_name_str', 'number', ['number'], [unit.classId]);
    const label = stringIndex >= 0 ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [stringIndex]) : '';
    return label ? { kind: 'unit', id: unit.id, classId: unit.classId, stringIndex, label, x: unit.x, y: unit.y } : null;
  }
  // A TOWN NPC IS A UNIT and retail names it the same way it names a monster -- but it is not
  // HOSTILE, and `selectPrimaryTarget` filters on hostility (`gameplay-model.mjs`), so hovering
  // Akara resolved to nothing at all (client-parity V5/F9b). The hit test is the shell's own
  // `npcUnderPointer` -- the door the talk click already goes through -- never a second one.
  const npc = npcUnderPointer(pointer.x, pointer.y);
  if (npc) {
    const npcIndex = Module.ccall('d2_monster_name_str', 'number', ['number'], [npc.classId]);
    const npcLabel = npcIndex >= 0 ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [npcIndex]) : '';
    if (npcLabel) return { kind: 'unit', npc: true, id: npc.id, classId: npc.classId, stringIndex: npcIndex, label: npcLabel, x: npc.x, y: npc.y };
  }
  if (item) {
    const code = itemCode(item);
    const label = Module.ccall('d2_lang_string', 'string', ['string'], [code]) || code;
    return { kind: 'item', id: item.id, classId: item.classId, code, label, x: item.x, y: item.y };
  }
  const object = objectUnderPointer(pointer.x, pointer.y);
  if (!object) return null;
  const label = objectRetailName(Module, object.classId);
  return label ? { kind: 'object', id: object.id, classId: object.classId, token: object.token,
    label, x: object.x, y: object.y } : null;
}

// V4/F6b: WHICH cel retail draws is decided by what is under the pointer, and the shell already
// owns both resolvers -- the hostile selection the attack path consumes and the NPC hit test the
// talk path consumes. This asks them; it never keeps a second hover model of its own.
function gameCursorContext() {
  if (!session || screen !== SCREEN.GAME) return null;
  const { unitId } = resolvePointerTarget(pointer.x, pointer.y);
  if (unitId) return { kind: 'attack', target: { id: unitId, type: 1 } };
  const npc = npcUnderPointer(pointer.x, pointer.y);
  if (npc) return { kind: 'talk', target: { id: npc.id, type: 1 } };
  return { kind: 'hand' };
}

function drawHoverLabel(c, target, projectUnit, projectStatic) {
  if (!target || !hoverFont) return null;
  const glyphs = [], font = 'data\\local\\font\\latin\\font16.dc6';
  if (target.kind === 'unit') {
    // Retail: a colour-5 mode-1 box centred on the HUD's top
    // centre, the name as Font16 glyphs padded by two spaces each side (baseline y=31), then the
    // MonType line (MonType.txt StrSing, "Demon") as centred Font6 at y=49.
    const text = `  ${target.label}  `, width = hoverFont.measure(text), cx = Math.round(canvas.width / 2);
    const left = cx - Math.trunc(width / 2), box = { x: left, y: 18, width: width + 8, height: 16 };
    c.save(); c.fillStyle = 'rgba(0,0,0,.5)'; c.fillRect(box.x, box.y, box.width, box.height); c.restore();
    recordPrim('box', box.x, box.y, box.x + box.width, box.y + box.height, 5, 1);
    if (hoverGlyphPainting) hoverFont.drawAnchored(c, text, left + 4, 31, (glyph) => glyphs.push(glyph));
    const typeIndex = Module.ccall('d2_monster_type_str', 'number', ['number'], [target.classId]);
    const typeLine = typeIndex > 0 ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [typeIndex]) : '';
    if (typeLine && automapFont && !target.npc) automapFont.drawAnchored(c, typeLine + ' ', cx - Math.trunc(automapFont.measure(typeLine + ' ') / 2) + 4, 49);   // +4: retail's Font6 centred-text glyphs start 4px right of the text origin (measured, f7 vector)
    return { ...target, requested: target.label, font, glyphs, plateText: text, typeLine, x: cx, y: 31, box, placement: 'hud-top-centre' };
  }
  if (target.kind === 'object')
    return drawObjectHoverLabel(c, target, hoverFont, projectStatic, recordPrim, hoverGlyphPainting);
  // The drop stays world-anchored above its static-grid feet (retail's drop label: s286).
  const label = target.label, width = hoverFont.measure(label);
  const [worldX, foot] = projectStatic(target.x, target.y);
  const x = Math.round(worldX), y = Math.round(foot - 37);
  const left = Math.round(x - width / 2 - 5), top = y - hoverFont.lineHeight - 5;
  c.save(); c.fillStyle = 'rgba(0,0,0,.5)'; c.fillRect(left, top, width + 10, hoverFont.lineHeight + 10); c.restore();
  if (hoverGlyphPainting) hoverFont.draw(c, label, Math.round(x - width / 2), y - hoverFont.lineHeight,
    (glyph) => glyphs.push(glyph));
  return { ...target, requested: label, font, glyphs,
    x, y, box: { x: left, y: top, width: width + 10, height: hoverFont.lineHeight + 10 }, placement: 'above-ground-drop' };
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

// Retail's renderer brightens the final 8-bit palette before it reaches the screen (D2Gfx
// SetGamma/contrast into the renderer DLL -- D2MOO stops at the pfSetGamma call; Fog.dll's
// GFXUTIL_GenerateGammaRamp generates the ramp, called with the default Gamma 155 and Contrast 100
// by D2GFX_SetContrastAndGamma_6FA710C0). It is applied once, to the finished frame, exactly where
// retail applies it -- and it is DOWNLOADED FROM RETAIL'S OWN SCREEN, not fitted.
//
// How it was measured (client-parity C1d): retail's hook dumps its finished 8-bit framebuffer as
// palette indices (retail/drawlists/<scene>.fb.png.idx, drawhook kind 45 at EndDraw) beside a real
// screenshot of the same scene (retail/drawlists/<scene>.png, `import -window`). Pairing the two --
// index -> that act's pal.dat entry, per channel -- gives retail's own (in value -> out value) map
// directly. Thirteen in-game captures across the ACT1/ACT4/ACT5 palettes plus the s179 Warriv menu
// capture (retail/frames/npc-menu-before.idx with retail/frames/menu-warriv.png) agree EXACTLY on
// every value the palettes contain: 14 independent captures, 0 disagreements.
//
// What it is NOT: floor(v * 1.55). That was the title screen's fit, and the title is dark. It holds
// up to v = 76 and then runs one step high, saturating at 255 where retail saturates at 254:
// v = 80 -> 123 (not 124), 100 -> 154 (not 155), 148 -> 228 (not 229), and every value from 168 up
// -> 254 (not 255). No affine map fits the measurement (v = 20 needs a slope >= 1.55, v = 80 needs
// one < 1.55), so this is a table, which is also what a DirectDraw gamma ramp IS: retail fills a
// 256-entry ramp and installs it.
//
// The measurement is on the values a D2 palette holds: the classic palettes are 6-BIT per channel
// (every entry of ACT1..ACT4/Units/fechar/Sky is a multiple of 4, and so is all but eight entries of
// the expansion's ACT5), so the values that matter are the 6-bit grid plus the pure white index 255
// -- which is what proves the last entry: the ONLY colour of 2,748 observed in the fourteen captures
// that a table mapping 255 -> 254 cannot produce is (255,255,255).
const RETAIL_RAMP_6BIT = [
  0, 6, 12, 18, 24, 31, 37, 43, 49, 55, 62, 68, 74, 80, 86, 93, 99, 105, 111, 117, 123, 130,
  136, 142, 148, 154, 161, 167, 173, 179, 185, 191, 198, 204, 210, 216, 222, 228, 235, 241,
  247, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
  254, 254, 254, 254, 254, 254,
];
// A value between two measured grid points is not reachable from a classic palette; the expansion
// ACT5's eight odd entries are, and every one of their channels measures as floor(v * 1.55) (so do
// all 1,838 creation-screen colours -- C2h), so that is the fill -- clamped at the same 254.
const DISPLAY_RAMP = Uint8ClampedArray.from({ length: 256 }, (_, v) =>
  v === 255 ? 255 : v % 4 === 0 ? RETAIL_RAMP_6BIT[v >> 2] : Math.min(254, Math.floor(v * 155 / 100)));
function applyDisplayRamp(w, h) {
  const img = ctx.getImageData(0, 0, w, h), d = img.data;
  for (let i = 0; i < d.length; i += 4) { d[i] = DISPLAY_RAMP[d[i]]; d[i + 1] = DISPLAY_RAMP[d[i + 1]]; d[i + 2] = DISPLAY_RAMP[d[i + 2]]; }
  ctx.putImageData(img, 0, 0);
}

function render() {
  // The frame-cost instrument (assets/frame-perf.mjs) is refreshed per frame and, while off, every
  // perfT0()/perfT() below is a no-op: the shipped frame draws byte-identically.
  perfSync();
  const pRender = perfT0();
  const pWorld = perfT0();
  drawFrame();
  perfT('world', pWorld);
  if (screen === SCREEN.GAME && gameMenu?.open && !controlsPanel?.open) { const p = perfT0(); gameMenu.draw(ctx, canvas.width, canvas.height); perfT('menu', p); }
  if (screen === SCREEN.GAME && controlsPanel?.open) { const p = perfT0(); controlsPanel.draw(ctx, canvas.width, canvas.height); perfT('controls', p); }   // F218
  // THE POINTER IS THE LAST THING DRAWN, over the menu as over everything else: it is what retail's
  // D2Client paints its own cursor cel as, and the layer takes the browser arrow with it (never
  // neither). Before the ramp, because retail's cursor cel is a palette frame like every other.
  if (pointerLayer && screen === SCREEN.GAME) {
    const p = perfT0();
    const drawn = pointerLayer.draw(ctx, canvas, { session, items: snapshot.items, pointerX: pointer.x, pointerY: pointer.y,
      context: gameCursorContext(), tick: snapshot.tick,
      cursorId: Module._d2_session_cursor_item ? Module.ccall('d2_session_cursor_item', 'number', [], []) : 0 });
    cursorItemEvidence = drawn.item; gameCursorEvidence = drawn.game;
    perfT('pointer', p);
  }
  // The display ramp is retail's PALETTE brightening, and a boot movie is not a palette frame: its
  // pixels are blitted straight to the primary surface. Measured -- retail's own capture of a movie
  // frame matches our unramped decode to 0.14% RMSE and the same frame ramped to 2.6% (assets/
  // cinematics.mjs), so the ramp must stay off while a movie is up.
  if (!(screen === SCREEN.TITLE && bootMovieSpec(titlePhase)) && !(screen === SCREEN.CINEMATICS && cinematicsScreen?.evidence.playing != null)) {
    const p = perfT0(); applyDisplayRamp(canvas.width, canvas.height); perfT('ramp', p);
  }
  perfT('render', pRender);
  perfCountFrame();
}

function drawFrame() {
  fitView();   // also per frame: a resize that lands between events must never leave a stale view
  const w = canvas.width, h = canvas.height;
  // The title, character select and the movies draw retail's own pointer sprite (ohand.dc6); the
  // system arrow would be a second one. IN GAME the pointer layer below owns this style instead: the
  // arrow has to be hidden exactly while a pointer is actually drawn, which only that layer knows.
  if (screen !== SCREEN.GAME) {
    const cursorStyle = (screen === SCREEN.TITLE || screen === SCREEN.CINEMATICS || screen === SCREEN.SELECT) ? 'none' : '';
    if (canvas.style.cursor !== cursorStyle) canvas.style.cursor = cursorStyle;
  }
  if (screen === SCREEN.TITLE) {
    ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
    // A boot movie owns the whole screen while it runs, and the screen is BLACK until it opens --
    // retail's own ~6 s of black while the game initialises, not a placeholder frame.
    if (bootMovieSpec(titlePhase)) {
      bootMovie?.draw(ctx, w, h);
      hud.textContent = bootMovieError ? `cinematic: ${bootMovieError}` : (bootMovie ? '' : 'cinematic: loading…');
    } else if (titleScreen) {
      titleScreen.draw(ctx, w, h, titleTick, titlePhase);
      hud.textContent = titleError ? `title screen: ${titleError}` : '';
    } else {
      ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '16px Georgia,serif';
      ctx.fillText(titleError || 'loading…', w / 2, h / 2);
      hud.textContent = titleError ? `title screen: ${titleError}` : '';
    }
    // F91: the exit fade, then the notice, are painted over the finished menu (assets/exit-sequence.mjs).
    exitSeq?.draw(ctx, w, h);
    return;
  }
  if (screen === SCREEN.CINEMATICS) {
    ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
    if (cinematicsScreen) cinematicsScreen.draw(ctx, w, h);
    else { ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '16px Georgia,serif'; ctx.fillText(cinematicsError || 'loading…', w / 2, h / 2); }
    hud.textContent = cinematicsError ? `cinematics: ${cinematicsError}` : '';
    return;
  }
  if (screen === SCREEN.CREDITS) {
    ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h);
    if (creditsScreen) creditsScreen.draw(ctx, w, h, creditsTick);
    else { ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '16px Georgia,serif'; ctx.fillText(creditsError || 'loading…', w / 2, h / 2); }
    hud.textContent = creditsError ? `credits: ${creditsError}` : '';
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
  const camera = worldToScreen(cameraGrid().x, cameraGrid().y);
  const proj = (wx, wy) => { const p = worldToScreen(wx, wy); return [p.x - camera.x + viewCX(), p.y - camera.y + (h / 2 + UNIT_CENTRE_Y)]; };
  // Objects and ground items are static: on the integer grid under the half-subtile camera (8 px higher).
  const projStatic = (wx, wy) => proj(wx - 0.5, wy - 0.5);
  const hovered = hoverTarget();

  // Sprites are handed to the terrain pass rather than painted after it, so a wall in front of a
  // monster covers the monster. `depth` is the projected screen Y -- the same key the walls sort on.
  const sprites = [];
  corpseEvidence = null; unitDrawEvidence = []; objectDrawEvidence = []; waypointPadEvidence = []; missileEvidence = []; npcAlertEvidence = [];
  for (const entity of snapshot.entities) {
    // The player is drawn by drawPlayer and ground items by the item pass; neither is a dot here.
    if (entity.kind === 'item' || (entity.kind === 'player' && entity.id === snapshot.playerId)) continue;
    const [sx, sy] = proj(...unitAnchor(entity));
    // Another player unit in the dead mode is the player's corpse, drawn in its death pose
    // (TODO(client-parity F19): the corpse's facing is not on the unit record; direction 0).
    // A missile draws from its Missiles.txt CelFile (F20 keeps the dot for one that has none).
    if (entity.kind === 'missile') {
      // F20: the engine's own record for this missile -- its 64-slot facing and its sequence clock --
      // beside the Missiles.txt CelFile.  A missile the engine has no record for keeps the dot.
      const missile = entity.missile || null;
      sprites.push({ depth: sy, wx: entity.x, wy: entity.y, draw(c) {
        let drawn = null;
        try { drawn = missileRenderer?.draw(c, sx, sy, entity, missile ? missile.direction : 0, snapshot.tick,
          missile ? (missile.framePrecise >>> 8) : null, missile ? missile.trans : null); } catch { drawn = null; }
        if (drawn) missileEvidence.push({ ...drawn, id: entity.id, mode: missile ? missile.mode : null, x: Math.round(sx), y: Math.round(sy) });
        else { c.fillStyle = '#958968'; c.beginPath(); c.arc(sx, sy, 5, 0, Math.PI * 2); c.fill(); }
      } });
      continue;
    }
    if (entity.kind === 'player') {
      // The player's own body (F19). The evidence carries the ENGINE unit the recovery command
      // acts on (id/class/position/mode) and the anchor this blit was painted at -- the same point
      // the pointer path tests -- so a row can click the renderer's own anchor rather than
      // re-deriving the projection beside it and drifting from what was drawn.
      sprites.push({ depth: sy, wx: entity.x, wy: entity.y, draw(c) {
        const unit = { id: entity.id, classId: entity.classId, x: entity.x, y: entity.y, mode: entity.mode };
        const anchor = { x: Math.round(sx), y: Math.round(sy) };
        try { corpseEvidence = { unit, anchor, ...(corpseRenderer?.draw(c, sx, sy, { playerClass: entity.classId, mode: entity.mode, direction: 0, tick: snapshot.tick, act: snapshot.act }) || {}) }; }
        catch (error) { corpseEvidence = { unit, anchor, error: String(error) }; }
      } });
      continue;
    }
    sprites.push({ depth: sy, wx: entity.x, wy: entity.y, draw(c) {
      // Draw the retail sprite for ANY unit that has an animation. This used to require
      // entity.hostile, which tied "has a body" to "is an enemy" -- so once town NPCs were
      // correctly marked non-hostile they stopped being drawn and Charsi became a dot.
      const being = entity.animation ? monsterRenderer?.draw(c, sx, sy, entity.animation, snapshot.act, hovered?.kind === 'unit' && hovered.id === entity.id) : null;
      const drawn = Boolean(being);
      // F22: the alert's pending state is the ENGINE's per-unit bit (d2_session_npc_speech_alerts,
      // read into entity.speechPending above), never a client quest guess. The renderer's own record
      // of what it painted is kept beside the unit so a row can read the canvas back at that
      // rectangle -- the blit that reached the canvas, not the request that produced it.
      const alert = npcSpeechAlertRenderer?.draw(c, sx, sy, entity, entity.speechPending, snapshot.tick, snapshot.act);
      if (alert && alert.drawn) npcAlertEvidence.push({ id: entity.id, classId: entity.classId,
        anchorX: Math.round(sx), anchorY: Math.round(sy), tick: snapshot.tick, ...alert });
      // Per unit, what was drawn and where (s000e asserts Harrogath's townsfolk against retail's draw list).
      // The identity the renderer DREW is carried through beside the mode the shell handed it -- without
      // it a row can only say a unit was drawn, not that it drew the sequence the engine asked for (s278).
      unitDrawEvidence.push({ id: entity.id, classId: entity.classId, code: entity.animation?.code ?? null, mode: entity.animation?.mode ?? null,
        token: being?.token ?? null, drawMode: being?.mode ?? null, frame: being?.frame ?? null, frames: being?.frames ?? null, direction: being?.direction ?? null,
        directions: being?.directions ?? null,
        // F14: the cel rectangle this blit painted, anchor-relative -- the rectangle a hover-box rule can
        // be asked about per unit (`x`/`y` here are the same rounded anchor `unitBoxHit` tests against).
        celWidth: being?.width ?? null, celHeight: being?.height ?? null, celXOffset: being?.xOffset ?? null, celYOffset: being?.yOffset ?? null,
        animBox: being?.animBox ?? null,
        drawn, x: Math.round(sx), y: Math.round(sy) });
      if (drawn) return;
      c.fillStyle = entity.hostile ? '#b33b2e' : '#958968';
      c.beginPath(); c.arc(sx, sy, 5, 0, Math.PI * 2); c.fill();
    } });
  }
  for (const object of snapshot.objects) {
    const [sx, sy] = projStatic(object.x, object.y);
    const isHovered = hovered?.kind === 'object' && hovered.id === object.id;
    sprites.push({ depth: sy, wx: object.x, wy: object.y, draw(c) {
      const drawn = objectRenderer?.draw(c, sx, sy, object, snapshot.act, isHovered);
      if (drawn) objectDrawEvidence.push({ id: object.id, classId: object.classId, token: object.token,
        mode: drawn.mode, drawMode: isHovered ? 7 : 5, direction: drawn.direction, frame: drawn.frame,
        x: Math.round(sx), y: Math.round(sy), width: drawn.width, height: drawn.height,
        xOffset: drawn.xOffset, yOffset: drawn.yOffset, layers: drawn.layers.map((layer) => ({ ...layer })) });
      const waypoint = waypointPadAnchor(Module, object, drawn, sx, sy); if (waypoint) waypointPadEvidence.push(waypoint);
    } });
  }
  for (const item of snapshot.items) if (item.mode === 3 && item.invPage === 255) {
    const [sx, sy] = projStatic(item.x, item.y);
    sprites.push({ depth: sy, wx: item.x, wy: item.y, draw(c) { itemRenderer?.draw(c, sx, sy, item); } });
  }
  sprites.push({ depth: (h / 2 + UNIT_CENTRE_Y), wx: snapshot.player.x, wy: snapshot.player.y, draw: drawPlayer });

  terrainStats = null;
  // The frame's own terrain read, handed to the automap: it fills its per-level cell layer from this
  // rather than reading the tiles a second time (F5b/V6; the layer must fill on frames the player is
  // not looking at the map too, so it cannot be scoped to the overlay's own draw).
  let terrainTiles = null;
  if (terrain && session) {
    try {
      // The engine's library list is rebuilt with the world (rooms of a neighbouring level bring
      // their own): read it with the tiles that index it, never from an earlier frame. The tiles of
      // the neighbouring levels' active rooms come with it (across a seam retail draws them, F24).
      const pRead = perfT0();
      const { tiles, files } = readSessionTerrain(Module);
      terrainTiles = tiles;
      if (tiles.length) {
        terrainFiles = files;
        // Floors are lit from the engine's light map, as D2Client lights them.
        const lightmap = readSessionLightmap(Module);
        terrain.setLightGrid(lightmap ? (a, b) => lightmap.grid(a, b) : null, lightmap?.at, lightmap?.ambient);
        // Retail's weather (F11b): the level's own Levels.txt `Rain` byte decides, and the rain draws
        // between the floors and the walls, where retail's own frame puts it (session-terrain.mjs).
        const rain = weatherRenderer && Module._d2_session_level_rain ? Module.ccall('d2_session_level_rain', 'number', [], []) : -1;
        weatherRenderer?.setLevel(rain);
        weatherRenderer?.advance(engineTicks);
        weatherEvidence = { rain, blits: [] };
        // Retail's camera is the player's PRECISE position, a subtile's centre (x + 0.5, y + 0.5): the
        // static world (floors, walls, objects, ground items) sits on the integer grid under it, while
        // units stand at their centres too.
        const wcam = worldToScreen(cameraGrid().x, cameraGrid().y);
        perfT('worldRead', pRead);
        const pTerrain = perfT0();
        terrainStats = terrain.draw(ctx, tiles, terrainFiles, { ...snapshot.player, ...cameraCentre() }, w, h, sprites,
          // A broken weather must not take the world down with it: the terrain pass catches nothing, and
          // the error is reported on the evidence a row reads rather than swallowed.
          weatherRenderer ? (c, floors, viewport) => {
            try { weatherEvidence = { rain, blits: weatherRenderer.draw(c, w, h, engineTicks, wcam.x, wcam.y, weatherWaterAnchors(floors, viewport)), errors: weatherRenderer.errors }; }
            catch (error) { weatherEvidence = { rain, blits: [], errors: weatherRenderer.errors, error: String(error) }; }
          } : null, viewCX());
        perfT('terrain', pTerrain);
      } else perfT('worldRead', pRead);
    } catch (error) { terrainError = String(error); terrain = null; }
  }
  // Without terrain there is no wall pass to interleave into, so the sprites still need painting.
  if (!terrainStats?.drawn) for (const sprite of sprites) sprite.draw(ctx);
  // Retail's falling rain drops are line primitives drawn after the whole world, before the HUD (F11b).
  if (weatherRenderer && terrain && weatherEvidence.rain > 0) {
    const pWeather = perfT0();
    try { const cam = worldToScreen(cameraGrid().x, cameraGrid().y); weatherEvidence.lines = weatherRenderer.drawStreaks(ctx, w, h, engineTicks, cam.x, cam.y); weatherEvidence.cycle = { ...weatherRenderer.cycle.state, intensity: weatherRenderer.cycle.intensity }; }
    catch (error) { weatherEvidence.streakError = String(error); }
    perfT('weather', pWeather);
  }
  // Draw after the world passes so the name plate remains legible over foreground terrain, like
  // the HUD's foreground label.  Its subject was resolved before any drawing from F14's target.
  // The name plate is chrome, like the rest of the HUD: ClearScreen draws none of it (F3c).
  const pHover = perfT0();
  hoverEvidence = clearScreen ? null : drawHoverLabel(ctx, hovered, proj, projStatic);
  perfT('hover', pHover);
  // The whole statement (argument construction included) is the automap pass: its npc-name projection
  // runs retail string lookups per npc every frame, and a pass timed only around the call would miss it.
  const pAutomap = perfT0();
  automapOverlay?.draw(ctx, w, h, snapshot, terrainTiles, automapOverlay.isOpen ? { camera: cameraCentre(), font: automapFont,
    npcs: snapshot.entities.filter((e) => e.npc).map((e) => { const [sx, sy] = proj(...unitAnchor(e));
      const idx = Module.ccall('d2_monster_name_str', 'number', ['number'], [e.classId]);
      return { sx, sy, label: idx >= 0 ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [idx]) : '' }; }) } : null);
  perfT('automap', pAutomap);
  // A panel open on a half turns that half into tiled stone with its frame (SGD2FreeRes draw_screen_background).
  const pWide = perfT0();
  wideFrameEvidence = null; globalThis.__d2WideFrame = false;
  if (window.__d2Entry?.fill && w > 800 && !clearScreen && ensureFreeRes()?.ready) {
    const [left, right] = panelSides(), sides = [left && 'left', right && 'right'].filter(Boolean), parts = [];
    for (const side of sides) { parts.push(...stoneTiles(w, h, side), ...halfParts(w, h, side)); }
    drawParts(ctx, freeResArt, parts);
    if (sides.length) { globalThis.__d2WideFrame = true; wideFrameEvidence = { sides, shiftX: viewCX() - w / 2, parts: parts.map(({ name, frame, x, y, w: pw, h: ph }) => ({ name, frame, x, y, w: pw, h: ph })) }; }
  }
  perfT('wide', pWide);
  // Everything from here to the end of the frame is HUD chrome (death screen, npc menu, every panel,
  // the control panel, the belt, the area name, the help and message overlays): one 'hud' pass so the
  // chrome's share of the frame is attributable, separate from the world.
  const pHud = perfT0();
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
  // Dead: retail's lettering (play/assets/death-screen.mjs); Esc continues. Stat 175 is
  // STAT_GOLDLOST, which the engine's own death path (PLRMODE_StartID_Death ->
  // PLAYER_ApplyDeathPenalty) sets; retail draws its string only when it is non-zero.
  if (session && snapshot.mode === 17 && ensureDeathScreen()) {
    deathScreenEvidence = deathScreen.draw(ctx, w, h, { difficulty: snapshot.difficulty ?? 0, act: snapshot.act,
      goldLost: Module.ccall('d2_session_player_stat', 'number', ['number', 'number'], [175, 0]) });
  } else deathScreenEvidence = null;
  // The NPC's menu hangs over their head until an entry is picked.
  npcMenuEvidence = null;
  if (npcMenu?.open) {
    const npc = snapshot.entities.find((u) => u.id === npcMenu.state.npcId && u.type === 1);
    if (npc) { const cam = worldToScreen(cameraGrid().x, cameraGrid().y), pt = worldToScreen(npc.x, npc.y);
      npcMenuEvidence = npcMenu.draw(ctx, pt.x - cam.x + viewCX(), pt.y - cam.y + (h / 2 + UNIT_CENTRE_Y), snapshot.act); }
    else npcMenu.close();
  }
  hireWindowEvidence = npcMenu?.hireOpen ? npcMenu.drawHire(ctx, snapshot.act) : null;
  // An open conversation draws over the world, the way retail's NPC window does.
  // A shop opening brings retail's inventory up beside it, on its opening tab.
  syncShopInventory();
  // __d2HideShop: a panel row's no-panel render (lib/panel-frame.mjs toggle), never set by play.
  // Active speech wins over an already-open shop (Codex review of ba9bb11): retail speaks FIRST, even
  // to a vendor -- Akara's greeting shows the speech box, and only picking Trade brings up her shop.
  // Checking shopOpen() before npcState.spoken drew the trade panel under a vendor's own greeting.
  // But the engine's dialog record keeps the SAME message id for the rest of the interaction (Esc's
  // ack tells the server it was heard, it does not clear our copy), so "spoken" alone would redraw
  // the speech box over Akara's own shop forever once Trade was picked. A shop that is actually open
  // (her stock fetched -- shopOpen()) cannot happen in retail before its greeting is already gone.
  if (npcState?.spoken != null && !shopOpen() && ensureSpeechBox()) {
    try {
      // P7b: an Introduction / Gossip speech auto-scrolls (retail talk-*-r22) and hands back to the topic box when it ends.
      const scrolling = npcState.speechT0 !== undefined;
      const spoke = speechBox.draw(ctx, npcState.greeting, undefined, scrolling ? { t0: npcState.speechT0 } : {});
      npcPanelEvidence = spoke; npcPanelError = '';
      if (scrolling && spoke.done) dismissSpeech();
    }
    catch (error) { npcPanelError = String(error); npcPanelEvidence = null; }
  } else if (shopOpen() && !globalThis.__d2HideShop && ensureVendorPanel()) {
    try { npcPanelEvidence = vendorPanel.draw(ctx, w, h, { ...npcState, pointer, act: snapshot?.act, stashGold: Module._d2_session_stash_gold ? Module.ccall('d2_session_stash_gold', 'number', ['number'], [0]) : -1 }); npcPanelError = ''; }
    catch (error) { npcPanelError = String(error); npcPanelEvidence = null; }
  } else if (npcState && ensureSpeechBox()) {
    // Plain talk, no shop, no active speech message: still the speech box (
    // F21), not the trade panel -- that was drawn for every conversation regardless of whether the
    // NPC sells anything, which is not what retail shows for a Talk with no shop behind it.
    try { npcPanelEvidence = speechBox.draw(ctx, npcState.greeting); npcPanelError = ''; }
    catch (error) { npcPanelError = String(error); npcPanelEvidence = null; }
  } else if (npcState && ensureNpcPanel()) {
    try { npcPanelEvidence = npcPanel.draw(ctx, w, h, { ...npcState, playerItems: snapshot.items }); npcPanelError = ''; }
    catch (error) { npcPanelError = String(error); npcPanelEvidence = null; }
  } else if (objectSpeech && ensureSpeechBox()) {
    try { npcPanelEvidence = speechBox.draw(ctx, objectSpeech.text); npcPanelError = ''; }
    catch (error) { npcPanelError = String(error); npcPanelEvidence = null; }
  } else npcPanelEvidence = null;
  // Inventory.txt's record is per class: re-read whenever the session's character changes (a new
  // session, a loaded save, another class), not only when none is cached.
  const layoutKey = session ? `${snapshot.playerClass}/${snapshot.playerId}/${snapshot.seed}` : '';
  if (inventoryPanelRenderer && session && (!inventoryPanelRenderer.layout || inventoryLayoutKey !== layoutKey)) {
    inventoryPanelRenderer.setLayout(readInventoryLayout()); inventoryLayoutKey = layoutKey; }
  if (inventoryPanelRenderer) inventoryPanelEvidence = inventoryPanelRenderer.draw(ctx, w, h, inventoryVisible && session ? { ...snapshot, gold: Module.ccall('d2_session_player_gold', 'number', [], []) } : snapshot, panelShown(inventoryVisible));
  pollUiEvents();
  if (stashPanelRenderer) {
    if (stashPanelRenderer?.plugy.on) { stashPanelRenderer.plugy.pageName = `${plugyStash.showShared ? 'Shared' : 'Personal'} Page n\u00b0${plugyStash.page + 1}`; stashPanelRenderer.plugy.nameColour = plugyStash.showShared ? 1 : 0; }
    try { stashEvidence = stashPanelRenderer.draw(ctx, w, h, stashOpen ? readStashLayout() : null, snapshot.items, stashOpen, snapshot.act); stashPanelError = ''; }
    catch (error) { stashPanelError = String(error); stashEvidence = null; }
    plugyStash.text = stashPanelRenderer.text; plugyStash.pointer(pointer.x, pointer.y, w, h);
    plugyEvidence = plugyStash.draw(ctx, w, h, stashOpen ? readStashLayout() : null, stashOpen, snapshot.act || 1);
  }
  if (cubePanelRenderer) {
    try { cubeEvidence = cubePanelRenderer.draw(ctx, w, h, cubeOpen ? readCubeLayout() : null, snapshot.items, cubeOpen, snapshot.act); }
    catch (error) { stashPanelError = 'cube: ' + error; cubeEvidence = null; }
  }
  if (stashGoldRenderer) {
    try { stashGoldEvidence = stashGoldRenderer.draw(ctx, w, h, stashOpen ? readStashLayout() : null, stashOpen,
      { ...(stashGoldDialog || {}), bank: stashGoldStat(STASH_GOLD.bank) }); stashGoldError = ''; }
    catch (error) { stashGoldError = String(error); stashGoldEvidence = null; }
  }
  if (waypointPanelRenderer) {
    try {
      const wp = waypointMenu ? readWaypointTable() : null;
      waypointEvidence = waypointPanelRenderer.draw(ctx, w, h, wp ? wp.rows(waypointMenu.act) : [], waypointMenu?.act ?? snapshot.act, Boolean(waypointMenu), snapshot.level, snapshot.act, wp?.tabs ?? null);
      waypointPanelError = '';
    }
    catch (error) { waypointPanelError = String(error); waypointEvidence = null; }
  }
  if (characterPanelRenderer) {
    const adv = readAdvancement();
    // Life and mana are the snapshot's, not the advancement record's: the panel shows what the
    // player has right now, and the engine is the only thing that decides that.
    characterPanelEvidence = characterPanelRenderer.draw(ctx, w, h, adv && {
      ...adv, name: chosenName || '', className: Module.ccall('d2_lang_string', 'string', ['string'], [CLASS_NAMES[snapshot.playerClass] || '']) || CLASS_NAMES[snapshot.playerClass] || '',
      // Retail stores life and mana as 8.8 fixed point (50 life is 12800) and displays the whole
      // part. Drawing the raw stat put "14848" in the Life slot.
      life: (snapshot.player?.hp ?? 0) >> 8, lifeMax: (snapshot.player?.hpMax ?? 0) >> 8,
      mana: (snapshot.player?.mana ?? 0) >> 8, manaMax: (snapshot.player?.manaMax ?? 0) >> 8, act: snapshot.act,
      screen: characterVisible ? readCharacterScreen() : null,
      string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]),
    }, panelShown(characterVisible));
  }
  questEvidence = null;
  if (questLogRenderer && panelShown(questVisible) && session && !clearScreen) {
    const words = readQuestWords(), act = questAct || (questAct = snapshot.act || 1, questSelected = questFirst(words, questAct), questAct);
    questEvidence = questLogRenderer.draw(ctx, w, h, { act, tabs: questTabs(words, snapshot.act || 1), words, selected: questSelected, descrOn: questDescrOn,
      anim: { quests: questAnim, tick: engineTicks - questOpenedAt },
      string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]) }, true, snapshot.act || 1);
  }
  if (skillPanelRenderer) {
    const adv = readAdvancement();
    skillPanelEvidence = skillPanelRenderer.draw(ctx, w, h, skillsVisible ? {
      // The class is the snapshot's top-level field (player.playerClass does not exist: the tree was the Amazon's for everyone).
      playerClass: snapshot.playerClass ?? 0, level: adv?.level ?? 1, act: snapshot.act,
      string: (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]),
      skillPoints: adv?.skillPoints ?? 0, skills: readClassSkills() || [],
      // SkillDesc's wStrName/wStrLong/wStrMana are .tbl indices, so the hover plate's name,
      // description and mana line are retail's own strings, not a table this client keeps.
      line: (index) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]), skillDesc: readSkillDesc, skillMana: (id, level) => Module.ccall('d2_session_skill_mana', 'number', ['number', 'number'], [id, level]) } : null, panelShown(skillsVisible));
  }
  // s693: retail's level-ENTRY banner -- the arriving level's own `EntryFile` art, drawn for 120 game
  // frames from the arrival. It goes here, before the HUD chrome, because that is where retail's own
  // frame draws it (the capture's two celcolor records precede the control panel's cels in call
  // order). The gate is the shell's: the level the world was last drawn for, and the engine tick that
  // level was first seen on. A change of level IS an arrival -- a waypoint, a seam, a new game -- and
  // the level id is the engine's, never the client's. The 120 is retail's own measured window (assets/level-entry-banner.mjs). Nothing is
  // drawn for a ClearScreen (F3c) or before a session exists.
  entryBannerEvidence = null; entryBannerError = '';
  if (session && entryBannerRenderer && entryBannerLevel > 0) {
    if (!clearScreen && engineTicks - entryBannerAt < ENTRY_BANNER.frames) {
      try {
        entryBannerEvidence = entryBannerRenderer.draw(ctx, w, h, { levelId: entryBannerLevel, act: snapshot.act,
          entryFile: Module.ccall('d2_level_entry_file', 'string', ['number'], [entryBannerLevel]) });
      } catch (error) { entryBannerEvidence = null; entryBannerError = String(error); }
    }
  } else if (!session) entryBannerLevel = -1;
  // The bar receives the engine's advancement record directly. It does not keep a client-side XP
  // counter: a kill, quest reward, or level-up can change both fields at any engine tick.
  // The control panel's HUD pieces (the stamina bar, the run/walk button, the icon row and its
  // open/close arrow -- V8/F10) are drawn here too. The stamina pair is the
  // engine's own (d2_session_character_screen out[4]/out[5], the same read the character screen
  // uses), never a client-side copy.
  //
  // The icon row slides away from a side panel and is hidden when one is open on each side, which is
  // what retail's own draw lists show: charpanel and waypoint-menu (left) draw it 131 px right,
  // skilltree and skilltree-act5 (right) 128 px left, stash-town and vendor-malah-act5 (both sides)
  // draw no minipanel_s and no minipanelbtn at all while the arrow stays at x=392 in every one.
  if (helpRenderer && helpVisible) helpRenderer.dim(ctx, w, h);   // retail: the world is dimmed, then the chrome and the screen are drawn over it
  if (controlPanelRenderer && clearScreen) controlPanelEvidence = null;
  if (controlPanelRenderer && !clearScreen) {
    const screen = readCharacterScreen();
    const left = characterVisible || questVisible || stashOpen || waypointMenu || Boolean(npcState);
    const right = skillsVisible || inventoryVisible;
    const shift = HUD_LAYOUT.miniPanelShift;
    controlPanelEvidence = controlPanelRenderer.draw(ctx, w, h, snapshot, readAdvancement(),
      { iconRow: hudIconRow && !(left && right), offsetX: left && !right ? shift.left : right && !left ? shift.right : 0,
        // The ARROW's own toggle, which is a different state from the row being hidden: retail
        // keeps menubutton frame 2 while the row is gone (stash-town.txt), so only the arrow's own
        // click may show frame 0. s391 measures this against retail's own list.
        arrowOpen: hudIconRow,
        // The run/walk toggle is the ENGINE's, like every other HUD state here: the same flag that
        // decides whether d2_session_command_move sends retail's 0x01 Walk or its 0x03 Run packet
        // is what picks the run button's cel. The client never keeps its own copy of it.
        runToggle: readRunToggle(), miniPressed: miniDown,
        // The ornate bar strips between the skill buttons and the console (wide views only; fitView's `fill`).
        strips: w > 800 && ensureFreeRes()?.ready ? (c) => { c.save(); c.setTransform(1, 0, 0, 1, 0, 0); drawParts(c, freeResArt, barParts(w, h)); c.restore(); } : null,
        stamina: screen ? { value: screen[4], maximum: screen[5] } : null });
  }
  // F115: retail's area-name block -- the level's own name above `v 1.13` and `EXPANSION` -- drawn
  // exactly while the automap overlay is drawn, and never otherwise. The block is the AUTOMAP's, not a
  // timed play-screen window: the three draw-list vectors that carry any of its three text records are
  // the only three that carry automap cells (826, 890 and 885 maximap draws), and the automap-OFF twins
  // of two of them have neither the cells nor the lines, in the same scene, with the rest of the HUD
  // still drawn (assets/area-name-block.mjs). So the state read here is the automap's own, taken after
  // its draw on this frame -- the gate a level-change timer used to be. The name is read from the engine
  // every time it is drawn, so a level the client never saw named still draws its own name. Chrome like
  // the rest of the HUD, so ClearScreen draws none of it (F3c).
  areaNameEvidence = null; areaNameError = '';
  if (session && !clearScreen && automapOverlay?.isOpen) {
    try { areaNameEvidence = areaNameRenderer?.draw(ctx, engineLevelName(snapshot.level), snapshot.act); }
    catch (error) { areaNameEvidence = null; areaNameError = String(error); }
  }
  hudHelpEvidence = null;
  // `!helpVisible` joins the gate on the same rule the other four already follow (assets/hud-help.mjs:
  // the label is drawn when the right-hand half is free): the help screen's own "Right Mouse- / Button
  // Skill / (Click to Change)" text occupies exactly the rows and columns this label lands on
  // (font16 rows 394..403 at x 714..790 against formal12 at x 620..730), and it is the screen this
  // label announces, so drawing it over the screen it opens is a collision, not information.
  if (hudHelp && !clearScreen && !helpVisible && !helpMenuSeen() && !skillsVisible && !shopOpen() && !stashOpen) {
    try { const ho = hudOrigin(w, h, 'bottom-right'); ctx.save(); ctx.translate(ho.x, ho.y);
      try { { const d = hudHelp.draw(ctx, snapshot.act || 1); hudHelpEvidence = { ...d, hit: { x: Math.round(ho.x + d.hit.x), y: Math.round(ho.y + d.hit.y), w: d.hit.w, h: d.hit.h }, origin: { x: ho.x, y: ho.y } }; } } finally { ctx.restore(); } hudHelpError = ''; }
    catch (error) { hudHelpEvidence = null; hudHelpError = String(error); }
  }
  // Retail's Help screen, over the HUD it describes -- its labels' leader lines point at HUD
  // controls, so the chrome must still be under it -- and under the pointer. Shown/hidden from its
  // own state (`H`), never from a panel's.
  messageEvidence = null;
  if (messageLogRenderer && messageVisible && !clearScreen) {
    const str = (key) => Module.ccall('d2_lang_string', 'string', ['string'], [key]);
    messageEvidence = messageLogRenderer.draw(ctx, w, h, { title: str('strMsgLog'), close: str('strClose'), lines: messageLines, scroll: messageScroll }, true, snapshot.act || 1);
  }
  if (messageLogRenderer && chat !== null && !clearScreen) {
    chatEvidence = messageLogRenderer.drawChatBar(ctx, w, h, chat, snapshot.act || 1);
  } else chatEvidence = null;
  postedEvidence = null;
  if (messageLogRenderer && !messageVisible && chat === null && messageLines.length && performance.now() - postedAt < POSTED_MS && !clearScreen) {
    postedEvidence = messageLogRenderer.drawPosted(ctx, w, h, messageLines[messageLines.length - 1], snapshot.act || 1);
  }
  // The belt is drawn with the HUD row's boxes, not the whole worn record (readBeltHudBoxes above):
  // `record`/`recordBoxes` carry which Belts.txt row the engine read and how many boxes it has, so a
  // row can tell a 16-box Girdle from the four-box record a bare character has, and `boxes` is what
  // was actually laid out.
  if (beltRenderer) { const bo = hudOrigin(w, h, 'bottom');   // belt boxes are retail 800x600 coordinates; bottom-centre anchored
    ctx.save(); ctx.translate(bo.x, bo.y);
    beltEvidence = { ...beltRenderer.draw(ctx, snapshot.items, readBeltHudBoxes(), true),
      record: readBeltIndex(), recordBoxes: readBeltBoxes()?.length ?? 0, origin: { x: bo.x, y: bo.y } };
    ctx.restore(); }
  else if (clearScreen) beltEvidence = null;
  // The item on the cursor and the world cel are drawn by the pointer layer in render(), after the
  // Esc menu -- the last draw of the frame, which is where retail's own pointer goes.
  if (skillSelectRenderer && session && !clearScreen) { try { skillSelectEvidence = skillSelectRenderer.draw(ctx, w, h, readSkillChoices(), readSelectedSkills(),
    { act: snapshot.act, hotkeys: Array.from({ length: 8 }, (_, i) => ({ skill: Number(Module.ccall('d2_session_hotkey_skill', 'number', ['number'], [i])), left: Boolean(Module.ccall('d2_session_hotkey_hand', 'number', ['number'], [i])) })), skillStrings: readSkillStrings, plateState: skillPlateState(), line: (i) => (i ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [i]) : '') }); } catch (error) { skillSelectError = String(error); } }
  else if (clearScreen) { skillSelectEvidence = null; skillSelectError = ''; }
  // Retail draws the Help screen over the finished HUD chrome (belt, skill sockets): after them, not before.
  helpEvidence = null;
  if (helpRenderer && helpVisible) {
    try { helpEvidence = helpRenderer.draw(ctx, w, h); helpError = ''; }
    catch (error) { helpEvidence = null; helpError = String(error); }
  }
  hud.textContent = hudLines(snapshot, Boolean(session), fps).join('\n');
  perfT('hud', pHud);
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
  // the unsupported-browser path sets __d2.ready=true with error=null (the probe failing
  // before the engine's script ever loads Module is not a boot FAILURE, it is the honest "this
  // browser cannot run it" verdict) -- so the `window.__d2.error` check above does not catch it, and
  // this loop used to call straight into ensureBootMovie's `createMpqAssetReader(Module)`, throwing
  // "ReferenceError: Module is not defined" into the debug log every 200ms behind the very screen
  // that exists to tell the player why, in words, nothing is happening (measured: this ALSO fires
  // for a genuine boot failure with a synchronous exception, since window.__d2.error is set but the
  // check above runs before this new one -- harmless, both are "do not touch the front end", just
  // one extra class check). document.body.classList.contains('unsupported') is the one flag both
  // showUnsupported and showBootFailure (play/index.html) always set, so it is the correct gate.
  if (!window.__d2?.ready || window.__d2.error || document.body.classList.contains('unsupported')) return;
  if (screen === SCREEN.TITLE) { ensureTitleScreen(); stepBootMovie(); }
  else if (screen === SCREEN.CREDITS) ensureCreditsScreen();
  else if (screen === SCREEN.CINEMATICS) { ensureCinematicsScreen(); cinematicsScreen?.step(); }
  else if (screen === SCREEN.SELECT) ensureCharSelect();
  else if (screen === SCREEN.CREATE) ensureCharCreate();
  // play=1 boots straight into a session; the same trap applies (d2-lxc's headless tabs report
  // visibilityState "hidden" and never run a frame, so no session ever started).
  else if (!bootstrapStarted && PLAY_MODE && window.__d2Entry.retail && sessionAdapter()) bootstrapSession();   // same one answer as above
}, 200);

function frame(now) {
  perfSync();
  // The rAF interval is the honest frame time in a real browser (a visible tab): what the player
  // feels, vsync and all. It is recorded before `last` moves; in the test runner's background tab rAF
  // never fires, so this pass is absent there and the per-frame samples are the engine's own cost.
  if (perfEnabled()) perfSample('frameInterval', now - last);
  accumulator = Math.min(accumulator + now - last, STEP * 5); last = now;
  try {
    const pUpdate = perfT0();
    while (accumulator >= STEP) { update(); accumulator -= STEP; }
    perfT('update', pUpdate);
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
    for (const code of Module.HEAPU8.slice(p, p + n)) {
      // Retail opens the inventory beside the stash (s197). A close invalidates
      // any modal amount before another key can turn it into a stale transfer.
      if (code === 16) { stashOpen = true; inventoryVisible = true; }
      else if (code === 21) { cubeOpen = true; inventoryVisible = true; }   // UPDATEUI_OPENCUBE, sent by the cube's use spell
      else if (code === 17) {
        stashOpen = false;
        inventoryVisible = false;
        closeStashGold();
      }
    }
    if (Module.ccall('d2_session_take_waypoint_menu', 'number', ['bigint'], [BigInt(p)])) waypointMenu = { id: Module.HEAPU32[p / 4], act: snapshot.act };
  } finally { Module._free(p); }
}
// The current level's own name, from the engine's own table -- Levels.txt's LevelName resolved through
// the string tables exactly as the waypoint menu names its rows. The client keeps no level-name table:
// a second copy here is what F115 exists to rule out.
function engineLevelName(levelId) {
  const key = Module.ccall('d2_level_name', 'string', ['number'], [levelId | 0]);
  return Module.ccall('d2_lang_string', 'string', ['string'], [key]) || key;
}
// The engine's whole waypoint table, as the engine reports it, named through retail's own strings:
// the rows of one act (what the menu lists) and the acts retail draws a tab for (tabActsOf -- one
// per act with an active waypoint; retail's captures draw one, two and four).
function readWaypointTable() {
  const p = Module._malloc(64 * 16);
  try {
    const n = Module.ccall('d2_session_waypoints', 'number', ['bigint', 'number'], [BigInt(p), 256]);
    const a = new Int32Array(Module.HEAPU8.buffer.slice(p, p + n * 16)), byAct = new Map();
    for (let i = 0; i < n; i++) {
      const act = a[i * 4 + 2] + 1;
      if (!byAct.has(act)) byAct.set(act, []);
      const key = Module.ccall('d2_level_name', 'string', ['number'], [a[i * 4 + 1]]);
      byAct.get(act).push({ waypointNo: a[i * 4], levelId: a[i * 4 + 1], active: a[i * 4 + 3] === 1,
        name: Module.ccall('d2_lang_string', 'string', ['string'], [key]) || key });
    }
    return { rows: (act) => byAct.get(act) || [], tabs: tabActsOf(byAct) };
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
    if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadSessionTerrain(snapshot.act - 1);
  }
  // Preserve level 0 too: it is retail's close packet, and the browser proof distinguishes an
  // engine-accepted close from merely hiding the panel locally.
  lastWaypointTravel = { levelId, result };
  return result;
}
// The stash grid is retail's own Inventory.txt record for a LoD character, via the engine.
// The player's own inventory panel from Inventory.txt through the engine: the class's row
// (UNITS_GetInventoryRecordId), 800x600, its grid and its ten equipment slots.
let cursorItemEvidence = null, lastItemCommand = null;
const pointer = { x: 0, y: 0 };
function readInventoryLayout() {
  if (!Module._d2_session_inventory_components) return null;
  const row = Module.ccall('d2_session_player_inventory_record', 'number', [], []);
  if (row < 0) return null;
  const p = Module._malloc(64 * 4);
  try {
    if (!Module.ccall('d2_session_inventory_layout', 'number', ['number', 'number', 'bigint', 'number'], [row, 1, BigInt(p), 12])) return null;
    const grid = Array.from(new Int32Array(Module.HEAPU8.buffer.slice(p, p + 48)));
    if (!Module.ccall('d2_session_inventory_components', 'number', ['number', 'number', 'bigint', 'number'], [row, 1, BigInt(p), 60])) return null;
    const comps = Array.from(new Int32Array(Module.HEAPU8.buffer.slice(p, p + 240)));
    return { ...inventoryLayoutFromEngine(grid, comps), row };
  } finally { Module._free(p); }
}
// Retail's item-cursor packets, through the engine's recovered handlers. Each returns the handler's
// result and is recorded in lastItemCommand.
function itemCommand(name, ...args) {
  let result;
  if (name === 'lift') result = Module.ccall('d2_session_command_item_to_cursor_packet', 'number', ['number'], [args[0]]);             // 0x19
  // 0x18 {itemGUID, x, y, invPage}: page 0 is the pack, 4 the LoD stash (the handler refuses page 4
  // outside a town room), 3 the cube. The page is the caller's, never assumed.
  else if (name === 'put') result = Module.ccall('d2_session_command_insert_item_packet', 'number', ['number', 'number', 'number', 'number'], [args[0], args[1], args[2], args[3] ?? 0]);
  else if (name === 'equip') result = Module.ccall('d2_session_command_equip_packet', 'number', ['number', 'number'], [args[0], args[1]]);   // 0x1A
  else if (name === 'swap') result = Module.ccall('d2_session_command_cursor_item_packet', 'number', ['number', 'number', 'number'], [0x1D, args[0], args[1]]);   // 0x1D
  else if (name === 'drop') result = Module.ccall('d2_session_command_cursor_item_packet', 'number', ['number', 'number', 'number'], [0x17, args[0], 0]);   // 0x17
  else if (name === 'unequip') {   // 0x1C {bodyLoc:u16}
    const p = Module._malloc(3); Module.HEAPU8.set([0x1C, args[0] & 255, args[0] >> 8], Number(p));
    try { result = Module.ccall('d2_session_client_packet', 'number', ['bigint', 'number'], [BigInt(p), 3]); } finally { Module._free(p); }
  }
  lastItemCommand = { name, args, result, cursor: Module.ccall('d2_session_cursor_item', 'number', [], []) };
  snapshot = session.snapshot();
  return result;
}
function readCubeLayout() {
  if (cubeLayout) return cubeLayout;
  const p = Module._malloc(48);
  try {
    const ok = Module.ccall('d2_session_inventory_layout', 'number', ['number', 'number', 'bigint', 'number'], [TRANSMOGRIFY_BOX_PAGE_1, 1, BigInt(p), 12]);
    cubeLayout = ok ? stashGeometry(Array.from(new Int32Array(Module.HEAPU8.buffer.slice(p, p + 48)))) : null;
  } finally { Module._free(p); }
  return cubeLayout;
}
function readStashLayout() {
  if (stashLayout) return stashLayout;
  const p = Module._malloc(48);
  try {
    const ok = Module.ccall('d2_session_inventory_layout', 'number', ['number', 'number', 'bigint', 'number'], [BIG_BANK_PAGE_1, 1, BigInt(p), 12]);
    stashLayout = ok ? stashGeometry(Array.from(new Int32Array(Module.HEAPU8.buffer.slice(p, p + 48)))) : null;
  } finally { Module._free(p); }
  return stashLayout && plugyStash.enabled && stashPanelRenderer?.plugy.on ? { ...stashLayout, ...PLUGY_GRID } : stashLayout;   // PY3: PlugY's 10x10 BigStash grid (the engine keeps its own 6x8 rows)
}
// Both figures are read from retail's stat list on every use. The dialog owns
// no balance and never predicts a capped transfer; button 19/20 decides that.
function stashGoldStat(which) {
  return session ? Module.ccall('d2_session_player_stat', 'number', ['number', 'number'], [which, 0]) : 0;
}
function openStashGold(mode) {
  const available = stashGoldStat(mode === 'withdraw' ? STASH_GOLD.bank : STASH_GOLD.carried);
  // Retail opens the deposit box on ALL the carried gold and the withdraw box on nothing (both measured,
  // the retail captures and stash-gold-withdraw.txt).
  stashGoldDialog = { open: true, mode, available, amount: mode === 'withdraw' ? 0 : available };
}
function closeStashGold() { stashGoldDialog = null; }
function changeStashGoldAmount(delta) {
  if (!stashGoldDialog) return;
  stashGoldDialog.amount = Math.max(0, Math.min(stashGoldDialog.available, stashGoldDialog.amount + delta));
}
function submitStashGold() {
  // The engine may have closed the stash since the dialog was drawn. Do not
  // turn a stale amount into a retail button command after that event.
  if (!stashOpen || !stashGoldDialog) return 0;
  const button = stashGoldDialog.mode === 'withdraw' ? STASH_GOLD.withdraw : STASH_GOLD.deposit;
  const amount = Math.max(0, Math.min(stashGoldDialog.available, stashGoldDialog.amount | 0));
  const result = Module.ccall('d2_session_command_button', 'number', ['number', 'number'], [button, amount]);
  // Retail owns every consequence (including capacity refusal); reread it and
  // close this client dialog rather than locally adjusting either balance.
  snapshot = session.snapshot(); closeStashGold();
  return result;
}
// The level exit under the pointer: retail hit-tests the warp's LvlWarp selection box in SCREEN space
// (SelectX/Y/DX/DY px, y down, from the warp's anchor = tile*5 + Offset - 2 subtiles; Diablerie Warp.cs).
// The engine exports the box and anchor (D2SessionWarp); the smallest box containing the pointer wins.
// Declared: the anchor's half-subtile convention is Diablerie's; retail measured the box size and edges, but only against a
// hero-derived point, so the anchor (F6: measured O+(-16,+4) px, +-4) is unreconciled with `originX/Y`.
function warpUnderPointer(px, py) {
  if (!session) return null;
  let best = null, bestArea = Infinity;
  for (const warp of snapshot.warps || []) {
    if (!warp.enabled || warp.located === false || !warp.select || warp.destinationLevel === snapshot.level) continue;
    const [ax, ay] = window.__d2Shell.project(warp.originX, warp.originY), b = warp.select;
    // F6 MEASURED (retail, hover-select-f14-f6.json): edges are INCLUSIVE, the box is (DX+1)x(DY+1) px, y negative = up.
    if (px < ax + b.x || px > ax + b.x + b.dx || py < ay + b.y || py > ay + b.y + b.dy) continue;
    if (b.dx * b.dy < bestArea) { best = warp; bestArea = b.dx * b.dy; }
  }
  return best;
}
let lastWarpTaken = null;
// The object under the pointer, the same way npcUnderPointer finds a person.
// The character screen's computed numbers (d2_session_character_screen): defense, attack rating,
// damage, stamina, resistances.
function readCharacterScreen() {
  if (!session || !Module._d2_session_character_screen) return null;
  const p = Module._malloc(64);
  try { if (!Module.ccall('d2_session_character_screen', 'number', ['bigint'], [BigInt(p)])) return null; return Array.from(new Int32Array(Module.HEAPU8.buffer.slice(Number(p), Number(p) + 64))); }
  finally { Module._free(p); }
}
// Retail's run/walk toggle (`d2_session_run_state`): the engine's own flag, which is also what picks
// the move packet (0x03 Run / 0x01 Walk) and therefore the mode and the stamina drain. The client
// reads it; it never keeps a second copy that could disagree with the hero's actual mode.
function readRunToggle() {
  return Boolean(session && Module._d2_session_run_state && Module.ccall('d2_session_run_state', 'number', [], []) === 1);
}
function setRunToggle(on) {
  if (!Module._d2_session_toggle_run) return false;
  return Module.ccall('d2_session_toggle_run', 'number', ['number'], [on ? 1 : 0]) === 1;
}
function objectUnderPointer(px, py) {
  if (!session) return null;
  const camera = worldToScreen(cameraCentre().x, cameraCentre().y);   // the SAME camera render() draws with
  const w = canvas.width, h = canvas.height;
  let best = null, bestDistance = Infinity;
  // Retail's hover skips an object its mode makes unselectable (Objects.txt Selectable<mode>): an
  // opened chest is not clicked again -- s000a re-operated one every 6 frames while picking up its
  // drop, and each operate re-armed its trap (96 trap-firebolt monsters from one chest).
  for (const object of snapshot.objects || []) {
    if (object.selectable === false) continue;
    const point = worldToScreen(object.x, object.y);
    const ox = point.x - camera.x + viewCX(), oy = point.y - camera.y + (h / 2 + UNIT_CENTRE_Y);
    // The hit test is the DRAWN SPRITE's box (F71), not a 36 px circle around the object's origin:
    // the circle made a walk leg aimed at the floor beside a door resolve to the door and operate
    // it, so the walk shut the door its own last leg had opened. The box comes from the renderer's
    // own draw, measured HERE rather than kept from the last frame: step()/stepN() do not render,
    // and a scenario's click arrives with no frame of its own, so a box list built in the render
    // pass would be absent or stale exactly where the click is (the camera moves between frames).
    // Cases the box alone loses, all of them measured (s438): an object whose sprite cannot be
    // drawn has no box at all, and an object's art need not cover its own anchor -- the class-24
    // gate's sprite box ends 2 px ABOVE its origin, and with the origin dropped that door could not
    // be opened at all, while the shared player's own gate walk and every scenario click the
    // object's COORDINATES (project(x, y)). So an object whose art does not reach its own anchor
    // keeps a small radius around it: 6 px, well inside the 16 px of the smallest aim F71 was
    // measured at, and the smallest that holds a sprite which does not reach its own anchor.
    // F98c: the radius is for that art, not for every object. An OPEN door (mode 2) paints retail's
    // own 1x1 frame (F72, s961: `d5trlitonhth.dcc` is one opaque pixel), so its 6 px disk reached
    // 6 px of floor on every side and a click beside an open doorway closed it (s438 flipped 2 -> 5
    // on a repeat aimed at the door's own coordinates). A box that already covers its origin IS the
    // object's whole pick box, and there the radius goes (s380 measures the contract).
    const box = objectRenderer?.measure ? objectRenderer.measure(object, snapshot.act) : null;
    const d = Math.hypot(ox - px, oy - py);
    let inBox = false, coversOrigin = false;
    if (box && box.width > 0 && box.height > 0) {
      const left = Math.round(ox + box.xOffset), top = Math.round(oy + box.yOffset);
      inBox = px >= left && px < left + box.width && py >= top && py < top + box.height;
      coversOrigin = box.xOffset <= 0 && box.xOffset + box.width > 0 && box.yOffset <= 0 && box.yOffset + box.height > 0;
    }
    const hit = inBox || (!coversOrigin && d <= 6);
    if (hit && d < bestDistance) { best = object; bestDistance = d; }
  }
  return best;
}
// Pointer minus the unit's cel anchor, screen px (the box test's input).
function unitBoxHit(entity, px, py) {
  const camera = worldToScreen(cameraGrid().x, cameraGrid().y), point = worldToScreen(entity.x, entity.y);   // F101: the camera the SPRITE pass draws units with -- the F14 box is measured about retail's cel anchor, so it must sit on the anchor we draw at (cameraCentre() put it 8 px above)
  // the cel anchor is an integer pixel (retail's measurement is 1 px exact), so round it the way the sprite is placed
  return inUnitHoverBox(px - Math.round(point.x - camera.x + viewCX()), py - Math.round(point.y - camera.y + (canvas.height / 2 + UNIT_CENTRE_Y)), entity.hostile === true);
}

function npcUnderPointer(px, py) {
  if (!session) return null;
  let best = null, bestDistance = Infinity;
  const at = pointerWorldAt(px, py);
  for (const entity of snapshot.entities) {
    if (!entity.npc || !unitBoxHit(entity, px, py)) continue;
    const distance = (entity.x - at.x) ** 2 + (entity.y - at.y) ** 2;
    if (distance < bestDistance) { best = entity; bestDistance = distance; }
  }
  return best;
}

// Retail lists Cain's Identify Items only while the hero carries something he would identify
//: the engine's own
// ITEMS_GetNoOfUnidItems, the count the server's identify handler tests.
const hasUnidentifiedItem = () => Module.ccall('d2_session_unidentified_count', 'number', [], []) > 0;
// Kashya lists Hire only once A1Q2 (Sisters' Burial Grounds) is done, the Den alone gives Talk / Cancel
//.
const KASHYA = 150, S_HIRE = 3397;
const menuEntriesFor = (npc) => {
  const entries = npcMenuEntries(npc.classId)?.filter((e) => e.action !== 'identify' || hasUnidentifiedItem()) ?? null;
  if (entries && npc.classId === KASHYA && questStatus(readQuestWords(), 1, 2) === 'done') entries.splice(1, 0, { stringId: S_HIRE, action: 'hire' });
  return entries;
};

// The engine's 0x27 message list for the open interaction: [{ stringId, menu }] (D2SessionNpcDialog: u16 ids at +36, menus at +52).
function readDialogMessages() {
  const at = Number(Module.ccall('d2_session_npc_dialog', 'number', [], []));
  if (!at) return [];
  const view = new DataView(Module.HEAPU8.buffer, at, 64), n = Math.min(view.getUint32(32, true), 8);
  // Retail sorts the whole text list by StringId before either speech or Talk reads it (D2MOO
  // D2Common/D2Text.cpp: TEXT_SortTextNodeListByStringId; F463 verifies the first-click order).
  return Array.from({ length: n }, (_, i) => ({ stringId: view.getUint16(36 + 2 * i, true), menu: Module.HEAPU8[at + 52 + i] }))
    .sort((a, b) => a.stringId - b.stringId);
}
// P7b: Talk opens retail's topic box (Introduction / Gossip / one line per quest the NPC has a topic for / Cancel) over the same NPC.
function openTalkBox(npcId, menuState) {
  // Hearing a message changes the NPC's list (the server answers 0x31 with a fresh 0x27): ask the engine for the current one.
  Module.ccall('d2_session_command_npc', 'number', ['number'], [npcId]);
  noteIntro(menuState.classId);
  // Hearing the intro (0x31) ends the engine's interaction, so the re-issue above is refused and the list is empty: the topics the
  // list held when the conversation began (minus the ones heard since) are still the NPC's Talk topics.
  const now = readDialogMessages();
  if (now.length) npcTopics.set(npcId, now); else if (npcTopics.has(npcId)) now.push(...npcTopics.get(npcId));
  const entries = talkEntries(now, (act, n) => Module.ccall('d2_lang_string', 'string', ['string'], [`qstsa${act}q${n}`]));
  npcMenu.show(npcId, menuState.classId, Module.ccall('d2_lang_string_by_index', 'string', ['number'], [3381]), entries);
}

// Dismissing the speech (Esc, or the end of an auto-scrolled topic speech). Returns true when another message took its place.
function dismissSpeech() {
  // Dismissing the speech tells the server it was heard -- retail's 0x31 with the message id --
  // which is where a quest message does its work (Akara's A1Q1 reward is granted on message 76).
  // A `silent` speech (an intro/gossip line the engine's list does not hold) is never acked.
  if (npcState.spoken != null && !npcState.silent) { Module.ccall('d2_session_command_npc_response', 'number', ['number', 'number'], [npcState.npcId, npcState.spoken]); npcTopics.set(npcState.npcId, (npcTopics.get(npcState.npcId) || []).filter((m) => m.stringId !== npcState.spoken)); }
  // More messages queued (Drognan: the Tainted Sun thanks, then A2Q4's): the next one is shown, the interaction stays open.
  if (npcState.spoken != null && !npcState.silent && npcMsgSkip.npcId === npcState.npcId) { npcMsgSkip.n++; const next = readNpcState(); if (next?.spoken != null) { npcState = next; return true; } }
  // A first speech gives way to the NPC's menu, the interaction still open; otherwise Esc ends it.
  if (menuAfterSpeech && menuAfterSpeech.npcId === npcState.npcId && ensureNpcMenu()) {
    const m = menuAfterSpeech; menuAfterSpeech = null;
    npcMenu.show(m.npcId, m.classId, m.name, m.entries); npcState = null;
  } else {
    menuAfterSpeech = null;
    Module.ccall('d2_session_command_end_npc', 'number', ['number'], [npcState.npcId]);
    npcState = readNpcState();
  }
  return false;
}
function talkToNpc(npcId) {
  try {
    if (!Module.ccall('d2_session_command_npc', 'number', ['number'], [npcId])) return false;
    // Retail's client answers the interaction with the NPC's MENU (D2Client's own table,
    // play/assets/npc-menu.mjs); what happens next is the entry the player picks.
    const npc = snapshot.entities.find((u) => u.id === npcId && u.type === 1);
    const entries = npc ? menuEntriesFor(npc) : null;
    if (entries && ensureNpcMenu()) {
      const nameStr = Module.ccall('d2_monster_name_str', 'number', ['number'], [npc.classId]);
      const name = nameStr >= 0 ? Module.ccall('d2_lang_string_by_index', 'string', ['number'], [nameStr]) : '';
      // Retail (retail/frames/s1..s4): an NPC with something to say says it first -- Warriv's intro
      // plays on the click -- and the menu comes up when the speech is dismissed (Esc).
      const said = readNpcState(); { const l = readDialogMessages(); if (l.length) npcTopics.set(npcId, l); }
      if (said?.spoken != null) { noteIntro(npc.classId); npcState = said; menuAfterSpeech = { npcId, classId: npc.classId, name, entries }; return true; }
      npcMenu.show(npcId, npc.classId, name, entries);
      npcState = null;
      return true;
    }
    // An NPC the table does not list: the dialogue, and a shop if they keep one.
    Module.ccall('d2_session_command_npc_vendor', 'number', ['number', 'number'], [npcId, 1]);
    npcState = readNpcState();
    return true;
  } catch { return false; }
}
// P7b/F21: retail's scroll now lives in the box that draws it (speech-box.mjs): every speech
// crawls, so the caller only names the speech's start and reads back `done`.
// The hero-class introduction the engine offers (its plain menu-0 message), remembered: the list empties once it is heard.
const npcIntroHeard = new Map(), npcTopics = new Map();   // npcTopics: npcId -> the dialog list last seen
const noteIntro = (classId) => { const p = readDialogMessages().find((m) => m.menu === 0); if (p) npcIntroHeard.set(classId, p.stringId); };
let npcMenu = null, npcMenuError = '', npcMenuEvidence = null, menuAfterSpeech = null;
function ensureNpcMenu() {
  if (npcMenu || npcMenuError || !window.__d2?.ready) return npcMenu;
  try {
    npcMenu = createNpcMenu({ read: createMpqAssetReader(Module), decodeDc6, decodeDatPalette,
      string: (index) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [index]) });
  } catch (error) { npcMenuError = String(error); }
  return npcMenu;
}
// A menu entry, as D2Client's handlers do it: Talk shows what the NPC says (the dialogue the
// interaction already started), Trade/Gamble ask the server for the shop (retail's 0x38 with the
// counter), Cancel ends the interaction (0x30).
function chooseNpcMenu(entry) {
  const npcId = npcMenu.state.npcId, menuState = { ...npcMenu.state };
  npcMenu.close();
  // What Talk says is dismissed back to this menu, as the first speech is.
  // TODO(client-parity F18): after Talk retail's menu returning is assumed, not yet framed.
  if (entry.action === 'talk') { openTalkBox(npcId, menuState); return; }
  // P7b: the Talk box's own entries. A quest topic speaks its message (0x31 goes out when the speech is dismissed, as for
  // every message); Introduction / Gossip speak the engine's plain message when there is one -- which of the NPC's gossip
  // lines retail picks, and the intro once it was heard, are uncaptured (P7b), so without one the box stays up.
  if (entry.action === 'topic' || entry.action === 'intro' || entry.action === 'gossip') {
    const inList = readDialogMessages(), plain = inList.find((m) => m.menu === 0);
    // Introduction = the NPC's self-introduction (the first-click greeting, replayed identically); Gossip = its one fixed line.
    const said = npcIntroHeard.get(menuState.classId), sp = npcSpeech(menuState.classId);
    const id = entry.action === 'topic' ? entry.stringId : entry.action === 'intro' ? (plain?.stringId || said || sp?.intro) : sp?.gossip;
    if (!id) { npcMenu.show(npcId, menuState.classId, menuState.name, menuState.entries); return; }
    menuAfterSpeech = menuState;
    npcState = { ...(readNpcState() || { npcId, classId: menuState.classId, items: [], gold: 0 }), npcId, spoken: id,
      greeting: Module.ccall('d2_lang_string_by_index', 'string', ['number'], [id]),
      silent: !inList.some((m) => m.stringId === id), own: true };
    if (entry.action !== 'topic') npcState.speechT0 = performance.now();
    return;
  }
  if (entry.action === 'cancel') { Module.ccall('d2_session_command_end_npc', 'number', ['number'], [npcId]); npcState = readNpcState(); return; }
  if (entry.action === 'trade') Module.ccall('d2_session_command_npc_vendor', 'number', ['number', 'number'], [npcId, 1]);
  // Cain: no window (retail capture f18-ui2-cain-menu-identify): 0x34 identifies every item at once and
  // the menu is redrawn without the Identify row.
  else if (entry.action === 'identify') {
    const packet = new Uint8Array([0x34, npcId & 255, (npcId >>> 8) & 255, (npcId >>> 16) & 255, (npcId >>> 24) & 255]);
    const raw = Module._malloc(5), at = Number(raw); Module.HEAPU8.set(packet, at);
    try { Module.ccall('d2_session_client_packet', 'number', ['bigint', 'number'], [BigInt(at), 5]); } finally { Module._free(raw); }
    const npc = snapshot.entities.find((u) => u.id === npcId && u.type === 1);
    npcMenu.show(npcId, menuState.classId, menuState.name, npc ? menuEntriesFor(npc) : menuState.entries.filter((e) => e.action !== 'identify'));
    return;
  }
  else if (entry.action === 'gamble') Module.ccall('d2_session_command_npc_vendor', 'number', ['number', 'number'], [npcId, 2]);
  else if (entry.action === 'hire') { openHireWindow(npcId); return; }
  npcState = readNpcState();
}

// P7: Hire opens retail's 'Hire which Mercenary?' window over the vendor's own list: 0x38 {action 3} ->
// D2GAME_NPC_BuildHirelingList (s85) answers 0x4F then one 0x4E {u16 name, u32 seed} per offer; each row's
// Lvl / Life / Def / Cost is the engine's MONSTERS_HirelingInit for that offer (d2_session_hire_offer_info).
// A click on a row sends 0x36 (s85), Cancel ends the interaction.
const HIRE_DESC = [0, 3371, 3372, 11090, 11088, 11089, 3377, 3376, 3375];
function openHireWindow(npcId) {
  Module.ccall('d2_session_command_npc_action_packet', 'number', ['number', 'number'], [npcId, 3]);
  const len = Module.ccall('d2_session_packet_stream_length', 'number', [], []);
  const base = Number(Module.ccall('d2_session_packet_stream', 'number', [], []));
  const bytes = new Uint8Array(Module.HEAPU8.buffer, base, len), view = new DataView(bytes.buffer, base, len);
  const string = (i) => Module.ccall('d2_lang_string_by_index', 'string', ['number'], [i]);
  const offers = [];
  for (let p = 0; p < len;) {
    const h = bytes[p];
    if (h === 0x4e) {
      const mercName = view.getUint16(p + 1, true), seed = view.getUint32(p + 3, true);
      const at = Number(Module.ccall('d2_session_hire_offer_info', 'number', ['number', 'number'], [mercName, seed]));
      if (at) { const v = new Int32Array(Module.HEAPU8.buffer, at, 13);
        offers.push({ mercName, seed, name: string(mercName), level: v[1], life: v[2], def: v[7], cost: v[5], skill: string(HIRE_DESC[v[12]] || 0) }); }
      p += 7; continue;
    }
    const sz = h === 0x4f ? 1 : h === 0x27 ? 40 : h === 0x28 ? 0x67 : h === 0x29 ? 0x61 : 0;
    if (!sz) break;
    p += sz;
  }
  npcMenu.showHire(npcId, offers, Module.ccall('d2_session_player_gold', 'number', [], []));
}
function chooseHire(hit) {
  const { npcId, offers } = npcMenu.hireState;
  npcMenu.closeHire();
  if (hit?.row !== undefined) {
    const o = offers[hit.row];
    lastHire = { npcId, mercName: o.mercName, result: Module.ccall('d2_session_command_hire_packet', 'number', ['number', 'number'], [npcId, o.mercName]) };
  } else Module.ccall('d2_session_command_end_npc', 'number', ['number'], [npcId]);
  npcState = readNpcState();
}

// WHICH OPEN PANEL OWNS A CANVAS POINT, or null -- the click routing's own rule that an open panel
// takes a click before the world does. Retail's rule, taken from the client prior art: OpenDiablo2's
// `d2game/d2player/game_controls.go` `isInActiveMenusRect()` tests every world click -- left AND right
// -- against the open panel's rectangle before it lets the click through to `OnPlayerMove` /
// `OnPlayerCast`, so a click inside an open panel never moves or casts for the hero. Our shell had the
// rule only for a HELD item's drop; the panels' own blank interiors fell through to the world (`spent
// 'world'`: a walk on the left button, an attack on the right), measured on all four panels by s427.
//
// The rectangles are read from each panel's OWN record -- `panelRect()` on the renderer that draws it,
// the same rectangle its own `contains` hit test is handed -- never re-derived here. The inventory and
// the stash already tested theirs; the character screen and the skill tree had none, which is why the
// two inputs disagreed about the same point.
//
// Rect chosen: the panel ART's own rectangle (the inventory's 400..720 x 60..501, the character
// screen's 80..400 x 61..502). OpenDiablo2 uses the whole half-screen instead (leftMenuRect
// 0,0,400,600 / rightMenuRect 400,0,400,600), and retail's own border draws span the same half
// (panel-chrome.mjs's BORDER_LEFT/BORDER_RIGHT, retail's 800BorderFrame anchors in
// the retail captures) -- but the two panels that already worked (the inventory, the
// stash) already use the tight art rectangle, and narrowing keeps the world strips beside a panel
// walkable. s427 pins both edges of that judgement: it asserts a click 20 px outside every panel is
// still a world click. Widening to the border's full half is a decision for an instrument that
// measures what retail does at x 0..80 with a panel open; it is recorded in .
function openPanelAt(x, y) {
  const sc = Math.min(1, canvas.width / 800, canvas.height / 600);
  const px = (x - (canvas.width - 800 * sc) / 2) / sc, py = (y - (canvas.height - 600 * sc) / 2) / sc;
  const inside = (r) => Boolean(r) && px >= r.left && px < r.left + r.width && py >= r.top && py < r.top + r.height;
  if (characterVisible && inside(characterPanelRenderer?.panelRect?.())) return 'character';
  if (questVisible && inside({ left: 80, top: 61, width: 320, height: 440 })) return 'quest';
  if (skillsVisible && inside(skillPanelRenderer?.panelRect?.())) return 'skills';
  if (inventoryVisible && inside(inventoryPanelRenderer?.panelRect?.())) return 'inventory';
  if (stashOpen && inside(stashPanelRenderer?.panelRect?.(readStashLayout()))) return 'stash';
  if (cubeOpen && inside(cubePanelRenderer?.panelRect?.(readCubeLayout()))) return 'cube';
  return null;
}

function canvasPoint(event) {
  const r = canvas.getBoundingClientRect();
  return [(event.clientX - r.left) * canvas.width / r.width, (event.clientY - r.top) * canvas.height / r.height];
}
addEventListener('keydown', (e) => {
  // Escape skips a boot movie, exactly as a click does (retail aborts the logo films on a key too).
  if (screen === SCREEN.TITLE && bootMovieSpec(titlePhase) && e.code === 'Escape' && !e.repeat) { titleAdvance(); e.preventDefault(); return; }
  if (screen === SCREEN.CREDITS && e.code === 'Escape' && !e.repeat) { exitToTitle(); e.preventDefault(); return; }
  if (screen === SCREEN.CINEMATICS && e.code === 'Escape' && !e.repeat) { cinematicsScreen?.escape(); e.preventDefault(); return; }
  if (screen === SCREEN.CREATE) { if (charCreate?.typeKey(e.code, e.key)) e.preventDefault(); return; }
  // The amount box is modal. It gets Esc/Enter and numeric typing before the
  // global bindings, so Esc does not accidentally close the stash behind it.
  if (screen === SCREEN.GAME && stashGoldDialog?.open) {
    if (e.code === 'Escape') closeStashGold();
    else if (e.code === 'Enter') submitStashGold();
    else if (e.code === 'Backspace') stashGoldDialog.amount = Math.floor(stashGoldDialog.amount / 10);
    else if (/^[0-9]$/.test(e.key)) stashGoldDialog.amount = Math.min(stashGoldDialog.available, stashGoldDialog.amount * 10 + Number(e.key));
    else return;
    e.preventDefault(); return;
  }
  // No gameplay key is answered outside a game: retail's front-end screens take the mouse (and their
  // own fields, above). Without this gate a key pressed on the title or select screen sat in
  // CommandInput's queue and ran the moment a game started, and a direction held there walked the
  // hero -- the handler had no screen gate at all (F3b).
  if (screen !== SCREEN.GAME) return;
  // F218: the Configure Controls panel is modal over the Options page -- every key while it is up is
  // its own, and this sits above the gameplay keys because a key press is what REBINDS a cell. Left
  // below them, pressing I would open the inventory behind the panel and change nothing.
  if (controlsPanel?.open) { controlsPanel.key(e.code); e.preventDefault(); return; }
  // V8c: Enter opens the chat bar; typing fills it; Enter posts `<name>: text` to the message log (single player echoes it); Esc closes.
  if (chat !== null) {
    if (e.code === 'Escape') chat = null;
    else if (e.code === 'Enter') { if (chat) { messageLines.push(`ÿc4${chosenName || 'Player'}ÿc0: ${chat}`); postedAt = performance.now(); } chat = null; }
    else if (e.code === 'Backspace') chat = chat.slice(0, -1);
    else if (e.key.length === 1 && !e.ctrlKey && !e.metaKey) chat += e.key;
    else return;
    e.preventDefault(); return;
  }
  if (e.code === 'Enter' && !e.repeat && !npcState && !gameMenu?.open) { chat = ''; e.preventDefault(); return; }
  actMusic?.play(snapshot.act - 1).catch((error) => { musicError = String(error); }); if (e.code === 'KeyI' && !e.repeat) { inventoryVisible = !inventoryVisible; e.preventDefault(); return; }
  if (e.code === 'KeyC' && !e.repeat) { characterVisible = !characterVisible; if (characterVisible) questVisible = false; e.preventDefault(); return; }
  if (e.code === 'KeyQ' && !e.repeat) { toggleQuest(); e.preventDefault(); return; }
  if (e.code === 'KeyM' && !e.repeat) { toggleMessage(); e.preventDefault(); return; }
  // Retail's ClearScreen (`CfgClearScreen`; OpenDiablo2's key_map.go ties it to KeySpace): hide the
  // HUD, and the same key restores it. The chrome's hit tests are gated with the flag below, so a
  // click where a hidden control used to be reaches the world instead of the invisible control (F3c).
  // F3c, retail's own frames: Space closes an open
  // panel and hides NO chrome -- the orbs, belt and mini panel stay drawn -- and with no panel open it does nothing.
  if (e.code === 'Space' && !e.repeat) { inventoryVisible = characterVisible = skillsVisible = questVisible = messageVisible = false; e.preventDefault(); return; }
  // Retail's right-hand skill selector (`CfgSkillPick`; OpenDiablo2's ToggleRightSkillSelector is
  // KeyS): the SAME primitive as a click on the right socket, so there is one popup and one owner --
  // what it lists and what it selects is the engine's, exactly as the clicked path is.
  if (e.code === 'KeyS' && !e.repeat) {
    if (session && skillSelectRenderer) skillSelectRenderer.open = skillSelectRenderer.open === 'right' ? null : 'right';
    e.preventDefault(); return;
  }
  // Retail's Help screen (`H`, CfgHelp; OpenDiablo2's key_map.go ties ToggleHelpScreen to KeyH):
  // the key opens the screen and the same key closes it. While it is open the world does not get the
  // key, which is why this sits above the dropped-key list like Space and S.
  if (e.code === 'KeyH' && !e.repeat) { helpVisible = !helpVisible; markHelpMenuSeen(); e.preventDefault(); return; }
  // Retail's Swap Weapons (`W`), which retail's own Configure Controls screen binds. The client does not swap anything itself:
  // it asks the engine, whose recovered `sub_6FC46840` moves the equipment between the two weapon sets
  // and flips the active one -- so what the paperdoll and the skills show afterwards is the engine's
  // answer, in one place (F3).
  if (e.code === 'KeyW' && !e.repeat) { session?.swapWeapons?.(); e.preventDefault(); return; }
  // Esc, as retail: it closes what is open before it does anything else. An open conversation is
  // ended through retail's own 0x30 TerminateEntityChat handler, and the window reflects what the
  // ENGINE then reports rather than being hidden locally; then the open panels. With nothing open
  // it falls through to the command path (which cancels the selected skill, s46).
  if (screen === SCREEN.GAME && gameMenu?.open && ['ArrowUp', 'ArrowDown', 'Enter'].includes(e.code)) { gameMenu.key(e.code); e.preventDefault(); return; }
  // Retail's keys are the only ones a game answers (assets/retail-key-bindings.mjs has the list and
  // the retail evidence): this is below the Esc menu, so the menu keeps its arrows, and it is the one
  // place the walk (W/A/S/D, the arrows) and the attack (Space) this shell used to give them are gone.
  if (DROPPED_GAMEPLAY_KEYS.has(e.code)) { e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && npcMenu?.hireOpen) { chooseHire({ cancel: true }); e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && npcMenu?.open) { chooseNpcMenu({ action: 'cancel' }); e.preventDefault(); return; }
  // Dead, Esc continues (retail's youdiedinst: PRESS ESC TO CONTINUE): the player returns to town.
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && session && snapshot?.mode === 17) {
    if (session.respawn()) { snapshot = session.snapshot(); motion = session.motion(); }
    e.preventDefault(); return;
  }
  // The help screen is what Esc closes first while it is up (OpenDiablo2's onEscKey: an open overlay
  // is handled before the escape menu), and it does not open the menu behind it.
  // F157: an object's speech is dismissed before anything else Esc does (it is drawn over the world, like an NPC's).
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && objectSpeech && !npcState) { dismissObjectSpeech(); e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && helpVisible) { helpVisible = false; e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && cubeOpen) {
    // TRADEBTN_CLOSECUBE (23): the window closes when the engine accepts it, as the stash's does.
    if (Module.ccall('d2_session_command_button', 'number', ['number', 'number'], [23, 0])) cubeOpen = false;
    e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && gameMenu?.open) { gameMenu.back(); e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && (messageVisible || questVisible) && !(waypointMenu || stashOpen || npcState || inventoryVisible || characterVisible || skillsVisible)) { messageVisible = false; questVisible = false; e.preventDefault(); return; }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && !(waypointMenu || stashOpen || npcState || inventoryVisible || characterVisible || skillsVisible)) {
    ensureGameMenu()?.toggle(); e.preventDefault(); return;
  }
  if (e.code === 'Escape' && !e.repeat && screen === SCREEN.GAME && (waypointMenu || stashOpen || npcState || inventoryVisible || characterVisible || skillsVisible)) {
    if (waypointMenu) {
      // Retail closes the waypoint menu with 0x49 and level 0.
      takeWaypoint(0);
    } else if (stashOpen) {
      // Retail's client closes the stash and tells the server with 0x4F TRADEBTN_CLOSESTASH (18);
      // the window closes when the engine accepts it.
      if (Module.ccall('d2_session_command_button', 'number', ['number', 'number'], [18, 0])) { stashOpen = false; inventoryVisible = false; closeStashGold(); }
    } else if (npcState) {
      if (dismissSpeech()) { e.preventDefault(); return; }
    } else { inventoryVisible = false; characterVisible = false; skillsVisible = false; }
    e.preventDefault(); return;
  }
  if (e.code === 'KeyT' && !e.repeat) { skillsVisible = !skillsVisible; e.preventDefault(); return; }
  if (input.key(e.code, true, e.repeat)) e.preventDefault(); });
addEventListener('keyup', (e) => { if (input.key(e.code, false)) e.preventDefault(); });
addEventListener('blur', () => { input.keys.clear(); worldPointerHeld = false; heldPointerTicks = 0; });
canvas.addEventListener('wheel', (e) => { if (npcMenu?.hireOpen) { npcMenu.hireScroll(Math.sign(e.deltaY)); e.preventDefault(); } }, { passive: false });
canvas.addEventListener('pointermove', (e) => {
  const [x, y] = canvasPoint(e);
  pointer.x = x; pointer.y = y;
  if (worldPointerHeld && (e.buttons & 1) && screen === SCREEN.GAME && session && !gameMenu?.open) {
    input.pointer(x, y, 0, true); heldPointerTicks = 0;
  }
  if (screen === SCREEN.GAME && gameMenu?.open) { gameMenu.hover(x, y, canvas.width, canvas.height); return; }
  if (session && skillSelectRenderer && screen === SCREEN.GAME) skillSelectRenderer.hover(x, y, canvas.width, canvas.height, readSkillChoices());
  if (skillsVisible && skillPanelRenderer && session)
    skillPanelRenderer.hover(x, y, canvas.width, canvas.height, { skills: readClassSkills() || [] });
  if (screen === SCREEN.GAME && npcMenu?.open) npcMenu.hover(x, y);
  if (screen === SCREEN.GAME && npcMenu?.hireOpen) npcMenu.hireHover(x, y);
  // Retail draws no pointer cel on the trademark screen (its draw list has none), so a move there
  // only ends the phase's clock through the click path below.
  if (screen === SCREEN.TITLE && titlePhase === 'menu') titleScreen?.hover(x, y, canvas.width, canvas.height);
  else if (screen === SCREEN.CINEMATICS) cinematicsScreen?.hover(x, y, canvas.width, canvas.height);
  else if (screen === SCREEN.SELECT) charSelect?.hover(x, y, canvas.width, canvas.height);
  else if (screen === SCREEN.CREATE) charCreate?.hover(x, y, canvas.width, canvas.height);
});
canvas.addEventListener('pointerdown', (e) => {
  const [x, y] = canvasPoint(e);
  const click = noteClick(x, y, e.button);
  syncShopInventory();
  const modalOwner = modalPointerOwner({ gameMenuOpen: screen === SCREEN.GAME && gameMenu?.open, stashGoldDialogOpen: screen === SCREEN.GAME && stashGoldDialog?.open });
  // F218: the Configure controls panel sits over the Options page and owns the pointer while it is
  // up -- checked before the game menu because the menu is still "open" underneath it.
  if (screen === SCREEN.GAME && controlsPanel?.open) { click.spent = 'configureControls'; const [mx, my] = canvasPoint(e); controlsPanel.click(mx, my, canvas.width, canvas.height); canvas.focus(); e.preventDefault(); return; }
  if (modalOwner === MODAL_POINTER_OWNER.GAME_MENU) { click.spent = 'gameMenu'; const [mx, my] = canvasPoint(e); gameMenu.click(mx, my, canvas.width, canvas.height); canvas.focus(); e.preventDefault(); return; }
  if (screen === SCREEN.TITLE || screen === SCREEN.CREDITS || screen === SCREEN.CINEMATICS || screen === SCREEN.CREATE || screen === SCREEN.SELECT) {
    click.spent = `screen-${screen}`;
    const [tx, ty] = [x, y];
    // A front-end control is ARMED here and fires on the matching release (see the pointerup
    // listener): retail activates a button on mouse-up, and only when the pointer is still over
    // the control it went down on. The press is also what puts the button in its down cel -- hover
    // alone never does. TITLE keeps titleAdvance(): the trademark phase owns its own countdown, so
    // a press during it arms the phase rather than reaching the menu behind it (s198).
    frontendPointerStates.press(e.pointerId, frontendPointerControl(), tx, ty, canvas.width, canvas.height);
    canvas.focus(); e.preventDefault(); return;
  }
  actMusic?.play(snapshot.act - 1).catch((error) => { musicError = String(error); });
  // The amount box is modal, including outside its visible controls. Consume
  // its click before a skill socket or other HUD control can react behind it.
  if (modalOwner === MODAL_POINTER_OWNER.STASH_GOLD_DIALOG && stashGoldRenderer && session) {
    click.spent = 'stashGoldDialog';
    const hit = stashGoldRenderer.hitAt(x, y, canvas.width, canvas.height, readStashLayout(), true);
    if (hit?.action === 'submit') submitStashGold();
    else if (hit?.action === 'close') closeStashGold();
    else if (hit?.action === 'increase') changeStashGoldAmount(1);
    else if (hit?.action === 'decrease') changeStashGoldAmount(-1);
    canvas.focus(); e.preventDefault(); return;
  }
  // Retail's Help screen owns the mouse while it is up: OpenDiablo2's game_controls.go consumes a
  // press inside the overlay's rect before the world sees it, so the player cannot walk the hero
  // through the panel they are reading. The one control on it that answers is its Close box.
  if (screen === SCREEN.GAME && helpVisible && helpRenderer) {
    const closed = helpRenderer.hitClose(x, y, canvas.width, canvas.height);
    if (closed) helpVisible = false;
    click.spent = closed ? 'help-close' : 'help';
    canvas.focus(); e.preventDefault(); return;
  }
  // The HUD's Help button (drawn only while its label is, hudHelpEvidence): a left press on it opens the Help screen, as `H` does (F3d).
  const hb = hudHelpEvidence?.hit;
  if (e.button === 0 && screen === SCREEN.GAME && hb && x >= hb.x && x < hb.x + hb.w && y >= hb.y && y < hb.y + hb.h) {
    helpVisible = true; markHelpMenuSeen(); click.spent = 'help-button'; canvas.focus(); e.preventDefault(); return;
  }
  if (e.button === 0 && screen === SCREEN.GAME && messageVisible && messageLogRenderer && !(controlPanelRenderer?.hitMiniButton(controlPanelEvidence, x, y, canvas.width, canvas.height) >= 0)) {
    if (messageLogRenderer.hitClose(x, y, canvas.width, canvas.height)) messageVisible = false;
    else { const dir = messageLogRenderer.hitScroll(x, y, canvas.width, canvas.height);   // V8d: one arrow click scrolls ONE line
      if (dir) messageScroll = Math.max(0, Math.min(messageScrollMax(messageLines.length), messageScroll + (dir === 'down' ? 1 : -1))); }
    click.spent = 'message-log'; canvas.focus(); e.preventDefault(); return;
  }
  if (e.button === 0 && screen === SCREEN.GAME && questVisible && questLogRenderer && questEvidence) {
    const words = readQuestWords(), hit = questLogRenderer.hit(x, y, canvas.width, canvas.height, questEvidence.tabs, questEvidence.act);
    if (hit?.close) questVisible = false;
    else if (hit?.descr) questDescrOn = !questDescrOn;   // the questlast description toggle (V8d)
    else if (hit?.tab) { questAct = hit.tab; questSelected = questFirst(words, questAct); }
    else if (hit?.quest && questStatus(words, questAct, hit.quest) !== 'locked') questSelected = hit.quest;
    if (hit || openPanelAt(x, y) === 'quest') { click.spent = 'quest-log'; canvas.focus(); e.preventDefault(); return; }
  }
  const panelClose = e.button === 0 && panelCloseAt(x, y, canvas.width, canvas.height, { inventory: inventoryVisible, skills: skillsVisible && { playerClass: snapshot.playerClass, tab: skillPanelRenderer?.tab } });
  if (panelClose) { if (panelClose.panel === 'inventory') { if (stashOpen) closeStashFromInventory({ stashOpen, commandButton: (button, amount) => Module.ccall('d2_session_command_button', 'number', ['number', 'number'], [button, amount]), onAccepted: () => { stashOpen = false; inventoryVisible = false; closeStashGold(); } }); else inventoryVisible = false; } else skillsVisible = false; canvas.focus(); e.preventDefault(); return; }
  // The mini-panel's open/close arrow collapses and expands the icon row. It is a HUD control the
  // renderer actually placed, so only a click inside it consumes (hud-hit-test's rule).
  // ClearScreen hides the chrome, so the chrome answers nothing: each of these four hit tests is a
  // control that is not on the screen, and an invisible control that acts is worse than a dead one --
  // its click belongs to the world (F3c). `controlPanelEvidence` is null while hidden and these
  // hit tests already fail closed on a null box, but the gate is stated once and explicitly.
  if (e.button === 0 && !clearScreen && controlPanelRenderer?.hitMenuButton(controlPanelEvidence, x, y, canvas.width, canvas.height)) {
    hudIconRow = !hudIconRow; render(); canvas.focus(); e.preventDefault(); return;
  }
  // The mini-panel icons: press draws the pressed cel (frame+1), release over the same icon acts.
  if (e.button === 0 && !clearScreen && (miniDown = controlPanelRenderer?.hitMiniButton(controlPanelEvidence, x, y, canvas.width, canvas.height) ?? -1) >= 0) {
    render(); canvas.focus(); e.preventDefault(); return;
  }
  // F10: the lit add-point buttons either side of the belt open the panel that spends the points
  // (left: character, right: skill tree), exactly as the mini-panel's icons / C and T do.
  { const lv = !clearScreen && e.button === 0 ? (controlPanelRenderer?.hitLevelButton(controlPanelEvidence, x, y, canvas.width, canvas.height) ?? -1) : -1;
    if (lv === 0) { characterVisible = !characterVisible; if (characterVisible) questVisible = false; render(); canvas.focus(); e.preventDefault(); return; }
    if (lv === 1) { skillsVisible = !skillsVisible; render(); canvas.focus(); e.preventDefault(); return; } }
  // Retail's run/walk toggle: the click flips the ENGINE's flag (d2_session_toggle_run) -- the same
  // one that chooses the move packet and the hero's mode -- and the button's cel follows the engine
  // on the next frame, so the drawn state cannot disagree with the state the hero moves in.
  if (e.button === 0 && !clearScreen && controlPanelRenderer?.hitRunButton(controlPanelEvidence, x, y, canvas.width, canvas.height)) {
    lastRunToggle = { at: [x, y], from: readRunToggle(), to: setRunToggle(!readRunToggle()) };
    render(); canvas.focus(); e.preventDefault(); return;
  }
  // Only a socket or an icon the renderer drew consumes the click; world input falls through.
  if (e.button === 0 && !clearScreen && dispatchSkillSelectHit(skillSelectRenderer, session, x, y, canvas.width, canvas.height, readSkillChoices(), selectSkill)) { canvas.focus(); e.preventDefault(); return; }
  // F162: the two gold coins open the two retail dialogs their own captures show. The stash panel's
  // coin (155,100) opens WITHDRAW; the inventory panel's coin beside the carried gold (484,469) opens
  // DEPOSIT. Each submit sends retail 0x4F with the dialog's own button (19 withdraw, 20 deposit).
  if (stashOpen && stashGoldRenderer && session) {
    const hit = stashGoldRenderer.hitAt(x, y, canvas.width, canvas.height, readStashLayout(), Boolean(stashGoldDialog));
    if (hit) {
      click.spent = 'stashGold';
      if (hit.action === 'withdraw') openStashGold('withdraw');
      else if (hit.action === 'deposit') openStashGold('deposit');
      else if (hit.action === 'submit') submitStashGold();
      else if (hit.action === 'close') closeStashGold();
      else if (hit.action === 'increase') changeStashGoldAmount(1);
      else if (hit.action === 'decrease') changeStashGoldAmount(-1);
      canvas.focus(); e.preventDefault(); return;
    }
  }
  // Retail's belt: a plain click drinks (0x63's own read, useBelt); shift+click on a box with an
  // empty cursor lifts that item onto the cursor instead (0x24, beltToCursor above) -- the reverse
  // direction of the inventory's own shift+click-to-belt a few branches below, and the SAME latch
  // (M9's shift-armed tap) a finger already has no other gesture to make it with.
  // Retail's own handler (D2GAME_PACKETCALLBACK_Rcv0x24_ItemFromBelt, PlrMsg.cpp:2427) requires a
  // FREE cursor to lift; it never turns an occupied cursor into a drop. So a shift-tap on a belt box
  // ALWAYS spends the click here -- lifting when the cursor is empty, doing nothing when it is not --
  // rather than falling through past both belt branches into the world's own cursorDrop a few hundred
  // lines below (Codex review of platform-touch, 2026-09-28: with an item already held, a shift-tap on
  // the belt fell through and dropped it; s495 proves this fails without the `e.shiftKey` guard here).
  const beltItem = clearScreen ? null : beltRenderer?.itemAt(x - hudOrigin(canvas.width, canvas.height, 'bottom').x, y - hudOrigin(canvas.width, canvas.height, 'bottom').y, snapshot.items, readBeltHudBoxes());
  if (beltItem && e.shiftKey) {
    if (!Module.ccall('d2_session_cursor_item', 'number', [], [])) {
      lastBeltToCursor = { itemId: beltItem.id, result: beltToCursor(beltItem.id) };
      snapshot = session.snapshot();
    }
    click.spent = 'belt'; canvas.focus(); e.preventDefault(); return;
  }
  if (beltItem && !e.shiftKey) {
    const result = session?.useBelt(beltItem.gridX);
    if (result) snapshot = session.snapshot();
    window.__d2Step19Trace?.record('belt-use', { source: 'pointer-click', slot: beltItem.gridX, result: Boolean(result) },
      { step: window.__s000?.state?.step, mode: window.__s000?.mode, snapshot });
    if (result) { click.spent = 'belt'; canvas.focus(); e.preventDefault(); return; }
  }
  // An open waypoint menu takes the click: tabs change only the shown engine rows, close and active
  // rows go through retail's 0x49.  The isolated input module shares the draw layout's coordinates.
  if (waypointMenu && waypointPanelRenderer) {
    const rows = readWaypointTable().rows(waypointMenu.act);
    const hit = e.button === 0 && waypointMenuInput.hitTest(x, y, canvas.width, canvas.height, rows,
      (px, py, width, height, menuRows) => waypointPanelRenderer.rowAt(px, py, width, height, menuRows));
    click.spent = 'waypointMenu';
    if (hit?.action === 'tab') waypointMenu.act = hit.act;
    else if (hit?.action === 'close') takeWaypoint(0);
    else if (hit?.action === 'row' && hit.row.active) takeWaypoint(hit.row.levelId);
    canvas.focus(); e.preventDefault(); return;
  }
  if (npcMenu?.hireOpen && e.button === 0) {
    click.spent = 'hireWindow';
    const hit = npcMenu.hireAt(x, y);
    if (hit) chooseHire(hit);
    canvas.focus(); e.preventDefault(); return;
  }
  // An open NPC menu takes the click: an entry, or (anywhere else) Cancel, as retail's does.
  if (npcMenu?.open && e.button === 0) {
    click.spent = 'npcMenu';
    const entry = npcMenu.entryAt(x, y);
    chooseNpcMenu(entry || { action: 'cancel' });
    canvas.focus(); e.preventDefault(); return;
  }
  // An open shop: a tab switches the stock shown, an item in the grid is bought, and an item in the
  // player's own inventory (retail's inventory panel beside the shop) is sold.
  if (shopOpen() && vendorPanel && e.button === 0) {
    const t = vendorPanel.tabAt(x, y, canvas.width, canvas.height);
    if (t !== null) { click.spent = 'vendorTab'; vendorPanel.setTab(t); canvas.focus(); e.preventDefault(); return; }
    const button = vendorPanel.buttonAt(x, y, canvas.width, canvas.height, npcState.classId);
    const mine = inventoryVisible && inventoryPanelRenderer ? inventoryPanelRenderer.itemAt(x, y, canvas.width, canvas.height, snapshot) : null;
    if (button === 'repairAll') { click.spent = 'vendorRepairAll'; repairAllAtVendor(); canvas.focus(); e.preventDefault(); return; }
    if (button === 'exit') { click.spent = 'vendorExit'; Module.ccall('d2_session_command_end_npc', 'number', ['number'], [npcState.npcId]); npcState = readNpcState(); canvas.focus(); e.preventDefault(); return; }
    if (handleVendorRepairClick(button, mine?.id, repairAtVendor)) { click.spent = 'vendorRepair'; canvas.focus(); e.preventDefault(); return; }
    if (button) { click.spent = 'vendorButton'; canvas.focus(); e.preventDefault(); return; }   // Buy / Sell are not modes; a click buys or sells directly.
    // Which command a click on an offer is depends on the counter the ENGINE has open, not on the
    // window: action 2 is a gambler's inventory and its offers are spent, action 1 is trade stock
    // and they are bought. Sending the buy for a gamble offer asks the NPC for an item it never
    // listed, so the click has to name the command the counter means.
    const item = vendorPanel.itemAt(x, y, canvas.width, canvas.height, npcState);
    if (item) { const gamble = npcState.action === 2; click.spent = gamble ? 'vendorGamble' : 'vendorBuy';
      (gamble ? gambleAtVendor : buyFromVendor)(item.id); canvas.focus(); e.preventDefault(); return; }
    if (mine) { click.spent = 'vendorSell'; sellToVendor(mine.id); canvas.focus(); e.preventDefault(); return; }
  }
  // The item cursor (retail's inventory handling, the engine's packet handlers deciding): over the
  // open inventory, a click lifts a carried item (0x19) or a worn one (0x1C) onto the cursor, and
  // with an item held puts it in the grid at the cell under it (0x18) or on a body slot (0x1A, or
  // 0x1D to swap with what is worn there); shift+click sends a carried potion to the belt (0x63).
  // Anywhere else in the world, a held item is dropped (0x17). The panel swallows its own clicks.
  if (session && e.button === 0 && !shopOpen() && inventoryPanelRenderer && Module._d2_session_cursor_item) {
    pointer.x = x; pointer.y = y;
    const cursorId = Module.ccall('d2_session_cursor_item', 'number', [], []);
    // The stash's grid (Inventory.txt BIG_BANK_PAGE_1, invPage 4) is the same inventory the item
    // cursor works on -- D2Inventory keeps the basket as a page, so 0x19 lifts out of it and 0x18
    // puts back into it with the page in the packet. The panel swallows its clicks: a click on it is
    // never a walk, and one off the grid leaves the held item on the cursor.
    const cubeGeometry = cubeOpen && inventoryVisible ? readCubeLayout() : null;
    if (cubeGeometry && cubePanelRenderer && cubePanelRenderer.contains(x, y, canvas.width, canvas.height, cubeGeometry)) {
      if (cubePanelRenderer.transmuteAt(x, y, canvas.width, canvas.height, cubeGeometry)) {
        // The engine's own cube recipe pass (TRADEBTN_TRANSMUTE); the result lands on page 3.
        lastItemCommand = { name: 'transmute', args: [], result: Module.ccall('d2_session_command_button', 'number', ['number', 'number'], [24, 0]), cursor: cursorId };
      } else if (!cursorId) { const item = cubePanelRenderer.itemAt(x, y, canvas.width, canvas.height, cubeGeometry, snapshot.items); if (item) itemCommand('lift', item.id); }
      else {
        const held = snapshot.items.find((i) => i.id === cursorId) || { id: cursorId };
        const cell = cubePanelRenderer.cellAt(x, y, canvas.width, canvas.height, cubeGeometry, held);
        if (cell) itemCommand('put', cursorId, cell.x, cell.y, INVPAGE_CUBE);
      }
      click.spent = 'cubePanel';
      snapshot = session.snapshot(); canvas.focus(); e.preventDefault(); return;
    }
    const geometry = stashOpen ? readStashLayout() : null;
    if (geometry && stashPanelRenderer && stashPanelRenderer.contains(x, y, canvas.width, canvas.height, geometry)) {
      if (plugyStash.click(x, y, canvas.width, canvas.height, geometry, stashOpen)) { click.spent = 'stashPanel'; canvas.focus(); e.preventDefault(); return; }
      if (!cursorId) { const item = stashPanelRenderer.itemAt(x, y, canvas.width, canvas.height, geometry, snapshot.items); if (item) itemCommand('lift', item.id); }
      else {
        const held = snapshot.items.find((i) => i.id === cursorId) || { id: cursorId };
        const cell = stashPanelRenderer.cellAt(x, y, canvas.width, canvas.height, geometry, held);
        if (cell) itemCommand('put', cursorId, cell.x, cell.y, INVPAGE_STASH);
      }
      click.spent = 'stashPanel';
      snapshot = session.snapshot(); canvas.focus(); e.preventDefault(); return;
    }
    if (inventoryVisible && inventoryPanelRenderer.contains(x, y, canvas.width, canvas.height)) {
      const W = canvas.width, H = canvas.height, slot = inventoryPanelRenderer.slotAt(x, y, W, H, snapshot);
      if (!cursorId) {
        const item = inventoryPanelRenderer.itemAt(x, y, W, H, snapshot);
        if (item && e.shiftKey && !npcState) lastGridToBelt = { itemId: item.id, result: gridToBelt(item.id) };
        else if (item) itemCommand('lift', item.id);
        else if (slot?.item) itemCommand('unequip', slot.bodyLoc);
      } else {
        const held = snapshot.items.find((i) => i.id === cursorId) || { id: cursorId };
        const cell = inventoryPanelRenderer.cellAt(x, y, W, H, held);
        if (slot) itemCommand(slot.item ? 'swap' : 'equip', cursorId, slot.bodyLoc);
        else if (cell) itemCommand('put', cursorId, cell.x, cell.y);
      }
      click.spent = 'inventoryPanel';
      snapshot = session.snapshot(); canvas.focus(); e.preventDefault(); return;
    }
    // Retail drops a held item only on a click in the world: an open panel takes its own clicks first
    // and the item stays on the cursor. WHICH point is the panel's is `openPanelAt`'s answer and no
    // other's -- the same rectangle the click routing tests before the world's own entities (M18), and
    // the same one each panel's own `contains` test is handed. This path used to carry a SECOND
    // rectangle of its own (a hand-rolled `py < 432` crossed with `px < 320` / `px >= 480`), and the
    // two disagreed at every panel edge: over bands 80 game px wide and 61-70 game px tall it either
    // dropped an item on a point the panel owned or kept it on the cursor over a point in the world.
    // s435 measures each band on all four panels and pins both answers; the surviving control is that
    // a point outside every panel is still a world drop.
    //
    // What that second rectangle also carried, and what is now a measured, declared consequence: its
    // `npcState` term called the whole left 320x432 a panel whenever a conversation was up, including
    // the speech box (237..562 x 7..129 -- in no panel's rectangle, and no region of OpenDiablo2's
    // `isInActiveMenusRect`). With an item held, a click in the speech box is now a world drop like any
    // other non-panel point, and because the drop spends the click the shell's own "a click while
    // talking ends it" no longer runs on that point. Retail's answer there is not determinable without
    // a capture of a held item over the box; the single rectangle is the smaller change and the
    // direction the prior art gives, and s435 asserts the outcome so that instrument can move it.
    if (cursorId && !openPanelAt(x, y)) {
      // Retail's inventory client keeps quest items on the cursor when a player clicks the world.
      // The packet handler's drop path has no guard; the observed refusal is driven by quest handling
      // (F241 / F328). Use the engine's Items.txt verdict here, not a client-maintained item-code list.
      // The held item follows the pointer on subsequent world clicks, just as retail's does.
      const questItem = Module._d2_session_item_is_quest
        ? Module.ccall('d2_session_item_is_quest', 'number', ['number'], [cursorId]) : -1;
      if (questItem === 1) {
        click.spent = 'cursorDropRefusedQuestItem';
        canvas.focus(); e.preventDefault(); return;
      }
      click.spent = 'cursorDrop'; itemCommand('drop', cursorId); canvas.focus(); e.preventDefault(); return;
    }
  }
  // An open NPC window takes the click before the world does, so buying does not also walk.
  if (npcState && npcPanel && !shopOpen()) {
    const item = npcPanel.itemAt(x, y, canvas.width, canvas.height,
      { ...npcState, playerItems: snapshot.items });
    // The trade screen has two grids: the shop's stock above, the player's items below. A click on
    // your own item is a SALE, not a purchase -- the panel says which side was hit.
    if (item?.mine) { click.spent = 'vendorSell'; sellToVendor(item.id); canvas.focus(); e.preventDefault(); return; }
    if (item) { click.spent = 'vendorBuy'; buyFromVendor(item.id); canvas.focus(); e.preventDefault(); return; }
  }
  // A click out in the world while a conversation is open closes it, as retail's client does (0x30
  // through retail's handler) -- and nothing else: the click is spent on closing. Left open, the
  // engine keeps the player "interacting", and OBJECTS_OperateHandler then refuses every object
  // (the Cold Plains waypoint could not be touched after a stray click in town started a chat).
  if (npcState && e.button === 0) {
    click.spent = 'npcClose';
    menuAfterSpeech = null;
    Module.ccall('d2_session_command_end_npc', 'number', ['number'], [npcState.npcId]);
    npcState = readNpcState();
    canvas.focus(); e.preventDefault(); return;
  }
  // Left-clicking a talkable NPC walks over and starts the conversation, which is what a player
  // expects; anything else falls through to the normal command path.
  // A hostile monster under the pointer is what a left click acts on, ahead of the tile or object
  // behind it: a Fallen standing on the Den of Evil's exit was clicked 400 times and every click
  // walked to the exit instead (s000a). Retail picks a UNIT under the cursor before a level-exit
  // tile. TODO(client-parity F14): the exact hover order between overlapping units (D2Client's
  // hover selection) is not recovered; monsters-first is the observed behaviour, not the code.
  // Through pointerWorldAt, like hover and the skill paths: this used to round the world cell on its
  // own, without pointerWorldAt's half-subtile, so at a cell boundary the click tested a different cell
  // than hover had highlighted -- hostileHit could be null while a monster was plainly under the
  // cursor, and an object, NPC or warp behind it took the click instead (codex-f7's review finding).
  // AN OPEN PANEL IS TESTED BEFORE THE WORLD'S OWN ENTITIES, not only before the world path's tail.
  // `openPanelAt` is the routing's rule that an open panel owns every point inside its own rectangle;
  // s427 put it where the click would otherwise become the world's, which left every entity the shell
  // resolves AHEAD of that point -- an NPC, an object, a level exit, the player's own corpse -- still
  // taking the click (measured: a left click on an open character screen that lands on a town NPC
  // spends 'npc', walks the hero over and starts the conversation behind the panel). The direction
  // rule is retail's, taken from the client prior art: OpenDiablo2's `d2game/d2player/game_controls.go`
  // `isInActiveMenusRect()` is tested by BOTH buttons before a click reaches `OnPlayerMove` /
  // `OnPlayerCast`, and in that client EVERY world interaction -- an NPC, an object, an item, a level
  // exit -- IS one of those two packets, the world's entities being the server's to resolve. So the
  // rule applies to every entity class and not to some: a left click inside an open panel resolves no
  // entity at all, and the ground item and the hostile (which the world path resolves for itself,
  // through `resolvePointerTarget`) are behind the same test by construction.
  //
  // What this must NOT take is a control the panel DRAWS: the close box, a tab, a "+", a grid cell --
  // all of those are tested above, or inside the `else` below before `panelOwner` is read, and they
  // still win. Only the world's own entity resolution is skipped.
  const panelOwner = openPanelAt(x, y);
  const worldAt = e.button === 0 && session && !panelOwner ? pointerWorldAt(x, y) : null;
  const hostileHit = worldAt ? selectPrimaryTarget(snapshot, worldAt, Infinity, (entity) => unitBoxHit(entity, x, y)) : null;
  // What the world dispatch itself saw under the pointer, whichever handler ends up spending the
  // click: the aim point it resolved, and the hostile unit standing on it (the same test the four
  // handlers below and the world path key off). Recorded here so "which handler spent it" and "what
  // was it aimed at" are answered from one reading.
  click.aim = worldAt ? { world: [worldAt.x, worldAt.y], hostile: hostileHit || 0 } : null;
  // The player's own corpse: clicking it walks over and takes the gear back (retail's interact with
  // the body, the engine's recovered corpse handler).
  const corpseHit = !panelOwner && !hostileHit && e.button === 0 && session ? (() => {
    const cam = worldToScreen(cameraGrid().x, cameraGrid().y); let best = null, bestD = 36;
    for (const u of snapshot.entities) if (u.kind === 'player' && u.id !== snapshot.playerId && u.mode === 17) {
      const pt = worldToScreen(u.x, u.y), d = Math.hypot(pt.x - cam.x + viewCX() - x, pt.y - cam.y + (canvas.height / 2 + UNIT_CENTRE_Y) - y);
      if (d < bestD) { best = u; bestD = d; } }
    return best; })() : null;
  if (corpseHit) {
    click.spent = 'corpse'; click.walkTo = [corpseHit.x, corpseHit.y];
    const recover = (stuck) => {
      lastCorpseRecovery = { id: corpseHit.id, result: Module.ccall('d2_session_command_recover_corpse', 'number', ['number'], [corpseHit.id]), stuck };
      snapshot = session.snapshot(); };
    // A walk that stalls short of the body (a monster or the body's own footprint in the way) still
    // asks: the engine's recovery decides, as it does for objects and exits.
    // Already beside it (the body's own cell and the monsters' blocking can keep a walk from ever
    // arriving or stalling -- s000a stood 3 subtiles from its second body, clicking): take it now.
    const p0 = snapshot.player;
    if (Math.max(Math.abs(corpseHit.x - p0.x), Math.abs(corpseHit.y - p0.y)) <= 4) { walkEvidence = { active: true, hops: 0, reason: 0 }; recover(false); canvas.focus(); e.preventDefault(); return; }
    armWalk({ x: corpseHit.x, y: corpseHit.y, within: 2, onArrive: () => recover(false), onStuck: () => recover(true) });
    canvas.focus(); e.preventDefault(); return;
  }
  const npcHit = !panelOwner && !hostileHit && e.button === 0 && session ? npcUnderPointer(x, y) : null;
  // Left-clicking an OBJECT walks to it and operates it, as retail does. What happens next is the
  // engine's: the stash, for one, answers with 0x77 UPDATEUI_OPENSTASH (pollUiEvents).
  const objectHit = !panelOwner && !hostileHit && !npcHit && e.button === 0 && session && !stashOpen ? objectUnderPointer(x, y) : null;
  // Left-clicking a level exit walks to it and takes it, through the engine's warp command (which
  // only accepts a real generated exit of this level).
  const warpHit = !panelOwner && !hostileHit && !npcHit && !objectHit && e.button === 0 && session ? warpUnderPointer(x, y) : null;
  if (warpHit) {
    click.spent = 'warp'; click.walkTo = [warpHit.x, warpHit.y];
    const takeWarp = (stuck) => {
      const previousAct = snapshot.act;
      const result = Module.ccall('d2_session_command_warp', 'number', ['number', 'number'], [warpHit.destinationLevel, warpHit.warpId]);
      lastWarpTaken = { destinationLevel: warpHit.destinationLevel, warpId: warpHit.warpId, result, stuck };
      if (result) { snapshot = session.snapshot(); motion = session.motion();
        if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadSessionTerrain(snapshot.act - 1); }
    };
    // A walk that stalls short of the exit (the Den's mouth, approached over its rocks) still asks:
    // the engine's warp command decides whether the player is close enough, as the server does.
    armWalk({ x: warpHit.x, y: warpHit.y, within: 2, onArrive: () => takeWarp(false), onStuck: () => takeWarp(true) });
  } else if (npcHit) {
    click.spent = 'npc'; click.walkTo = [npcHit.x, npcHit.y];
    armWalk({ x: npcHit.x, y: npcHit.y, within: 4, onArrive: () => talkToNpc(npcHit.id) });
  } else if (objectHit) {
    click.spent = 'object'; click.walkTo = [objectHit.x, objectHit.y];
    // Retail's interact-range test (UNITS_IsObjectInInteractRange) can still refuse at two subtiles
    // -- the Cold Plains waypoint was clicked from there forever (s000a) -- while a portal's own
    // footprint keeps the player from getting closer. So: operate on arrival, and only if the engine
    // refuses, step one subtile closer and try once more.
    const operateObject = (closer) => { const level = snapshot.level; const done = session.operate(objectHit.id); snapshot = session.snapshot(); pollUiEvents();
      // Refused = nothing happened: no change of mode, no window opened, no change of level.
      if (!done && !waypointMenu && !stashOpen && snapshot.level === level && !closer) {
        // The object's own cells are occupied; stand on the free cell beside it nearest the player.
        const p = snapshot.player; let spot = null, best = Infinity;
        for (let dy = -2; dy <= 2; dy++) for (let dx = -2; dx <= 2; dx++) {
          if (Math.max(Math.abs(dx), Math.abs(dy)) === 0) continue;
          const x = objectHit.x + dx, y = objectHit.y + dy;
          if (!Module.ccall('d2_session_walkable', 'number', ['number', 'number'], [x, y])) continue;
          const d = Math.hypot(x - p.x, y - p.y) + 3 * Math.max(Math.abs(dx), Math.abs(dy));
          if (d < best) { best = d; spot = { x, y }; }
        }
        if (spot) armWalk({ x: spot.x, y: spot.y, within: 0, onArrive: () => operateObject(true) });
      } };
    armWalk({ x: objectHit.x, y: objectHit.y, within: 2, onArrive: () => operateObject(false), onStuck: () => operateObject(true) });
  } else {
    // A "+" on the character screen spends a point through retail's 0x3A handler. Re-read rather
    // than adjust a local copy: the engine decides what a point did, including refusing it.
    // The skill tree takes the click before the world does, so learning a skill does not also walk.
    const skillHit = skillsVisible && skillPanelRenderer && session
      ? skillPanelRenderer.skillAt(x, y, canvas.width, canvas.height,
          { skills: readClassSkills() || [] }) : null;
    if (skillHit?.tab) { click.spent = 'skillPanelTab'; skillPanelRenderer.tab = skillHit.tab; canvas.focus(); e.preventDefault(); return; }
    if (skillHit?.skill) {
      click.spent = 'skillPanel';
      Module.ccall('d2_session_command_add_skill', 'number', ['number'], [skillHit.skill.id]);
      classSkills = null; advancement = null;
      canvas.focus(); e.preventDefault(); return;
    }
    const statHit = characterVisible && characterPanelRenderer && session
      ? characterPanelRenderer.buttonAt(x, y, canvas.width, canvas.height, readAdvancement()) : null;
    if (statHit !== null && statHit !== undefined) {
      click.spent = 'statPanel';
      Module.ccall('d2_session_command_add_stat', 'number', ['number', 'number'], [statHit, 1]);
      advancement = null;
      canvas.focus(); e.preventDefault(); return;
    }
    const carried = inventoryVisible && inventoryPanelRenderer
      ? inventoryPanelRenderer.itemAt(x, y, canvas.width, canvas.height, snapshot) : null;
    // With a shop open, clicking something you are carrying sells it; otherwise the inventory
    // click is swallowed as before and anything else goes to the world.
    // Shift+left-click on a carried item puts it in the belt (retail's client packet 0x63, sGridToBelt:
    // the engine decides whether it fits and which box).
    if (carried && npcState) { click.spent = 'vendorSell'; sellToVendor(carried.id); }
    else if (carried) click.spent = 'swallowed';
    else {
      // An open panel's own rectangle is not the world (see `openPanelAt`): the click stays with the
      // panel, so it neither walks the hero nor casts for him. It reaches here only after every
      // control the panel draws -- its close box, its tab, its "+", its grid and body slots, all
      // above -- has had its chance; a point on one of those was answered there. `panelOwner` was
      // read before the world's own entities for the same reason (see there), and the panels cannot
      // have opened or closed in between: every branch between the two points either returns or
      // leaves the four flags `openPanelAt` reads exactly as they were.
      if (panelOwner) click.spent = `panel:${panelOwner}`;
      else {
        click.spent = 'world'; input.pointer(x, y, e.button);
        if (e.button === 0) {
          worldPointerHeld = true;
          heldPointerTicks = 0;
          try { canvas.setPointerCapture(e.pointerId); } catch {}
        }
      }
    }
  }
  canvas.focus(); e.preventDefault(); });
// A front-end gesture completes where the pointer is RELEASED, and only if the release is still
// over the control its own press armed. Pressing DELETE CHARACTER's YES and dragging off is a
// cancel, not a deleted save. A release that never had a matching press does nothing.
// Mini-panel icon release: acts only when released over the icon that was pressed. Order is retail's
// (character, inventory, skill tree, automap, message log, quest log, game menu); the message and quest
// panels do not exist in the client yet, so those two press but open nothing.
globalThis.addEventListener('pointerup', (e) => {
  if (e.button === 0) { worldPointerHeld = false; heldPointerTicks = 0; }
  if (miniDown < 0) return;
  const down = miniDown; miniDown = -1;
  const [x, y] = canvasPoint(e);
  if (controlPanelRenderer?.hitMiniButton(controlPanelEvidence, x, y, canvas.width, canvas.height) === down) {
    if (down === 0) { characterVisible = !characterVisible; if (characterVisible) questVisible = false; }
    else if (down === 1) inventoryVisible = !inventoryVisible;
    else if (down === 2) skillsVisible = !skillsVisible;
    else if (down === 3) automapOverlay?.toggle();
    else if (down === 4) toggleMessage();
    else if (down === 5) toggleQuest();
    else if (down === 6) ensureGameMenu()?.toggle();
  }
  render();
});
canvas.addEventListener('pointerup', (e) => {
  if (e.button === 0) { worldPointerHeld = false; heldPointerTicks = 0; }
  if (!(screen === SCREEN.TITLE || screen === SCREEN.CREDITS || screen === SCREEN.CINEMATICS || screen === SCREEN.CREATE || screen === SCREEN.SELECT)) return;
  const [x, y] = canvasPoint(e);
  frontendPointerStates.release(e.pointerId, frontendPointerControl(), x, y, canvas.width, canvas.height);
  canvas.focus(); e.preventDefault();
});
// Browsers cancel a touch for an interruption or a gesture the page is not handling; that gesture
// is over, so it must neither activate its control later nor keep drawing it pressed.
canvas.addEventListener('pointercancel', (e) => {
  worldPointerHeld = false; heldPointerTicks = 0;
  if (!frontendPointerStates.cancel(e.pointerId)) return;
  canvas.focus(); e.preventDefault();
});
canvas.addEventListener('contextmenu', (e) => { if (session && inventoryVisible && inventoryPanelRenderer) {
    const [x, y] = canvasPoint(e), item = inventoryPanelRenderer.itemAt(x, y, canvas.width, canvas.height, snapshot);
    // Retail's right click on an inventory item USES it (0x20: a scroll is read, a potion drunk);
    // it used to drop the item here, which retail never does.
    if (item && session.useItem(item.id)) { snapshot = session.snapshot(); pollUiEvents(); }
  } e.preventDefault(); });
window.__d2Shell = { tooltipSpec: (id, cost, code) => tooltipSpec({ cost, code, ...tipFields(id, code) }), input, warpAt: (x, y) => warpUnderPointer(x, y), npcAt: (x, y) => npcUnderPointer(x, y)?.id ?? 0, get snapshot() { return snapshot; },
  // The ramp this shell actually blits every finished frame through (C1d), so a row can assert the
  // transform that reaches the canvas is retail's own rather than re-deriving the constant.
  displayRamp: DISPLAY_RAMP,
  step() { const p = perfT0(); update(true); perfT('update', p); },   // one shell update AND one game frame, so a scenario's own clock is exact
  // The rAF path's cadence without requestAnimationFrame: n shell updates, the engine advanced by
  // the 25 Hz clock alone. `stepClock(25)` must advance the engine 10 frames, not 25 -- that is the
  // whole of retail's tick rate, asserted without a wall clock (s430).
  stepClock(n) { const p = perfT0(); for (let i = 0; i < n; i++) update(false); perfT('update', p); },
  // --- The frame-cost instrument (assets/frame-perf.mjs). OFF unless __d2Perf is set; the row
  // (s2310) sets it, drives frames with stepClock/renderNow, and reads perfReport(). ---
  perfEnable() { globalThis.__d2Perf = true; perfSync(); return perfEnabled(); },
  perfDisable() { globalThis.__d2Perf = false; perfSync(); return perfEnabled(); },
  perfReset() { perfReset(); },
  perfReport() { return perfReport(); },
  get gameStepMs() { return GAME_STEP; },
  // How many times the engine has been advanced. Retail's rule is a RATE -- 25 a second -- so the
  // only way to assert it is to compare this against the shell frames that produced it.
  get engineTicks() { return engineTicks; }, set engineTicks(v) { engineTicks = v; },
  // How many legs the walk controller has issued for the current goal. This is the number that moves
  // when walkStep runs on shell updates the engine did not advance -- the stall counter is measured in
  // ENGINE frames, so an update that advanced nothing must not count. s412 asserts it.
  get walkHops() { return walkEvidence.hops; },
  // n frames with one shell update at the end: the engine ticks the others directly (the shell's
  // per-frame world read is ~2.5 ms of a 3.6 ms step). Any frame that needs the shell's own per-frame
  // logic -- a click-walk in progress, the paused game menu, a screen other than the game -- runs the
  // full update instead, so this only skips re-reading a world nothing is waiting on.
  stepN(n) {
    const p = perfT0();
    for (let i = 0; i < n - 1; i++) {
      if (screen === SCREEN.GAME && session && !walkGoal && !gameMenu?.open && Module._d2_session_tick) { engineTicks++; Module.ccall('d2_session_tick', 'number', ['number'], [1]); drainUnitSounds(); const lv = Module.ccall('d2_session_player_level', 'number', [], []); if (lv !== entryBannerLevel) { entryBannerLevel = lv; entryBannerAt = engineTicks; } }
      else update(true);
    }
    if (n > 0) update(true);
    perfT('update', p);
  },
  // Force one frame. requestAnimationFrame is throttled hard in a headless/background tab, so a
  // caller that drives the shell with step() and then reads PIXELS can otherwise be looking at a
  // frame from before its own input -- which reads exactly like "the screen never changed".
  renderNow() { render(); },
  // The title's animation clock (60 Hz updates; logo frame = floor(tick * 25 / 60) mod 29). A draw-list row pins it to the
  // phase retail's captured frame shows (s180), so the comparison is of one scene, not two phases.
  get titleTick() { return titleTick; }, set titleTick(v) { titleTick = v; },
  // Camera viewport centre after accounting for panels. Speech windows do not occupy a side panel.
  get worldCentreX() { return viewCX(); },
  // The creation screen's clock (60 Hz updates; charcreate.mjs maps it to retail's 25 Hz loops).
  get createTick() { return charCreate ? charCreate.tick : 0; }, set createTick(v) { if (charCreate) charCreate.tick = v; },
  // World (subtile) -> canvas pixel, through the SAME isometric transform render() and the pointer
  // path use. Exposed so a caller aiming at a unit does not re-derive the projection and quietly
  // drift from it; input.pointer(...project(u.x, u.y)) round-trips back to that unit.
  // F101: this is the STATIC-GRID point (centre camera): a unit's drawn anchor is 8 px below it. Objects, warps and the
  // scenarios' aims were calibrated on this; unit hover boxes use the drawn anchor (unitBoxHit, cameraGrid).
  project(wx, wy) {
    const c = worldToScreen(cameraCentre().x, cameraCentre().y), p = worldToScreen(wx, wy);   // the static grid
    return [p.x - c.x + viewCX(), p.y - c.y + (canvas.height / 2 + UNIT_CENTRE_Y)];
  },
  // F101: the CENTRE of a unit's hover box (unitBoxHit: the drawn cel anchor via cameraGrid; a click on a tall NPC aims 32 px above it, the box centre). Aim unit clicks with this; the retail F14 box is x -24..19, y -78..15 about it.
  // With `id`, the first of anchor / mid-body / head-height that resolves to THAT unit (a pack's boxes overlap: the anchor alone often names a neighbour).
  unitAim(wx, wy, id) {
    const c = worldToScreen(cameraGrid().x, cameraGrid().y), p = worldToScreen(wx, wy);
    const bx = p.x - c.x + viewCX(), by = p.y - c.y + (canvas.height / 2 + UNIT_CENTRE_Y);
    if (id != null) for (const [dx, dy] of [[0, 0], [-2, -32], [0, -16], [-2, -56], [-2, -8]]) if (resolvePointerTarget(bx + dx, by + dy)?.unitId === id) return [bx + dx, by + dy];
    return [bx, by];
  },
  get playerRenderer() { return playerRenderer; }, get monsterRenderer() { return monsterRenderer; },
  get missileEvidence() { return missileEvidence.map((entry) => ({ ...entry })); },
  // Retail's weather (F11b): the level's own Rain byte and one identity per rain blit drawn this
  // frame -- the record shape retail's draw lists use, so a row can compare them record for record.
  get weatherEvidence() { return { rain: weatherEvidence.rain, errors: weatherEvidence.errors ? weatherEvidence.errors.slice() : [], blits: weatherEvidence.blits.map((b) => ({ ...b })), ripples: weatherRenderer ? weatherRenderer.ripples : [], lines: (weatherEvidence.lines || []).map((l) => ({ ...l })), streakError: weatherEvidence.streakError || null, cycle: weatherEvidence.cycle || null }; },
  get weatherRenderer() { return weatherRenderer; },
  get npcSpeechAlertRenderer() { return npcSpeechAlertRenderer; },
  // F22: the units the engine reports as having a speech pending, and one drawn-blit identity per
  // alert actually painted this frame -- the cel's own rectangle on the canvas and the decoded hash
  // of the frame rasterised into it, so a row reads the blit back rather than the request.
  get npcSpeechAlerts() { return { pending: snapshot.entities.filter((e) => e.speechPending === true)
      .map((e) => ({ id: e.id, classId: e.classId, x: e.x, y: e.y, preciseX: e.preciseX, preciseY: e.preciseY })), count: speechAlertCount }; },
  get npcAlertEvidence() { return npcAlertEvidence.map((entry) => ({ ...entry })); },
  // F115: what the area-name block was asked for and what reached the canvas this frame -- the engine's
  // own level name, each line's anchor and every glyph blit (font path, frame, frame-bytes hash,
  // anchor, colour shift), so a row compares it blit by blit with retail's own records. `open` is the
  // automap's own state, which is the whole gate: retail draws this block with the automap and not
  // without it, so there is no window and no elapsed tick to report.
  get areaNameEvidence() { return {
    level: snapshot?.level ?? 0, open: Boolean(automapOverlay?.isOpen),
    error: areaNameError, drawn: areaNameEvidence && JSON.parse(JSON.stringify(areaNameEvidence)) }; },
  // s693: what the level-ENTRY banner was asked for and what reached the canvas this frame -- the
  // engine's own EntryFile for the level, that level's own art, and one record per frame blitted (path,
  // frame, frame-bytes hash, anchor, mode, colour shift), so a row compares it record for record with
  // retail's own. `arrivedAt`/`tick` are the gate itself: the engine tick the level was first seen on
  // and the tick now, whose difference is the frame of retail's measured 120-frame window this frame
  // is. `window` is the window's length, so a row does not restate it. Nothing is drawn while a
  // ClearScreen is up, and `drawn` is null then -- the same "nothing blitted" the row's control asks for.
  get entryBannerEvidence() { return {
    level: entryBannerLevel > 0 ? entryBannerLevel : (snapshot?.level ?? 0), tick: engineTicks,
    arrivedAt: entryBannerLevel > 0 ? entryBannerAt : null, window: ENTRY_BANNER.frames, error: entryBannerError,
    drawn: entryBannerEvidence && JSON.parse(JSON.stringify(entryBannerEvidence)) }; },
  get objectRenderer() { return objectRenderer; },
  get waypointPadEvidence() { return waypointPadEvidence.map((entry) => JSON.parse(JSON.stringify(entry))); },
  get itemRenderer() { return itemRenderer; },
  get inventoryPanelEvidence() { return inventoryPanelEvidence && { ...inventoryPanelEvidence }; },
  get characterPanelEvidence() { return characterPanelEvidence && { ...characterPanelEvidence }; },
  get skillPanelEvidence() { return skillPanelEvidence && { ...skillPanelEvidence }; },
  get warpEvidence() { return { lastWarpTaken, warps: (snapshot.warps || []).map((w) => ({ destinationLevel: w.destinationLevel, warpId: w.warpId, enabled: w.enabled, x: w.x, y: w.y })) }; },
  get waypointEvidence() { return { open: Boolean(waypointMenu), waypointId: waypointMenu?.id ?? 0, shownAct: waypointMenu?.act ?? 0, error: waypointPanelError,
    drawn: waypointEvidence && JSON.parse(JSON.stringify(waypointEvidence)), art: waypointPanelRenderer?.evidence ?? null,
    lastTravel: lastWaypointTravel }; },
  waypointTargets() { return waypointMenu ? waypointMenuInput.targets(canvas.width, canvas.height) : null; },
  get plugyStash() { return { fail: plugyFail, ready: plugyReady, error: plugyError, settle: () => plugyLast, pages: plugyPages, artReady: Boolean(plugyStash.art), enabled: plugyStash.enabled, set enabled(v) { plugyStash.enabled = v; }, page: plugyStash.page, drawn: plugyEvidence }; },
  get stashEvidence() { return { open: stashOpen, error: stashPanelError, drawn: stashEvidence && JSON.parse(JSON.stringify(stashEvidence)),
    art: stashPanelRenderer?.evidence ?? null, gold: { error: stashGoldError, drawn: stashGoldEvidence && JSON.parse(JSON.stringify(stashGoldEvidence)),
      art: stashGoldRenderer?.evidence ?? null } }; },
  get questVisible() { return questVisible; }, set questVisible(v) { if (Boolean(v) !== questVisible) toggleQuest(); },
  // V8d: the questlast description toggle's state (the button's own frame: 0 off, 1 on). Reset on open.
  get questDescrOn() { return questDescrOn; }, set questDescrOn(v) { questDescrOn = Boolean(v); },
  get chatOpen() { return chat !== null; }, get chatEvidence() { return chatEvidence; }, get postedEvidence() { return postedEvidence; }, get messageLines() { return messageLines.slice(); },
  get objectSpeech() { return objectSpeech; }, dismissObjectSpeech,
  get messageVisible() { return messageVisible; }, set messageVisible(v) { messageVisible = Boolean(v); },
  // V8d: the message log's scroll offset in lines (0 = newest at the top, one arrow click = one line).
  get messageScroll() { return messageScroll; }, set messageScroll(v) { messageScroll = Math.max(0, Math.min(messageScrollMax(messageLines.length), v | 0)); },
  get messageScrollMax() { return messageScrollMax(messageLines.length); },
  scrollMessageLog(dir) { this.messageScroll = messageScroll + (dir === 'down' ? 1 : -1); return messageScroll; },
  get questEvidence() { return questEvidence; }, get messageEvidence() { return messageEvidence; },
  get skillsVisible() { return skillsVisible; },
  // The inventory panel's own visibility: the third of the three panel toggles, so a row can name the
  // panel a physical `KeyI` opened (F3/F3b).
  get inventoryVisible() { return inventoryVisible; },
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
  // The belt's hit boxes -- the SAME rectangles the click path's own itemAt() is handed below, so an
  // input layer that has to aim a finger at them (input-touch.mjs, M14) reads one source of truth
  // rather than re-deriving retail's Belts.txt geometry. Read-only; null before the engine is up.
  get beltHudBoxes() { return readBeltHudBoxes(); },
  get wideFrameEvidence() { return wideFrameEvidence; },
  get freeRes() { return freeResArt ? { ready: freeResArt.ready, error: freeResArt.error } : null; },
  get lastGridToBelt() { return lastGridToBelt; },
  get lastBeltToCursor() { return lastBeltToCursor; },
  get cursorItemEvidence() { return cursorItemEvidence && JSON.parse(JSON.stringify(cursorItemEvidence)); },
  get lastItemCommand() { return lastItemCommand && { ...lastItemCommand }; },
  get inventoryLayout() { return inventoryPanelRenderer?.layout ? JSON.parse(JSON.stringify(inventoryPanelRenderer.layout)) : null; },
  // The inventory panel's own rectangle, in 800x600 screen pixels: the SAME rectangle its `contains`
  // hit test is handed, so an input layer that has to bound a finger's aim by the panel's chrome
  // (input-touch.mjs, M15/s377) reads the panel's own answer rather than re-deriving retail's anchors.
  // Read-only; null before the engine is up.
  get inventoryPanelRect() { return inventoryPanelRenderer ? inventoryPanelRenderer.panelRect() : null; },
  // The character screen's and the skill tree's own rectangles, the same way: the two panels that had
  // no rectangle at all until the click routing needed one (s427). Each is the panel's own answer.
  get characterPanelRect() { const r = characterPanelRenderer?.panelRect(); return r ? { ...r } : null; },
  get skillPanelRect() { const r = skillPanelRenderer?.panelRect(); return r ? { ...r } : null; },
  get cubeEvidence() { return { open: cubeOpen, error: stashPanelError, drawn: cubeEvidence && JSON.parse(JSON.stringify(cubeEvidence)), geometry: cubeOpen ? readCubeLayout() && { ...readCubeLayout() } : null }; },
  get stashGeometry() { const g = stashOpen ? readStashLayout() : null; return g ? JSON.parse(JSON.stringify(g)) : null; },
  get stashPanelRect() { const r = stashOpen ? stashPanelRenderer?.panelRect(readStashLayout()) : null; return r ? { ...r } : null; },
  get controlPanelEvidence() { return controlPanelEvidence && { ...controlPanelEvidence }; },
  get hudHelpEvidence() { return hudHelpEvidence && { ...hudHelpEvidence }; },
  get hudHelpError() { return hudHelpError; },
  // Retail's Help screen (`H`). No setter, deliberately: the only way it opens is the key (or its
  // Close box closing it), so a row cannot pass by flipping the flag itself.
  get helpVisible() { return helpVisible; },
  get helpMenuSeen() { return helpMenuSeen(); },
  resetHelpMenu() { try { localStorage.removeItem(HELP_MENU_KEY); } catch { /* no store */ } },
  get helpError() { return helpError; },
  get helpEvidence() { return helpEvidence && JSON.parse(JSON.stringify(helpEvidence)); },
  // The screen's own Close box, in canvas coordinates: the row asks the renderer that draws it
  // whether a point is on it, rather than re-deriving the rectangle from the layout.
  helpHitClose(x, y) { return helpRenderer ? helpRenderer.hitClose(x, y, canvas.width, canvas.height) : false; },
  // The run/walk toggle's action, both ends: the flag the engine is in and the click that set it.
  get runToggle() { return readRunToggle(); },
  get lastRunToggle() { return lastRunToggle && { ...lastRunToggle }; },
  get automapEvidence() { return automapOverlay?.evidence ?? { open: false, error: 'not built' }; },
  // The live automap layer's cells as retail `.ma0` quints {layer, frame, x, y, wall} -- what the
  // engine's writer is handed on Save and Exit (F268). The scenario surface, so a row can compare the
  // .ma0 the engine composed against the exact cells it was given.
  automapCells() { return automapOverlay?.savedCells?.() ?? []; },
  get skillSelectEvidence() { return skillSelectEvidence && JSON.parse(JSON.stringify({ ...skillSelectEvidence, error: skillSelectError })); },
  get selectedSkill() { return Math.max(0, readSelectedSkills().right); }, get lastSkillCommand() { return lastSkillCommand && { ...lastSkillCommand }; },
  get lastBeltCommand() { return lastBeltCommand && { ...lastBeltCommand }; },
  get lastMoveCommand() { return lastMoveCommand && { ...lastMoveCommand }; },
  // Both hands, straight from the engine's own record: a row that proves `S` picked the RIGHT
  // selector has to be able to say the left hand did not move (F3c).
  get selectedSkills() { return readSelectedSkills(); },
  // Retail's ClearScreen state (CfgClearScreen, `Space`). The row reads it to know which frame it is
  // looking at; what it ASSERTS is the frame itself.
  get clearScreen() { return clearScreen; },
  // WHICH hand's skill selector is open -- the renderer's own state, the field its socket click and
  // the `S` key both set. `skillSelectEvidence` reports the last DRAWN frame, so a row that only
  // wants to know what the key did (s660) reads this instead of forcing a frame.
  get skillSelectOpen() { return skillSelectRenderer ? (skillSelectRenderer.open || null) : null; },
  get screen() { return screen; },
  get frameError() { return frameError; }, get frameErrors() { return frameErrors; },
  // The front-end gesture the pointer layer is holding: which pointer armed which control token,
  // so a row can prove one finger cannot release another's control (and that a press with no
  // release leaves the control merely armed).
  get frontendPointerEvidence() { return { armed: frontendPointerStates.armed }; },
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
      npcState = npcMenu?.open ? null : readNpcState();   // the menu comes first; the window opens on its entry
      return Boolean(ok);
    } catch { return false; }
  },
  gambleForItem(itemId) { return gambleAtVendor(itemId); },
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
  get hireWindowEvidence() { return hireWindowEvidence && { ...hireWindowEvidence, offers: npcMenu.hireState.offers.map((o) => ({ ...o })) }; },
  get hireEvidence() { return lastHire ? { ...lastHire } : null; },
  get repairAllEvidence() { return lastRepairAll ? { ...lastRepairAll } : null; },
  // Both sides of the merge touched this line: main added the stall counter (the walk controller
  // counts stalls in engine frames), this branch added the goal. Keep both.
  get walkEvidence() { return { ...walkEvidence, stall: walkGoal ? (walkGoal.stall || 0) : 0, goal: walkGoal ? { x: walkGoal.x, y: walkGoal.y } : null }; },
  // Which handler spent each click, and what the world path did with the ones it got: counts by
  // handler, counts by what a ground click resolved to and did, and the last records in full. A walk
  // is a sequence of these, so a walk that oscillates is read off here (see noteClick).
  clickDispatchEvidence() {
    const by = {}, world = {}, refusedBy = {}, heroModes = {};
    for (const r of clickDispatch) {
      by[r.spent ?? 'unanswered'] = (by[r.spent ?? 'unanswered'] || 0) + 1;
      // The engine's reason for a refused walk, and the hero's mode when the click was sent (see
      // noteClick): a leg whose every click is refused reads here as "N refused, all reason 6, hero mode
      // 17" -- the walker was clicking at a dead player -- instead of "N refused, subject unknown".
      if (r.reason != null) refusedBy[r.reason] = (refusedBy[r.reason] || 0) + 1;
      if (r.hero) heroModes[r.hero.mode] = (heroModes[r.hero.mode] || 0) + 1;
      // Keyed by the button as well: a left world click is a walk and a right one is an attack, and a
      // single tally of the two reads as a leg that fights when it is only walking.
      // The coordinates go, the OUTCOME stays: "move -> 0" is a refused walk and "move -> 1" an
      // accepted one, and those are the two answers this tally exists to separate.
      if (r.spent === 'world') { const k = (r.button === 2 ? 'R ' : 'L ') + (r.result || 'unanswered').replace(/ \([-\d,]*\)/g, ''); world[k] = (world[k] || 0) + 1; }
    }
    return { clicks: clickDispatch.length, by, world, refusedBy, heroModes, last: clickDispatch.slice(-40).map((r) => ({ ...r })) };
  },
  walkTo(x, y, within = 4) { armWalk({ x, y, within }); },
  // `minDistance` (subtiles, 0 = no floor): talk to the nearest NPC at least that far away. s136
  // proves the WALKER by talking to someone the hero has to walk to, and with the corrected Rogue
  // Encampment spawn the nearest NPC is 7 subtiles away -- a step, not a walk.
  talkToNearestNpc(minDistance = 0) {
    // Actually the nearest: picking the first NPC in the entity list walked past Charsi to Gheed.
    const p = snapshot.player;
    let npc = null, best = Infinity;
    for (const entity of snapshot.entities) {
      if (!entity.npc) continue;
      const d = chebyshev(entity.x, entity.y, p.x, p.y);
      if (d >= minDistance && d < best) { best = d; npc = entity; }
    }
    if (!npc) return false;
    armWalk({ x: npc.x, y: npc.y, within: 4, onArrive: () => talkToNpc(npc.id) });
    return true;
  },
  get npcPanelEvidence() {
    return { open: Boolean(npcState), error: npcPanelError,
      npcId: npcState?.npcId ?? 0, spoken: npcState?.spoken ?? 0, greeting: npcState?.greeting ?? '',
      items: npcState?.items?.length ?? 0,
      gold: npcState?.gold ?? -1,
      firstItems: (npcState?.items ?? []).slice(0, 6).map((i) => ({ id: i.id, code: i.code, name: i.name, cost: i.cost, invFile: i.invFile })),
      drawn: npcPanelEvidence, panel: (shopOpen() ? vendorPanel?.evidence : npcPanel?.evidence) ?? null };
  },
  get unitDrawEvidence() { return unitDrawEvidence.map((u) => ({ ...u })); },
  get objectDrawEvidence() { return objectDrawEvidence.map((o) => ({ ...o })); },
  talkTo(npcId) { return talkToNpc(npcId); },   // test hook (s2151): the click-arrival path
  // Test hook (s2150): the plate a hovered monster of this class gets, drawn on a scratch canvas.
  hoverPlateFor(classId) {
    const idx = Module.ccall('d2_monster_name_str', 'number', ['number'], [classId]);
    const label = Module.ccall('d2_lang_string_by_index', 'string', ['number'], [idx]);
    const cv = document.createElement('canvas'); cv.width = canvas.width; cv.height = canvas.height;
    return JSON.parse(JSON.stringify(drawHoverLabel(cv.getContext('2d'), { kind: 'unit', id: 0, classId, label, x: 0, y: 0 }, null, null)));
  },
  get hoverEvidence() { return hoverEvidence && JSON.parse(JSON.stringify(hoverEvidence)); },
  get hoverFontReady() { return Boolean(hoverFont); },
  // s286 checks the hover/click boundary contract through the same resolver the input paths call.
  pointerTargetAt(x, y) {
    const target = resolvePointerTarget(x, y);
    return { at: { ...target.at }, unitId: target.unitId, itemId: target.item?.id ?? 0 };
  },
  get hoverGlyphPainting() { return hoverGlyphPainting; },
  set hoverGlyphPainting(value) { hoverGlyphPainting = Boolean(value); },
  get gameCursorEvidence() { return gameCursorEvidence && JSON.parse(JSON.stringify(gameCursorEvidence)); },
  get gameCursorAssets() { return gameCursor?.assets && JSON.parse(JSON.stringify(gameCursor.assets)); },
  get npcEvidence() {
    const ents = snapshot.entities;
    return { npcs: npcCount, hostile: ents.filter((e) => e.hostile).length,
      npcClasses: [...new Set(ents.filter((e) => e.npc).map((e) => e.classId))].sort((a, b) => a - b) };
  },
  // persistRequested: whether navigator.storage.persist() has been asked for yet -- the
  // saveStore's own once-only flag, read live rather than re-derived, so a row can assert it flips
  // from false to true exactly at the first successful put() and never again after.
  get saveEvidence() { return { written: savesWritten, error: lastSaveError, autosaveTicks: autosaveAt, inFlight: autosaveInFlight, persistRequested: saveStore.persistRequested }; },
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
  plugySwapTo(n) { const was = plugyStash.page; plugyStash.page = n; const ok = plugySwap(was, n); if (!ok) plugyStash.page = was; return ok; },   // sets the page first, as the click handler does   // scenario surface: the page control's swap without a click
  putAuxRaw(key, bytes) { return saveStore.putAux(key, bytes); },
  getAuxRaw(key) { return saveStore.getAux(key); },
  selectCharacter(name) { return charSelect ? charSelect.selectByName(name) : -1; },
  playSelectedCharacter() { charSelect?.play(); },
  get charCreateEvidence() { return charCreate ? { ...charCreate.evidence, error: charCreateError } : { error: charCreateError || 'not built' }; },
  get charCreateSoundEvidence() { return charCreateSounds ? { ...charCreateSounds.evidence, error: charCreateSoundError } : { events: [], decoded: [], state: 'unavailable', error: charCreateSoundError || 'not built' }; },
  chooseClass(key) { return charCreate ? charCreate.chooseByKey(key) : -1; },
  setCharacterName(value) { return charCreate ? charCreate.setName(value) : null; },
  submitCharacter() { charCreate?.submit(); },
  // The flags the ENGINE holds for the character being created (C2c), not a copy of the screen's:
  // null until the create screen has handed its checkboxes over.
  get clientSaveFlags() { return lastClientSaveFlags ?? (window.Module?._d2_session_client_flags ? Module.ccall('d2_session_client_flags', 'number', [], []) : null); },
  // The engine's active weapon set (0 primary, 1 alternate), for the rows that prove retail's Swap
  // Weapons (`W`, F3) actually moved it. The client never keeps its own copy: it draws what the
  // engine reports.
  get weaponSet() { return window.Module?._d2_session_weapon_switch ? Module.ccall('d2_session_weapon_switch', 'number', [], []) : -1; },
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
  get corpseEvidence() { return corpseEvidence && { ...corpseEvidence }; }, get lastCorpseRecovery() { return lastCorpseRecovery && { ...lastCorpseRecovery }; },
  get npcMenuEvidence() { return npcMenuEvidence ? { ...npcMenuEvidence, error: npcMenuError } : { error: npcMenuError }; },
  get deathScreenEvidence() { return deathScreenEvidence ? { ...deathScreenEvidence, error: deathScreenError } : { error: deathScreenError }; },
  get optionsEvidence() { return { open: !!gameMenu?.open && gameMenu.page === 'options' }; },
  get gameMenuEvidence() { return gameMenu ? { ...gameMenu.evidence, error: gameMenuError } : { open: false, error: gameMenuError }; },
  // F218: retail's Configure Controls panel (the Options page's fourth line). The getter is the
  // harness surface -- the panel itself, so a row can click/key it by retail's own coordinates and
  // read the state it acted on; the evidence is the small truth a dossier wants.
  get configureControls() { return controlsPanel; },
  get configureControlsEvidence() {
    if (!controlsPanel) return { open: false, error: controlsPanelError };
    return { ...controlsPanel.evidence, error: controlsPanelError };
  },
  // Hostile monsters near the player with what they are (d2_session_unit_info 8..11): MonsterData
  // type flags (0x04 champion, 0x08 unique, 0x10 minion, 0x02 superunique), monster level, damage%.
  get monsterTypes() {
    const s = this.snapshot, p = s?.player;
    if (!p || !window.Module?._d2_session_unit_info) return [];
    return s.entities.filter((e) => e.type === 1 && e.hostile && e.hp > 0 && Math.hypot(e.x - p.x, e.y - p.y) < 30).map((e) => {
      // The call first, then the heap: a call that grows the heap leaves an earlier HEAPU8 read stale (traps.md).
      const at = Number(Module.ccall('d2_session_unit_info', 'number', ['number', 'number'], [1, e.id]));
      const v = new Int32Array(Module.HEAPU8.buffer, at, 12);
      return { id: e.id, classId: e.classId, typeFlag: v[8], level: v[9], damagePercent: v[10], umod: v[11] };
    });
  },
  // The MENU phase's identity. Retail shows the trademark screen first, so a row waiting on the
  // menu's `background` waits for the menu and not merely for the front end to exist; the trademark
  // phase reports its own evidence below.
  get titleEvidence() {
    if (!titleScreen) return { error: titleError || 'not built' };
    const e = { ...titleScreen.evidence, phase: titlePhase, error: titleError };
    return titlePhase === 'menu' ? e : { phase: e.phase, error: e.error, trademark: e.trademark };
  },
  /** The trademark phase's identity: the art, the font and the two copyright lines it drew. */
  get titleTrademarkEvidence() {
    if (!titleScreen) return { error: titleError || 'not built' };
    return { ...titleScreen.evidence.trademark, phase: titlePhase, error: titleError };
  },
  get titlePhase() { return titlePhase; },
  /** Every phase this page entered, in order, with the page's own clock (performance.now()). */
  get titlePhaseLog() { return titlePhaseLog.map((e) => ({ ...e })); },
  /**
   * The boot movie the current phase owns (client-parity C1h), with the numbers read from the movie
   * file's OWN header -- path, frames, fps, width, height -- and the frame index the decoder and the
   * canvas are actually on. `null` movie on the screens; an `error` string if the file would not open.
   */
  get cinematicsEvidence() {
    const spec = bootMovieSpec(titlePhase);
    if (!spec) return { phase: titlePhase, movie: null, error: bootMovieError };
    if (bootMovieError) return { phase: titlePhase, movie: spec.path, error: bootMovieError };
    if (!bootMovie) return { phase: titlePhase, movie: spec.path, opening: bootMovieOpening, error: '' };
    return { phase: titlePhase, startedAt: bootMovieStart, ...bootMovie.evidence() };
  },
  /**
   * The blit, not the module's report of it: for a few points, the RGB the decoder holds for the
   * frame it drew paired with the canvas pixel the browser actually has there once rendered
   * (`drawn`). A row compares the two and, by sampling twice, that the comparison can tell one movie
   * frame from another.
   */
  movieSamples() { return bootMovie ? bootMovie.sample(ctx, canvas.width, canvas.height) : null; },
  /**
   * Hold the boot movie on one frame, or release it with -1, the way a row pins `titleTick` (s198).
   * Pinned, the movie stops being paced by the clock, so the phase cannot advance under a row's feet
   * while it compares frames against a retail capture.
   */
  async pinMovieFrame(index) {
    if (!bootMovie) return -1;
    moviePin = index;
    return bootMovie.seek(index);
  },
  // A scenario pins the phase the way it pins titleTick (s198, s194).
  set titlePhase(phase) { setTitlePhase(phase); },
  get cinematicsScreenEvidence() { return cinematicsScreen ? cinematicsScreen.evidence : { error: cinematicsError || 'not built' }; },
  get creditsEvidence() { return creditsScreen ? { ...creditsScreen.evidence, error: creditsError } : { error: creditsError || 'not built' }; },
  get exitEvidence() { return { ...exitEvidence, sequence: exitSeq ? exitSeq.evidence : null }; },
  /**
   * Canvas-pixel click targets for the front-end screens, from the same hit tests their clicks use.
   * s000 aims real pointer events with these, the way world clicks are aimed with project().
   */
  uiTargets() {
    const w = canvas.width, h = canvas.height, out = { screen };
    if (screen === SCREEN.TITLE && titleScreen && titlePhase === 'menu') {
      // The menu's own boxes, and only once the menu is up: retail shows the trademark screen first,
      // and during it there is nothing to click (the phase's click is what `titleAdvance` handles).
      out.title = titleScreen.targets(w, h);
      // The region dialog is modal over the menu: while it is up, its rows are what the pointer hits.
      if (titleScreen.gatewaysOpen()) out.titleGateways = titleScreen.gatewayTargets(w, h);
    }
    if (screen === SCREEN.CINEMATICS && cinematicsScreen) out.cinematics = cinematicsScreen.targets(w, h);
    if (screen === SCREEN.CREDITS && creditsScreen) out.credits = creditsScreen.targets(w, h);
    if (screen === SCREEN.CREATE && charCreate?.targets) out.create = charCreate.targets(w, h);
    if (screen === SCREEN.SELECT && charSelect?.targets) out.select = charSelect.targets(w, h);
    if (screen === SCREEN.GAME && gameMenu?.open) out.gameMenu = gameMenu.targets(w, h);
    if (screen === SCREEN.GAME && npcMenu?.open) out.npcMenu = npcMenu.targets();
    // The shop's stock under the pointer, found through the same itemAt the click handler uses:
    // item id -> { at, code, cost } for the vendor's side of the trade screen.
    if (screen === SCREEN.GAME && shopOpen() && vendorPanel) {
      out.shop = {};
      for (let y = 2; y < h; y += 4) for (let x = 2; x < w; x += 4) {
        const it = vendorPanel.itemAt(x, y, w, h, npcState);
        if (it && !(it.id in out.shop)) out.shop[it.id] = { at: [x + 6, y + 6], code: it.code, cost: it.cost };
      }
      out.shopTabs = vendorPanel.tabTargets(w, h);
      out.shopButtons = vendorPanel.buttonTargets(w, h, npcState.classId);
    }
    // The character screen's "+" buttons and the skill tree's cells, found by probing the SAME
    // hit-tests the click handler uses, so an aimed click cannot land somewhere else.
    const probe = (hit) => { const found = {}; for (let y = 2; y < h; y += 4) for (let x = 2; x < w; x += 4) { const k = hit(x, y); if (k !== null && k !== undefined && !(k in found)) found[k] = [[x, y]]; else if (k !== null && k !== undefined) found[k].push([x, y]); }
      return Object.fromEntries(Object.entries(found).map(([k, pts]) => [k, [Math.round(pts.reduce((a, p) => a + p[0], 0) / pts.length), Math.round(pts.reduce((a, p) => a + p[1], 0) / pts.length)]])); };
    if (screen === SCREEN.GAME && skillSelectRenderer && session) out.skillSelect = skillSelectRenderer.targets(w, h, readSkillChoices());
    if (screen === SCREEN.GAME && characterVisible && characterPanelRenderer && session)
      out.character = probe((x, y) => characterPanelRenderer.buttonAt(x, y, w, h, readAdvancement()));
    if (screen === SCREEN.GAME && skillsVisible && skillPanelRenderer && session) {
      const skills = readClassSkills() || [];
      out.skills = probe((x, y) => skillPanelRenderer.skillAt(x, y, w, h, { skills })?.skill?.id ?? null);
    }
    // The inventory grid and its equipment slots, probed through `cellAt`/`slotAt` -- the same hit
    // tests the mouse's own drop and equip clicks use -- so the controller's snap-to-slot navigation
    // has a canvas point for every CELL, not
    // only the ones an item currently occupies: `cellAt` answers for an empty cell exactly as it does
    // for a full one, which `itemAt` does not.
    if (screen === SCREEN.GAME && inventoryVisible && inventoryPanelRenderer && session) {
      out.inventoryGrid = probe((x, y) => { const c = inventoryPanelRenderer.cellAt(x, y, w, h, null); return c ? `${c.x},${c.y}` : null; });
      out.inventorySlots = probe((x, y) => { const s = inventoryPanelRenderer.slotAt(x, y, w, h, snapshot); return s ? s.bodyLoc : null; });
    }
    // The stash grid, the same way -- `cellAt` again, over whichever page the stash has open.
    if (screen === SCREEN.GAME && stashOpen && stashPanelRenderer && session) {
      const geometry = readStashLayout();
      out.stashGrid = probe((x, y) => { const c = stashPanelRenderer.cellAt(x, y, w, h, geometry, null); return c ? `${c.x},${c.y}` : null; });
    }
    // The stash's coin and, once that dialog is up, its own controls -- probed through the same
    // `hitAt` the pointerdown handler above dispatches on, so a row aims at the button the shell
    // will react to and not at a coordinate it derived. Added for the controller's G13 row: a pad
    // has no digit keys, so the amount box is the one field it has to reach by grid (the same
    // one-line shape as `out.shopButtons` above). `modal` means "the dialog's background", which
    // is most of the screen; it is reported for completeness and no row should aim at it.
    if (screen === SCREEN.GAME && stashOpen && stashGoldRenderer && session)
      out.stashGold = probe((x, y) => stashGoldRenderer.hitAt(x, y, w, h, readStashLayout(), Boolean(stashGoldDialog))?.action ?? null);
    return out;
  },
  titleClick(label) {
    if (!titleScreen) return -1;
    // Click a menu entry by name, so a scenario does not hardcode button pixel positions. Naming an
    // entry means the menu is up: the trademark screen has no entries to name, so this ends that
    // phase first (which is what a player's first click does there anyway).
    if (titlePhase === 'trademark') setTitlePhase('menu');
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
// F235: the Sound/Music slider values and the live gain each player is actually held at.
Object.defineProperty(window.__d2Shell, 'audioVolume', { get: () => audioVolume.evidence });
// C3h: the tab-close save runs through the journal, then the page tears down exactly as before.
// S2's UI click sound belongs on this one listener rather than a second beforeunload of its own: the
// journal write must happen while the engine is still alive, and teardown() is the single place that
// knows when it has.
installUnloadAutosave({ shouldSave: () => Boolean(session && chosenName), saveBytes: saveCurrentCharacterBytes, store: saveStore,
  onStored: () => { savesWritten++; lastSaveError = ''; }, onError: (error) => { lastSaveError = String(error); },
  teardown: () => { session?.destroy(); actMusic?.close(); charCreateSounds?.close(); canvas.__d2UiClickSound?.close(); } });
requestAnimationFrame(frame);
