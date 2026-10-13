import test from 'node:test';
import assert from 'node:assert/strict';
import { armSkillTreeSelection, allocateSkillTreePointOnRelease } from '../play/assets/skill-tree-input.mjs';
import { createUiButtonGesture } from '../play/assets/ui-button-gesture.mjs';

test('skill-tree left-down selects the right hand, then matching release may allocate a point', () => {
  const events = [], gesture = createUiButtonGesture();
  const token = armSkillTreeSelection(47, 0, (id, hand) => events.push(['select', id, hand]));
  gesture.press(5, token);
  assert.deepEqual(events, [['select', 47, 'right']]);
  assert.equal(gesture.release(5, token), token);
  assert.equal(allocateSkillTreePointOnRelease(token, 47, [{ id: 47, canLearn: true }], 1,
    (id) => events.push(['allocate', id])), true);
  assert.deepEqual(events, [['select', 47, 'right'], ['allocate', 47]]);
});

test('other button, pointer, icon, unlearnable skill, and empty points cannot allocate', () => {
  let selects = 0, allocations = 0;
  assert.equal(armSkillTreeSelection(47, 2, () => selects++), null);
  const token = armSkillTreeSelection(47, 0, () => selects++);
  const gesture = createUiButtonGesture();
  gesture.press(5, token);
  assert.equal(gesture.release(6, token), null);
  const wrongIcon = gesture.release(5, 'skill-tree:36');
  if (wrongIcon) allocateSkillTreePointOnRelease(wrongIcon, 36, [{ id: 47, canLearn: true }], 1,
    () => allocations++);
  assert.equal(wrongIcon, null);
  assert.equal(allocateSkillTreePointOnRelease(token, 47, [{ id: 47, canLearn: false }], 1,
    () => allocations++), false);
  assert.equal(allocateSkillTreePointOnRelease(token, 47, [{ id: 47, canLearn: true }], 0,
    () => allocations++), false);
  assert.equal(selects, 1);
  assert.equal(allocations, 0);
});
