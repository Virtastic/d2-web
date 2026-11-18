// The stats-panel cell rules, read from the engine (the same code D2Client's stats panel port runs).
// The shell binds the wasm module once; the panels only ask.
import { decodeDrawList } from './draw-list.mjs';
let mod = null;
export function bindClientRules(module) { mod = module; }

/** The orb draw calls (D2Client+0x27BD0 then +0x27A90 run against the engine's draw sink): records in retail's call order. */
export function orbDrawList(life, lifeMax, mana, manaMax, alive, cellFluid, cellOverlap) {
  if (!mod) throw new Error('client rules are not bound to the engine');
  const raw = mod._malloc(8 * 16 * 4), p = Number(raw);
  try {
    const n = mod.ccall('d2_client_draw_orbs', 'number', ['number', 'number', 'number', 'number', 'number', 'number', 'number', 'bigint'],
      [life | 0, lifeMax | 0, mana | 0, manaMax | 0, alive ? 1 : 0, cellFluid, cellOverlap, BigInt(p)]);
    return decodeDrawList(mod.HEAP32, p >> 2, n);
  } finally { mod._free(raw); }
}

function call(name, types, args) {
  if (!mod) throw new Error('client rules are not bound to the engine');
  return mod.ccall(name, 'number', types, args);
}

/** Colour of a cell drawn against its base: 3 above, 1 below, `col` otherwise. */
export function cellColour(value, base, col = 0) {
  return call('d2_client_cell_colour', ['number', 'number', 'number'], [value | 0, base | 0, col | 0]);
}

/** Which cells a character-screen skill row draws (D2Client+0x2FD30): bit 0 damage, bit 1 attack rating. */
export function skillRowCells(descdam, descatt) {
  return call('d2_client_skill_row_cells', ['number', 'number'], [descdam | 0, descatt | 0]);
}

/** Party-panel row colour: 2 my party, 8 another party, 1 hostile to me, 0 none. */
export function partyRowMode(share, party, mine, hostile) {
  return call('d2_client_party_row_mode', ['number', 'number', 'number', 'number'], [share, party, mine, hostile ? 1 : 0]);
}

export const rulesBound = () => mod !== null;

/** Quest log (D2Client+0x1EF10): the acts whose tab draws. `done[i]` = quest word 7/0xF/0x17/0x1A (i = act 2..5) has state 0 set. */
export function questTabActs(selAct, expansion, done) {
  const mask = done.reduce((m, d, i) => m | (d ? 1 << (i + 1) : 0), 0);
  const bits = call('d2_client_quest_tabs', ['number', 'number', 'number'], [selAct - 1, expansion ? 1 : 0, mask]);
  return [1, 2, 3, 4, 5].filter((a) => bits & (1 << (a - 1)));
}

/** Quest log icon cel for a record mode (0 completing, 1 done, 2 locked, 3 in progress): { done: questdone sheet?, frame }. */
export function questIcon(mode, prev, anim, iconIndex) {
  const v = call('d2_client_quest_icon', ['number', 'number', 'number', 'number'], [mode, prev ? 1 : 0, anim | 0, iconIndex]);
  return { done: (v >> 16) === 1, frame: v & 0xffff };
}

/** A resist cell: `adj` is the difficulty penalty added to the value. Returns { value, colour }. */
export function resistCell(value, maxStat, adj = 0, col = 0) {
  if (!mod) throw new Error('client rules are not bound to the engine');
  const raw = mod._malloc(4), p = Number(raw);
  try {
    const colour = call('d2_client_resist_cell', ['number', 'number', 'number', 'number', 'bigint'], [value | 0, maxStat | 0, col | 0, adj | 0, BigInt(p)]);
    return { value: mod.HEAP32[p >> 2], colour };
  } finally { mod._free(raw); }
}

/** Experience bar width in pixels of 0x77 (D2Client+0x26970); `have`/`total` are relative to the level start. */
export function expBarPixels(have, total) {
  return call('d2_client_exp_bar_pixels', ['number', 'number'], [have >>> 0, total >>> 0]) >>> 0;
}

/** Rows (of 80) of an orb's fluid: kind 0 life (D2Client+0x27BD0), 1 mana (+0x27A90). */
export function orbFill(cur, max, kind, alive = true) {
  return call('d2_client_orb_fill', ['number', 'number', 'number', 'number'], [cur | 0, max | 0, kind, alive ? 1 : 0]);
}

/** Stamina bar (D2Client+0x278B0): { width of 0x66, slot 0 red / 1 normal / 2 blue }. */
export function staminaBar(cur, max, over = false) {
  const v = call('d2_client_stamina_bar', ['number', 'number', 'number'], [cur | 0, max | 0, over ? 1 : 0]);
  return { width: v & 0xffff, slot: v >>> 16 };
}

/** The local game's HUD values (d2_session_hud_orbs): { life, lifeMax, mana, manaMax, stamina, staminaMax }, raw 1/256 stat values,
 *  each current one smoothed through D2Client+0x26B20 at the client tick; null without a player. Pointers cross wasm64 as BigInt. */
export function readHudOrbValues(module) {
  const raw = module._malloc(24), p = Number(raw);
  try {
    if (!module.ccall('d2_session_hud_orbs', 'number', ['bigint'], [BigInt(p)])) return null;
    const v = new Int32Array(module.HEAPU8.buffer.slice(p, p + 24));
    return { life: v[0], lifeMax: v[1], mana: v[2], manaMax: v[3], stamina: v[4], staminaMax: v[5] };
  } finally { module._free(raw); }
}

/** The stamina bar's palette index per slot ([red, normal, blue]) over a 256-entry RGBA palette, as 278B0 latches them
 *  through D2Win #10190 (D2CMP+0x9D30, nearest entry); null without the engine. */
export function staminaColours(palette) {
  if (!mod?._d2_client_stamina_colours || !palette || palette.length < 1024) return null;
  const raw = mod._malloc(1024), p = Number(raw);
  try {
    mod.HEAPU8.set(palette.subarray(0, 1024), p);
    const v = call('d2_client_stamina_colours', ['bigint'], [BigInt(p)]) >>> 0;
    return [v & 255, (v >>> 8) & 255, (v >>> 16) & 255];
  } finally { mod._free(raw); }
}

// Quest log (D2Client+0x1CE00): the record's mode for a quest id over the player's 96-byte record and the 41 state bytes
// the quest-log state packet (0x52) filled; null without the engine or before those bytes are known. 0 completing, 1 done,
// 2 locked, 3 in progress. Pointers cross the wasm64 boundary as BigInt.
let questLogStates = null;
export function setQuestLogStates(bytes) { questLogStates = bytes && bytes.length >= 41 ? bytes : null; }
export function questRecordMode(words, questId) {
  // Without the state bytes the rule would show a started quest as locked: the caller falls back.
  if (!mod || !mod._d2_client_quest_mode || !questLogStates || !words || words.byteLength < 96) return null;
  const raw = mod._malloc(96 + 41), w = Number(raw);
  try {
    mod.HEAPU8.set(new Uint8Array(words.buffer, words.byteOffset, 96), w);
    mod.HEAPU8.set(questLogStates.subarray(0, 41), w + 96);
    return call('d2_client_quest_mode', ['bigint', 'bigint', 'number'], [BigInt(w), BigInt(w + 96), questId]);
  } finally { mod._free(raw); }
}

/** The 41 state bytes of the last quest-log state packet (0x52, 42 bytes) in a run of server packets; `sizeOf(at)` is the packet size rule. */
export function questLogStatesIn(stream, sizeOf) {
  let states = null;
  for (let at = 0; at < stream.length;) {
    const n = sizeOf(at);
    if (!(n > 0) || at + n > stream.length) break;
    if (stream[at] === 0x52 && n === 42) states = stream.slice(at + 1, at + 42);
    at += n;
  }
  return states;
}

// A game with no packet client (?localrealm=0) asks its own server as the log opens, as retail's client does: the one-byte
// 0x40 goes to D2Game's handler (1.13c D2Game+0xCBD30 -> QUESTS_StatusCallback), and the 0x52 it composes is read out of
// the packet stream that call filled, walked by the 1.13c client's packet size rule. Null when nothing answered.
// (A joined game, the local one included, sends the 0x40 over its connection and the client's 0x52 handler keeps the bytes.)
export function requestLocalQuestLogStates() {
  if (!mod?._d2_session_client_packet || !mod._d2_session_packet_stream || !mod._d2_realm_d2gs_packet_size) return null;
  const raw = mod._malloc(1), p = Number(raw);
  let answered;
  try {
    mod.HEAPU8[p] = 0x40;
    answered = call('d2_session_client_packet', ['bigint', 'number'], [BigInt(p), 1]) === 0;   // the handler's 0: answered
  } finally { mod._free(raw); }
  if (!answered) return null;
  const length = call('d2_session_packet_stream_length', [], []) >>> 0;
  const at = Number(call('d2_session_packet_stream', [], []));
  if (!at || !length) return null;
  const stream = mod.HEAPU8.slice(at, at + length);
  return questLogStatesIn(stream, (o) => call('d2_realm_d2gs_packet_size', ['bigint', 'number'], [BigInt(at + o), length - o]));
}

/** The run button (+0x28320) then the menu arrow (+0x26880) against the draw sink, pointer in 800x600 space: records in call order. */
export function hudButtonsDrawList(mx, my, running, runHot, arrowOpen, arrowHot, cellRun, cellMenu) {
  if (!mod) throw new Error('client rules are not bound to the engine');
  const raw = mod._malloc(8 * 4 * 4), p = Number(raw);
  try {
    const n = mod.ccall('d2_client_draw_hud_buttons', 'number', [...Array(8).fill('number'), 'bigint'],
      [mx | 0, my | 0, running ? 1 : 0, runHot ? 1 : 0, arrowOpen ? 1 : 0, arrowHot ? 1 : 0, cellRun, cellMenu, BigInt(p)]);
    return decodeDrawList(mod.HEAP32, p >> 2, n);
  } finally { mod._free(raw); }
}
