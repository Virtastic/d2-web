import assert from 'node:assert/strict';
import { overlayKey } from '../play/assets/session-terrain.mjs';
import { goldPileFrame } from '../play/assets/item-renderer.mjs';
const cmp = (a, b) => { for (let i = 0; i < 4; i++) if (a[i] !== b[i]) return a[i] - b[i]; return 0; };
const item = overlayKey({ sprite: { wx: 102, wy: 203, pass: 'item', floor: true, rank: 1 }, depth: 50 });
const player = overlayKey({ sprite: { wx: 103, wy: 204, pass: 'unit', rank: 0 }, depth: 40 });
assert(cmp(item, player) < 0, 'a floor-list item draws before (behind) the player on one tile');
assert(cmp(item, overlayKey({ tile: { worldY: 0, worldX: 0, ordinal: 1 } })) < 0, 'a floor-list item draws before every wall');
assert(cmp(overlayKey({ tile: { worldY: 40, worldX: 20, ordinal: 1 } }), player) < 0, 'that tile walls before the unit');
// flpgld: 4 directions x 17 frames, final frame of the amount-tier direction
for (const [amt, dir] of [[0, 0], [99, 0], [100, 1], [499, 1], [500, 2], [4999, 2], [5000, 3], [1e6, 3]])
  assert.equal(goldPileFrame(amt, 68, 4096, 4), dir * 17 + 16, `amount ${amt}`);
console.log('ok');
// Two floor-list items in one tile order by their room-list rank, not by screen Y; tile row/column still lead.
{
  const k = (wx, wy, rank, depth) => overlayKey({ sprite: { wx, wy, pass: 'item', floor: true, rank }, depth });
  assert(cmp(k(103, 203, 0, 90), k(102, 203, 1, 10)) < 0, 'same tile: lower rank first whatever the depth');
  assert(cmp(k(102, 203, 5, 90), k(102, 205, 0, 10)) < 0, 'lower tile row first');
}
