import test from 'node:test';
import assert from 'node:assert/strict';
import { createUiButtonGesture } from '../play/assets/ui-button-gesture.mjs';

test('HUD Help stays armed through another pointer and opens only on its matching in-bounds release', () => {
  const gesture = createUiButtonGesture();
  let helpVisible = false;

  gesture.press(7, 'hud-help');
  assert.equal(gesture.token, 'hud-help', 'pointer-down supplies the held socket state');
  assert.equal(helpVisible, false, 'the Help action waits for release');

  assert.equal(gesture.release(8, 'hud-help'), null);
  assert.equal(gesture.token, 'hud-help', 'another pointer cannot end the held press');
  assert.equal(gesture.release(7, null), null, 'release outside the native hit rectangle cancels');
  assert.equal(gesture.token, null);
  assert.equal(helpVisible, false);

  gesture.press(9, 'hud-help');
  if (gesture.release(9, 'hud-help') === 'hud-help') helpVisible = true;
  assert.equal(helpVisible, true, 'a matching in-bounds release opens Help');
});

test('cancelling the HUD Help pointer clears its held state without opening Help', () => {
  const gesture = createUiButtonGesture();
  let helpVisible = false;

  gesture.press(11, 'hud-help');
  assert.equal(gesture.cancel(12), false);
  assert.equal(gesture.token, 'hud-help');
  assert.equal(gesture.cancel(11), true);
  assert.equal(gesture.release(11, 'hud-help'), null);
  assert.equal(gesture.token, null);
  assert.equal(helpVisible, false);
});
