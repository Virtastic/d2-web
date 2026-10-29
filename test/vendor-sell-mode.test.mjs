import test from 'node:test';
import assert from 'node:assert/strict';
import { toggleSellMode, resetSellMode, vendorClickAction as act } from '../play/assets/vendor-sell-mode.mjs';
test('plain click on a carried item picks up; no sell packet', () => {
  resetSellMode(); assert.equal(act('mine', 0), 'pickup');
});
test('sell mode sells on click, toggles off', () => {
  resetSellMode(); toggleSellMode(); assert.equal(act('mine', 0), 'sell');
  toggleSellMode(); assert.equal(act('mine', 0), 'pickup');
});
test('drop on the shop panel sells the cursor item only', () => {
  resetSellMode(); assert.equal(act('panel', 7), 'sell-cursor'); assert.equal(act('panel', 0), 'none');
});
test('a held item dropped on a stocked cell sells, it does not buy the stock under it', () => {
  resetSellMode(); assert.equal(act('panel', 7, true), 'sell-cursor'); assert.equal(act('panel', 0, true), 'buy'); assert.equal(act('panel', 0, false), 'none');
});
