import test from 'node:test';
import assert from 'node:assert/strict';
import { createAutomapCellOrderCache } from '../play/assets/automap-overlay.mjs';

function retailOrderedEntries(layer) {
  return [...layer.entries()].sort((a, b) => a[1].layer - b[1].layer);
}

test('automap cached cell order preserves draw output and avoids repeated sorting', () => {
  // The retail town draw list contains 890 cells; use that measured layer size for the frame loop.
  const layer = new Map();
  for (let i = 0; i < 890; i++) {
    layer.set(`cell-${i}`, { layer: [0, 1, 2][i % 3], frame: (i * 17) % 83, ordinal: i });
  }
  const cache = createAutomapCellOrderCache();
  const expected = retailOrderedEntries(layer);

  let baselineSorts = 0;
  for (let frame = 0; frame < 8; frame++) {
    baselineSorts++;
    assert.deepEqual(cache.get(layer), expected);
  }
  assert.equal(baselineSorts, 8);
  assert.equal(cache.builds, 1, 'an unchanged visible layer is sorted once across eight frames');

  // Absorb adds a cell, invalidates the order, and keeps equal-layer insertion order on rebuild.
  layer.set('new-cell', { layer: 1, frame: 9, ordinal: 890 });
  cache.invalidate(layer);
  assert.deepEqual(cache.get(layer), retailOrderedEntries(layer));
  assert.equal(cache.builds, 2);

  // A saved-map seed may replace an entry without changing Map.size; explicit invalidation still
  // updates the draw order when that entry's layer changes.
  layer.set('cell-0', { layer: 2, frame: 77, ordinal: 0 });
  cache.invalidate(layer);
  assert.deepEqual(cache.get(layer), retailOrderedEntries(layer));
  assert.equal(cache.builds, 3);
});
