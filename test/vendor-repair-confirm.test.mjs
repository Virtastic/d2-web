import test from 'node:test';
import assert from 'node:assert/strict';
import { createRepairConfirm, REPAIR_TITLE_STRING, YES_STRING } from '../play/assets/vendor-repair-confirm.mjs';

test('the repair item click only opens the dialog; Yes sends once, No sends nothing', () => {
  const sent = [];
  const c = createRepairConfirm((id) => sent.push(id));
  assert.equal(c.request(7), true);
  assert.deepEqual(sent, []);
  assert.equal(c.request(8), false);      // modal: a second click cannot replace the pending item
  assert.equal(c.yes(), true);
  assert.deepEqual(sent, [7]);
  assert.equal(c.yes(), false);
  c.request(9); assert.equal(c.no(), true); assert.deepEqual(sent, [7]);
  assert.equal(c.open, false);
  assert.equal(c.request(-1), false);
});
test('title is string 3351', () => { assert.equal(REPAIR_TITLE_STRING, 0xd17); assert.equal(YES_STRING, 0xd10); });

test('an item the 0x45B40 gate rejects never opens the dialog', () => {
  const seen = [];
  const c = createRepairConfirm(() => {}, (id) => { seen.push(id); return id === 2; });
  assert.equal(c.request(1), false); assert.equal(c.open, false);
  assert.equal(c.request(2), true); assert.equal(c.open, true);
  assert.deepEqual(seen, [1, 2]);
});
