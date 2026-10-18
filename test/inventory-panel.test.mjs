import test from 'node:test';
import assert from 'node:assert/strict';
import { AMAZON2_EQUIPMENT_LAYOUT, AMAZON2_INVENTORY_LAYOUT, inventoryGridPosition, inventoryItemAsset, inventoryItemAt, parseInventoryItems } from '../play/assets/inventory-panel.mjs';
import { itemTooltipSpec } from '../play/assets/item-tooltip.mjs';

test('Amazon2 inventory grid retains retail Inventory.txt placement', () => {
  assert.deepEqual(AMAZON2_INVENTORY_LAYOUT, {
    screenX: 400, screenY: 60, width: 320, height: 441,
    gridX: 19, gridY: 255, columns: 10, rows: 4, cellWidth: 29, cellHeight: 29,
  });
  assert.deepEqual(inventoryGridPosition({ x: 0, y: 0 }), { x: 19, y: 255 });
  assert.deepEqual(inventoryGridPosition({ x: 9, y: 3 }), { x: 280, y: 342 });
  assert.throws(() => inventoryGridPosition({ x: 10, y: 0 }), /invalid inventory grid position/);
});

test('Amazon2 equipped hand slots retain retail Inventory.txt bounds', () => {
  assert.deepEqual(AMAZON2_EQUIPMENT_LAYOUT[4], { x: 20, y: 47, width: 55, height: 112 });
  assert.deepEqual(AMAZON2_EQUIPMENT_LAYOUT[5], { x: 251, y: 47, width: 55, height: 112 });
});

test('inventory item filenames stay within the retail item asset namespace', () => {
  assert.equal(inventoryItemAsset('invhax'), 'data\\global\\items\\invhax.dc6');
  assert.throws(() => inventoryItemAsset('../hax'), /invalid inventory item file/);
  assert.throws(() => inventoryItemAsset(''), /invalid inventory item file/);
});

test('inventory records decode the fixed-width engine item ABI without ground entries', () => {
  const bytes = new Uint8Array(224), view = new DataView(bytes.buffer);
  view.setUint32(0, 91, true); view.setUint32(4, 7, true); view.setUint32(8, 0x20786168, true);
  view.setUint32(24, 2, true); view.setUint32(28, 0x10, true);
  bytes.set(new TextEncoder().encode('invhax'), 72); view.setInt16(104, 3, true); view.setInt16(106, 2, true); bytes[108] = 0;
  bytes.set(new TextEncoder().encode('invhp1'), 112 + 72); bytes[112 + 108] = 0xff;
  assert.deepEqual(parseInventoryItems(bytes, 2), [{ id: 91, classId: 7, code: 0x20786168, quality: 2,
    itemFlags: 0x10, invFile: 'invhax', x: 3, y: 2, invPage: 0, bodyLoc: 0 }]);
  assert.throws(() => parseInventoryItems(bytes.subarray(0, 223), 2), /invalid item presentation extent/);
});

test('inventory hover names the item in the cell and the tooltip record is the engine lines', () => {
  // Amazon2: grid at 419,315 (400+19, 60+255), 29 px cells. The item covers cell (3,2) only.
  const item = { id: 91, code: 0x20786168, mode: 0, invPage: 0, gridX: 3, gridY: 2, invWidth: 1, invHeight: 1 };
  const other = { id: 92, code: 0x20786169, mode: 0, invPage: 0, gridX: 4, gridY: 2, invWidth: 1, invHeight: 1 };
  const items = [item, other];
  assert.equal(inventoryItemAt(items, 506 + 14, 373 + 14), item);      // centre of cell (3,2)
  assert.equal(inventoryItemAt(items, 506 + 29 + 14, 373 + 14), other); // one cell away is the other item
  assert.equal(inventoryItemAt([item], 506 + 29 + 14, 373 + 14), null); // and null with no item there
  // The record drawn for the hovered item: the engine's name and property lines, and P8j colours.
  assert.deepEqual(itemTooltipSpec({ name: 'Hand Axe', quality: 4, props: ['Damage: 3 to 6', '+10% Enhanced Damage', 'Required Level: 3'],
    propColours: [0, 3, 1] }),
  { lines: ['Hand Axe', 'Damage: 3 to 6', '+10% Enhanced Damage', 'Required Level: 3'], colours: [3, 0, 3, 1] });
});
