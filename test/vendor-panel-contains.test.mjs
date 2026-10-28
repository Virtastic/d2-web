import test from 'node:test';
import assert from 'node:assert/strict';
import { vendorButtons, vendorPanelContains, VENDOR_LAYOUT } from '../play/assets/vendor-panel.mjs';

test('button set per NPC flag: smith repairs, others blank + exit', () => {
  assert.deepEqual(vendorButtons(154), ['buy', 'sell', 'repair', 'repairAll']);
  assert.deepEqual(vendorButtons(148), ['buy', 'sell', 'blank', 'exit']); // Akara
});

// D17 click half: a click on the panel below the item grid is swallowed by the panel (the shell spends it
// before its npcClose path, the only thing that sends 0x30), so no close-trade packet goes out, as retail.
test('vendor panel rect (320x432 buysell art) swallows clicks below the item grid; outside it does not', () => {
  const hit = (x, y) => vendorPanelContains(VENDOR_LAYOUT, 320, 432, x, y);
  assert.equal(hit(200, 440), true);   // below the 10x10 grid (ends y 413)
  assert.equal(hit(200, 490), true);   // lowest panel row
  assert.equal(hit(100, 70), true);
  assert.equal(hit(79, 300), false);
  assert.equal(hit(400, 300), false);
  assert.equal(hit(200, 20), false);
  assert.equal(hit(200, 493), false);
});
