import test from 'node:test';
import assert from 'node:assert/strict';
import { drawCentredLines } from '../play/assets/centred-text.mjs';

test('drawCentredLines draws every line centred, step apart; false without a font', () => {
  const calls = [];
  const font = { measure: (t) => t.length * 2, draw: (_c, t, x, y) => calls.push([t, x, y]) };
  assert.equal(drawCentredLines({}, font, ['abcd', 'ab'], 100, 10, 22), true);
  assert.deepEqual(calls, [['abcd', 96, 10], ['ab', 98, 32]]);
  assert.equal(drawCentredLines({}, null, ['x'], 0, 0), false);
});
