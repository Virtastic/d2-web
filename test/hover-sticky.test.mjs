import test from 'node:test';
import assert from 'node:assert/strict';
import { selectPointerCandidate } from '../play/gameplay-model.mjs';

// D2Client FUN_6FB01E80: the previous hover target wins while it is still under the cursor.
test('prior hover target is kept over a higher-priority overlapping candidate', () => {
  const item = { id: 7, x: 10, y: 10 }, unit = { id: 3, hostile: true, mode: 1, x: 12, y: 12 };
  const c = { units: [unit], itemsOnCell: [item] };
  assert.equal(selectPointerCandidate(c, {}).kind, 'unit');
  assert.equal(selectPointerCandidate(c, { prior: { id: 7, kind: 'item' } }).kind, 'item');
  assert.equal(selectPointerCandidate(c, { prior: { id: 99, kind: 'item' } }).kind, 'unit');
});
