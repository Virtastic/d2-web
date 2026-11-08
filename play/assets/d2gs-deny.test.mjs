import test from 'node:test';
import assert from 'node:assert/strict';
import { d2gsDenyString, readD2gsDeny, findD2gsDeny, d2gsDenyError } from './d2gs-deny.mjs';

test('wire codes map to the retail strings', () => {
  const want = [5365, 5366, 5367, 5368, 5369, 5371, 5373, 5374, 5375, 5376, 5377, 5378, 5379, 5380, 5381, 5360, 5364, 5363, 5362, 5361, 5359, 5372, 10101, 10102, 5370, 5371];
  assert.deepEqual(Array.from({ length: 26 }, (_, i) => d2gsDenyString(i + 1)), want);
  assert.equal(d2gsDenyString(0), d2gsDenyString(22));
  assert.equal(d2gsDenyString(27), d2gsDenyString(22));
});
test('expansion wording for the two level-limit refusals', () => {
  assert.equal(d2gsDenyString(17), 5364);
  assert.equal(d2gsDenyString(17, true), 21794);
  assert.equal(d2gsDenyString(18, true), 21793);
  assert.equal(d2gsDenyString(19, true), d2gsDenyString(19));
});
test('refusal packet parsing', () => {
  assert.deepEqual(readD2gsDeny(Uint8Array.of(0xb4, 3, 0, 0, 0)), { code: 3 });
  assert.equal(readD2gsDeny(Uint8Array.of(0xb4, 3, 0)), null);
  const size = (b) => (b[0] === 0x01 ? 2 : 0);
  assert.equal(findD2gsDeny(Uint8Array.of(1, 9, 0xb4, 16, 0, 0, 0), size), 16);
  assert.equal(findD2gsDeny(Uint8Array.of(1, 9), size), null);
  assert.equal(d2gsDenyError(15).noticeString, 5381);
});
