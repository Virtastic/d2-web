import test from 'node:test';
import assert from 'node:assert/strict';
import { popupRows, popupGate } from './belt-popup.mjs';
// Belts.txt box counts: belt 12, sash 8, default 4, girdle 16, light belt 8, heavy belt 12, uber belt 16.
test('the popup shows the worn belt\'s extra rows, and none for the default record', () => {
  const boxes = [12, 8, 4, 16, 8, 12, 16];
  assert.deepEqual(boxes.map((n, record) => popupRows(record, n)), [2, 1, 0, 3, 1, 2, 3]);
  assert.deepEqual([0, 1, 2, 3, 4, 5, 6].map(popupGate), [1, 1, 0, 1, 1, 1, 1]);
});
test('record 2 stays closed even if it had more boxes', () => assert.equal(popupRows(2, 16), 0));
