import test from 'node:test';
import assert from 'node:assert/strict';
import { openingTab } from '../play/assets/vendor-panel.mjs';

test('shop opens on Weapons for a smith, Misc otherwise, first stocked tab as last resort', () => {
  assert.equal(openingTab([0, 1, 2, 3], true), 1);
  assert.equal(openingTab([0, 3], true), 3);
  assert.equal(openingTab([1, 3], false), 3);
  assert.equal(openingTab([0, 1], false), 0);
  assert.equal(openingTab([], false), 3);
});
