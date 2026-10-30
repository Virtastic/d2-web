// The coloured box retail draws under (and, for a held item, over) an item in a grid or a body slot.
// One rule set and one painter for every panel: inventory, stash, cube, vendor, and the body slots.
//
// What retail draws (D2Client 1.13c, recovered from the client; the private spec holds the details):
//   * Colour is an index into a five-entry table built once per act (item-box-colour.mjs boxIndices):
//     0 red, 1 green, 2 blue, 3 yellow, 4 brown. Every box is D2Gfx FillArea in draw mode 0, which the
//     page renders as the act PL2's third AlphaBlend table, pl2[13568 + 2*65536 + under*256 + colour].
//   * A grid item gets one box per CELL of its Items.txt footprint, (left + cw*cx, top + ch*cy, cw, ch),
//     drawn BEFORE its cel, so it sits behind the item. A body-slot item gets ONE box over the whole
//     slot rectangle, also before the cel.
//   * The held item over a grid gets its boxes after every placed item, so they sit over them.
//   * The vendor grid reads no purse: an item the hero cannot afford looks like any other.
import { BOX_RGB, boxIndices } from './item-box-colour.mjs';
import { overlappedItems, CUBE_PAGE } from './grid-place.mjs';

export const BOX_RED = 0, BOX_GREEN = 1, BOX_BLUE = 2, BOX_YELLOW = 3, BOX_BROWN = 4;
export const BOX_NAMES = ['red', 'green', 'blue', 'yellow', 'brown'];
export { BOX_RGB, boxIndices };

// The engine's per-item test bits (d2_session_item_box_state).
export const BOX_STATE = Object.freeze({
  REQUIREMENTS: 0x001, FLAG_4: 0x002, STATLIST_2: 0x004, PLAYER_STATE_54: 0x008, NOT_EQUIPPABLE: 0x010,
  QUEST_LOCKED: 0x020, NEEDS_OTHER_HAND: 0x040, IDENTIFIED: 0x080, TRANSMOGRIFY: 0x100, FOUND: 0x200 });
const S = BOX_STATE;
// Retail's cursor mode for "an item is being used on another" (identify, socket, and the like).
export const CURSOR_MODE_USE_ON = 8;

// The tests for `item`: the engine's own when it has the item, otherwise what the snapshot carries
// (`itemFlags` / `flags` = dwItemFlags, `propColours` 1 = an unmet requirement line).
export function itemBoxState(item, engineState = null) {
  const s = item && engineState ? engineState(item.id) >>> 0 : 0;
  if (s & S.FOUND) return s;
  const flags = (item?.itemFlags ?? item?.flags ?? 0) >>> 0;
  return S.FOUND | ((item?.propColours || []).includes(1) ? S.REQUIREMENTS : 0) | (flags & 0x4 ? S.FLAG_4 : 0) | (flags & 0x10 ? S.IDENTIFIED : 0);
}

// A placed item in a grid (inventory, stash, cube, vendor, trade).
export function gridBoxSlot(state, hovered, cursorMode = 0) {
  if (hovered) return cursorMode === CURSOR_MODE_USE_ON && !(state & S.TRANSMOGRIFY) ? BOX_RED : BOX_GREEN;
  if (state & (S.REQUIREMENTS | S.FLAG_4 | S.STATLIST_2 | S.PLAYER_STATE_54 | S.NOT_EQUIPPABLE)) return BOX_RED;
  if (!(state & S.IDENTIFIED)) return BOX_BROWN;
  return state & S.QUEST_LOCKED ? BOX_RED : BOX_BLUE;
}

// An item worn in a body slot; null = no box (an identified, usable item).
export function slotBoxSlot(state, hovered, cursorMode = 0) {
  if (hovered) return cursorMode === CURSOR_MODE_USE_ON ? BOX_RED : BOX_GREEN;
  if (state & (S.NEEDS_OTHER_HAND | S.REQUIREMENTS | S.STATLIST_2 | S.PLAYER_STATE_54 | S.NOT_EQUIPPABLE)) return BOX_RED;
  return state & S.IDENTIFIED ? null : BOX_BROWN;
}

// One box per cell of a w x h footprint whose top-left cell is (gx, gy) in a grid at (left, top).
export function cellBoxes(left, top, cw, ch, gx, gy, w, h, slot, order = 'behind') {
  const out = [];
  for (let cy = 0; cy < h; cy++) for (let cx = 0; cx < w; cx++)
    out.push({ x: left + cw * (gx + cx), y: top + ch * (gy + cy), w: cw, h: ch, slot, order });
  return out;
}

// The boxes for a placed grid item.
export function placedItemBoxes(grid, item, state, hovered, cursorMode = 0) {
  return cellBoxes(grid.left, grid.top, grid.cw, grid.ch, item.gridX, item.gridY, item.invWidth || 1, item.invHeight || 1,
    gridBoxSlot(state, hovered, cursorMode));
}

// The held item `held` over cell (x, y) of `page`: green over its own footprint when it fits, yellow
// over the one item it would swap with (or the Horadric Cube it would go into), red otherwise.
export function heldItemBoxes(grid, items, page, x, y, held) {
  if (!held || x < 0 || y < 0) return [];
  const w = held.invWidth || 1, h = held.invHeight || 1;
  const hit = overlappedItems(items, page, x, y, w, h, held.id);
  const own = (slot) => cellBoxes(grid.left, grid.top, grid.cw, grid.ch, x, y, w, h, slot, 'over');
  const over = (it) => cellBoxes(grid.left, grid.top, grid.cw, grid.ch, it.gridX, it.gridY, it.invWidth || 1, it.invHeight || 1, BOX_YELLOW, 'over');
  if (hit.length === 0) return own(BOX_GREEN);
  if (hit.length === 1 && (!(page === CUBE_PAGE && held.code === 'box') || hit[0].code === 'box')) return over(hit[0]);
  return own(BOX_RED);
}

// The act tables the painter blends through.
export function boxTables(pal, pl2) {
  const index = new Map();
  for (let k = 255; k >= 0; k--) index.set((pal[k * 4] << 16) | (pal[k * 4 + 1] << 8) | pal[k * 4 + 2], k);
  return { pal, pl2, index, colours: boxIndices(pal) };
}

const MODE0_BASE = 13568 + 2 * 65536;
// Paints `boxes` in mode 0, in list order, at (b.x + dx, b.y + dy) in the context's current
// coordinates, and returns the evidence: where (b.x, b.y), which table entry, which palette index,
// the mode, and behind/over.
export function paintBoxes(ctx, tables, boxes, dx = 0, dy = 0) {
  const out = [];
  if (!boxes.length) return out;
  const t = ctx.getTransform();
  ctx.save(); ctx.setTransform(1, 0, 0, 1, 0, 0);
  for (const b of boxes) {
    const colour = tables.colours[b.slot];
    const X = Math.round(t.e + (b.x + dx) * t.a), Y = Math.round(t.f + (b.y + dy) * t.d), W = Math.round(b.w * t.a), H = Math.round(b.h * t.d);
    if (W > 0 && H > 0) {
      const img = ctx.getImageData(X, Y, W, H), d = img.data;
      for (let i = 0; i < d.length; i += 4) { const u = tables.index.get((d[i] << 16) | (d[i + 1] << 8) | d[i + 2]); if (u === undefined) continue;
        const c = tables.pl2[MODE0_BASE + u * 256 + colour]; d[i] = tables.pal[c * 4]; d[i + 1] = tables.pal[c * 4 + 1]; d[i + 2] = tables.pal[c * 4 + 2]; }
      ctx.putImageData(img, X, Y);
    }
    out.push({ x: b.x, y: b.y, w: b.w, h: b.h, colour: BOX_NAMES[b.slot], colourIndex: b.slot, paletteIndex: colour, mode: 0, order: b.order });
  }
  ctx.restore();
  return out;
}
