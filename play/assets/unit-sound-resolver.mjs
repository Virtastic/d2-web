// D2Client's unit-sound resolver, 1.13c: packet 0x2C {unitType, unitGuid, soundEvent} -> a Sounds.txt row.
// The decision is the wasm client's (D2Client+0x23F20 / +0x23200, exported as d2_client_unit_sound_event_row);
// results are Sounds.txt DATA ROW indices (the `Index` column; row 4 = cursor_button_click), not names: the caller
// maps a row through its own Sounds.txt. The engine decides the event; the client resolves it.
// Event 1 (item to cursor, row 235) is NOT resolved here: retail submits it once with no unit (a retail recording Q 240),
// played by event-sounds.mjs when the item leaves the ground.

/**
 * Resolve one server sound event to a Sounds.txt row.
 * @param {{unitType:number, classId:number, soundEvent:number, isLocalPlayer?:boolean, monsterTaunt?:number, skillStartSoundRow?:number}} e
 *   `monsterTaunt` is the unit's MonSounds `Taunt` row; `skillStartSoundRow` is the SkillsTxt `StSound` selected
 *   through the unit's native 0x44 stat list for event 0x0c.
 * @returns {number|null} the row, or null when retail plays nothing (or the engine is not loaded).
 */
export function resolveUnitSound({ unitType, classId, soundEvent, isLocalPlayer = false, monsterTaunt = 0, skillStartSoundRow = 0 }) {
  const wasm = globalThis.Module?._d2_client_unit_sound_event_row;
  if (typeof wasm !== 'function') return null;
  const r = Number(wasm(unitType | 0, classId | 0, soundEvent | 0, isLocalPlayer ? 1 : 0, monsterTaunt | 0, skillStartSoundRow | 0)) | 0;
  return r < 0 ? null : r;
}

// PERF: Sounds.txt is about 4,700 lines and was split again on every played row (about 4% of a Baal wave
// fight's frame). The split is kept for the last table text seen; every caller still builds a fresh row object.
let linesText = null, linesMemo = null;
const tableLines = (soundsTxt) => (soundsTxt === linesText ? linesMemo : (linesMemo = soundsTxt.split(/\r?\n/), linesText = soundsTxt, linesMemo));

// The loader (D2Client 0x5DFD0) keeps every column in its 0x8E-byte record; the byte columns read as integers.
const num = (v, d = 0) => { const n = Number(v); return v === undefined || v === '' || !Number.isFinite(n) ? d : n; };
function parseRow(c, row, groupBase) {
  return {
    row, handle: c[0], file: c[2], volume: c[3] === undefined ? 255 : num(c[3]),
    // Group Size 0 is stored as 1 (one variant).
    groupSize: Math.max(1, num(c[4])), groupBase, loop: num(c[5]) === 1, fadeIn: num(c[6]), fadeOut: num(c[7]),
    deferInst: num(c[8]) === 1, stopInst: num(c[9]) === 1, duration: num(c[10]), compound: num(c[11]),
    reverb: num(c[12]) === 1, falloff: num(c[13]), cache: num(c[14]) === 1, asyncOnly: num(c[15]) === 1,
    priority: num(c[16]), stream: num(c[17]) === 1,
    stereo: num(c[18]) === 1, tracking: num(c[19]) === 1, solo: num(c[20]) === 1, music: num(c[21]) === 1,
    blocks: [num(c[22]), num(c[23]), num(c[24])],
  };
}

// The loader's group base (record +0x60): a row inside the span [g, g+size) opened by the last row with a
// nonzero Group Size keeps g; every other row is its own base. The walk starts at data row 1.
let basesText = null, basesMemo = null;
function groupBase(soundsTxt, row) {
  if (soundsTxt !== basesText) {
    const lines = tableLines(soundsTxt), bases = new Int32Array(Math.max(0, lines.length - 1));
    let g = 0, size = 0;
    for (let r = 1; r < bases.length; r++) {
      bases[r] = g !== 0 && r >= g && r < g + size ? g : r;
      const s = num(lines[r + 1]?.split('\t')[4]);
      if (s !== 0) { size = s; g = r; }
    }
    basesText = soundsTxt; basesMemo = bases;
  }
  return row > 0 && row < basesMemo.length ? basesMemo[row] : row;
}

/** Map a row to its Sounds.txt line (every column, plus the loader's group base) given the table's text. */
export function soundRow(soundsTxt, row) {
  const line = tableLines(soundsTxt)[row + 1];
  if (!line) return null;
  return parseRow(line.split('\t'), row, groupBase(soundsTxt, row));
}

/** Resolve a retail Sounds.txt handle (used by Speech.txt) to its indexed row and WAV path. */
export function soundRowByHandle(soundsTxt, handle) {
  if (!handle) return null;
  const lines = tableLines(soundsTxt);
  for (let row = 1; row < lines.length; row++) {
    const c = lines[row].split('\t');
    if (c[0]?.toLowerCase() === String(handle).toLowerCase())
      return parseRow(c, row - 1, groupBase(soundsTxt, row - 1));
  }
  return null;
}

/** Resolve a Sounds.txt source by its relative FileName, case-insensitively. */
export function soundRowByFile(soundsTxt, file) {
  if (!file) return null;
  const wanted = String(file).replaceAll('/', '\\').toLowerCase();
  const lines = tableLines(soundsTxt);
  const matches = [];
  for (let row = 1; row < lines.length; row++) {
    const c = lines[row].split('\t');
    if (c[2]?.replaceAll('/', '\\').toLowerCase() === wanted)
      matches.push(parseRow(c, row - 1, groupBase(soundsTxt, row - 1)));
  }
  if (matches.length > 1) throw new Error(`expected at most one Sounds.txt row for ${file}, found ${matches.length}`);
  return matches[0] ?? null;
}
