import test from 'node:test';
import assert from 'node:assert/strict';
import { newFade, stepFade } from '../play/assets/roof-fade.mjs';

test('hide: opaque on the first frame, 0xff -> 0x80 linearly over 500 ms, then steady', () => {
  const s = newFade();
  assert.equal(stepFade(s, true, 1000), 0xff);
  assert.equal(stepFade(s, true, 1250), 0xff - 63);        // 255 + trunc(-127*250/500)
  assert.equal(stepFade(s, true, 1499), 0x80 + 1);
  assert.equal(stepFade(s, true, 1500), 0x80);
  assert.equal(s.fading, false);
  assert.equal(stepFade(s, true, 9000), 0x80);
});
test('reversal mid-fade is continuous, and the fade back takes the remaining share of 500 ms', () => {
  const s = newFade(); stepFade(s, true, 0);
  const a = stepFade(s, true, 250);
  assert.ok(Math.abs(stepFade(s, false, 250) - a) <= 1);   // integer truncation, as in the asm
  assert.equal(stepFade(s, false, 250 + 500 * (255 - a) / 127 + 1), 0xff);
});
test('a wall that never hid stays 0xff', () => assert.equal(stepFade(newFade(), false, 5), 0xff));
