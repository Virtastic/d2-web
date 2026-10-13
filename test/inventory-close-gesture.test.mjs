import test from 'node:test';
import assert from 'node:assert/strict';
import {
  beginInventoryClosePress, cancelInventoryClosePress, finishInventoryClosePress,
  inventoryCloseAt, inventoryCloseFrame,
} from '../play/assets/inventory-close-gesture.mjs';
import { TRADEBTN_CLOSESTASH, closeStashFromInventory } from '../play/assets/stash-close.mjs';

test('inventory close uses the native inclusive hit rectangle, separate from its cel', () => {
  assert.equal(inventoryCloseAt(418, 444, 800, 600, true), true);
  assert.equal(inventoryCloseAt(450, 476, 800, 600, true), true);
  assert.equal(inventoryCloseAt(417.99, 460, 800, 600, true), false);
  assert.equal(inventoryCloseAt(450.01, 460, 800, 600, true), false);
  assert.equal(inventoryCloseAt(430, 443.99, 800, 600, true), false);
  assert.equal(inventoryCloseAt(430, 476.01, 800, 600, true), false);
  assert.equal(inventoryCloseAt(430, 460, 800, 600, false), false);
  // A 400x300 viewport scales the native coordinates by one half.
  assert.equal(inventoryCloseAt(209, 222, 400, 300, true), true);
});

test('inventory close arms only on a visible close hit and draws the native held frame', () => {
  assert.deepEqual(beginInventoryClosePress(12, true, true), { pointerId: 12 });
  assert.equal(beginInventoryClosePress(12, false, true), null);
  assert.equal(beginInventoryClosePress(12, true, false), null);
  assert.equal(inventoryCloseFrame(false), 10);
  assert.equal(inventoryCloseFrame(true), 11);
});

test('inventory close activates only on the matching left release inside while still visible', () => {
  const press = beginInventoryClosePress(12, true, true);
  assert.deepEqual(finishInventoryClosePress(press, 12, true, true, true), { matched: true, activate: true });
  assert.deepEqual(finishInventoryClosePress(press, 12, true, true, false), { matched: true, activate: false });
  assert.deepEqual(finishInventoryClosePress(press, 12, false, true, true), { matched: true, activate: false });
  assert.deepEqual(finishInventoryClosePress(press, 12, true, false, true), { matched: true, activate: false });
  assert.deepEqual(finishInventoryClosePress(press, 13, true, true, true), { matched: false, activate: false });
});

test('inventory close press can be canceled for its pointer or on global focus loss', () => {
  const press = beginInventoryClosePress(12, true, true);
  assert.equal(cancelInventoryClosePress(press, 12), true);
  assert.equal(cancelInventoryClosePress(press, 13), false);
  assert.equal(cancelInventoryClosePress(press, null), true);
  assert.equal(cancelInventoryClosePress(null, 12), false);
});

test('accepted stash close still sends button 18 and refusal leaves paired panels open', () => {
  const calls = [];
  let panelsOpen = true;
  const accepted = closeStashFromInventory({ stashOpen: true,
    commandButton: (button, amount) => { calls.push([button, amount]); return 1; },
    onAccepted: () => { panelsOpen = false; } });
  assert.equal(accepted, true);
  assert.deepEqual(calls, [[TRADEBTN_CLOSESTASH, 0]]);
  assert.equal(panelsOpen, false);

  panelsOpen = true;
  const refused = closeStashFromInventory({ stashOpen: true,
    commandButton: (button, amount) => { calls.push([button, amount]); return 0; },
    onAccepted: () => { panelsOpen = false; } });
  assert.equal(refused, false);
  assert.deepEqual(calls[1], [18, 0]);
  assert.equal(panelsOpen, true);
});
