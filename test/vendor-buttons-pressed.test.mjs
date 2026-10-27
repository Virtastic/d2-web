import test from 'node:test';
import assert from 'node:assert/strict';
import { createUiButtonGesture } from '../play/assets/ui-button-gesture.mjs';
import { vendorButtonFrame, vendorButtons, vendorButtonHitRect, VENDOR_LAYOUT } from '../play/assets/vendor-panel.mjs';

// Mirrors the shell: press arms, draw shows the pressed cel only while over the button, release over it fires.
const hit = (x, y, smith) => vendorButtons(smith ? 154 : 1).find((n, k) => { const r = vendorButtonHitRect(VENDOR_LAYOUT, k); return n !== 'blank' && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; }) ?? null;
const centre = (name, smith = true) => { const r = vendorButtonHitRect(VENDOR_LAYOUT, vendorButtons(smith ? 154 : 1).indexOf(name)); return [r.x + 5, r.y + 5]; };
function click(name, upAt) {
  const g = createUiButtonGesture(), fired = [], frames = [];
  const show = (x, y) => frames.push(vendorButtons(154).map((n) => vendorButtonFrame(n, g.token === n && hit(x, y, true) === n)));
  let [x, y] = centre(name); g.press(1, hit(x, y, true)); show(x, y);
  [x, y] = upAt ?? [x, y]; show(x, y);
  const a = g.release(1, hit(x, y, true)); if (a) fired.push(a);
  show(x, y);
  return { fired, frames };
}
test('pressed cel is normal+1 for the pressed button only', () => {
  assert.deepEqual([vendorButtonFrame('buy'), vendorButtonFrame('sell'), vendorButtonFrame('repair'), vendorButtonFrame('repairAll'), vendorButtonFrame('exit')], [2, 4, 6, 18, 10]);
  assert.deepEqual(['buy', 'sell', 'repair', 'repairAll', 'exit'].map((n) => vendorButtonFrame(n, true)), [3, 5, 7, 19, 11]);
  assert.equal(vendorButtonFrame('blank', true), 0);
});
test('hold over button draws pressed, release over fires once, then normal', () => {
  const { fired, frames } = click('repair');
  assert.deepEqual(frames[0], [2, 4, 7, 18]); assert.deepEqual(fired, ['repair']); assert.deepEqual(frames[2], [2, 4, 6, 18]);
});
test('leaving the button shows normal and release elsewhere fires nothing', () => {
  const { fired, frames } = click('repairAll', [0, 0]);
  assert.deepEqual(frames[1], [2, 4, 6, 18]); assert.deepEqual(fired, []);
});
