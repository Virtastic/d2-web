import test from 'node:test';
import assert from 'node:assert/strict';
import { isButtonHeldOverTarget } from '../play/assets/frontend-pointer-states.mjs';
import { d2winHeldFrame } from '../play/assets/d2win-button-frame.mjs';

test('a held button draws its down feedback only while the pointer remains over the armed control', () => {
  const armed = 'button-create';
  assert.equal(isButtonHeldOverTarget(armed, armed), true);
  assert.equal(isButtonHeldOverTarget(armed, 'button-exit'), false);
  assert.equal(isButtonHeldOverTarget(armed, null), false);
  assert.equal(isButtonHeldOverTarget(null, armed), false);

  const frames = [{}, {}];
  assert.equal(d2winHeldFrame(frames, 0, isButtonHeldOverTarget(armed, armed)), 1);
  assert.equal(d2winHeldFrame(frames, 0, isButtonHeldOverTarget(armed, 'button-exit')), 0);
  assert.equal(d2winHeldFrame(frames, 0, isButtonHeldOverTarget(armed, null)), 0);
});
