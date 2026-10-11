// D2Client's unit-sound resolver, 1.13c: packet 0x2C {unitType, unitGuid, soundEvent} -> a Sounds.txt row.
// Recovered from D2Client.dll: 0x2C is the size-8 entry of the client's
// server-packet table; its handler calls UnitSnd's event switch (6fad3f20), and the row played is the value
// in EBX at each call of the mixer's queue-add (6fb38a70). Results are Sounds.txt DATA ROW indices (the
// `Index` column; row 4 = cursor_button_click), not names: the caller maps a row through its own Sounds.txt.
// The engine decides the event; the client resolves it -- nothing here reads the engine.

// Per player class 0..6 (Amazon, Sorceress, Necromancer, Paladin, Barbarian, Druid, Assassin), the voice
// table D2Client keeps behind 6fba7694[class], keyed by byte offset. Constants of the DLL.
export const PLAYER_VOICE = Object.freeze([
  { 0x08: 2953, 0x0c: 2957, 0x10: 2955, 0x14: 2934, 0x18: 2936, 0x1c: 2959, 0x20: 2937, 0x24: 2961 },
  { 0x08: 3265, 0x0c: 3269, 0x10: 3267, 0x14: 3246, 0x18: 3248, 0x1c: 3271, 0x20: 3249, 0x24: 3273 },
  { 0x08: 3109, 0x0c: 3113, 0x10: 3111, 0x14: 3090, 0x18: 3092, 0x1c: 3115, 0x20: 3093, 0x24: 3117 },
  { 0x08: 3187, 0x0c: 3191, 0x10: 3189, 0x14: 3168, 0x18: 3170, 0x1c: 3193, 0x20: 3171, 0x24: 3195 },
  { 0x08: 3031, 0x0c: 3035, 0x10: 3033, 0x14: 3012, 0x18: 3014, 0x1c: 3037, 0x20: 3015, 0x24: 3039 },
  { 0x08: 3421, 0x0c: 3425, 0x10: 3423, 0x14: 3402, 0x18: 3404, 0x1c: 3427, 0x20: 3405, 0x24: 3429 },
  { 0x08: 3343, 0x0c: 3347, 0x10: 3345, 0x14: 3324, 0x18: 3326, 0x1c: 3349, 0x20: 3327, 0x24: 3351 },
]);

// Events any unit type answers (the jump table in 6fad3f20): a fixed row.
const ANY_UNIT = { 0x0a: 2673, 0x0d: 2634, 0x0e: 2635, 0x5a: 4379, 0x5d: 2553 };
// Events keyed on the unit's class: class 271 (the Act I rogue hireling) takes the first row, others the second.
const BY_CLASS_271 = { 0x54: [4615, 4625], 0x55: [4612, 4616], 0x56: [4613, 4619], 0x57: [4614, 4622] };
// Player (unitType 0) events that reach 6fad3200 when 6fad3f20 has no case of its own: fixed rows.
const PLAYER_FIXED = { 1: 235, 2: 7, 3: 10, 4: 11, 5: 12, 6: 13, 7: 2230, 8: 2231, 9: 9, 11: 228 };
// Player events whose row is a field of the class voice table.
const PLAYER_FIELD = { 0x13: 0x08, 0x14: 0x18, 0x15: 0x0c, 0x16: 0x10, 0x17: 0x14, 0x18: 0x1c };

/**
 * Resolve one server sound event to a Sounds.txt row.
 * @param {{unitType:number, classId:number, soundEvent:number, isLocalPlayer?:boolean, monsterTaunt?:number}} e
 *   `monsterTaunt` is the unit's MonSounds `Taunt` row (D2MonSoundsTxt+0x74; MonStats.MonSound/UMonSound, or the
 *   SuperUniques.MonSound record, picks the MonSounds row) -- the only event that reads the monster tables.
 * @returns {number|null} the row, or null when retail plays nothing (or the row depends on state not passed).
 */
export function resolveUnitSound({ unitType, classId, soundEvent, isLocalPlayer = false, monsterTaunt = 0 }) {
  const ev = soundEvent & 0xffff;
  if (ev in ANY_UNIT) return ANY_UNIT[ev];
  if (ev in BY_CLASS_271) return BY_CLASS_271[ev][classId === 271 ? 0 : 1];
  if (ev === 0x0f) return classId === 271 ? 4290 : null;
  if (ev === 0x10) return unitType === 1 && monsterTaunt > 0 ? monsterTaunt : null;
  if (ev === 0x5b) return isLocalPlayer ? 8 : null;
  // 0x0c (a stat-gated skill sound), 0x11 (monster taunt roll), 0x12 (computed by 6fb4cce0) depend on live
  // unit state; 0x5c only arms a timer. None yields a row from (type, class, event) alone.
  if (ev === 0x0c || ev === 0x11 || ev === 0x12 || ev === 0x5c) return null;
  if (unitType !== 0 || classId < 0 || classId > 6) return null; // 6fad3f20 forwards only players to 6fad3200
  const voice = PLAYER_VOICE[classId];
  if (ev in PLAYER_FIXED) return PLAYER_FIXED[ev];
  if (ev in PLAYER_FIELD) return voice[PLAYER_FIELD[ev]];
  if (ev >= 0x19 && ev <= 0x20) return voice[0x20] + ev * 2 - 0x32;
  if (ev >= 0x21 && ev <= 0x53) return voice[0x24] + ev - 0x21;
  return null;
}

/** Map a row to its Sounds.txt line (name, file) given the table's text. */
export function soundRow(soundsTxt, row) {
  const line = soundsTxt.split(/\r?\n/)[row + 1];
  if (!line) return null;
  const c = line.split('\t');
  return { row, handle: c[0], file: c[2], volume: c[3] === undefined ? 255 : Number(c[3]), stereo: Number(c[18] ?? 0) === 1, tracking: Number(c[19] ?? 0) === 1, music: Number(c[21] ?? 0) === 1 };
}

/** Resolve a retail Sounds.txt handle (used by Speech.txt) to its indexed row and WAV path. */
export function soundRowByHandle(soundsTxt, handle) {
  if (!handle) return null;
  const lines = soundsTxt.split(/\r?\n/);
  for (let row = 1; row < lines.length; row++) {
    const c = lines[row].split('\t');
    if (c[0]?.toLowerCase() === String(handle).toLowerCase())
      return { row: row - 1, handle: c[0], file: c[2], volume: c[3] === undefined ? 255 : Number(c[3]), stereo: Number(c[18] ?? 0) === 1, tracking: Number(c[19] ?? 0) === 1, music: Number(c[21] ?? 0) === 1 };
  }
  return null;
}

/** Resolve a Sounds.txt source by its relative FileName, case-insensitively. */
export function soundRowByFile(soundsTxt, file) {
  if (!file) return null;
  const wanted = String(file).replaceAll('/', '\\').toLowerCase();
  const lines = soundsTxt.split(/\r?\n/);
  const matches = [];
  for (let row = 1; row < lines.length; row++) {
    const c = lines[row].split('\t');
    if (c[2]?.replaceAll('/', '\\').toLowerCase() === wanted)
      matches.push({ row: row - 1, handle: c[0], file: c[2], volume: c[3] === undefined ? 255 : Number(c[3]), stereo: Number(c[18] ?? 0) === 1, tracking: Number(c[19] ?? 0) === 1, music: Number(c[21] ?? 0) === 1 });
  }
  if (matches.length > 1) throw new Error(`expected at most one Sounds.txt row for ${file}, found ${matches.length}`);
  return matches[0] ?? null;
}
