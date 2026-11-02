import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { celAt, gridCelAt } from './item-cel-place.mjs';

// Retail (D2Client+0x6B5A0 / +0x95A54): a grid item is anchored at its top-left cell corner, never
// centred by item size against cell size; the art is one row below the anchor.
test('grid cel anchors at the cell corner, one row down, never centred', () => {
  for (const [w, h] of [[29, 29], [31, 31]])
    for (const [gx, gy] of [[0, 0], [3, 2], [9, 3]])
      assert.deepEqual(gridCelAt(418, 315, w, h, gx, gy), { left: 418 + gx * w, top: 315 + gy * h + 1 });
  assert.deepEqual(celAt(10, 20, { offsetX: 2, offsetY: -3 }), { left: 12, top: 18 });
});

// Every grid renderer (inventory, stash, cube, trade, vendor, belt) goes through the shared rule.
test('every item grid renderer places through item-cel-place', () => {
  for (const f of ['inventory-panel', 'stash-panel', 'npc-panel', 'vendor-panel', 'belt-renderer'])
    assert.match(readFileSync(new URL(`./${f}.mjs`, import.meta.url), 'utf8'), /item-cel-place\.mjs/, f);
});
