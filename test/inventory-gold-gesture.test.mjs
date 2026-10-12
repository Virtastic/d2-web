import test from 'node:test';
import assert from 'node:assert/strict';
import { beginInventoryGoldPress, cancelInventoryGoldPress, finishInventoryGoldPress } from '../play/assets/inventory-gold-gesture.mjs';

test('inventory gold coin arms only when its visible panel control is enabled', () => {
  assert.deepEqual(beginInventoryGoldPress(7, true), { pointerId: 7 });
  assert.equal(beginInventoryGoldPress(7, false), null);
  assert.equal(beginInventoryGoldPress(null, true), null);
});

test('inventory gold deposit activates only on matching left release over the coin', () => {
  const press = beginInventoryGoldPress(7, true);
  assert.deepEqual(finishInventoryGoldPress(press, 7, true, true, true), { matched: true, activate: true });
  assert.deepEqual(finishInventoryGoldPress(press, 7, true, true, false), { matched: true, activate: false });
  assert.deepEqual(finishInventoryGoldPress(press, 7, true, false, true), { matched: true, activate: false });
  assert.deepEqual(finishInventoryGoldPress(press, 7, false, true, true), { matched: true, activate: false });
  assert.deepEqual(finishInventoryGoldPress(press, 8, true, true, true), { matched: false, activate: false });
});

test('inventory gold cancellation is owned by its pointer only', () => {
  const press = beginInventoryGoldPress(7, true);
  assert.equal(cancelInventoryGoldPress(press, 8), false);
  assert.equal(cancelInventoryGoldPress(press, 7), true);
});
