import test from 'node:test';
import assert from 'node:assert/strict';
import { npcVendorIsOpen, closeInventoryPanel } from '../play/assets/npc-vendor-state.mjs';

test('inventory X with a trade open ends the trade and clears the panel state', () => {
  let eng = { npcId: 7, vendorNpcId: 7, action: 1, result: 1 }, inventoryVisible = true;
  const open = () => npcVendorIsOpen(eng);
  assert.equal(open(), true);
  closeInventoryPanel({ shopOpen: open(), endNpc: () => { eng = { npcId: 0, vendorNpcId: 0, action: 0, result: 0 }; }, hide: () => { inventoryVisible = false; } });
  assert.equal(open(), false);           // vendor panel (left) no longer drawn
  assert.equal(inventoryVisible, false); // right panel hidden -> no side shift
});

test('inventory X without a trade sends nothing', () => {
  let ended = 0;
  closeInventoryPanel({ shopOpen: false, endNpc: () => ended++, hide: () => {} });
  assert.equal(ended, 0);
});
