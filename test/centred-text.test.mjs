import test from 'node:test';
import assert from 'node:assert/strict';
import { drawCentredBitmap } from '../play/assets/centred-text.mjs';

test('centres by measured advance and reports no font', () => {
  const calls = [];
  const font = { measure: (t) => t.length * 7, draw: (...a) => calls.push(a) };
  assert.equal(drawCentredBitmap({}, font, 'abc', 400, 20), true);
  assert.deepEqual(calls[0].slice(1), ['abc', 389, 20]);
  assert.equal(drawCentredBitmap({}, null, 'abc', 400, 20), false);
});

test('left bitmap text draws at whole pixels and reports no font', async () => {
  const { drawLeftBitmap } = await import('../play/assets/centred-text.mjs');
  const calls = [];
  const font = { draw: (...a) => calls.push(a) };
  assert.equal(drawLeftBitmap({}, font, 'Hireling', 16.7, 18.2), true);
  assert.deepEqual(calls[0].slice(1), ['Hireling', 16, 18]);
  assert.equal(drawLeftBitmap({}, null, 'x', 1, 1), false);
});

import { drawCentredMessage } from '../play/assets/centred-text.mjs';
test('message uses bitmap font and never falls back to canvas text', () => {
  const calls = [], ctx = { fillText: (...a) => calls.push(a) };
  drawCentredMessage(ctx, { measure: () => 10, draw: (...a) => calls.push(a) }, 'x', 400, 300);
  assert.equal(calls.length, 1); assert.deepEqual(calls[0].slice(1), ['x', 195, 142]);
  drawCentredMessage(ctx, null, 'x', 400, 300);
  assert.equal(calls.length, 1); assert.equal(ctx.font, undefined);
});
