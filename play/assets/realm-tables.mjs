// The data tables a joined Battle.net game reads for the items, objects and panels the D2GS server
// describes by number: Weapons/Armor/Misc.txt (class id -> art and size), Objects.txt (class id ->
// token), Inventory.txt and Belts.txt (panel geometry). Each reader takes `read(path)` -> bytes, the
// MPQ asset reader the shell already holds, and parses the tab-separated file once on first use.

function rows(bytes) {
  const lines = new TextDecoder('latin1').decode(bytes).replace(/\r/g, '').split('\n').filter((l) => l.length);
  const header = lines.shift().split('\t').map((name) => name.trim().toLowerCase());
  return lines.map((line) => {
    const values = line.split('\t'), row = {};
    for (let i = 0; i < header.length; i++) row[header[i]] = (values[i] ?? '').trim();
    return row;
  });
}
const num = (v, fallback = 0) => { const n = Number(v); return Number.isFinite(n) && v !== '' ? n : fallback; };
const fourcc = (code) => { const s = String(code).padEnd(4, ' ');
  return (s.charCodeAt(0) | (s.charCodeAt(1) << 8) | (s.charCodeAt(2) << 16) | (s.charCodeAt(3) << 24)) >>> 0; };

/**
 * Items.txt as the engine numbers it: every Weapons.txt row, then Armor.txt, then Misc.txt, an item's class
 * id being its position in that run (the id the D2GS item bitstream's four-character code resolves to).
 */
export function createItemTable(read) {
  let byClass = null, byCode = null;
  function load() {
    if (byClass) return;
    byClass = []; byCode = new Map();
    for (const file of ['weapons', 'armor', 'misc']) {
      for (const row of rows(read(`data\\global\\excel\\${file}.txt`))) {
        if (!row.code) continue;   // the "Expansion" separator row has no code
        const entry = { classId: byClass.length, kind: file, code: row.code, name: row.name, type: row.type,
          flippyFile: row.flippyfile ?? '', invFile: row.invfile ?? '',
          invWidth: num(row.invwidth, 1) || 1, invHeight: num(row.invheight, 1) || 1,
          wclass: row.wclass ?? '', twoHandedWclass: row['2handedwclass'] ?? '', twoHanded: row['2handed'] === '1',
          belt: file === 'armor' ? num(row.belt, -1) : -1 };
        byClass.push(entry); byCode.set(fourcc(row.code), entry);
      }
    }
  }
  return {
    byCode(code) { load(); return byCode.get(code >>> 0) ?? null; },
    byClass(classId) { load(); return byClass[classId] ?? null; },
  };
}

/** Objects.txt: class id (its Id column) -> the two-letter art token. */
/** Levels.txt: the Rain byte of a level (the one the single-player engine's weather reads), by level id. */
export function createLevelTable(read) {
  let rain = null;
  return {
    rain(levelId) {
      if (!rain) {
        rain = new Map();
        for (const row of rows(read('data\\global\\excel\\levels.txt'))) if (row.id !== '') rain.set(num(row.id, -1), num(row.rain));
      }
      return rain.has(levelId) ? rain.get(levelId) : -1;
    },
  };
}

export function createObjectTable(read) {
  let byClass = null;
  function load() {
    if (byClass) return byClass;
    byClass = new Map();
    for (const row of rows(read('data\\global\\excel\\objects.txt'))) {
      const id = num(row.id, -1);
      if (id >= 0 && row.token) byClass.set(id, { classId: id, token: row.token, name: row.name });
    }
    return byClass;
  }
  return { byClass(classId) { return load().get(classId) ?? null; } };
}

const PLAYER_CLASS_NAME = ['amazon', 'sorceress', 'necromancer', 'paladin', 'barbarian', 'druid', 'assassin'];
const INVENTORY_COLUMNS = ['invleft', 'invright', 'invtop', 'invbottom', 'gridx', 'gridy', 'gridleft', 'gridright',
  'gridtop', 'gridbottom', 'gridboxwidth', 'gridboxheight'];
const COMPONENT_PREFIX = ['rarm', 'torso', 'larm', 'head', 'neck', 'rhand', 'lhand', 'belt', 'feet', 'gloves'];

/**
 * Inventory.txt: the twelve grid numbers of a named row (what the single-player engine hands the panel
 * readers), and for the player's own row also the sixty equipment-slot numbers (ten slots of
 * left/right/top/bottom/width/height). `null` for a row the file does not have.
 */
export function createInventoryTable(read) {
  let table = null;
  const load = () => (table ??= new Map(rows(read('data\\global\\excel\\inventory.txt')).map((r) => [r.class.toLowerCase(), r])));
  return {
    grid(name) {
      const row = load().get(String(name).toLowerCase());
      return row ? INVENTORY_COLUMNS.map((c) => num(row[c])) : null;
    },
    components(name) {
      const row = load().get(String(name).toLowerCase());
      if (!row) return null;
      return COMPONENT_PREFIX.flatMap((p) => ['left', 'right', 'top', 'bottom', 'width', 'height'].map((s) => num(row[p + s])));
    },
    // The row the panel for a class reads: the expansion's own (`Amazon2`...) or the classic one.
    playerRow(classId, expansion = true) {
      const base = PLAYER_CLASS_NAME[classId];
      if (!base) return null;
      return expansion && load().has(base + '2') ? base + '2' : base;
    },
  };
}

/** Belts.txt: record index -> the box rectangles (what d2_session_belt_boxes returns). Row 2 is the beltless record. */
export function createBeltTable(read) {
  let table = null;
  const load = () => (table ??= rows(read('data\\global\\excel\\belts.txt')));
  return {
    boxes(record) {
      const row = load()[record];
      if (!row) return null;
      const n = num(row.numboxes), boxes = [];
      for (let i = 1; i <= n; i++) boxes.push({ left: num(row[`box${i}left`]), right: num(row[`box${i}right`]),
        top: num(row[`box${i}top`]), bottom: num(row[`box${i}bottom`]) });
      return boxes;
    },
  };
}
