import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { beltCelAt, celAt, cursorCelAt, gridCelAt, mercSlotCelAt, slotCelAt } from '../play/assets/item-cel-place.mjs';
import { AMAZON2_EQUIPMENT_LAYOUT, AMAZON2_INVENTORY_LAYOUT, WORN_ANCHOR_OFFSET, weaponSetArmShift } from '../play/assets/inventory-panel.mjs';
import { TRADE_LAYOUT } from '../play/assets/npc-panel.mjs';
import { VENDOR_LAYOUT } from '../play/assets/vendor-panel.mjs';
import { beltBoxes } from '../play/assets/belt-renderer.mjs';
import { stashGeometry } from '../play/assets/stash-panel.mjs';

// OPEND2-18463: small items straddled the cell lines in some panels. Retail 1.13c D2Client has ONE
// rule per kind of placement (item-cel-place.mjs cites the RVAs); these rows pin it for every footprint
// and every panel, written out independently of the module so the module cannot drift unseen.
//
// Retail invfile cels are 28 px per cell with zero DC6 offsets, so a WxH item's art is 28W x 28H.
const SIZES = [[1, 1], [1, 2], [2, 2], [2, 3], [2, 4]];
const cel = (w, h) => ({ w: 28 * w, h: 28 * h });

// The retail grid rule: the art's left is the cell's left, its top one row below the cell's top --
// whatever the art's size (D2Client+0x95A54 anchor, +0x6B5A0 blit).
const retailGrid = (left, top, cell, gx, gy) => ({ left: left + gx * cell, top: top + gy * cell + 1 });

const GRIDS = {
  inventory: { left: AMAZON2_INVENTORY_LAYOUT.screenX + AMAZON2_INVENTORY_LAYOUT.gridX, top: AMAZON2_INVENTORY_LAYOUT.screenY + AMAZON2_INVENTORY_LAYOUT.gridY, cell: 29 },
  // d2_session_inventory_layout's twelve numbers for the LoD bank page and the cube: only the grid
  // origin and the box size matter to placement.
  stash: (() => { const g = stashGeometry([80, 400, 280, 512, 6, 8, 153, 327, 143, 375, 29, 29]); return { left: g.gridLeft, top: g.gridTop, cell: g.boxW }; })(),
  cube: (() => { const g = stashGeometry([80, 400, 280, 512, 3, 4, 197, 284, 199, 315, 29, 29]); return { left: g.gridLeft, top: g.gridTop, cell: g.boxW }; })(),
  'trade npc side': { left: TRADE_LAYOUT.screenX + TRADE_LAYOUT.gridX, top: TRADE_LAYOUT.screenY + TRADE_LAYOUT.gridY, cell: TRADE_LAYOUT.cell },
  'trade player side': { left: TRADE_LAYOUT.screenX + TRADE_LAYOUT.gridX, top: TRADE_LAYOUT.screenY + TRADE_LAYOUT.playerGridY, cell: TRADE_LAYOUT.cell },
  vendor: { left: VENDOR_LAYOUT.gridLeft, top: VENDOR_LAYOUT.gridTop, cell: VENDOR_LAYOUT.cell },
  gamble: { left: VENDOR_LAYOUT.gridLeft, top: VENDOR_LAYOUT.gridTop, cell: VENDOR_LAYOUT.cell },
};

test('every grid panel draws every footprint at its top-left cell, one row down, never centred', () => {
  for (const [panel, g] of Object.entries(GRIDS)) for (const [w, h] of SIZES) for (const [gx, gy] of [[0, 0], [1, 0], [3, 2]]) {
    assert.deepEqual(gridCelAt(g.left, g.top, g.cell, g.cell, gx, gy), retailGrid(g.left, g.top, g.cell, gx, gy), `${panel} ${w}x${h} at ${gx},${gy}`);
  }
  // Written out once, for the reported panels: a 1x1 in stash cell (0,0) and in trade cell (2,1).
  assert.deepEqual(gridCelAt(153, 143, 29, 29, 0, 0), { left: 153, top: 144 });
  assert.deepEqual(gridCelAt(100, 101, 29, 29, 2, 1), { left: 158, top: 131 });
  // A 1x1's 28 px art then fills x 153..180 of the cell 153..181: inside it, not across its line.
  const a = gridCelAt(153, 143, 29, 29, 0, 0), c = cel(1, 1);
  assert.ok(a.left >= 153 && a.left + c.w <= 153 + 29 && a.top >= 143 && a.top + c.h <= 143 + 29);
});

// The retail body-slot rule: the item's CELL box is centred with an arithmetic shift (floor), then
// the body-location constant and, in weapon set II, the arm shift; the art one row below.
const retailSlot = (s, w, h, loc, set) => {
  const [bx, by] = WORN_ANCHOR_OFFSET[loc], [ax, ay] = set === 1 && (loc === 4 || loc === 5) ? [4, -4] : [0, 0];
  return { left: s.left + Math.floor((s.width - 29 * w) / 2) + bx + ax, top: s.top + Math.floor((s.height - 29 * h) / 2) + by + ay + 1 };
};

test('equipment slots, both weapon sets, centre the cell box rounding down plus the body constant', () => {
  for (const set of [0, 1]) for (const [loc, r] of Object.entries(AMAZON2_EQUIPMENT_LAYOUT)) for (const [w, h] of SIZES) {
    const slot = { left: 400 + r.x, top: 60 + r.y, width: r.width, height: r.height };
    const [bx, by] = WORN_ANCHOR_OFFSET[loc], [ax, ay] = weaponSetArmShift(set, Number(loc));
    assert.deepEqual(slotCelAt(slot, w, h, 29, 29, [bx + ax, by + ay]), retailSlot(slot, w, h, Number(loc), set), `bodyLoc ${loc} set ${set} ${w}x${h}`);
  }
  // Right arm (420,107 55x112), a 2x4 in set I: (55-58)>>1 = -2, (112-116)>>1 = -2.
  assert.deepEqual(slotCelAt({ left: 420, top: 107, width: 55, height: 112 }, 2, 4, 29, 29), { left: 418, top: 106 });
  // ... and in set II the arm shift moves it (+4,-4).
  assert.deepEqual(slotCelAt({ left: 420, top: 107, width: 55, height: 112 }, 2, 4, 29, 29, weaponSetArmShift(1, 4)), { left: 422, top: 102 });
});

test('mercenary slots centre by a truncating division on the cell width, with no body constant', () => {
  const slot = { left: 100, top: 80, width: 55, height: 112 };
  for (const [w, h] of SIZES) {
    assert.deepEqual(mercSlotCelAt(slot, w, h, 29),
      { left: 100 + Math.trunc((55 - 29 * w) / 2), top: 80 + Math.trunc((112 - 29 * h) / 2) + 1 }, `merc ${w}x${h}`);
  }
  // Where it differs from the player's floor: a 2x4 overhangs by 3 and 4 -> -1 (not -2) and -2.
  assert.deepEqual(mercSlotCelAt(slot, 2, 4, 29), { left: 99, top: 79 });
  assert.deepEqual(slotCelAt(slot, 2, 4, 29, 29), { left: 98, top: 79 });
});

test('belt potions sit at the Belts.txt box left, one row below its top', () => {
  for (const b of beltBoxes(null)) for (const [w, h] of SIZES)
    assert.deepEqual(beltCelAt(b), { left: b.left, top: b.top + 1 }, `belt ${w}x${h}`);
  assert.deepEqual(beltCelAt(beltBoxes(null)[0]), { left: 423, top: 563 });
});

test('the cursor item takes half its art size, by a shift, off the pointer, one row down', () => {
  for (const [w, h] of SIZES) { const c = cel(w, h);
    assert.deepEqual(cursorCelAt(400, 300, c.w, c.h), { left: 400 - (c.w >> 1), top: 300 - (c.h >> 1) + 1 }, `cursor ${w}x${h}`); }
  assert.deepEqual(cursorCelAt(400, 300, 28, 28), { left: 386, top: 287 });
  assert.deepEqual(cursorCelAt(400, 300, 27, 27), { left: 387, top: 288 });  // odd size: the shift rounds down
});

test('DC6 frame offsets add to the blit, as D2Gfx adds them', () => {
  assert.deepEqual(celAt(10, 20, { offsetX: 2, offsetY: -3 }), { left: 12, top: 18 });
});

test('every item-drawing panel places through the shared rule', () => {
  const uses = { 'inventory-panel': ['celAt(', 'slotCelAt(', 'cursorCelAt('], 'stash-panel': ['gridCelAt('],
    'npc-panel': ['gridCelAt('], 'vendor-panel': ['gridCelAt('], 'belt-renderer': ['beltCelAt('] };
  for (const [file, calls] of Object.entries(uses)) {
    const src = readFileSync(new URL(`../play/assets/${file}.mjs`, import.meta.url), 'utf8');
    for (const call of calls) assert.ok(src.includes(call), `${file} calls ${call}`);
  }
});
