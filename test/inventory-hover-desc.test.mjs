import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { itemTooltipSpec } from '../play/assets/item-tooltip.mjs';

test('hover spec: magic name blue, requirement red, engine lines in order', () => {
  assert.deepEqual(itemTooltipSpec({ name: 'Hand Axe', quality: 4, props: ['Damage: 3 to 6', 'Required Level: 3'], propColours: [0, 1] }),
    { lines: ['Hand Axe', 'Damage: 3 to 6', 'Required Level: 3'], colours: [3, 0, 1] });
});

test('inventory hover falls back to the worn item under the pointer (slotAt)', () => {
  const src = readFileSync(new URL('../play/assets/inventory-panel.mjs', import.meta.url), 'utf8');
  assert.match(src, /this\.itemAt\([^)]*\) \|\| this\.slotAt\([^)]*\)\?\.item/);
});

import { inventoryItemAt } from '../play/assets/inventory-panel.mjs';
// Shapes as the shell's snapshot carries them (parseItemPresentations): a pack weapon, a worn one, a
// ground item and a belt potion. Engine layout: grid 10x4 of 29 px at 420,315.
const layout = { grid: { left: 420, top: 315, columns: 10, rows: 4, cellWidth: 29, cellHeight: 29 }, slots: {} };
const pack = { id: 36, mode: 0, invPage: 0, bodyLoc: 0, gridX: 3, gridY: 1, invWidth: 1, invHeight: 3, quality: 6 };
const worn = { id: 37, mode: 1, invPage: 255, bodyLoc: 5, gridX: 3, gridY: 1, invWidth: 2, invHeight: 3, quality: 7 };
const ground = { id: 38, mode: 3, invPage: 255, bodyLoc: 0, gridX: -1, gridY: -1, invWidth: 1, invHeight: 1, quality: 2 };
const belt = { id: 39, mode: 2, invPage: 255, bodyLoc: 0, gridX: 99, gridY: 0, invWidth: 1, invHeight: 1, quality: 2 };

test('backpack hit: the pack item at its cells, never a worn/ground/belt item on the same coords', () => {
  const items = [worn, ground, belt, pack];
  for (const l of [layout, null]) {
    const at = l ? [420 + 3 * 29 + 5, 315 + 29 + 5 + 29] : [506 + 3 * 29 + 5, 373 - 2 * 29 + 29 + 5 + 29];
    if (l) assert.equal(inventoryItemAt(items, ...at, l), pack);   // third cell of the 1x3 item
    assert.doesNotThrow(() => inventoryItemAt(items, 0, 0, l));    // ground gridX -1 / belt gridX 99 must not throw
  }
  assert.equal(inventoryItemAt(items, 420 + 3 * 29 + 5, 315 + 4 * 29 + 5, layout), null);   // below the item's 3 rows
});

test('tooltip name colour per D2Client+0x93450: 4..9 -> 3,2,9,4,8,10; else grey if socketed/ethereal, white otherwise', () => {
  const col = (quality, itemFlags = 0) => itemTooltipSpec({ name: 'X', quality, itemFlags }).colours[0];
  assert.deepEqual([4, 5, 6, 7, 8, 9].map((q) => col(q)), [3, 2, 9, 4, 8, 10]);
  assert.deepEqual([1, 2, 3].map((q) => col(q)), [0, 0, 0]);
  assert.equal(col(2, 0x800), 5);
  assert.equal(col(3, 0x400000), 5);
  assert.equal(col(4, 0x800), 3);   // a magic socketed item stays blue
});
