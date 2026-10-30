import test from 'node:test';
import assert from 'node:assert/strict';
import { HUD_LAYOUT, levelButtonFrame } from './control-panel.mjs';
import { isUiButtonClick } from './ui-click-sound.mjs';

// A held add-point button draws Level.dc6 frame 1 (the sunken gem, 841 opaque px against frame 0's 870);
// the other lit button and an idle state keep frame 0. Disabled stays frame 2.
test('the held add-point button draws its pressed frame, the other stays lit', () => {
  const s = HUD_LAYOUT.levelButtons;
  assert.equal(s.pressedFrame, 1);
  assert.equal(new Set([s.litFrame, s.pressedFrame, s.disabledFrame]).size, 3);
  assert.equal(levelButtonFrame(0, 0), s.pressedFrame);
  assert.equal(levelButtonFrame(1, 0), s.litFrame);
  assert.equal(levelButtonFrame(1, 1), s.pressedFrame);
  assert.equal(levelButtonFrame(0, -1), s.litFrame);
  assert.equal(levelButtonFrame(0, undefined), s.litFrame);
});

// Retail plays the button click on the press itself: the shell marks the event as a menu activation
// (no world spend) and the pointerdown listener plays cursor_button_click.
test('a press on a lit add-point button is a UI button click', () => {
  const press = { button: 0, defaultPrevented: true, __d2ClickRecord: { spent: null } };
  assert.equal(isUiButtonClick(press), true);
  assert.equal(isUiButtonClick({ button: 2, defaultPrevented: true, __d2ClickRecord: { spent: null } }), false);
  assert.equal(isUiButtonClick({ button: 0, defaultPrevented: true, __d2ClickRecord: { spent: 'world' } }), false);
});
