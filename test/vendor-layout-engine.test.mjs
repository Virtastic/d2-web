import test from 'node:test';
import assert from 'node:assert/strict';
import { vendorLayoutFromEngine, VENDOR_LAYOUT } from '../play/assets/vendor-panel.mjs';

test('vendor stock grid comes from the engine layout, constant only as fallback', () => {
  const g = [0, 0, 0, 0, 10, 10, 100, 0, 130, 0, 29, 29];
  const l = vendorLayoutFromEngine(g);
  assert.deepEqual([l.gridLeft, l.gridTop, l.columns, l.rows, l.cell], [100, 130, 10, 10, 29]);
  assert.equal(l.tabStep, VENDOR_LAYOUT.tabStep);
  assert.equal(vendorLayoutFromEngine(null), VENDOR_LAYOUT);
  assert.equal(vendorLayoutFromEngine([0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0]), VENDOR_LAYOUT);
});

test('a malformed engine read never leaves the grid unbuilt: it falls back to the constant retail layout', () => {
  const ok = [0, 0, 0, 0, 10, 10, 96, 0, 123, 0, 29, 0];
  for (const bad of [undefined, 'x', {}, [], ok.slice(0, 11), Object.assign([...ok], { 6: NaN }), Object.assign([...ok], { 8: -1 }), Object.assign([...ok], { 4: 0 })])
    assert.equal(vendorLayoutFromEngine(bad), VENDOR_LAYOUT);
  assert.equal(vendorLayoutFromEngine(ok).gridLeft, 96);
});

test('fallback grid is retail Inventory.txt "Monster" (10x10 of 29px at 96,123), so a shop is never a names/prices list', () => {
  assert.deepEqual([VENDOR_LAYOUT.gridLeft, VENDOR_LAYOUT.gridTop, VENDOR_LAYOUT.columns, VENDOR_LAYOUT.rows, VENDOR_LAYOUT.cell], [96, 123, 10, 10, 29]);
});
