import test from 'node:test';
import assert from 'node:assert/strict';
import { characterPanelCloseAt, characterPanelCloseFrame, createCharacterPanelCloseGesture } from '../play/assets/character-panel-close.mjs';

test('character panel close hit bounds are the native inclusive rectangle', () => {
  assert.equal(characterPanelCloseAt(208, 448, 800, 600), true);
  assert.equal(characterPanelCloseAt(240, 480, 800, 600), true);
  assert.equal(characterPanelCloseAt(207, 464, 800, 600), false);
  assert.equal(characterPanelCloseAt(241, 464, 800, 600), false);
  assert.equal(characterPanelCloseAt(224, 447, 800, 600), false);
  assert.equal(characterPanelCloseAt(224, 481, 800, 600), false);
  assert.equal(characterPanelCloseAt(180, 370, 640, 480), true, 'scaled canvas maps back to retail coordinates');
});

test('held pointer keeps the close gesture pressed and release inside activates', () => {
  const gesture = createCharacterPanelCloseGesture();
  assert.equal(gesture.pressed, false);
  assert.equal(characterPanelCloseFrame(gesture.pressed), 10);
  assert.equal(gesture.press(7), true);
  assert.equal(gesture.pressed, true);
  assert.equal(characterPanelCloseFrame(gesture.pressed), 11);
  assert.equal(gesture.release(8, true), false, 'another pointer cannot release it');
  assert.equal(gesture.pressed, true);
  assert.equal(gesture.release(7, true), true);
  assert.equal(gesture.pressed, false);
  assert.equal(characterPanelCloseFrame(gesture.pressed), 10);
});

test('release outside and cancel/lost capture clear the held state without activation', () => {
  const release = createCharacterPanelCloseGesture();
  release.press(3);
  assert.equal(release.release(3, false), false);
  assert.equal(release.pressed, false);

  for (const event of ['pointercancel', 'lostpointercapture']) {
    const gesture = createCharacterPanelCloseGesture();
    gesture.press(12);
    assert.equal(gesture.cancel(13), false, `${event} from another pointer leaves ownership intact`);
    assert.equal(gesture.pressed, true);
    assert.equal(gesture.cancel(12), true, `${event} clears the owning held state`);
    assert.equal(gesture.pressed, false);
    assert.equal(gesture.release(12, true), false, `${event} prevents later activation`);
  }
});
