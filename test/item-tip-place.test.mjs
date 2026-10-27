import test from 'node:test';
import assert from 'node:assert/strict';
import { inventoryTipCell } from '../play/assets/inventory-panel.mjs';
import { placeItemTip } from '../play/assets/item-tooltip.mjs';
// D2Client+0x945D0/0x93450/D2Win 0x133A0: box centred on the cell, above it if it fits, else below; clamped.
test('item tip placement', () => {
  assert.deepEqual(placeItemTip({ x: 300, y: 300, w: 29, h: 87 }, 100, 48), { bx: 264, by: 254 });
  // retail capture p8-ui2-shop-*-hover: Hand Axe at cell (96,123), box 0,45-248,125
  assert.deepEqual(placeItemTip({ x: 96, y: 123, w: 29, h: 29 }, 248, 80), { bx: 0, by: 45 });
  assert.equal(placeItemTip({ x: 700, y: 300, w: 29, h: 29 }, 248, 48).bx, 552);
});
test('inventory tip cell: grid and worn', () => {
  const g = (i) => ({ left: 10 + i.gridX * 29, top: 20, width: 29 * i.invWidth, height: 29 * i.invHeight });
  const s = () => ({ left: 100, top: 100, width: 58, height: 87 });
  assert.deepEqual(inventoryTipCell({ mode: 0, gridX: 1, invWidth: 1, invHeight: 2 }, g, s), { x: 39, y: 20, w: 29, h: 58 });
  assert.deepEqual(inventoryTipCell({ mode: 1, bodyLoc: 1, invWidth: 2, invHeight: 2 }, g, s), { x: 100, y: 114, w: 58, h: 58 });
});
test('D16: right-edge vendor tip is on screen and drawn after the side borders', async () => {
  const { bx } = placeItemTip({ x: 96 + 9 * 29, y: 123, w: 29, h: 29 }, 120, 32);
  assert.ok(bx >= 0 && bx + 120 <= 800);
  assert.equal(bx, 96 + 9 * 29 + 14 - 60); // centred on the cell, as retail
  const src = (await import('node:fs')).readFileSync(new URL('../play/assets/vendor-panel.mjs', import.meta.url), 'utf8');
  assert.ok(src.indexOf('chrome.border(') < src.indexOf('placeItemTip(cell'));
});
