// The grid cell under an 800x600 screen point. The rule is the engine's (src/core/d2client/inv_grid_cell_96aa0.cpp,
// exported as d2_client_inv_grid_cell, 1.13c D2Client+0x96AA0): unsigned (mouse - origin) / box, in the grid's
// columns x rows. The shell installs the export; there is no JS division.
let engine = null;
export const setGridCellEngine = (fn) => { engine = fn; };
/** { x, y } of the cell, or null off the grid. g = { left, top, cellWidth, cellHeight, columns, rows }. */
export function gridCellAt(g, sx, sy) {
  const x = Math.floor(sx), y = Math.floor(sy);
  return engine ? engine(g.left | 0, g.top | 0, g.cellWidth | 0, g.cellHeight | 0, x, y, g.columns | 0, g.rows | 0) : null;
}

// The cell a held item's top-left is put down at. The rule is D2Client+0x94820 (src/core/d2client/held_cell_94820.cpp,
// d2_client_held_cell): the pointer's cell by the unsigned division, for an even footprint re-divided with a quarter of the
// item's art size added, then half the footprint taken off. The shell installs the export. Null off the grid; the cell is
// clamped so the footprint stays in the grid (retail's hover gate, 0x945D0, refuses an overhanging cell instead).
let heldEngine = null;
export const setHeldCellEngine = (fn) => { heldEngine = fn; };
/** g = { left, top, cellWidth, cellHeight, columns, rows }; item = { invWidth, invHeight }; art size = footprint x cell box. */
export function heldCellAt(g, item, sx, sy) {
  const x = Math.floor(sx), y = Math.floor(sy);
  if (x < g.left || y < g.top || x >= g.left + g.columns * g.cellWidth || y >= g.top + g.rows * g.cellHeight) return null;
  const w = item?.invWidth || 1, h = item?.invHeight || 1;
  const c = heldEngine && item && heldEngine(g, w, h, w * g.cellWidth, h * g.cellHeight, x, y);
  if (!c) return null;
  return { x: Math.max(0, Math.min(g.columns - w, c.x)), y: Math.max(0, Math.min(g.rows - h, c.y)) };
}
