// The held-item cell: the old centred-and-rounded rule against D2Client+0x94820's arithmetic (JS model of
// src/core/d2client/d2client_cells.cpp d2_client_94820 with the sizes d2_client_held_cell hands it).
import test from 'node:test';
import assert from 'node:assert/strict';
import { centredCell, heldCellAt, setHeldCellEngine } from './grid-cell.mjs';

const u = (n) => n >>> 0;
// 94820: a and b are the art pixel size (6E930 reads D2CMP #10060 and #10000).
function model94820(g, w, h, a, b, x, y) {
  let cx = Math.floor(u(x - g.left) / g.cellWidth), cy = Math.floor(u(y - g.top) / g.cellHeight);
  if (!(w & 1)) cx = Math.floor(u(((a >>> 2) - g.left) + x) / g.cellWidth);
  if (w === g.columns) cx = w >> 1;
  if (!(h & 1)) cy = Math.floor(u(((b >>> 2) - g.top) + y) / g.cellHeight);
  if (h === g.rows) cy = h >> 1;
  if (w > 1) { const t = (cx - (w >> 1)) | 0; cx = t < 0 ? 0 : t; }
  if (h > 1) { const t = (cy - (h >> 1)) | 0; cy = t < 0 ? 0 : t; }
  return { x: cx, y: cy };
}
const inv = { left: 417, top: 315, cellWidth: 29, cellHeight: 29, columns: 10, rows: 4 };
const sizes = [[1, 1], [1, 2], [1, 3], [1, 4], [2, 2], [2, 3], [2, 4], [3, 3]];
const clamp = (g, w, h, c) => ({ x: Math.max(0, Math.min(g.columns - w, c.x)), y: Math.max(0, Math.min(g.rows - h, c.y)) });

test('the engine rule goes through heldCellAt and is clamped to the grid', () => {
  setHeldCellEngine((g, w, h, pw, ph, x, y) => model94820(g, w, h, pw, ph, x, y));
  for (const [w, h] of sizes) for (let x = inv.left; x < inv.left + 290; x += 7) for (let y = inv.top; y < inv.top + 116; y += 5) {
    assert.deepEqual(heldCellAt(inv, { invWidth: w, invHeight: h }, x, y), clamp(inv, w, h, model94820(inv, w, h, w * 29, h * 29, x, y)));
  }
  assert.equal(heldCellAt(inv, { invWidth: 1, invHeight: 1 }, 0, 0), null);
  setHeldCellEngine(null);
});

test('record where the old rule and 94820 differ', () => {
  const diff = {};
  const grids = { inventory: inv, stash: { left: 100, top: 143, cellWidth: 29, cellHeight: 29, columns: 6, rows: 8 }, cube: { left: 100, top: 143, cellWidth: 29, cellHeight: 29, columns: 3, rows: 4 } };
  for (const [name, g] of Object.entries(grids)) for (const [w, h] of sizes) {
    let n = 0, tot = 0, ex = null;
    for (let x = g.left; x < g.left + g.columns * 29; x++) for (let y = g.top; y < g.top + g.rows * 29; y++) {
      tot++;
      const o = clamp(g, w, h, centredCell(g, w, h, x, y)), e = clamp(g, w, h, model94820(g, w, h, w * 29, h * 29, x, y));
      if (o.x !== e.x || o.y !== e.y) { n++; ex ||= { at: [x, y], old: o, engine: e }; }
    }
    diff[`${name} ${w}x${h}`] = { differ: n, of: tot, first: ex };
  }
  console.log(JSON.stringify(diff));
  assert.ok(Object.values(diff).some((d) => d.differ > 0));
});
