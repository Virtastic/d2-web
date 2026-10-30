import test from 'node:test';
import assert from 'node:assert/strict';
import { createAutomapRecordIndex, selectAutomapRecord, selectIndexedAutomapRecord } from '../play/assets/automap-record-index.mjs';

test('indexed AutoMap lookup preserves ordered predicate results', () => {
  const rows = [
    { id: 0, levelName: '1 Town', tileName: 'fl', style: 3, startSequence: 0, endSequence: 8 },
    { id: 1, levelName: '1 Town', tileName: 'fl', style: -1, startSequence: 4, endSequence: 6 },
    { id: 2, levelName: '1 Town', tileName: 'fl', style: -1, startSequence: -1, endSequence: -1 },
    { id: 3, levelName: '2 Town', tileName: 'fl', style: -1, startSequence: -1, endSequence: -1 },
  ];
  const index = createAutomapRecordIndex(rows);
  for (const [level, tileName, tile] of [
    ['1 Town', 'fl', { mainIndex: 3, subIndex: 5 }],
    ['1 Town', 'fl', { mainIndex: 2, subIndex: 5 }],
    ['1 Town', 'fl', { mainIndex: 2, subIndex: 9 }],
    ['2 Town', 'fl', { mainIndex: 99, subIndex: 0 }],
    ['3 Town', 'fl', { mainIndex: 0, subIndex: 0 }],
  ]) {
    assert.equal(selectIndexedAutomapRecord(index, level, tileName, tile),
      selectAutomapRecord(rows, level, tileName, tile));
  }
});

test('index buckets retain first-match order when tile rows are interleaved', () => {
  const rows = [
    { id: 'specific-first', levelName: '1 Town', tileName: 'fl', style: 3, startSequence: 0, endSequence: 5 },
    { id: 'other-tile', levelName: '1 Town', tileName: 'wl', style: -1, startSequence: -1, endSequence: -1 },
    { id: 'wildcard-later', levelName: '1 Town', tileName: 'fl', style: -1, startSequence: 6, endSequence: 10 },
    { id: 'specific-later', levelName: '1 Town', tileName: 'fl', style: 3, startSequence: 4, endSequence: 9 },
  ];
  const index = createAutomapRecordIndex(rows);
  for (const [tileName, tile] of [
    ['fl', { mainIndex: 3, subIndex: 4 }],
    ['fl', { mainIndex: 3, subIndex: 7 }],
    ['fl', { mainIndex: 2, subIndex: 7 }],
    ['wl', { mainIndex: 2, subIndex: 7 }],
    ['wr', { mainIndex: 2, subIndex: 7 }],
  ]) {
    assert.equal(selectIndexedAutomapRecord(index, '1 Town', tileName, tile),
      selectAutomapRecord(rows, '1 Town', tileName, tile));
  }
  assert.equal(selectIndexedAutomapRecord(index, '1 Town', 'fl', { mainIndex: 3, subIndex: 7 })?.id,
    'wildcard-later');
});

test('EndSequence -1 is the unsigned byte 255 (open-ended)', () => {
  const rows = [{ id: 'wl', levelName: '1 Wilderness', tileName: 'wl', style: 0, startSequence: 2, endSequence: -1 }];
  assert.equal(selectAutomapRecord(rows, '1 Wilderness', 'wl', { mainIndex: 0, subIndex: 2 })?.id, 'wl');
  assert.equal(selectAutomapRecord(rows, '1 Wilderness', 'wl', { mainIndex: 0, subIndex: 200 })?.id, 'wl');
  assert.equal(selectAutomapRecord(rows, '1 Wilderness', 'wl', { mainIndex: 0, subIndex: 1 }), null);
});
