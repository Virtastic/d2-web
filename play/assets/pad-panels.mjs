// Pad navigation for the item/grid panels (inventory, stash, Horadric Cube, vendor, belt). Pure: the
// controller hands in the shell's own canvas points and snapshot, this returns the point list the d-pad
// snaps over. Nothing here decides a game rule; a click on a point is the click a mouse would make there.
import { BTN } from './plugy-stash-ui.mjs';

const PAGE_OF = { invg: 0, cube: 3, stash: 4 };
const STASH_BUTTONS = ['prev', 'next', 'shared', 'prevIndex', 'nextIndex'], GOLD_BUTTONS = ['putGold', 'takeGold'];
const CEL = 32;
const CELL = /^[a-z]+:(\d+),(\d+)$/;

export const panelOrigin = (width, height) => {
  const scale = Math.min(1, width / 800, height / 600);
  return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
};
const mean = (ps) => ({ x: Math.round(ps.reduce((a, p) => a + p.x, 0) / ps.length), y: Math.round(ps.reduce((a, p) => a + p.y, 0) / ps.length) });
const footprint = (it) => { const out = []; for (let y = it.gridY; y < it.gridY + (it.invHeight || 1); y++) for (let x = it.gridX; x < it.gridX + (it.invWidth || 1); x++) out.push(`${x},${y}`); return out; };

// Cells of one grid ("x,y" keys under `prefix`) become navigable points.
//   * nothing held: every cell an item covers collapses to ONE point at the middle of its footprint
//     (devilutionX GridMove skips cells of the same item); empty cells stay one each;
//   * item held: the item is centred on the pointer and clamped inside the grid (cellAt), so cells map
//     many-to-one onto anchors; one point per distinct anchor, at the middle of the w x h footprint.
export function gridPoints(cells, prefix, items, held) {
  const at = new Map();
  for (const c of cells) { const m = CELL.exec(c.key); if (m) at.set(`${m[1]},${m[2]}`, c); }
  if (!at.size) return cells;
  const keys = [...at.keys()].map((k) => k.split(',').map(Number));
  const cols = Math.max(...keys.map((k) => k[0])) + 1, rows = Math.max(...keys.map((k) => k[1])) + 1;
  if (held) {
    const w = held.invWidth || 1, h = held.invHeight || 1, out = [];
    for (let ay = 0; ay + h <= rows; ay++) for (let ax = 0; ax + w <= cols; ax++) {
      const a = at.get(`${ax},${ay}`), b = at.get(`${ax + w - 1},${ay + h - 1}`);
      if (a && b) out.push({ key: `${prefix}:a${ax},${ay}`, ...mean([a, b]) });
    }
    return out;
  }
  const page = PAGE_OF[prefix], covered = new Map(), out = [], done = new Set();
  for (const it of items || []) if (it.mode === 0 && it.invPage === page && it.gridX >= 0 && it.gridY >= 0) for (const k of footprint(it)) covered.set(k, it);
  for (const c of cells) {
    const m = CELL.exec(c.key), it = m && covered.get(`${m[1]},${m[2]}`);
    if (!it) { out.push(c); continue; }
    if (done.has(it.id)) continue;
    done.add(it.id);
    out.push({ key: `${prefix}:i${it.id}`, ...mean(footprint(it).map((k) => at.get(k)).filter(Boolean)) });
  }
  return out;
}

// Canvas points for the cube grid and Transmute, and the PlugY stash buttons, from the shell's touchPanelLayout().
export function layoutPoints(layout, width, height) {
  const cube = [], buttons = [];
  if (!layout) return { cube, buttons };
  const o = panelOrigin(width, height), px = (x, y) => ({ x: Math.round(o.x + x * o.scale), y: Math.round(o.y + y * o.scale) });
  const g = layout.cube?.geometry;
  if (g) {
    for (let y = 0; y < g.rows; y++) for (let x = 0; x < g.columns; x++) cube.push({ key: `cube:${x},${y}`, ...px(g.gridLeft + (x + 0.5) * g.boxW, g.gridTop + (y + 0.5) * g.boxH) });
    cube.push({ key: 'cube:transmute', ...px(g.gridLeft + g.columns * g.boxW / 2, g.gridTop + g.rows * g.boxH + 22) });
  }
  if (layout.stash?.plugy) for (const n of [...STASH_BUTTONS, ...(layout.stash.gold ? GOLD_BUTTONS : [])])
    buttons.push({ key: `stashbtn:${n}`, ...px(BTN[n].x + CEL / 2, BTN[n].y + CEL / 2) });
  return { cube, buttons };
}

// The whole point list. `base` is panelPoints(targets); ctx = { items, held, layout, width, height }.
export function enhancePanelPoints(base, targets, ctx) {
  if (!base || targets?.repairConfirm) return base;
  const { items, held, layout, width, height } = ctx;
  const { cube, buttons } = layoutPoints(layout, width, height);
  const group = (prefix) => base.filter((p) => p.key.startsWith(`${prefix}:`));
  const other = base.filter((p) => !/^(invg|stash|cube):/.test(p.key));
  const grids = [];
  for (const prefix of ['invg', 'stash']) grids.push(...gridPoints(group(prefix), prefix, items, held));
  grids.push(...gridPoints(cube.filter((p) => p.key !== 'cube:transmute'), 'cube', items, held), ...cube.filter((p) => p.key === 'cube:transmute'));
  const belt = Object.entries(targets?.belt || {}).filter(([, p]) => Array.isArray(p)).map(([k, p]) => ({ key: `belt:${k}`, x: p[0], y: p[1] }));
  const out = [...other, ...grids, ...buttons, ...belt];
  // A held item is thrown on the ground by clicking the world; the strip between the left and right
  // panels (game x 320..400) is never panel, so a click there does that.
  if (held && out.length) { const o = panelOrigin(width, height); out.push({ key: 'drop', x: Math.round(o.x + 360 * o.scale), y: Math.round(o.y + 300 * o.scale) }); }
  return out;
}

// Held-direction repeat: the first press moves at once; a hold repeats every `every` ms after `first` ms.
export function makeRepeater(first = 300, every = 150) {
  let since = null, last = 0;
  // `hold` overrides the first-repeat delay per call: menus (select list, waypoint...) wait 400 ms, item grids 300.
  return (pressed, now, hold = first) => {
    if (!pressed) { since = null; return false; }
    if (since === null) { since = now; last = now; return true; }
    if (now - since >= hold && now - last >= every) { last = now; return true; }
    return false;
  };
}

export const nearestIndex = (points, x, y) => points.reduce((best, p, i) => (Math.hypot(p.x - x, p.y - y) < Math.hypot(points[best].x - x, points[best].y - y) ? i : best), 0);

// ---- Inventory shortcuts for a pad. Each returns the canvas clicks to make (or does them through the
// ---- callbacks it is handed); every click is one a mouse would make, so the shell's own handlers decide.

const DEST_ORDER = ['stash', 'cube'];
const idOf = (key) => { const m = /^[a-z]+:i(\d+)$/.exec(key || ''); return m ? Number(m[1]) : 0; };
const prefixOf = (key) => (key || '').split(':')[0];

// First free spot, scanning left to right then down, for a w x h item on `page` of a grid whose cells are
// `cells` (points keyed "prefix:x,y"). Returns the held-item anchor point (gridPoints' own), or null.
function firstFree(cells, prefix, items, item) {
  const used = new Set();
  for (const it of items || []) if (it.mode === 0 && it.invPage === PAGE_OF[prefix] && it.id !== item.id && it.gridX >= 0 && it.gridY >= 0) for (const k of footprint(it)) used.add(k);
  const anchors = gridPoints(cells, prefix, items, item), w = item.invWidth || 1, h = item.invHeight || 1;
  const order = anchors.flatMap((p) => { const m = /:a(\d+),(\d+)$/.exec(p.key); return m ? [{ p, x: +m[1], y: +m[2] }] : []; }).sort((a, b) => a.y - b.y || a.x - b.x);
  for (const a of order) {
    let free = true;
    for (let y = a.y; y < a.y + h && free; y++) for (let x = a.x; x < a.x + w; x++) if (used.has(`${x},${y}`)) { free = false; break; }
    if (free) return a.p;
  }
  return null;
}

// Quick move of the item the highlight is on. 1.13c has no Ctrl-click transfer between panels (its only
// Ctrl-click is D2Client+0x96AA0's direct sale, with a vendor open), so a transfer is pick-up then put-down
// at the first free spot of the other panel. Returns [{x,y,ctrl?}] clicks, or null when there is nothing to do.
//   * vendor open, item in the pack: one Ctrl-click, the engine's direct sale;
//   * pack item with the stash (else the cube) open: lift it, put it down in that grid;
//   * stash or cube item: lift it, put it down in the pack.
export function quickMovePlan(base, targets, ctx, cur, held) {
  if (!base || !cur || held) return null;
  const id = idOf(cur.key), from = prefixOf(cur.key), item = id && (ctx.items || []).find((i) => i.id === id);
  if (!item || !PAGE_OF.hasOwnProperty(from)) return null;
  if (from === 'invg' && targets?.shop) return [{ x: cur.x, y: cur.y, ctrl: true }];
  const { cube } = layoutPoints(ctx.layout, ctx.width, ctx.height);
  const cells = { invg: base.filter((p) => p.key.startsWith('invg:')), stash: base.filter((p) => p.key.startsWith('stash:')), cube: cube.filter((p) => p.key !== 'cube:transmute') };
  const to = from === 'invg' ? DEST_ORDER.find((d) => cells[d].length) : 'invg';
  if (!to || !cells[to].length) return null;
  const spot = firstFree(cells[to], to, ctx.items, item);
  return spot ? [{ x: cur.x, y: cur.y }, { x: spot.x, y: spot.y }] : null;
}

// D2R's R3-in-inventory belt fill: a Shift-click on each potion in the pack (the shell sends it to the belt
// as retail's Shift-click does; a full belt refuses the rest). Nothing while a vendor is open or an item is held.
// The snapshot carries an item code as its little-endian four-byte number ("hp1 " = 0x20317068); tests may pass the string.
const codeStr = (c) => typeof c === 'number' ? String.fromCharCode(c & 255, (c >>> 8) & 255, (c >>> 16) & 255, (c >>> 24) & 255).trim() : (c || '');
const POTION = /^(hp[1-5]|mp[1-5]|rvs|rvl|vps|yps|wms|gpl|gpm|gps|opl|opm|ops)$/;
export function beltFillPlan(panel, targets, items, held) {
  if (!panel || held || targets?.shop) return null;
  const potion = new Set((items || []).filter((i) => i.mode === 0 && i.invPage === 0 && POTION.test(codeStr(i.code))).map((i) => i.id));
  const out = panel.filter((p) => p.key.startsWith('invg:i') && potion.has(idOf(p.key))).map((p) => ({ x: p.x, y: p.y, shift: true }));
  return out.length ? out : null;
}

// Hold A on an item: the tracker reports the key the press began on once the button has been down `ms`.
export function makeHold(ms = 400) {
  let since = null, key = null, fired = false;
  return (pressed, now, k) => {
    if (!pressed) { since = null; fired = false; return null; }
    if (since === null) { since = now; key = k; }
    if (!fired && now - since >= ms) { fired = true; return key; }
    return null;
  };
}

// Equip or swap the item on the cursor: click each worn-slot point until the cursor no longer holds it
// (the engine refuses a slot the item does not fit, so the first accepting slot ends the loop). Empty slots
// are tried before worn ones, so a ring takes the free finger and a swap happens only when none is free.
// `clickAt(x,y)` makes the shell's own click; `cursorId()` reads the cursor item. Returns the slot key or null.
export const EQUIP_ORDER = [3, 1, 2, 10, 9, 8, 6, 7, 4, 5];
export function equipHeld(id, panel, items, clickAt, cursorId) {
  const worn = new Set((items || []).filter((i) => i.mode === 1).map((i) => i.bodyLoc));
  const slots = EQUIP_ORDER.map((n) => ({ n, p: panel.find((p) => p.key === `invs:${n}`) })).filter((s) => s.p);
  for (const s of [...slots.filter((s) => !worn.has(s.n)), ...slots.filter((s) => worn.has(s.n))]) {
    clickAt(s.p.x, s.p.y);
    if (cursorId() !== id) return s.p.key;
  }
  return null;
}
