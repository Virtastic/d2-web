// The controller's skill buttons: which skill sits on each button, which set is live, and the wheel's
// stick geometry. Pure data and arithmetic; no DOM, no engine. The pad (input-controller.mjs) and the
// touch skill pad (touch-skill-pad.mjs) both read it.
//
// A sheet is per character and lives in the browser (localStorage), never in the save. The save's own
// left/right skills and hotkeys only seed it, once, the first time a character plays with skill buttons.

export const SLOTS = 6;            // A B X Y LB RB, each with a second set behind LT
export const EMPTY = -1;           // the button casts nothing
export const UNSET = -2;           // the main skill was never saved: seed it
export const INTERACT = -3;        // the button talks, uses or picks up, as R3 does
export const SHOW_ITEMS = -4;      // the button holds Show Items
export const INVENTORY = -5;
export const CHARACTER = -6;
export const SKILL_TREE = -7;
export const AUTOMAP = -8;
export const QUESTS = -9;
export const ACTIONS = [EMPTY, INTERACT, SHOW_ITEMS, INVENTORY, CHARACTER, SKILL_TREE, AUTOMAP, QUESTS];
export const WHEEL_SHOWN = 8;      // icons on the ring at once; the rest are further pages

// The panel shortcut a sheet id names, as the retail function that opens it; null when it is no panel.
const PANEL_FN = { [CHARACTER]: 'character', [INVENTORY]: 'inventory', [SKILL_TREE]: 'skillTree', [AUTOMAP]: 'automap', [QUESTS]: 'quest' };
export const panelFn = (id) => PANEL_FN[id] ?? null;
export const bindsAction = (id) => id === INTERACT || id === SHOW_ITEMS || panelFn(id) !== null;

export function actionLabel(id, full) {
  if (id === EMPTY) return 'Clear';
  if (id === INTERACT) return full ? 'Interact' : 'Use';
  if (id === SHOW_ITEMS) return full ? 'Show Items' : 'Items';
  if (id === INVENTORY) return full ? 'Inventory' : 'Inv';
  if (id === CHARACTER) return full ? 'Character' : 'Char';
  if (id === SKILL_TREE) return full ? 'Skill Tree' : 'Skills';
  if (id === AUTOMAP) return 'Map';
  if (id === QUESTS) return 'Quests';
  return '';
}

// Town Portal through a carried tome, and the entries the wheel never offers (inventory Identify, the
// generic Kick). Names compare without ASCII case.
export const portalName = (name) => name === 'Book of Townportal' || name === 'Town Portal' || name === 'Scroll of Townportal';
export const assignableName = (name) => !['kick', 'identify', 'scroll of identify', 'book of identify'].includes(String(name ?? '').toLowerCase());

export function blankSheet() {
  return { main: UNSET, altMain: UNSET, slots: Array(SLOTS * 2).fill(EMPTY) };
}
export const slotOf = (sheet, alternate, index) => sheet.slots[index + (alternate ? SLOTS : 0)];
export function setSlot(sheet, alternate, index, id) { sheet.slots[index + (alternate ? SLOTS : 0)] = id; }
// RT's skill, or LT+RT's when `alternate`. An alternate never saved is the main one.
export const triggerOf = (sheet, alternate) => (alternate && sheet.altMain !== UNSET ? sheet.altMain : sheet.main);

// First activation only. RT gets the left hand, RB the right hand, and the saved hotkeys fill A B X Y LB,
// then the second set A..RB, in hotkey order, skipping repeats. `enabled` is seven flags: the six slot
// buttons, then RT. Callers pass only skills the character can use and no item-charged entries.
export function initialSheet(left, right, hotkeys, enabled = Array(7).fill(true)) {
  const sheet = blankSheet();
  sheet.main = enabled[6] ? left : EMPTY;
  setSlot(sheet, false, 5, enabled[5] ? right : EMPTY);
  const destinations = [0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11];
  let next = 0;
  for (const skill of hotkeys) {
    if (skill < 0 || skill === sheet.main || sheet.slots.includes(skill)) continue;
    while (next < destinations.length && !enabled[destinations[next] % SLOTS]) next++;
    if (next === destinations.length) break;
    sheet.slots[destinations[next++]] = skill;
  }
  return sheet;
}

// Screen axes: +x right, +y down. Wedge 0 is centred half a step clockwise from straight up.
const TAU = 6.2831853, HALF_PI = 1.5707963;
export const wedgeCenter = (index, count) => ((index + 0.5) / count) * TAU - HALF_PI;
// IEEE remainder: the result is in [-y/2, y/2].
const remainder = (x, y) => x - y * Math.round(x / y);

// The highlighted wedge for a stick at (x, y): the current one is kept near the centre (0.30) and for a
// small margin (0.08 rad) past its edges, so a resting thumb does not flicker between two wedges.
export function stableWedge(x, y, count, current) {
  if (count <= 0 || Math.hypot(x, y) <= 0.30) return current;
  const angle = Math.atan2(y, x);
  if (current >= 0 && current < count) {
    const delta = remainder(angle - wedgeCenter(current, count), TAU);
    if (Math.abs(delta) <= TAU / count / 2 + 0.08) return current;
  }
  const turn = (((angle + HALF_PI + TAU) % TAU) + TAU) % TAU;
  return Math.min(count - 1, Math.floor((turn / TAU) * count));
}

// The d-pad's page step: the same place on the next (or previous) page, wrapping, clamped on a short last page.
export function nextPage(highlight, count, step) {
  if (count <= 0) return 0;
  const pages = Math.ceil(count / WHEEL_SHOWN);
  const page = (Math.floor(highlight / WHEEL_SHOWN) + (step > 0 ? 1 : pages - 1)) % pages;
  return Math.min(count - 1, page * WHEEL_SHOWN + (highlight % WHEEL_SHOWN));
}

// The set latched when a button went down; holding or letting go of LT afterwards does not move that
// hold to the other set. -1 while the button is up.
export function makeHoldSet() {
  const latched = Array(SLOTS).fill(-1);
  return (index, press, held, alternate) => {
    if (index < 0 || index >= SLOTS) return -1;
    if (press) latched[index] = alternate ? 1 : 0;
    else if (!held) latched[index] = -1;
    return latched[index];
  };
}
export function makeTriggerHold() {
  let latched = -1;
  return (press, held, alternate) => {
    if (press) latched = alternate ? 1 : 0;
    else if (!held) latched = -1;
    return latched;
  };
}

// What the right hand becomes when an aura button is pressed. `active` is the aura running (0 none).
// The same aura again goes back to the non-aura skill held before; another aura switches.
export function auraChoice(active, pressed, current, currentIsAura, remembered) {
  const memory = currentIsAura ? remembered : current;
  if (active === pressed) return { next: memory, remembered: memory };
  return { next: pressed, remembered: memory };
}

// Start's panel list, in order; each entry is the retail function that opens it (Menu is Escape).
export const PANELS = [
  ['Character', 'character'], ['Inventory', 'inventory'], ['Skill Tree', 'skillTree'], ['Automap', 'automap'],
  ['Quests', 'quest'], ['Party', 'party'], ['Hireling', 'hireling'], ['Help', 'help'], ['Menu', 'escape'],
];

// One line each: "initialized 1", "main <id>" and "amain <id>" when saved, "s0".."s5" the first set,
// "a0".."a5" the second. A missing main stays unset; a missing amain means "use main"; -1 is a clear.
export function writeSheet(sheet) {
  let out = 'initialized 1\n';
  if (sheet.main !== UNSET) out += `main ${sheet.main}\n`;
  if (sheet.altMain !== UNSET) out += `amain ${sheet.altMain}\n`;
  for (let i = 0; i < SLOTS; i++) out += `s${i} ${slotOf(sheet, false, i)}\na${i} ${slotOf(sheet, true, i)}\n`;
  return out;
}
export function readSheet(text) {
  const sheet = blankSheet();
  for (const line of String(text).split('\n')) {
    const m = /^(main|amain|s[0-5]|a[0-5]) (-?\d+)$/.exec(line);
    if (!m) continue;
    const v = Number(m[2]);
    if (m[1] === 'main') sheet.main = v;
    else if (m[1] === 'amain') sheet.altMain = v;
    else setSlot(sheet, m[1][0] === 'a', Number(m[1][1]), v);
  }
  return sheet;
}

// A per-character store over localStorage (or any object with getItem/setItem). `prefix` keeps the pad's
// and the touch pad's sheets apart. A character with no stored sheet is seeded once by `seed()`.
export function makeSheetStore(storage, prefix) {
  const cache = new Map();
  const keyOf = (name) => `${prefix}.${name}`;
  return {
    has(name) { return cache.has(name) || storage?.getItem?.(keyOf(name)) != null; },
    get(name) {
      if (!name) return null;
      if (!cache.has(name)) {
        const text = storage?.getItem?.(keyOf(name));
        if (text == null) return null;
        cache.set(name, readSheet(text));
      }
      return cache.get(name);
    },
    ensure(name, seed) {
      if (!name) return null;
      const have = this.get(name);
      if (have) return have;
      const sheet = seed();
      if (!sheet) return null;
      this.save(name, sheet);
      return sheet;
    },
    save(name, sheet) {
      cache.set(name, sheet);
      try { storage?.setItem?.(keyOf(name), writeSheet(sheet)); } catch { /* storage full or denied: the sheet still plays this session */ }
    },
  };
}
