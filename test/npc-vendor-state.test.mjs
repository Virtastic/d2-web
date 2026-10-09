import test from 'node:test';
import assert from 'node:assert/strict';
import { npcVendorIsOpen } from '../play/assets/npc-vendor-state.mjs';

test('an accepted retail vendor counter opens even when its stock is empty', () => {
  assert.equal(npcVendorIsOpen({ npcId: 12, vendorNpcId: 12, action: 1, result: 1 }), true);
  assert.equal(npcVendorIsOpen({ npcId: 12, vendorNpcId: 12, action: 2, result: 1 }), true);
});

test('failed, stale, or non-vendor actions do not open the vendor panel', () => {
  assert.equal(npcVendorIsOpen({ npcId: 12, vendorNpcId: 12, action: 1, result: 0 }), false);
  assert.equal(npcVendorIsOpen({ npcId: 12, vendorNpcId: 11, action: 1, result: 1 }), false);
  assert.equal(npcVendorIsOpen({ npcId: 12, vendorNpcId: 0, action: 1, result: 1 }), false);
  assert.equal(npcVendorIsOpen({ npcId: 12, vendorNpcId: 12, action: 0, result: 1 }), false);
  assert.equal(npcVendorIsOpen(null), false);
});

test('vendor visibility follows the accepted counter, including after stock is exhausted', () => {
  const emptyStock = [];
  const acceptedCounter = { npcId: 12, vendorNpcId: 12, action: 1, result: 1 };
  assert.equal(emptyStock.length, 0);
  assert.equal(npcVendorIsOpen(acceptedCounter), true);
  assert.equal(npcVendorIsOpen({ ...acceptedCounter, vendorNpcId: 13 }), false);
});

test('switching NPCs or ending the chat hides a stale vendor counter', () => {
  const counter = { npcId: 12, vendorNpcId: 12, action: 1, result: 1 };
  assert.equal(npcVendorIsOpen(counter), true);
  assert.equal(npcVendorIsOpen({ ...counter, npcId: 13 }), false);
  assert.equal(npcVendorIsOpen({ ...counter, npcId: 0 }), false);
});
