import test from 'node:test';
import assert from 'node:assert/strict';
import { vendorButtons } from '../play/assets/vendor-panel.mjs';

test('button set per NPC flag: smith repairs, others blank + exit', () => {
  assert.deepEqual(vendorButtons(154), ['buy', 'sell', 'repair', 'repairAll']);
  assert.deepEqual(vendorButtons(148), ['buy', 'sell', 'blank', 'exit']); // Akara
});
