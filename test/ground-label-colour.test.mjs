import test from 'node:test';
import assert from 'node:assert/strict';
import { groundLabelColour } from '../play/assets/hover-lifebar.mjs';
// Quality numbers are d2_session_item_quality's (dwQualityNo); table is D2Client+0x58FB0's switch.
test('ground label colour by quality', () => {
  assert.deepEqual([1, 2, 3, 4, 5, 6, 7, 8, 9].map(groundLabelColour), [0, 0, 0, 3, 2, 9, 4, 8, 10]);
  assert.equal(groundLabelColour(undefined), 0);
  assert.equal(groundLabelColour(-1), 0);
});
