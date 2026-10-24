import assert from 'node:assert/strict';
import { overlayKey } from '../play/assets/session-terrain.mjs';
import { goldPileFrame } from '../play/assets/item-renderer.mjs';
const cmp = (a, b) => { for (let i = 0; i < 4; i++) if (a[i] !== b[i]) return a[i] - b[i]; return 0; };
const item = overlayKey({ sprite: { wx: 102, wy: 203, pass: 'item' }, depth: 50 });
const player = overlayKey({ sprite: { wx: 103, wy: 204, pass: 'unit' }, depth: 40 });
assert(cmp(item, player) < 0, 'item draws before (behind) the player on one tile');
assert(cmp(player, overlayKey({ tile: { worldY: 40, worldX: 20, ordinal: 1 } })) < 0, 'unit before that tile walls');
// flpgld: 4 directions x 17 frames, final frame of the amount-tier direction
for (const [amt, dir] of [[0, 0], [99, 0], [100, 1], [499, 1], [500, 2], [4999, 2], [5000, 3], [1e6, 3]])
  assert.equal(goldPileFrame(amt, 68, 0, 4), dir * 17 + 16, `amount ${amt}`);
console.log('ok');
