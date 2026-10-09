import test from 'node:test';
import assert from 'node:assert/strict';
import { miniPanelPressAction, miniPanelReleaseAction } from '../play/assets/minipanel-input.mjs';

test('MiniPanel declines down and up while an inventory cursor item is held', () => {
  assert.equal(miniPanelPressAction(2, 0), 2);
  assert.equal(miniPanelPressAction(2, 0x1234), -1);
  assert.equal(miniPanelReleaseAction({ button: 0, pressed: 2, pointerMatches: true,
    releaseHit: 3, cursorItem: 0x1234 }), -1);
});

test('MiniPanel dispatches the release slot and requires its matching left-button pointer', () => {
  const base = { button: 0, pressed: 1, pointerMatches: true, cursorItem: 0 };
  assert.equal(miniPanelReleaseAction({ ...base, releaseHit: 4 }), 4);
  assert.equal(miniPanelReleaseAction({ ...base, releaseHit: -1 }), -1);
  assert.equal(miniPanelReleaseAction({ ...base, button: 2, releaseHit: 4 }), -1);
  assert.equal(miniPanelReleaseAction({ ...base, pointerMatches: false, releaseHit: 4 }), -1);
});
