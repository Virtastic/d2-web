import test from 'node:test';
import assert from 'node:assert/strict';
import { isButtonHeldOverTarget, isControlHeld } from '../play/assets/frontend-pointer-states.mjs';
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

test('hover alone never draws a down cel: OK and EXIT are held only when armed by their own press', () => {
  // Character Create's OK / EXIT regression: the pointer hovering OK with no button held showed the down frame.
  assert.equal(isControlHeld(null, 'ok', 'ok'), false, 'hover on OK without a press is the up cel');
  assert.equal(isControlHeld(null, 'exit', 'exit'), false, 'hover on EXIT without a press is the up cel');
  assert.equal(isControlHeld('ok', 'ok', 'ok'), true, 'a press on OK held over OK is the down cel');
  assert.equal(isControlHeld('exit', 'exit', 'exit'), true, 'a press on EXIT held over EXIT is the down cel');
  assert.equal(isControlHeld('ok', 'ok', 'exit'), false, 'a press on OK dragged onto EXIT is not held');
  assert.equal(isControlHeld('exit', 'ok', 'ok'), false, 'OK is not held while EXIT is the armed control');
  assert.equal(isControlHeld('ok', 'ok', null), false, 'a press dragged off every control is not held');
  assert.equal(d2winHeldFrame([{}, {}], 0, isControlHeld(null, 'exit', 'exit')), 0, 'hover keeps frame 0');
  assert.equal(d2winHeldFrame([{}, {}], 0, isControlHeld('exit', 'exit', 'exit')), 1, 'held draws frame 1');
});
