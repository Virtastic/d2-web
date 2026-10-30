// Where an item's inventory cel lands, for every panel that draws one: the one rule each renderer
// calls, so the grids, the body and the cursor cannot drift apart again (OPEND2-18463: the stash,
// cube and trade grids drew one row higher than the inventory and the vendor).
//
// Decompile-derived from 1.13c D2Client; the placement is engine behaviour carried here as a JS
// placeholder until the draw path moves to src/core/d2client/.
//
// The cel blit, D2Client+0x6B5A0 (unit in EBX, args x, y), hands D2Gfx the point (x, y + celHeight)
// and D2Gfx anchors the cel's bottom row ON that point, so the art's top row is y + 1, and the DC6
// frame offsets are added on top. Retail never centres the art by its own size. Every retail invfile
// cel is 28 px per cell with zero frame offsets (307 cels across d2data/d2exp/patch_d2), so in a
// 29 px cell the art sits flush left, one row down, and a 2-cell item leaves 2 px at the far edge.

// The art's top-left for a cel blitted at anchor (x, y).
export function celAt(x, y, frame = null) {
  return { left: x + (frame?.offsetX | 0), top: y + 1 + (frame?.offsetY | 0) };
}

// A grid item (inventory, stash, cube, trade both sides, vendor, gamble): the anchor is the item's
// top-left cell corner, grid.left + gx * cellW, grid.top + gy * cellH (D2Client+0x95A54 / +0x95B73;
// the cell size is the grid record's +0x14 / +0x15 bytes).
export function gridCelAt(gridLeft, gridTop, cellW, cellH, gx, gy, frame = null) {
  return celAt(gridLeft + gx * cellW, gridTop + gy * cellH, frame);
}

// A player body slot (D2Client+0x95604, second hand +0x954E3 / +0x955B7): the item's CELL box
// (cellW * invW by cellH * invH, not the art) is centred in the slot with an arithmetic shift, which
// rounds toward minus infinity, then the per-body-location constant (and the weapon-set II arm shift)
// is added by the caller as `adjust`.
export function slotCelAt(slot, invW, invH, cellW, cellH, adjust = [0, 0], frame = null) {
  const dx = (slot.width - cellW * invW) >> 1, dy = (slot.height - cellH * invH) >> 1;
  return celAt(slot.left + dx + adjust[0], slot.top + dy + adjust[1], frame);
}

// A mercenary equipment slot (D2Client+0x952A0): the same centring but by a signed DIVISION (cdq;
// idiv), which rounds toward zero, with the cell width used on both axes and no per-location constant.
export function mercSlotCelAt(slot, invW, invH, cell, frame = null) {
  const dx = Math.trunc((slot.width - cell * invW) / 2), dy = Math.trunc((slot.height - cell * invH) / 2);
  return celAt(slot.left + dx, slot.top + dy, frame);
}

// The item on the cursor (D2Client+0x169EC): the art's size halved by a shift is taken off the
// pointer, so an odd size rounds the art right/down, never toward the pointer.
export function cursorCelAt(pointerX, pointerY, celW, celH, frame = null) {
  return celAt(pointerX - (celW >> 1), pointerY - (celH >> 1), frame);
}

// A belt slot (D2Client+0x290DA): the anchor is the Belts.txt box's left and top as D2Common hands it.
export function beltCelAt(box, frame = null) {
  return celAt(box.left, box.top, frame);
}
