import test from 'node:test';
import assert from 'node:assert/strict';
import { tooltipSpec } from '../play/assets/vendor-panel.mjs';

test('rare tooltip: yellow name, base name on a second line in the same colour', () => {
  const t = tooltipSpec({ cost: 5, name: 'Beast Cry', baseName: 'Hand Axe', quality: 6, props: ['+5 Strength'], propColours: [3] });
  assert.deepEqual(t.lines, ['Cost: 5', 'Beast Cry', 'Hand Axe', '+5 Strength']);
  assert.deepEqual(t.colours, [9, 9, 9, 3]);
});
test('unique and set colours; no base line when absent', () => {
  assert.equal(tooltipSpec({ cost: 1, name: 'X', baseName: 'Y', quality: 7 }).colours[1], 4);
  assert.equal(tooltipSpec({ cost: 1, name: 'X', baseName: 'Y', quality: 5 }).colours[1], 2);
  assert.deepEqual(tooltipSpec({ cost: 1, name: 'Jagged Spear', quality: 4 }).lines, ['Cost: 1', 'Jagged Spear']);
});
