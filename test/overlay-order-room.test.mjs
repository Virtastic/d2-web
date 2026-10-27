import assert from 'node:assert/strict';
import { overlayKey, overlayCompare } from '../play/assets/session-terrain.mjs';
// Synthetic room: a tall wall in tile (x=2,y=1) whose art reaches far up the screen, units at tile seams.
// Retail walks tile rows then columns; a unit follows its own tile's walls. Screen Y must not matter.
const wall = (wx, wy, ordinal) => ({ tile: { worldX: wx, worldY: wy, ordinal }, depth: 0 });
const unit = (name, x, y, depth) => ({ name, sprite: { wx: x, wy: y, pass: 'unit' }, depth, ordinal: -1 });
const items = [
  wall(2, 1, 0), wall(2, 1, 1), wall(1, 1, 2), wall(3, 0, 3),
  unit('seamBefore', 9, 5, 900),   // last subtile of tile (1,1): before tile (2,1)'s walls despite huge depth
  unit('seamAfter', 10, 5, -900),  // first subtile of tile (2,1): after its walls despite tiny depth
  unit('rowAbove', 20, 4, 5000),   // tile (4,0): row 0, after (3,0) wall
  unit('rowBelow', 5, 10, -5000),  // tile (1,2): row 2, last
];
items.forEach((o) => { o.k = overlayKey(o); });
const order = items.slice().sort(overlayCompare).map((o) => o.name || `w${o.tile.worldX},${o.tile.worldY}#${o.tile.ordinal}`);
assert.deepEqual(order, ['w3,0#3', 'rowAbove', 'w1,1#2', 'seamBefore', 'w2,1#0', 'w2,1#1', 'seamAfter', 'rowBelow']);
console.log('ok overlay-order-room');
// Two units in ONE tile keep list (snapshot) order whatever their screen Y (D2Client+0x9EE90 walks the cell's unit list).
const u = [unit('first', 12, 7, 900), unit('second', 13, 8, -900)];
u.forEach((o) => { o.k = overlayKey(o); });
assert.deepEqual(u.slice().sort(overlayCompare).map((o) => o.name), ['first', 'second']);
console.log('ok overlay-order-unit-list');

// D20: two units in one tile order by position, whichever was pushed last.
{
  const k = (o) => { o.k = overlayKey(o); return o; };
  const p = (name, x, y) => k({ name, sprite: { wx: x, wy: y, pass: 'unit' }, depth: 0, ordinal: -1 });
  const north = p('player', 1002, 2001), south = p('zombie', 1003, 2003);
  for (const list of [[north, south], [south, north]])
    if (list.slice().sort(overlayCompare)[1].name !== 'zombie') throw new Error('D20: south unit must draw last');
}
