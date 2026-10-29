// FrameSnapshot (render-worker plan, Stage 2, single buffer). Every engine READ render() needs is
// taken here, once per frame, just before render(); the draw path then reads this object and makes
// no engine call for the fields below. The gates mirror the draw path's own conditions, and a field a
// gate left out stays `undefined` so the draw falls back to the old live read (never a silent skip).
// Nothing here writes game state: the only engine mutation is the hold_world flag, wrapped (hold,
// read, release) inside readSessionTerrain, and the allocator pairs inside the injected readers.
import { readSessionTerrain, readSessionLightmap } from './assets/session-terrain.mjs';
import { readSessionActiveNearFeed } from './assets/session-active-near-feed.mjs';

// .tbl indices the HUD tooltips look up every frame (control panel XP/stamina and level buttons).
export const HUD_LANG_INDICES = Object.freeze([4163, 4164, 3986, 3987]);

/**
 * @param {object} a
 * @param {object} a.Module     the engine module (ccall / HEAPU8 / exports)
 * @param {object} a.gates      booleans/values the shell derives from its UI state (see shell frameGates())
 * @param {object} a.readers    the shell's own read helpers, called unchanged (self-gated and cached)
 */
export function collectFrameSnapshot({ Module, gates, readers }) {
  const fs = { filled: true };
  const call = (name, ret, types, args) => Module.ccall(name, ret, types, args);
  if (gates.pointerLayer && Module._d2_session_cursor_item) fs.cursorItemId = call('d2_session_cursor_item', 'number', [], []);
  if (gates.world) {
    const { tiles, files } = readSessionTerrain(Module);   // hold_world(1) ... read ... hold_world(0)
    fs.tiles = tiles; fs.files = files;
    if (tiles.length) {
      fs.lightmap = readSessionLightmap(Module);
      fs.rain = gates.weather && Module._d2_session_level_rain ? call('d2_session_level_rain', 'number', [], []) : -1;
      if (gates.weather) fs.activeNearFeed = readSessionActiveNearFeed(Module);
    }
  }
  if (gates.death) {
    fs.clientFlags = Module._d2_session_client_flags ? call('d2_session_client_flags', 'number', [], []) : 0;
    fs.goldLost = call('d2_session_player_stat', 'number', ['number', 'number'], [175, 0]);
  }
  if (gates.inventory) fs.playerGold = call('d2_session_player_gold', 'number', [], []);
  if (gates.session) {
    fs.advancement = readers.advancement();
    fs.characterScreen = readers.characterScreen();
    fs.runToggle = readers.runToggle();
  }
  if (gates.skills) fs.classSkills = readers.classSkills();
  if (gates.belt) { fs.beltHudBoxes = readers.beltHudBoxes(); fs.beltIndex = readers.beltIndex(); fs.beltBoxCount = readers.beltBoxes()?.length ?? 0; }
  if (gates.skillSelect) {
    fs.skillChoices = readers.skillChoices(); fs.selectedSkills = readers.selectedSkills();
    fs.hotkeys = Array.from({ length: 8 }, (_, i) => ({ skill: Number(call('d2_session_hotkey_skill', 'number', ['number'], [i])),
      left: Boolean(call('d2_session_hotkey_hand', 'number', ['number'], [i])) }));
  }
  if (gates.hud) {
    fs.lang = new Map();
    for (const i of HUD_LANG_INDICES) fs.lang.set(i, call('d2_lang_string_by_index', 'string', ['number'], [i]));
  }
  // Stage 3 follow-up: the reads left in the draw path. Each is gated by the panel that draws it.
  if (gates.hover && readers.hoverTarget) fs.hoverTarget = readers.hoverTarget();
  if (gates.inventoryLayout && readers.inventoryLayout) fs.inventoryLayout = readers.inventoryLayout();
  if (gates.hireling && readers.hirelingRoster) fs.hirelingRoster = readers.hirelingRoster();
  if (gates.stash && readers.stashLayout) { fs.stashLayout = readers.stashLayout(); if (readers.bankGold) fs.bankGold = readers.bankGold(); }
  if (gates.cube && readers.cubeLayout) fs.cubeLayout = readers.cubeLayout();
  if (gates.waypoint && readers.waypointTable) fs.waypointTable = readers.waypointTable();
  if (gates.quest && readers.questWords) fs.questWords = readers.questWords();
  if (gates.shop && readers.vendorStashGold) fs.vendorStashGold = readers.vendorStashGold();
  if (gates.entryBanner && readers.entryFile) { fs.entryFileLevel = gates.entryBanner; fs.entryFile = readers.entryFile(gates.entryBanner); }
  // Lookups whose key the draw only learns while drawing (a hovered skill, a panel string, a missile class):
  // the shell hands in the keys the PREVIOUS frame asked for; a key not yet seen falls back to the live read.
  const seen = readers.seen, fillSeen = (name, read) => {
    const set = seen?.[name]; if (!set || !set.size) return undefined;
    const m = new Map(); for (const k of set) m.set(k, read(k)); return m;
  };
  if (gates.session && seen) {
    fs.langStr = fillSeen('langStr', (k) => call('d2_lang_string', 'string', ['string'], [k]));
    fs.langIdx = fillSeen('langIdx', (k) => call('d2_lang_string_by_index', 'string', ['number'], [k]));
    fs.missileCel = fillSeen('missileCel', (k) => (Module._d2_missile_celfile ? call('d2_missile_celfile', 'string', ['number'], [k]) : ''));
  }
  if (gates.skillTips && seen) {
    fs.skillDesc = fillSeen('skillDesc', (k) => readers.skillDesc(...String(k).split(':').map(Number)));
    fs.skillMana = fillSeen('skillMana', (k) => call('d2_session_skill_mana', 'number', ['number', 'number'], String(k).split(':').map(Number)));
    fs.skillStrings = fillSeen('skillStrings', (k) => readers.skillStrings(k));
  }
  return fs;
}

// `fs[key]` if the fill took it, else the old live read. The fallback is the "unsure: keep the old path" arm.
export function fromSnapshot(fs, key, live) { return fs && fs[key] !== undefined ? fs[key] : live(); }

// Stage 3 double buffer: the fill writes the BACK slot, swap() makes it the FRONT, the draw reads only front().
// The fill never touches the current front; swap() after the fill is the only point the draw's view changes.
export function createFrameSnapshots() {
  const slots = [null, null];
  let front = 0;
  return {
    fill(args) { slots[1 - front] = args ? collectFrameSnapshot(args) : null; },
    swap() { front = 1 - front; },
    front() { return slots[front]; },
    frontIndex() { return front; },
  };
}
